#include "cpp_find.h"
#include <QDirIterator>
#include <QRegularExpression>
#include <QTextStream>
#include <QFile>
#include <QDir>

namespace tlex {
   namespace cppfind {

      // ── Вспомогательные методы ────────────────────────────────────────────────────

      bool CppEntityExtractor::skip_angle_brackets() {
         if (p.m_currentIdx >= p.lexem_count() || !p.is_this_sym("<")) return false;
         int depth = 1; p.next();
         while (p.m_currentIdx < p.lexem_count() && depth > 0) {
            if (p.is_this_sym("<")) depth++;
            else if (p.is_this_sym(">")) depth--;
            else if (p.is_this_sym(">>")) { depth -= 2; if (depth < 0) return false; }
            p.next();
         }
         return depth == 0;
      }

      bool CppEntityExtractor::skip_parentheses() {
         if (p.m_currentIdx >= p.lexem_count() || !p.is_this_sym("(")) return false;
         int depth = 1; p.next();
         while (p.m_currentIdx < p.lexem_count() && depth > 0) {
            if (p.is_this_sym("(")) depth++;
            else if (p.is_this_sym(")")) depth--;
            p.next();
         }
         return depth == 0;
      }

      // НОВЫЙ: пропуск любой строки препроцессора (#include, #pragma, #ifdef и т.д.)
      void CppEntityExtractor::skip_preprocessor_line() {
         if (p.m_currentIdx >= p.lexem_count()) return;
         int start_line = p.cur().lin;
         while (p.m_currentIdx < p.lexem_count()) {
            if (p.cur().lin > start_line) {
               // Проверяем, не было ли '\' в конце предыдущей строки
               const TLex& prev = look_back(1);
               if (prev.v_s.length > 0 && prev.v_s.str[prev.v_s.length - 1] != U'\\') {
                  break;
               }
            }
            p.next();
         }
      }

      // НОВЫЙ: проверка, является ли токен ключевым словом C++, на котором нужно остановиться
      bool CppEntityExtractor::is_cpp_keyword() const {
         if (p.m_currentIdx >= p.lexem_count()) return false;
         return p.is_this_word("namespace") || p.is_this_word("class") ||
            p.is_this_word("struct") || p.is_this_word("enum") ||
            p.is_this_word("typedef") || p.is_this_word("using") ||
            p.is_this_word("template") || p.is_this_word("return") ||
            p.is_this_word("if") || p.is_this_word("else") ||
            p.is_this_word("while") || p.is_this_word("for") ||
            p.is_this_word("do") || p.is_this_word("switch") ||
            p.is_this_word("case") || p.is_this_word("break") ||
            p.is_this_word("continue") || p.is_this_word("goto") ||
            p.is_this_word("throw") || p.is_this_word("try") ||
            p.is_this_word("catch") || p.is_this_word("new") ||
            p.is_this_word("delete") || p.is_this_word("public") ||
            p.is_this_word("private") || p.is_this_word("protected");
      }

      // ── Рекурсивный движок ────────────────────────────────────────────────────────

      void CppEntityExtractor::parse_sequence() {
         while (p.m_currentIdx < p.lexem_count()) {
            if (p.is_this_sym("}")) {
               return;
            }

            // НОВЫЙ: обработка препроцессора ДО всего остального
            if (p.is_this_sym("#")) {
               // Заглядываем вперёд: если это #define, отдаём в try_parse_macro
               int next_idx = p.m_currentIdx + 1;
               if (next_idx < p.lexem_count() && p.lexem(next_idx).is_this_word("define")) {
                  // не пропускаем, try_parse_macro обработает
               }
               else {
                  // #include, #pragma, #ifdef, #endif и т.д. — просто пропускаем строку
                  skip_preprocessor_line();
                  continue;
               }
            }

            int prefix_idx = p.m_currentIdx;
            int saved_prefix_idx = collect_prefixes();

            if (try_parse_namespace(saved_prefix_idx)) continue;
            if (try_parse_class_struct(saved_prefix_idx)) continue;
            if (try_parse_enum(saved_prefix_idx)) continue;
            if (try_parse_alias(saved_prefix_idx)) continue;
            if (try_parse_macro(saved_prefix_idx)) continue;

            if (!is_inside_function()) {
               if (try_parse_function_or_var(saved_prefix_idx)) continue;
            }

            if (p.m_currentIdx < p.lexem_count() && p.is_this_sym("{")) {
               p.next();
               parse_sequence();
               if (p.m_currentIdx < p.lexem_count() && p.is_this_sym("}")) {
                  p.next();
               }
               continue;
            }

            p.next();
         }
      }

      bool CppEntityExtractor::try_parse_namespace(int prefix_idx) {
         if (p.m_currentIdx >= p.lexem_count() || !p.is_this_word("namespace")) return false;
         p.next();

         std::string name = "anonymous";
         if (p.m_currentIdx < p.lexem_count() && p.is_word()) {
            name = p.cur().v_s.to_std_string();
            p.next();
            while (p.m_currentIdx < p.lexem_count() && p.is_this_sym("::")) {
               p.next();
               if (p.m_currentIdx < p.lexem_count() && p.is_word()) {
                  name += "::" + p.cur().v_s.to_std_string();
                  p.next();
               }
            }
         }

         if (p.m_currentIdx < p.lexem_count() && p.is_this_sym("{")) {
            m_scopeStack.push_back({ "namespace", name });
            p.next();
            parse_sequence();

            int end_idx = p.m_currentIdx - 1;
            if (p.m_currentIdx < p.lexem_count() && p.is_this_sym("}")) p.next();
            if (p.m_currentIdx < p.lexem_count() && p.is_this_sym(";")) p.next();

            entities.push_back({ "namespace", name, get_current_scope(),
                extract_raw_text(prefix_idx, end_idx),
                p.lexem(prefix_idx).lin, p.lexem(end_idx).lin });

            m_scopeStack.pop_back();
            return true;
         }
         return false;
      }

      bool CppEntityExtractor::try_parse_class_struct(int prefix_idx) {
         if (p.m_currentIdx >= p.lexem_count()) return false;
         if (!p.is_this_word("class") && !p.is_this_word("struct")) return false;
         std::string type = p.cur().v_s.to_std_string();
         p.next();

         if (p.m_currentIdx >= p.lexem_count() || !p.is_word()) return false;
         std::string name = p.cur().v_s.to_std_string();
         p.next();

         if (p.m_currentIdx < p.lexem_count() && p.is_this_sym(":")) {
            p.next();
            while (p.m_currentIdx < p.lexem_count() && !p.is_this_sym("{") && !p.is_this_sym("}")) {
               if (p.is_this_sym("<")) skip_angle_brackets();
               else p.next();
            }
         }

         if (p.m_currentIdx < p.lexem_count() && p.is_this_sym("{")) {
            m_scopeStack.push_back({ type, name });
            p.next();
            parse_sequence();

            int end_idx = p.m_currentIdx - 1;
            if (p.m_currentIdx < p.lexem_count() && p.is_this_sym("}")) p.next();
            if (p.m_currentIdx < p.lexem_count() && p.is_this_sym(";")) p.next();

            entities.push_back({ type, name, get_current_scope(),
                extract_raw_text(prefix_idx, end_idx),
                p.lexem(prefix_idx).lin, p.lexem(end_idx).lin });

            m_scopeStack.pop_back();
            return true;
         }
         return false;
      }

      bool CppEntityExtractor::try_parse_enum(int prefix_idx) {
         if (p.m_currentIdx >= p.lexem_count() || !p.is_this_word("enum")) return false;
         p.next();
         if (p.m_currentIdx < p.lexem_count() && (p.is_this_word("class") || p.is_this_word("struct"))) p.next();

         std::string name = "anonymous";
         if (p.m_currentIdx < p.lexem_count() && p.is_word()) {
            name = p.cur().v_s.to_std_string();
            p.next();
         }

         if (p.m_currentIdx < p.lexem_count() && p.is_this_sym(":")) {
            p.next();
            while (p.m_currentIdx < p.lexem_count() && !p.is_this_sym("{") && !p.is_this_sym("}")) p.next();
         }

         if (p.m_currentIdx < p.lexem_count() && p.is_this_sym("{")) {
            m_scopeStack.push_back({ "enum", name });
            p.next();
            parse_sequence();

            int end_idx = p.m_currentIdx - 1;
            if (p.m_currentIdx < p.lexem_count() && p.is_this_sym("}")) p.next();
            if (p.m_currentIdx < p.lexem_count() && p.is_this_sym(";")) p.next();

            entities.push_back({ "enum", name, get_current_scope(),
                extract_raw_text(prefix_idx, end_idx),
                p.lexem(prefix_idx).lin, p.lexem(end_idx).lin });

            m_scopeStack.pop_back();
            return true;
         }
         return false;
      }

      bool CppEntityExtractor::try_parse_alias(int prefix_idx) {
         if (p.m_currentIdx >= p.lexem_count()) return false;
         if (!p.is_this_word("typedef") && !p.is_this_word("using")) return false;
         std::string type = p.cur().v_s.to_std_string();
         p.next();

         std::string name = "unknown";
         if (p.m_currentIdx < p.lexem_count() && type == "using" && p.is_word()) {
            name = p.cur().v_s.to_std_string();
         }

         int end_idx = p.m_currentIdx;
         while (p.m_currentIdx < p.lexem_count() && !p.is_this_sym(";") && !p.is_this_sym("}")) {
            if (p.is_word() && type == "typedef") name = p.cur().v_s.to_std_string();
            p.next();
         }
         end_idx = p.m_currentIdx;
         if (p.m_currentIdx < p.lexem_count() && p.is_this_sym(";")) p.next();

         entities.push_back({ type, name, get_current_scope(),
             extract_raw_text(prefix_idx, end_idx),
             p.lexem(prefix_idx).lin, p.lexem(end_idx).lin });
         return true;
      }

      bool CppEntityExtractor::try_parse_macro(int prefix_idx) {
         if (p.m_currentIdx >= p.lexem_count() || !p.is_this_sym("#")) return false;

         // Заглядываем вперёд, НЕ потребляя '#'
         int next_idx = p.m_currentIdx + 1;
         if (next_idx >= p.lexem_count() || !p.lexem(next_idx).is_this_word("define")) {
            return false; // Не #define — не наш случай
         }

         p.next(); // теперь потребляем '#'
         p.next(); // потребляем 'define'

         std::string name = "unknown";
         if (p.m_currentIdx < p.lexem_count() && p.is_word()) {
            name = p.cur().v_s.to_std_string();
            p.next();
         }

         int start_line = p.lexem(prefix_idx).lin;
         int end_idx = p.m_currentIdx - 1;

         while (p.m_currentIdx < p.lexem_count()) {
            if (p.cur().lin > start_line) {
               const TLex& prev = look_back(1);
               if (prev.v_s.length > 0 && prev.v_s.str[prev.v_s.length - 1] != U'\\') {
                  break;
               }
            }
            end_idx = p.m_currentIdx;
            p.next();
         }

         entities.push_back({ "macro", name, get_current_scope(),
             extract_raw_text(prefix_idx, end_idx),
             p.lexem(prefix_idx).lin, p.lexem(end_idx).lin });
         return true;
      }

      bool CppEntityExtractor::try_parse_function_or_var(int prefix_idx) {
         // ИСПРАВЛЕНИЕ: останавливаемся на '#' и ключевых словах C++
         while (p.m_currentIdx < p.lexem_count()
            && !p.is_this_sym("(") && !p.is_this_sym(";") && !p.is_this_sym("=")
            && !p.is_this_sym("{") && !p.is_this_sym("}")
            && !p.is_this_sym("#")
            && !is_cpp_keyword()) {
            if (p.is_this_sym("<")) skip_angle_brackets();
            else p.next();
         }

         if (p.m_currentIdx < p.lexem_count() && p.is_this_sym("(")) {
            int paren_start = p.m_currentIdx;
            skip_parentheses();

            while (p.m_currentIdx < p.lexem_count() && !p.is_this_sym("{") && !p.is_this_sym(";") && !p.is_this_sym("}")) {
               if (p.is_this_sym("<")) skip_angle_brackets();
               else p.next();
            }

            int name_idx = paren_start - 1;
            while (name_idx > prefix_idx && (p.lexem(name_idx).is_this_sym("*") || p.lexem(name_idx).is_this_sym("&"))) {
               name_idx--;
            }
            std::string name = p.lexem(name_idx).is_word() ? p.lexem(name_idx).v_s.to_std_string() : "unknown";

            int end_idx = p.m_currentIdx;
            if (p.m_currentIdx < p.lexem_count() && p.is_this_sym("{")) {
               m_scopeStack.push_back({ "func", name });
               p.next();
               parse_sequence();
               m_scopeStack.pop_back();

               end_idx = p.m_currentIdx - 1;
               if (p.m_currentIdx < p.lexem_count() && p.is_this_sym("}")) p.next();
            }
            else if (p.m_currentIdx < p.lexem_count() && p.is_this_sym(";")) {
               end_idx = p.m_currentIdx;
               p.next();
            }

            entities.push_back({ "func", name, get_current_scope(),
                extract_raw_text(prefix_idx, end_idx),
                p.lexem(prefix_idx).lin, p.lexem(end_idx).lin });
            return true;
         }
         else if (p.m_currentIdx < p.lexem_count() && (p.is_this_sym(";") || p.is_this_sym("="))) {
            int end_idx = p.m_currentIdx;
            while (p.m_currentIdx < p.lexem_count() && !p.is_this_sym(";") && !p.is_this_sym("}")) {
               if (p.is_this_sym("<")) skip_angle_brackets();
               else p.next();
            }
            end_idx = p.m_currentIdx;
            if (p.m_currentIdx < p.lexem_count() && p.is_this_sym(";")) p.next();

            std::string name = "unknown";
            int search_idx = end_idx - 1;
            while (search_idx >= prefix_idx) {
               if (p.lexem(search_idx).is_word()) {
                  name = p.lexem(search_idx).v_s.to_std_string();
                  break;
               }
               search_idx--;
            }

            if (name == "return" || name == "break" || name == "continue" ||
               name == "if" || name == "else" || name == "case" || name == "throw") {
               return false;
            }

            entities.push_back({ "var", name, get_current_scope(),
                extract_raw_text(prefix_idx, end_idx),
                p.lexem(prefix_idx).lin, p.lexem(end_idx).lin });
            return true;
         }
         return false;
      }

      // ── Парсинг аргументов и движок поиска ────────────────────────────────────────

      bool parse_find_args(const QString& arg_str, CppFindArgs& out) {
         TParser p;
         p.set_C_style(true);
         p.assign(arg_str);
         p.end_of_init();

         while (p.m_currentIdx < p.lexem_count()) {
            if (p.is_this_word("-r") || p.is_this_word("--recursive")) { out.recursive = true; p.next(); }
            else if (p.is_this_word("-l") || p.is_this_word("--list")) { out.list_only = true; out.show_code = false; p.next(); }
            else if (p.is_this_word("-S") || p.is_this_word("--show")) { out.show_code = true; out.list_only = false; p.next(); }
            else if (p.is_this_word("--include")) {
               p.next(); if (p.m_currentIdx < p.lexem_count() && p.is_this_sym("=")) p.next();
               if (p.m_currentIdx < p.lexem_count() && (p.is_str() || p.is_word())) { out.includes.push_back(p.cur().v_s.to_std_string()); p.next(); }
            }
            else if (p.is_this_word("--exclude")) {
               p.next(); if (p.m_currentIdx < p.lexem_count() && p.is_this_sym("=")) p.next();
               if (p.m_currentIdx < p.lexem_count() && (p.is_str() || p.is_word())) { out.excludes.push_back(p.cur().v_s.to_std_string()); p.next(); }
            }
            else if (p.is_this_word("--type") || p.is_this_word("-t")) {
               p.next(); if (p.m_currentIdx < p.lexem_count() && p.is_this_sym("=")) p.next();
               if (p.m_currentIdx < p.lexem_count() && p.is_word()) { out.type = p.cur().v_s.to_std_string(); p.next(); }
            }
            else if (p.is_this_word("--name") || p.is_this_word("-n")) {
               p.next(); if (p.m_currentIdx < p.lexem_count() && p.is_this_sym("=")) p.next();
               if (p.m_currentIdx < p.lexem_count() && (p.is_word() || p.is_str())) { out.name_pattern = p.cur().v_s.to_std_string(); p.next(); }
            }
            else if (p.is_this_word("--scope") || p.is_this_word("-s")) {
               p.next(); if (p.m_currentIdx < p.lexem_count() && p.is_this_sym("=")) p.next();
               if (p.m_currentIdx < p.lexem_count() && (p.is_word() || p.is_str())) { out.scope_pattern = p.cur().v_s.to_std_string(); p.next(); }
            }
            else {
               p.next();
            }
         }

         if (out.includes.empty()) {
            out.includes = { "*.cpp", "*.h", "*.hpp", "*.cc", "*.cxx", "*.inl" };
         }
         return true;
      }

      static bool match_glob(const std::string& glob, const QString& str) {
         QString rx = QRegularExpression::wildcardToRegularExpression(QString::fromStdString(glob));
         return QRegularExpression(rx).match(str).hasMatch();
      }

      static bool match_entity(const CppEntity& e, const CppFindArgs& args) {
         if (!args.type.empty() && args.type != "any" && e.type != args.type) return false;
         if (!args.name_pattern.empty()) {
            QString rx = QRegularExpression::wildcardToRegularExpression(QString::fromStdString(args.name_pattern));
            if (!QRegularExpression(rx).match(QString::fromStdString(e.name)).hasMatch()) return false;
         }
         if (!args.scope_pattern.empty()) {
            QString scope = QString::fromStdString(e.full_scope);
            QString pattern = QString::fromStdString(args.scope_pattern);
            if (!scope.startsWith(pattern)) return false;
         }
         return true;
      }

      QString CppFindEngine::execute(const CppFindArgs& args, const QString& baseFolder) {
         QString resultStr;
         QTextStream out(&resultStr);

         QDirIterator::IteratorFlags flags = args.recursive ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags;
         QDirIterator it(baseFolder, QDir::Files | QDir::Readable, flags);

         while (it.hasNext()) {
            QString filePath = it.next();
            QString fileName = it.fileName();

            bool included = false;
            for (const auto& glob : args.includes) {
               if (match_glob(glob, fileName)) { included = true; break; }
            }
            if (!included) continue;

            bool excluded = false;
            for (const auto& glob : args.excludes) {
               if (match_glob(glob, fileName)) { excluded = true; break; }
            }
            if (excluded) continue;

            QFile file(filePath);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) continue;

            QByteArray raw = file.readAll();
            file.close();

            TParser p;
            p.set_C_style(true);
            // ИСПРАВЛЕНИЕ: обязательно задаём маркеры комментариев для C++!
            p.set_end_ofLine_comment("//");
            p.set_open_comment("/*");
            p.set_close_comment("*/");
            p.assign(raw.constData(), TTextStorage::TEncoding::UTF8);
            p.end_of_init();

            CppEntityExtractor extractor(p);
            extractor.extract_all();

            for (const auto& e : extractor.get_entities()) {
               if (!match_entity(e, args)) continue;

               QString relPath = QDir(baseFolder).relativeFilePath(filePath);

               if (args.list_only) {
                  out << relPath << ":" << e.start_line << ": "
                     << QString::fromStdString(e.type) << " "
                     << QString::fromStdString(e.full_scope)
                     << (e.full_scope.empty() ? "" : "::")
                     << QString::fromStdString(e.name) << "\n";
               }
               else {
                  out << "--- " << relPath << " (Lines " << e.start_line << "-" << e.end_line << ") ---\n";
                  out << QString::fromStdString(e.raw_code) << "\n\n";
               }
            }
         }
         return resultStr;
      }

   } // namespace cppfind
} // namespace tlex
