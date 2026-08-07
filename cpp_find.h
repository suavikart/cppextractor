#pragma once
#include "tparser.h"
#include <vector>
#include <string>
#include <QString>

namespace tlex {
   namespace cppfind {

      struct CppEntity {
         std::string type;
         std::string name;
         std::string full_scope;
         std::string raw_code;
         int start_line = 0;
         int end_line = 0;
      };

      class CppEntityExtractor {
         TParser& p;
         std::vector<CppEntity> entities;

         struct ScopeNode { std::string type; std::string name; };
         std::vector<ScopeNode> m_scopeStack;

         const TLex& look_back(int offset = 1) const {
            int idx = p.m_currentIdx - offset;
            return (idx >= 0) ? p.lexem(idx) : p.fail;
         }

         std::string extract_raw_text(int start_idx, int end_idx) const {
            if (start_idx < 0 || end_idx >= p.lexem_count()) return "";
            const TLex& s_lex = p.lexem(start_idx);
            const TLex& e_lex = p.lexem(end_idx);
            const char32_t* start_ptr = s_lex.v_s.str;
            const char32_t* end_ptr = e_lex.v_s.str + e_lex.v_s.length;
            TStr raw_slice;
            raw_slice.assign(start_ptr, static_cast<int>(end_ptr - start_ptr));
            return raw_slice.to_std_string();
         }

         std::string get_current_scope() const {
            std::string res;
            for (const auto& n : m_scopeStack) {
               if (!res.empty()) res += "::";
               res += n.name;
            }
            return res;
         }

         bool is_inside_function() const {
            for (const auto& scope : m_scopeStack) {
               if (scope.type == "func") return true;
            }
            return false;
         }

         bool skip_angle_brackets();
         bool skip_parentheses();
         void skip_preprocessor_line();  // НОВЫЙ
         bool is_cpp_keyword() const;    // НОВЫЙ

         void parse_sequence();

         int collect_prefixes() {
            int start_idx = p.m_currentIdx;
            while (p.m_currentIdx < p.lexem_count()) {
               if (p.is_this_word("template")) {
                  p.next();
                  skip_angle_brackets();
               }
               else if (p.is_this_word("constexpr") || p.is_this_word("inline") ||
                  p.is_this_word("static") || p.is_this_word("virtual") ||
                  p.is_this_word("explicit") || p.is_this_word("export") ||
                  p.is_this_word("friend")) {
                  p.next();
               }
               else {
                  break;
               }
            }
            return start_idx;
         }

         bool try_parse_namespace(int prefix_idx);
         bool try_parse_class_struct(int prefix_idx);
         bool try_parse_enum(int prefix_idx);
         bool try_parse_alias(int prefix_idx);
         bool try_parse_macro(int prefix_idx);
         bool try_parse_function_or_var(int prefix_idx);

      public:
         explicit CppEntityExtractor(TParser& parser) : p(parser) {}
         void extract_all() {
            entities.clear();
            m_scopeStack.clear();
            parse_sequence();
         }
         const std::vector<CppEntity>& get_entities() const { return entities; }
      };

      struct CppFindArgs {
         bool recursive = false;
         bool list_only = false;
         bool show_code = true;
         std::vector<std::string> includes;
         std::vector<std::string> excludes;
         std::string type;
         std::string name_pattern;
         std::string scope_pattern;
      };

      bool parse_find_args(const QString& arg_str, CppFindArgs& out);

      class CppFindEngine {
      public:
         static QString execute(const CppFindArgs& args, const QString& baseFolder);
      };

   } // namespace cppfind
} // namespace tlex