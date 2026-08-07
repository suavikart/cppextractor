#pragma once
#include "tparser.h"
#include <vector>
#include <string>

namespace tlex {

// Структура для хранения найденной сущности
struct CppEntity {
    std::string type;       // "namespace", "class", "func", "macro", "alias"
    std::string name;       // Имя сущности (если удалось извлечь)
    std::string raw_code;   // Оригинальный UTF-8 текст (in-place)
    int start_line = 0;
    int end_line = 0;
};

class CppEntityExtractor {
    TParser& p;
    std::vector<CppEntity> entities;

    // ── Утилиты навигации и Look-back ───────────────────────────────────────
    // Взгляд назад (Look-back). Используем тот факт, что m_currentIdx публичен.
    const TLex& look_back(int offset = 1) {
        int idx = p.m_currentIdx - offset;
        return (idx >= 0) ? p.lexem(idx) : p.fail;
    }

    // In-place вырезка оригинального текста между двумя индексами токенов
    std::string extract_raw_text(int start_idx, int end_idx) {
        if (start_idx < 0 || end_idx >= p.lexem_count()) return "";
        const TLex& s_lex = p.lexem(start_idx);
        const TLex& e_lex = p.lexem(end_idx);
        
        const char32_t* start_ptr = s_lex.v_s.str;
        const char32_t* end_ptr = e_lex.v_s.str + e_lex.v_s.length;
        
        // Создаем временный TStr, указывающий на оригинальный буфер
        TStr raw_slice;
        raw_slice.assign(start_ptr, static_cast<int>(end_ptr - start_ptr));
        return raw_slice.to_std_string(); // Конвертация в UTF-8
    }

    // ── Пропуск скобочных структур (Балансировщики) ─────────────────────────
    
    // Пропуск < ... > с учетом вложенности и проблемы C++11 >>
    bool skip_angle_brackets() {
        if (!p.is_this_sym("<")) return false;
        int depth = 1;
        p.next();
        while (!p.is_eof() && depth > 0) {
            if (p.is_this_sym("<")) depth++;
            else if (p.is_this_sym(">")) depth--;
            else if (p.is_this_sym(">>")) { // Слияние двух > в один токен
                depth -= 2;
                if (depth < 0) return false; // Синтаксическая ошибка
            }
            p.next();
        }
        return depth == 0;
    }

    // Пропуск ( ... )
    bool skip_parentheses() {
        if (!p.is_this_sym("(")) return false;
        int depth = 1;
        p.next();
        while (!p.is_eof() && depth > 0) {
            if (p.is_this_sym("(")) depth++;
            else if (p.is_this_sym(")")) depth--;
            p.next();
        }
        return depth == 0;
    }

    // Пропуск { ... }
    bool skip_braces() {
        if (!p.is_this_sym("{")) return false;
        int depth = 1;
        p.next();
        while (!p.is_eof() && depth > 0) {
            if (p.is_this_sym("{")) depth++;
            else if (p.is_this_sym("}")) depth--;
            p.next();
        }
        return depth == 0;
    }

    // ── Парсинг префиксов (template, constexpr, inline, static) ─────────────
    // Возвращает индекс токена, с которого начинается "чистое" объявление
    int collect_prefixes() {
        int start_idx = p.m_currentIdx;
        while (!p.is_eof()) {
            if (p.is_this_word("template")) {
                p.next();
                skip_angle_brackets(); // Пропускаем <...>
            } 
            else if (p.is_this_word("constexpr") || 
                     p.is_this_word("inline") || 
                     p.is_this_word("static") ||
                     p.is_this_word("virtual") ||
                     p.is_this_word("explicit") ||
                     p.is_this_word("export")) {
                p.next();
            } 
            else {
                break; // Не префикс, выходим
            }
        }
        return start_idx;
    }

    // ── Парсеры конкретных сущностей (Строгий сверху вниз) ──────────────────

    bool try_parse_namespace(int prefix_idx) {
        if (!p.is_this_word("namespace")) return false;
        p.next();
        
        std::string name = "anonymous";
        if (p.is_word()) {
            name = p.cur().v_s.to_std_string();
            p.next();
        }
        
        if (p.is_this_sym("{")) {
            int end_idx = p.m_currentIdx;
            skip_braces();
            end_idx = p.m_currentIdx - 1; // Указываем на '}'
            
            entities.push_back({"namespace", name, extract_raw_text(prefix_idx, end_idx), p.lexem(prefix_idx).lin, p.cur().lin});
            return true;
        }
        return false;
    }

    bool try_parse_class_struct(int prefix_idx) {
        if (!p.is_this_word("class") && !p.is_this_word("struct")) return false;
        std::string type = p.cur().v_s.to_std_string();
        p.next();
        
        if (!p.is_word()) return false; // Ожидаем имя класса
        std::string name = p.cur().v_s.to_std_string();
        p.next();
        
        // Пропускаем возможные базовые классы (: public Base)
        if (p.is_this_sym(":")) {
            p.next();
            while (!p.is_eof() && !p.is_this_sym("{")) p.next();
        }
        
        if (p.is_this_sym("{")) {
            int end_idx = p.m_currentIdx;
            skip_braces();
            end_idx = p.m_currentIdx - 1;
            
            // Пропускаем ';' после '}'
            if (p.is_this_sym(";")) p.next();
            
            entities.push_back({type, name, extract_raw_text(prefix_idx, end_idx), p.lexem(prefix_idx).lin, p.cur().lin});
            return true;
        }
        return false;
    }

    bool try_parse_enum(int prefix_idx) {
        if (!p.is_this_word("enum")) return false;
        p.next();
        if (p.is_this_word("class") || p.is_this_word("struct")) p.next();
        
        std::string name = "anonymous";
        if (p.is_word()) {
            name = p.cur().v_s.to_std_string();
            p.next();
        }
        
        // Пропускаем тип (: int)
        if (p.is_this_sym(":")) {
            p.next();
            while (!p.is_eof() && !p.is_this_sym("{")) p.next();
        }

        if (p.is_this_sym("{")) {
            int end_idx = p.m_currentIdx;
            skip_braces();
            end_idx = p.m_currentIdx - 1;
            if (p.is_this_sym(";")) p.next();
            
            entities.push_back({"enum", name, extract_raw_text(prefix_idx, end_idx), p.lexem(prefix_idx).lin, p.cur().lin});
            return true;
        }
        return false;
    }

    bool try_parse_alias(int prefix_idx) {
        if (!p.is_this_word("typedef") && !p.is_this_word("using")) return false;
        std::string type = p.cur().v_s.to_std_string();
        p.next();
        
        // Ищем имя (для using оно сразу, для typedef - перед ';')
        std::string name = "unknown";
        if (type == "using" && p.is_word()) {
            name = p.cur().v_s.to_std_string();
        }
        
        // Идем до ';'
        int end_idx = p.m_currentIdx;
        while (!p.is_eof() && !p.is_this_sym(";")) {
            if (p.is_word() && type == "typedef") name = p.cur().v_s.to_std_string(); // Имя typedef обычно последнее перед ';'
            p.next();
        }
        end_idx = p.m_currentIdx;
        if (p.is_this_sym(";")) p.next();
        
        entities.push_back({type, name, extract_raw_text(prefix_idx, end_idx), p.lexem(prefix_idx).lin, p.cur().lin});
        return true;
    }

    bool try_parse_macro(int prefix_idx) {
        if (!p.is_this_sym("#")) return false;
        p.next();
        if (!p.is_this_word("define")) return false;
        p.next();
        
        std::string name = "unknown";
        if (p.is_word()) {
            name = p.cur().v_s.to_std_string();
            p.next();
        }
        
        // Макрос длится до конца строки, если нет '\' в конце
        int start_line = p.lexem(prefix_idx).lin;
        int end_idx = p.m_currentIdx - 1;
        
        while (!p.is_eof()) {
            if (p.cur().lin > start_line) {
                // Проверяем, был ли предыдущий токен с '\' на конце
                const TLex& prev = look_back(1);
                if (prev.v_s.length > 0 && prev.v_s.str[prev.v_s.length - 1] != U'\\') {
                    break; // Макрос закончился
                }
            }
            end_idx = p.m_currentIdx;
            p.next();
        }
        
        entities.push_back({"macro", name, extract_raw_text(prefix_idx, end_idx), p.lexem(prefix_idx).lin, p.cur().lin});
        return true;
    }

    bool try_parse_function_or_var(int prefix_idx) {
        // Эвристика: ищем паттерн "Identifier (" для функций
        // Пропускаем возвращаемый тип (слова, символы, <...>)
        while (!p.is_eof() && !p.is_this_sym("(") && !p.is_this_sym(";") && !p.is_this_sym("{") && !p.is_this_sym("=")) {
            if (p.is_this_sym("<")) skip_angle_brackets();
            else p.next();
        }
        
        if (p.is_this_sym("(")) {
            // Это функция!
            int end_idx = p.m_currentIdx;
            skip_parentheses();
            
            // Пропускаем const, noexcept, override, final и т.д.
            while (!p.is_eof() && !p.is_this_sym("{") && !p.is_this_sym(";")) {
                p.next();
            }
            
            if (p.is_this_sym("{")) {
                end_idx = p.m_currentIdx;
                skip_braces();
                end_idx = p.m_currentIdx - 1;
            } else if (p.is_this_sym(";")) {
                end_idx = p.m_currentIdx;
                p.next();
            }
            
            // Имя функции - это слово перед '('
            std::string name = look_back(1).v_s.to_std_string(); 
            entities.push_back({"func", name, extract_raw_text(prefix_idx, end_idx), p.lexem(prefix_idx).lin, p.cur().lin});
            return true;
        } 
        else if (p.is_this_sym(";") || p.is_this_sym("=")) {
            // Это переменная
            int end_idx = p.m_currentIdx;
            while (!p.is_eof() && !p.is_this_sym(";")) p.next();
            end_idx = p.m_currentIdx;
            if (p.is_this_sym(";")) p.next();
            
            std::string name = look_back(1).v_s.to_std_string();
            entities.push_back({"var", name, extract_raw_text(prefix_idx, end_idx), p.lexem(prefix_idx).lin, p.cur().lin});
            return true;
        }
        
        return false;
    }

public:
    CppEntityExtractor(TParser& parser) : p(parser) {}

    void extract_all() {
        entities.clear();
        while (!p.is_eof()) {
            int prefix_idx = collect_prefixes();
            
            if (try_parse_namespace(prefix_idx)) continue;
            if (try_parse_class_struct(prefix_idx)) continue;
            if (try_parse_enum(prefix_idx)) continue;
            if (try_parse_alias(prefix_idx)) continue;
            if (try_parse_macro(prefix_idx)) continue;
            if (try_parse_function_or_var(prefix_idx)) continue;
            
            // Если ничего не подошло, сдвигаемся на 1 токен, чтобы не зациклиться
            if (!p.is_eof()) p.next();
        }
    }

    const std::vector<CppEntity>& get_entities() const { return entities; }
};

} // namespace tlex
