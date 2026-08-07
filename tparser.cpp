#include "tparser.h"

using namespace tlex;

void TParser::assign(const TChar* src, int length)
{
     //m_textStorage.assign(src, length);
     m_lexem.assign(src, length);
     m_currentIdx = 0;
}
void TParser::assign(const char* src, TTextStorage::TEncoding enc)
{
     m_textStorage.assign(src, enc);
     m_lexem.assign(m_textStorage.c_str(), static_cast<int>(m_textStorage.length()) );
     m_currentIdx = 0;
}

void TParser::assign(const char* src)
{
     m_textStorage.assign(src, TTextStorage::TEncoding::ASCII);
     assign(m_textStorage.c_str(), static_cast<int>(m_textStorage.length()));
     m_currentIdx = 0;
}

const TLex& TParser::get_word()
{
     const TLex& lx = cur();
     if (lx.is_word())
     {
          ++m_currentIdx;
          return lx;
     }
     return fail;
}
const TLex& TParser::get_this_word(const char* word)
{
     const TLex& lx = cur();
     if (lx.is_this_word(word))
     {
          ++m_currentIdx;
          return lx;
     }
     return fail;
}
const TLex& TParser::get_this_word(const TStr& word)
{
     const TLex& lx = cur();
     if (lx.is_this_word(word))
     {
          ++m_currentIdx;
          return lx;
     }
     return fail;
}
const TLex& TParser::get_this_word(const std::string& word)
{
     const TLex& lx = cur();
     if (lx.is_this_word(word))
     {
          ++m_currentIdx;
          return lx;
     }
     return fail;
}
const TLex& TParser::get_sym()
{
     const TLex& lx = cur();
     if (lx.is_sym())
     {
          ++m_currentIdx;
          return lx;
     }
     return fail;
}
const TLex& TParser::get_this_sym(int code)
{
     const TLex& lx = cur();
     if (lx.is_this_sym(code))
     {
          ++m_currentIdx;
          return lx;
     }
     return fail;
}
const TLex& TParser::get_this_sym(const char* sym)
{
     const TLex& lx = cur();
     if (lx.is_this_sym(sym))
     {
          ++m_currentIdx;
          return lx;
     }
     return fail;
}
const TLex& TParser::get_this_sym(const std::string& sym)
{
     const TLex& lx = cur();
     if (lx.is_this_sym(sym))
     {
          ++m_currentIdx;
          return lx;
     }
     return fail;
}
const TLex& TParser::get_this_sym(const TStr& sym)
{
     const TLex& lx = cur();
     if (lx.is_this_sym(sym))
     {
          ++m_currentIdx;
          return lx;
     }
     return fail;
}
const TLex& TParser::get_str()
{
     const TLex& lx = cur();
     if (lx.is_str())
     {
          ++m_currentIdx;
          return lx;
     }
     return fail;
}
const TLex& TParser::get_float()
{
     const TLex& lx = cur();
     if (lx.is_float())
     {
          ++m_currentIdx;
          return lx;
     }
     return fail;
}
const TLex& TParser::get_int()
{
     const TLex& lx = cur();
     if (lx.is_int())
     {
          ++m_currentIdx;
          return lx;
     }
     return fail;
}
TLex TParser::get_number()
{
     int sign = 1;
     if (!get_this_sym("+"))
          if (get_this_sym("-"))
               sign = -1;

     if (TLex lx = get_int())
     {
          lx.v_i *= sign;
          lx.v_f = lx.v_i;
          return lx;
     }
     else
          if (TLex lx = get_float())
          {
               lx.v_f *= sign;
               lx.v_i = static_cast<int>(lx.v_f);
               return lx;
          }
     return fail;
}
const TLex& TParser::get_eof()
{
     const TLex& lx = cur();
     if (lx.is_eof())
          return lx;
     return fail;
}
const TLex& TParser::expected_word()
{
     const TLex& lx = get_word();
     if (!lx)
          m_lexem.m_error->error(cur(), parse_EXPECTED, "Expected word ");
     return lx;
}
const TLex& TParser::expected_this_word(const char* word)
{
     const TLex& lx = get_this_word(word);
     if (!lx)
          m_lexem.m_error->error(cur(), parse_EXPECTED, "Expected word '", word, "'");
     return lx;
}
const TLex& TParser::expected_this_word(const TStr& word)
{
     const TLex& lx = get_this_word(word);
     if (!lx)
          m_lexem.m_error->error(cur(), parse_EXPECTED, "Expected word '", word, "'");
     return lx;
}
const TLex& TParser::expected_this_word(const std::string& word)
{
     const TLex& lx = get_this_word(word);
     if (!lx)
          m_lexem.m_error->error(cur(), parse_EXPECTED, "Expected word '", word, "'");
     return lx;
}
const TLex& TParser::expected_sym()
{
     const TLex& lx = get_sym();
     if (!lx)
          m_lexem.m_error->error(cur(), parse_EXPECTED, "Expected symbol");
     return lx;
}
const TLex& TParser::expected_this_sym(int code)
{
     const TLex& lx = get_this_sym(code);
     if (!lx)
          m_lexem.m_error->error(cur(), parse_EXPECTED, "Expected symbol '", m_lexem.get_sym_str(code), "' code:", code);
     return lx;
}
const TLex& TParser::expected_this_sym(const char* sym)
{
     const TLex& lx = get_this_sym(sym);
     if (!lx)
          m_lexem.m_error->error(cur(), parse_EXPECTED, "Expected symbol '", sym, "' code:", m_lexem.get_sym_code(sym));
     return lx;
}
const TLex& TParser::expected_this_sym(const TStr& sym)
{
     const TLex& lx = get_this_sym(sym);
     if (!lx)
          m_lexem.m_error->error(cur(), parse_EXPECTED, "Expected symbol '", sym, "'");
     return lx;
}
const TLex& TParser::expected_this_sym(const std::string& sym)
{
     const TLex& lx = get_this_sym(sym);
     if (!lx)
          m_lexem.m_error->error(cur(), parse_EXPECTED, "Expected symbol '", sym, "'");
     return lx;
}
const TLex& TParser::expected_str()
{
     const TLex& lx = get_str();
     if (!lx)
          m_lexem.m_error->error(cur(), parse_EXPECTED, "Expected string");
     return lx;
}
const TLex& TParser::expected_float()
{
     const TLex& lx = get_float();
     if (!lx)
          m_lexem.m_error->error(cur(), parse_EXPECTED, "Expected float");
     return lx;
}
const TLex& TParser::expected_int()
{
     const TLex& lx = get_int();
     if (!lx)
          m_lexem.m_error->error(cur(), parse_EXPECTED, "Expected int");
     return lx;
}

TLex  TParser::expected_number()
{
     TLex lx = get_number();
     if (!lx)
          m_lexem.m_error->error(cur(), parse_EXPECTED, "Expected number");
     return lx;
}

const TLex& TParser::expected_eof()
{
     const TLex& lx = get_eof();
     if (!lx)
          m_lexem.m_error->error(cur(), parse_EXPECTED, "Expected eof");
     return lx;
}

void TParser::trace_items(TMatchContext* start) const
{
     using namespace tlex;
     TPrinter print;

     for (const TMatchContext* i = start; i; i = i->nextCtx)
     {
          switch (i->w.dtype)
          {
          case lit_NONE:             
               print("?lit_NONE? "); break;

          case lit_WORD_stdstr:      print("WORD_stdstr      <- ", *i->w.dest.m_stdstr); break;
          case lit_WORD_lex:         print("WORD_lex         <- ", *i->w.dest.m_lex);    break;
          case lit_WORD_tstr:        print("WORD_tstr        <- ", *i->w.dest.m_tstr);   break;
          case lit_THISWORD_cchar:   print("THISWORD_cchar   <- ", i->w.dest.m_constcharptr); break;
          case lit_THISWORD_clex:    print("THISWORD_clex    <- ", *i->w.dest.m_clex);       break;
          case lit_THISWORD_cstdstr: print("THISWORD_cstdstr <- ", *i->w.dest.m_cstdstr);    break;
          case lit_THISWORD_ctstr:   print("THISWORD_ctstr   <- ", *i->w.dest.m_ctstr);      break;

          case lit_SYM_int:          print("SYM_int          <- ", *i->w.dest.m_int);   break;
          case lit_SYM_lex:          print("SYM_lex          <- ", *i->w.dest.m_lex);   break;
          case lit_SYM_tstr:         print("SYM_tstr         <- ", *i->w.dest.m_tstr);  break;
          case lit_SYM_stdstr:       print("SYM_stdstr       <- ", *i->w.dest.m_stdstr); break;
          case lit_THISSYM_clex:     print("THISSYM_clex     <- ", *i->w.dest.m_clex);  break;
          case lit_THISSYM_cchar:    print("THISSYM_cchar    <- ", i->w.dest.m_constcharptr); break;
          case lit_THISSYM_int:      print("THISSYM_int      <- ", i->w.dest.m_code);   break;
          case lit_THISSYM_cstdstr:  print("THISSYM_cstdstr  <- ", *i->w.dest.m_cstdstr); break;
          case lit_THISSYM_ctstr:    print("THISSYM_ctstr    <- ", *i->w.dest.m_ctstr);  break;

          case lit_INT_int:          print("INT_int          <- ", *i->w.dest.m_int);   break;
          case lit_INT_lex:          print("INT_lex          <- ", *i->w.dest.m_lex);   break;
          case lit_INT_stdstr:       print("INT_stdstr       <- ", *i->w.dest.m_stdstr); break;
          case lit_INT_tstr:         print("INT_tstr         <- ", *i->w.dest.m_tstr);  break;

          case lit_FLOAT_float:      print("FLOAT_float      <- ", *i->w.dest.m_float);  break;
          case lit_FLOAT_double:     print("FLOAT_double     <- ", *i->w.dest.m_double); break;
          case lit_FLOAT_lex:        print("FLOAT_lex        <- ", *i->w.dest.m_lex);    break;
          case lit_FLOAT_tstr:       print("FLOAT_tstr       <- ", *i->w.dest.m_tstr);   break;

          case lit_STR_lex:          print("STR_lex          <- ", *i->w.dest.m_lex);    break;
          case lit_STR_stdstr:       print("STR_stdstr       <- ", *i->w.dest.m_stdstr); break;
          case lit_STR_tstr:         print("STR_tstr         <- ", *i->w.dest.m_tstr);   break;

          case lit_EOF:              print("EOF              <- "); break;
          case lit_EOF_lex:          print("EOF_lex          <- ", *i->w.dest.m_lex);    break;

          case lit_ANY:              print("ANY              <- ");                      break;
          case lit_ANY_lex:          print("ANY_lex          <- ", *i->w.dest.m_lex);    break;
          case lit_ANY_stdstr:       print("ANY_stdstr       <- ", *i->w.dest.m_stdstr); break;
          case lit_ANY_tstr:         print("ANY_tstr         <- ", *i->w.dest.m_tstr);   break;
          case lit_OPT:              print("OPT              <- "); break;
          case lit_EXPECTED:         print("EXPECTED         <- "); break;
          case lit_END:              print("END              <- "); break;
          case lit_ERROR_MSG:        print("ERROR_MSG        <- ", i->w.dest.m_errInfo->msg, "[", i->w.dest.m_errInfo->code,"]"); break;

          case lit_CASE:             print("CASE             <- "); break;
          case lit_ACT_class:        print("ACT class "); break;
          case lit_EXTERNPARSE:      print("EXTERNPARSE[", i->w.dest.m_externParse->show(i->w), "] "); break;
          // ── Внешний строковый тип ─────────────────────────────────────────
          case lit_WORD_qstr:        print("WORD_qstr        <- (QString)"); break;
          case lit_STR_qstr:         print("STR_qstr         <- (QString)"); break;
          case lit_SYM_qstr:         print("SYM_qstr         <- (QString)"); break;
          case lit_INT_qstr:         print("INT_qstr         <- (QString)"); break;
          case lit_FLOAT_qstr:       print("FLOAT_qstr       <- (QString)"); break;
          case lit_ANY_qstr:         print("ANY_qstr         <- (QString)"); break;
          case lit_THISWORD_cqstr:   print("THISWORD_cqstr   <- (const QString)"); break;
          case lit_THISSYM_cqstr:    print("THISSYM_cqstr    <- (const QString)"); break;
          }

          const TLex& lex = m_stream[i->thisIdx];
          print("  {", lex, "}\n");
     }

     printf("%s", print.data.c_str());
}


void show_list(const TItemList& list, TPrinter& print, int depth)
{
     using namespace tlex;
     auto sp = [](TPrinter& print, int c) { for (int i = 0; i < c; ++i) print(" "); };

     sp(print, depth);

     for (auto i : list)
     {
          switch (i.dtype)
          {
          case lit_NONE:             print("NONE "); break;
          case lit_WORD_stdstr:      print("WORD_stdstr[", *i.dest.m_stdstr, "] "); break;
          case lit_WORD_lex:         print("WORD_lex[", *i.dest.m_lex, "] "); break;
          case lit_WORD_tstr:        print("WORD_tstr[", *i.dest.m_tstr, "] "); break;
          case lit_THISWORD_cchar:   print("THISWORD_cchar[", i.dest.m_constcharptr, "] "); break;
          case lit_THISWORD_clex:    print("THISWORD_clex[", *i.dest.m_clex, "] "); break;
          case lit_THISWORD_cstdstr: print("THISWORD_cstdstr[", *i.dest.m_cstdstr, "] "); break;
          case lit_THISWORD_ctstr:   print("THISWORD_ctstr[", *i.dest.m_ctstr, "] "); break;

          case lit_SYM_int:          print("SYM_int[", *i.dest.m_int, "] "); break;
          case lit_SYM_lex:          print("SYM_lex[", *i.dest.m_lex, "] "); break;
          case lit_SYM_tstr:         print("SYM_tstr[", *i.dest.m_tstr, "] "); break;
          case lit_SYM_stdstr:       print("SYM_stdstr[", *i.dest.m_stdstr, "] "); break;
          case lit_THISSYM_clex:     print("THISSYM_clex[", *i.dest.m_clex, "] "); break;
          case lit_THISSYM_cchar:    print("THISSYM_cchar[", i.dest.m_constcharptr, "] "); break;
          case lit_THISSYM_int:      print("THISSYM_int[", i.dest.m_code, "(", (char)(i.dest.m_code), ")" "] "); break;
          case lit_THISSYM_cstdstr:  print("THISSYM_cstdstr[", *i.dest.m_cstdstr, "] "); break;
          case lit_THISSYM_ctstr:    print("THISSYM_ctstr[", *i.dest.m_ctstr, "] "); break;

          case lit_INT_int:          print("INT_int[", *i.dest.m_int, "] "); break;
          case lit_INT_lex:          print("INT_lex[", *i.dest.m_lex, "] "); break;
          case lit_INT_stdstr:       print("INT_stdstr[", *i.dest.m_stdstr, "] "); break;
          case lit_INT_tstr:         print("INT_tstr[", *i.dest.m_tstr, "] "); break;

          case lit_FLOAT_float:      print("FLOAT_float[", *i.dest.m_float, "] "); break;
          case lit_FLOAT_double:     print("FLOAT_double[", *i.dest.m_double, "] "); break;
          case lit_FLOAT_lex:        print("FLOAT_lex[", *i.dest.m_lex, "] "); break;
          case lit_FLOAT_tstr:       print("FLOAT_tstr[", *i.dest.m_tstr, "] "); break;

          case lit_STR_lex:          print("STR_lex[", *i.dest.m_lex, "] "); break;
          case lit_STR_stdstr:       print("STR_stdstr[", *i.dest.m_stdstr, "] "); break;
          case lit_STR_tstr:         print("STR_tstr[", *i.dest.m_tstr, "] "); break;

          case lit_EOF:              print("EOF "); break;
          case lit_EOF_lex:          print("EOF_lex[", *i.dest.m_lex, "] "); break;

          case lit_ANY:              print("ANY "); break;
          case lit_ANY_stdstr:       print("ANY_stdstr[ ", *i.dest.m_stdstr, "] "); break;
          case lit_ANY_lex:          print("ANY_lex[ ", *i.dest.m_lex, "] "); break;
          case lit_ANY_tstr:         print("ANY_tstr[ ", *i.dest.m_tstr, "] "); break;
          case lit_OPT:              print("OPT ");
               if (i.dest.m_itemList)
               {
                    print("{ ");
                    show_list(*i.dest.m_itemList, print, depth);
                    print("} ");
               }
               break;
          case lit_EXPECTED:         print("EXPECTED ");
               print("{ ");
               show_list(*i.dest.m_itemList, print, depth);
               print("} ");
               break;
          case lit_END:              print("END ");
               if (i.dest.m_int)
                    print("(int)");
               break;
          case lit_ERROR_MSG:        print("ERROR_MSG[", i.dest.m_errInfo->msg, ":", i.dest.m_errInfo->code, "] "); break;

          case lit_CASE:             sp(print, depth);  print("CASE ");
               sp(print, depth); print("{\n");
               for (auto j : *i.dest.m_caseList)
               {
                    sp(print, depth); print(" { ");
                    show_list(j, print, depth + 1);
                    sp(print, depth);  print(" }\n");
               }
               print("} ");
               break;
          case lit_ACT_class:        print("ACT_class "); break;
          case lit_EXTERNPARSE:      print("EXTERNPARSE[", i.dest.m_externParse->show(i), "] "); break;
          // ── Внешний строковый тип ─────────────────────────────────────────
          case lit_WORD_qstr:        print("WORD_qstr[(QString)] ");      break;
          case lit_STR_qstr:         print("STR_qstr[(QString)] ");       break;
          case lit_SYM_qstr:         print("SYM_qstr[(QString)] ");       break;
          case lit_INT_qstr:         print("INT_qstr[(QString)] ");       break;
          case lit_FLOAT_qstr:       print("FLOAT_qstr[(QString)] ");     break;
          case lit_ANY_qstr:         print("ANY_qstr[(QString)] ");       break;
          case lit_THISWORD_cqstr:   print("THISWORD_cqstr[(QString)] "); break;
          case lit_THISSYM_cqstr:    print("THISSYM_cqstr[(QString)] ");  break;
          default: print("Unknown type ");
          }
     }
}

void show_list(const TItemList& list)
{
     TPrinter print;
     print("----------\n");
     show_list(list, print, 0);
     printf("%s\n\n", print.data.c_str());
}

/*****************************************************************************************************/

void TParser::end_of_init()
{
     m_lexem.end_of_init();
     m_stream.clear();
     TLex lex;
     do
     {
          lex = m_lexem.cur();
          m_stream.push_back(lex);
          m_lexem.next_lex();
     } while (lex.type != lex_EOF);
     m_streamSize = static_cast<int>(m_stream.size());
     m_currentIdx = 0;
}

void TParser::assign_item_data(LexItem& dest, int cidx) const
{
     using namespace tlex;

     switch (dest.dtype)
     {
     case lit_EXTERNPARSE:   dest.dest.m_externParse->assign_item_data(*this, dest, cidx); break;
     default:
          if (const TLex& sour = lexem(cidx))
               assign_item_data(dest, sour);
     }

}

void TParser::assign_item_data(LexItem& dest, const TLex& sour) const
{
     using namespace tlex;

     switch (dest.dtype)
     {
     case lit_NONE: break;

     case lit_WORD_stdstr:   dest.dest.m_stdstr->clear();
                             sour.v_s.to_std_string(*dest.dest.m_stdstr);  break;
     case lit_WORD_lex:      *dest.dest.m_lex  = sour;                     break;
     case lit_WORD_tstr:     *dest.dest.m_tstr = sour.v_s;                 break;

     case lit_THISWORD_cchar:
     case lit_THISWORD_clex:
     case lit_THISWORD_cstdstr:
     case lit_THISWORD_ctstr:
          // thisword - значения используются только для чтения и проверки
          break;

     case lit_SYM_int:       *dest.dest.m_int    = sour.v_i;               break;
     case lit_SYM_lex:       *dest.dest.m_lex    = sour;                   break;
     case lit_SYM_tstr:      *dest.dest.m_tstr   = sour.v_s;               break;
     case lit_SYM_stdstr:    dest.dest.m_stdstr->clear();
                             sour.v_s.to_std_string(*dest.dest.m_stdstr);  break;

     case lit_THISSYM_clex:
     case lit_THISSYM_cchar:
     case lit_THISSYM_int:
     case lit_THISSYM_cstdstr:
     case lit_THISSYM_ctstr:
          // thissym - значения используются только для чтения и проверки
          break;

     case lit_INT_int:       *dest.dest.m_int    = sour.v_i;               break;
     case lit_INT_lex:       *dest.dest.m_lex    = sour;                   break;
     case lit_INT_stdstr:    dest.dest.m_stdstr->clear();
                             sour.v_s.to_std_string(*dest.dest.m_stdstr);  break;
     case lit_INT_tstr:      *dest.dest.m_tstr   = sour.v_s;               break;

     case lit_FLOAT_float:   *dest.dest.m_float  = static_cast<float>(sour.v_f);break;
     case lit_FLOAT_double:  *dest.dest.m_double = sour.v_f;               break;
     case lit_FLOAT_lex:     *dest.dest.m_lex    = sour;                   break;
     case lit_FLOAT_tstr:    *dest.dest.m_tstr   = sour.v_s;               break;

     case lit_STR_lex:       *dest.dest.m_lex    = sour;                   break;
     case lit_STR_stdstr:    dest.dest.m_stdstr->clear();
                             sour.v_s.to_std_string(*dest.dest.m_stdstr);  break;
     case lit_STR_tstr:      *dest.dest.m_tstr   = sour.v_s;               break;

     case lit_EOF:           break;
     case lit_EOF_lex:       *dest.dest.m_lex    = sour;                   break;

     case lit_ANY:           break;
     case lit_ANY_stdstr:    dest.dest.m_stdstr->clear(); 
                             sour.v_s.to_std_string(*dest.dest.m_stdstr);
                             break;
     case lit_ANY_tstr:      *dest.dest.m_tstr   = sour.v_s;               break;
     case lit_ANY_lex:       *dest.dest.m_lex = sour;                      break;

     // ── Внешний строковый тип (QString) ──────────────────────────────────────
     // Запись делегируется в IExtString::store → TExtString<Q>::store → Q::fromUcs4 / Q::number
     case lit_WORD_qstr:
     case lit_STR_qstr:
     case lit_SYM_qstr:
     case lit_ANY_qstr:      dest.m_ext->store(dest.dest.m_qstr, sour.v_s); break;
     case lit_INT_qstr:      dest.m_ext->store(dest.dest.m_qstr, sour.v_i); break;
     case lit_FLOAT_qstr:    dest.m_ext->store(dest.dest.m_qstr, sour.v_f); break;
     case lit_THISWORD_cqstr:
     case lit_THISSYM_cqstr: break;  // read-only: только сравнение, не запись
                            
     case lit_END:           if (dest.dest.m_int) 
                                *dest.dest.m_int = sour.type == lex_EOF ? 1 : 2; 
                             break;
     case lit_CASE:          break; // служебный итем, ничего не возвращает
     case lit_OPT:           break; // служебный итем (фрейм конструкции в цепочке)
     case lit_EXPECTED:      break; // служебный итем (фрейм конструкции в цепочке)
     case lit_ACT_class:     dest.dest.m_actClass->call(dest);              break;
     case lit_EXTERNPARSE:   break; // обрабатывается на уровне индексов
     }
}

void TParser::on_end_of_parce(TMatchContext& context)
{
     using namespace tlex;
     TMatchContext* start = nullptr;

     for (TMatchContext* i = &context; i; i = i->prevCtx)
          if (i->prevCtx)
               i->prevCtx->nextCtx = i;
          else start = i;

     if (ENABLE_TRACE)
          trace_items(start);

     for (const TMatchContext* i = start; i; i = i->nextCtx)
          assign_item_data(const_cast<LexItem&>(i->w), i->thisIdx);
}

TIdx TParser::match_item(const LexItem& w, int idx) const
{
     using namespace tlex;
     const TLex& lex = m_stream[idx];

     switch (w.dtype)
     {
     case lit_NONE:   
          break;

     case lit_WORD_stdstr:      if (lex.is_word())                           return TIdx(idx + 1);  break;
     case lit_WORD_lex:         if (lex.is_word())                           return TIdx(idx + 1);  break;
     case lit_WORD_tstr:        if (lex.is_word())                           return TIdx(idx + 1);  break;
     case lit_THISWORD_cchar:   if (lex.is_this_word(w.dest.m_constcharptr)) return TIdx(idx + 1);  break;
     case lit_THISWORD_clex:    if (lex.is_this_word(w.dest.m_clex->v_s))    return TIdx(idx + 1);  break;
     case lit_THISWORD_cstdstr: if (lex.is_this_word(*w.dest.m_cstdstr))     return TIdx(idx + 1);  break;
     case lit_THISWORD_ctstr:   if (lex.is_this_word(*w.dest.m_ctstr))       return TIdx(idx + 1);  break;
     case lit_SYM_int:          
     case lit_SYM_lex:
     case lit_SYM_tstr:
     case lit_SYM_stdstr:       if (lex.is_sym())                            return TIdx(idx + 1);  break;
     case lit_THISSYM_clex:     if (lex.is_this_sym(w.dest.m_clex->v_i))     return TIdx(idx + 1);  break;
     case lit_THISSYM_cchar:    if (lex.is_this_sym(w.dest.m_constcharptr))  return TIdx(idx + 1);  break;
     case lit_THISSYM_int:      if (lex.is_this_sym(w.dest.m_code))          return TIdx(idx + 1);  break;
     case lit_THISSYM_cstdstr:  if (lex.is_this_sym(*w.dest.m_cstdstr))      return TIdx(idx + 1);  break;
     case lit_THISSYM_ctstr:    if (lex.is_this_sym(*w.dest.m_ctstr))        return TIdx(idx + 1);  break;

     case lit_INT_int:          
     case lit_INT_lex:
     case lit_INT_stdstr:
     case lit_INT_tstr:         if (lex.is_int())                             return TIdx(idx + 1);  break;
     case lit_FLOAT_float:
     case lit_FLOAT_double:
     case lit_FLOAT_lex:
     case lit_FLOAT_tstr:       if (lex.is_float())                           return TIdx(idx + 1);  break;

     case lit_STR_lex:
     case lit_STR_stdstr:
     case lit_STR_tstr:         if (lex.is_str())                             return TIdx(idx + 1);  break;

     case lit_EOF:              
     case lit_EOF_lex:          if (lex.is_eof())                             return TIdx(idx + 1);  break;

     case lit_ANY:              
     case lit_ANY_stdstr:       
     case lit_ANY_tstr:
     case lit_ANY_lex:
                                if (!lex.is_eof())                            return TIdx(idx + 1);  break;

     // ── Внешний строковый тип (QString) ──────────────────────────────────────
     case lit_WORD_qstr:        if (lex.is_word())  return TIdx(idx + 1);  break;
     case lit_STR_qstr:         if (lex.is_str())   return TIdx(idx + 1);  break;
     case lit_SYM_qstr:         if (lex.is_sym())   return TIdx(idx + 1);  break;
     case lit_INT_qstr:         if (lex.is_int())   return TIdx(idx + 1);  break;
     case lit_FLOAT_qstr:       if (lex.is_float()) return TIdx(idx + 1);  break;
     case lit_ANY_qstr:         if (!lex.is_eof())  return TIdx(idx + 1);  break;
     // Сравнение: интерфейс вызывает TQBridge::match - UTF-16 decode без аллокации.
     case lit_THISWORD_cqstr:
          if (lex.is_word() && w.m_ext && w.m_ext->match(w.dest.m_cqstr, lex.v_s))
               return TIdx(idx + 1);
          break;
     case lit_THISSYM_cqstr:
          if (lex.is_sym()  && w.m_ext && w.m_ext->match(w.dest.m_cqstr, lex.v_s))
               return TIdx(idx + 1);
          break;

     case lit_CASE:
     case lit_ACT_class:
                                return TIdx(idx);

     case lit_EXTERNPARSE:      return w.dest.m_externParse->match_item(*this, w, idx);
     default:
          throw int(-1); // internal error
          return TIdx::Fail();
     }

     return TIdx::Fail();
}

TState TParser::match( TItemList::iterator list, const TItemList::iterator& end
                                     , TMatchContext& context, int depth)
{
     using namespace tlex;

     if (list == end)
     {
          // Конец под-списка конструкции (Case/Opt/Expected): переход по континуации
          // фрейма - продолжаем родительский список, не финализируя разбор.
          // (в терминах Prolog: goto_if_true последней инструкции блока)
          if (TMatchContext* f = context.frame)
          {
               context.frame  = f->frame;   // возврат на уровень родителя
               context.strict = f->strict;
               return match(f->nextUp, f->nextUpEnd, context, depth - 1);
          }

          // Конец шаблона верхнего уровня: финализация и присваивание данных.
          context.w.dtype = lit_EOF;
          on_end_of_parce(context);
          return context ? TState(context.thisIdx) : TState();
     }

     switch (list->dtype)
     {
     case lit_CASE:
          {
              // Контекст конструкции становится фреймом: хранит континуацию.
              context.w         = *list;
              context.nextUp    = list;
              ++context.nextUp;            // первый итем за Case
              context.nextUpEnd = end;
              for( auto &i : *(list->dest.m_caseList))
              {
                   TMatchContext subCtx;
                   subCtx.prevCtx = &context;
                   subCtx.thisIdx = context.thisIdx;
                   subCtx.ok      = true;
                   subCtx.frame   = &context;
                   subCtx.strict  = false;  // альтернативам разрешено отказывать
                   // Успех включает и родительский хвост (через континуацию);
                   // отказ хвоста откатывается сюда - пробуем следующую альтернативу.
                   if (auto res = match(i.begin(), i.end(), subCtx, depth+1))
                        return res;
              }
              if (context.strict) // Case внутри Expected: ни одна альтернатива не подошла
                   m_lexem.m_error->error(m_stream[context.thisIdx], parse_EXPECTED, "Unexpected lexem");
              return Fail();
          }
          break;

     case lit_OPT:
          {
          context.w         = *list;
          context.nextUp    = list;
          ++context.nextUp;
          context.nextUpEnd = end;
          if (list->dest.m_itemList) // Opt() без списка - пустой, сразу мимо
          {
               TMatchContext subCtx;
               subCtx.prevCtx = &context;
               subCtx.thisIdx = context.thisIdx;
               subCtx.ok      = true;
               subCtx.frame   = &context;
               subCtx.strict  = false;    // необязательному блоку разрешено отказывать
               if (auto res = match(list->dest.m_itemList->begin(), list->dest.m_itemList->end(), subCtx, depth + 1))
                    return res;
          }
          // Блок не совпал (или отказал хвост после совпавшего блока):
          // продолжаем без потребления лексем.
          {
               TMatchContext nextCtx;
               nextCtx.prevCtx = &context;
               nextCtx.thisIdx = context.thisIdx;
               nextCtx.ok      = true;
               nextCtx.frame   = context.frame;
               nextCtx.strict  = context.strict;
               return match(context.nextUp, end, nextCtx, depth);
          }
          }
          break;

     case lit_EXPECTED:
          {
               // Cut: под-список матчируется строго - несовпадение любого итема
               // порождает ошибку (strict), отката назад за Expected не существует.
               context.w         = *list;
               context.nextUp    = list;
               ++context.nextUp;
               context.nextUpEnd = end;

               TMatchContext subCtx;
               subCtx.prevCtx = &context;
               subCtx.thisIdx = context.thisIdx;
               subCtx.ok      = true;
               subCtx.frame   = &context;
               subCtx.strict  = true;
               if (auto res = match(list->dest.m_itemList->begin(), list->dest.m_itemList->end(), subCtx, depth + 1))
                    return res;
               return Fail(); // достижимо, только если обработчик ошибок не бросает исключение
          }
          break;

     case lit_END:
          {
          context.w = *list;

          TMatchContext nextCtx;
          nextCtx.prevCtx = &context;
          nextCtx.thisIdx = context.thisIdx;
          nextCtx.ok = true;
          on_end_of_parce(context);
          }
          return TState(context.thisIdx);

     case lit_ERROR_MSG:
          m_lexem.m_error->error(cur(), list->dest.m_errInfo->code, list->dest.m_errInfo->msg);
          return Fail();

     case lit_EOF:
     case lit_EOF_lex:
          on_end_of_parce(context);
          return context ? TState(context.thisIdx) : TState();

     case lit_ACT_class: // проверить, кажется, это неверно
     default:
          if (TIdx nidx = TParser::match_item(*list, context.thisIdx))
          {
               context.w = *list;

               TMatchContext nextCtx;
               nextCtx.prevCtx = &context;
               nextCtx.thisIdx = nidx.m_lexIdx;
               nextCtx.ok      = true;
               nextCtx.frame   = context.frame;   // наследуем уровень вложенности
               nextCtx.strict  = context.strict;
               return match(++list, end, nextCtx, depth);
          }
          if (context.strict) // внутри Expected: несовпадение = ошибка (cut)
               m_lexem.m_error->error(m_stream[context.thisIdx], parse_EXPECTED, "Unexpected lexem");
     }
     
     return TState();
}


TState TParser::parse(TState startIdx, const TItemList& list)
{
     if(startIdx)
     if (list.begin() != list.end())
     {
          if (ENABLE_SHOW_LIST)
          {
               printf("---------------------\n"); 
               show_list(list); 
               printf("---------------------\n");
          }
          TMatchContext ctx;
          ctx.ok = true;
          ctx.thisIdx = startIdx.lexIdx;
          ctx.prevCtx = nullptr;
          ctx.nextCtx = nullptr;
          ctx.nextUp = list.end();
          ctx.nextUpEnd = list.end();
          ctx.frame  = nullptr;   // верхний уровень: континуации нет
          ctx.strict = false;
          ctx.w = *list.begin();

          return match(list.begin(), list.end(), ctx, 0);
     }
     return TState();
}

TState TParser::parse(const TItemList& list)
{
    if (list.begin() != list.end())
         if (TState res = parse(get_state(), list))
         {
              restore_state(res);
              return res;
         }

     return TState();
}

#if 0
#include "mock_qstring.h"   // определяет class QString - до всех Qt-шаблонов
#include <cstdio>
#include <iostream>
//================================================================================
//================================================================================
#define CHECK(cond, msg)                                        \
    do {                                                        \
        ++g_total;                                              \
        if (cond) {                                             \
            ++g_passed;                                         \
        } else {                                                \
            ++g_failed;                                         \
            printf("  FAIL [%s:%d]  %s\n", __FILE__, __LINE__, msg); \
        }                                                       \
    } while(0)

#define SECTION(name)  printf("\n--- %s ---\n", name)

// ============================================================
//  Вспомогательные функции
// ============================================================

// Хранит TTextStorage (владелец сырых данных) вместе с TLexem.
// TStr::str указывает в m_text32 - TTextStorage должен жить дольше TLex.
struct TLexSession
{
     TTextStorage storage;
     TLexem       lex;
     bool         inited = false;

     TLexSession(bool cstyle = false)
     {
          if (cstyle) lex.set_C_style(true);
     }

     void feed(const char* src)
     {
          storage.assign(src, TTextStorage::TEncoding::ASCII);
          lex.assign(storage.c_str(), storage.length());
          if (!inited) { lex.end_of_init(); inited = true; }
          else { lex.next_lex(); }
     }

     void feed_utf8(const char* src)
     {
          storage.assign_from_utf8(src);
          lex.assign(storage.c_str(), storage.length());
          if (!inited) { lex.end_of_init(); inited = true; }
          else { lex.next_lex(); }
     }

     TLex cur() { return lex.cur(); }

     std::vector<TLex> all_tokens()
     {
          std::vector<TLex> result;
          while (!lex.is_eof())
          {
               result.push_back(lex.cur());
               lex.next_lex();
          }
          return result;
     }
};

// Строковое содержимое токена (UTF-8 копия)
namespace {
std::string str_of(const TLex& lx)
{
     return lx.v_s.to_std_string();
}
} // namespace


void Debugbreak() 
{
     //__debugbreak();
}
void test_tstr()
{
     TParser pars;

     pars.assign(R"ptext(
             x float; x2 x3 < x4 some hello world!
)ptext");
     pars.end_of_init();

     using namespace tlex;
     std::string varName;
     TLex val;
     TStr sx2, sx3, sx4;

     TStr some(U"some");



     if (!pars.parse({ varName, "float", ';', sx2, sx3, '<', sx4, ThisWord(some) }))
     {
          printf("Error test_tstr:0\n");
          Debugbreak();
     }

     if (varName != "x" || sx2 != "x2" || sx3 != "x3" || sx4 != "x4")
     {
          printf("Error test_tstr:1\n");
          Debugbreak();
     }

     TStr hello(U"hello");
     if( !pars.cur().is_this_word(hello) )
     {
          printf("Error test_tstr:2\n");
          Debugbreak();
     }
     pars.expected_word();
     TStr world(U"world");
     pars.expected_this_word(world);
}

void test_read_s()
{
     TParser pars;

     pars.assign(R"ptext(
             x float;
             z str;
)ptext");
     pars.end_of_init();

     using namespace tlex;
     std::string varName;
     TLex val;
     TState pres(0);

     struct TAct : public IParseAct
     {
          const char* msg = nullptr;
          TAct(const char* m) { msg = m; }

          virtual void call(const TLexItem&) const override
          {
               printf("%s ", msg);
          }
     };
     int doDrop = 0;

     while (
          pres = pars.parse(pres, { Case({
                { varName, "str",   ';', TAct("Ok, success str val")}
              , { varName, "float", ';', Act([&varName](const TLexItem& item) {printf("varName:%s\n", varName.c_str()); })}
              
              , {TAct("Other: err"), End(doDrop)}
                   })}))
     {
          if (doDrop)
               break;
          printf("!\n");
     }

}

void test_act()
{
     TParser pars;

     pars.assign(R"ptext(
             x float;
             z str;
)ptext");
     pars.end_of_init();

     using namespace tlex;
     std::string varName, v2;
     TLex val;
     TState pres(0);

     pres = pars.parse(pres, { varName, Act("Ok, success str val"), v2 });

}

namespace tlex
{
     struct Number : public IParse
     {
          mutable LexItem val;

          Number(double &v) : val(v) {}

          virtual TIdx match_item(const TParser& master, const LexItem& self, int cidx) const override
          {
               while(auto lex = master.lexem(cidx))
               {
                    if (lex.is_this_sym("+") || lex.is_this_sym("-"))
                         ++cidx;
                    else
                         break;
               }
               if(auto lex = master.lexem(cidx))
                    if (lex.is_int() || lex.is_float())
                         return  TIdx(cidx+1);

               return TIdx::Fail();
          }
          virtual void assign_item_data(const TParser& master, LexItem& dest, int cidx) const override
          {
               int sign = 1;

               while (auto lex = master.lexem(cidx))
               {
                    if (lex.is_this_sym("+"))
                    {
                    }
                    else
                         if (lex.is_this_sym("-"))
                              sign = -sign;
                         else break;
                    ++cidx;
               }
               if (auto lex = master.lexem(cidx))
               {
                    switch (val.dtype)
                    {
                    case lit_INT_int:
                    case lit_INT_lex:
                    case lit_INT_stdstr:
                    case lit_INT_tstr:
                         switch (lex.type)
                         {
                         case lex_INTEGER:   lex.v_i =                  lex.v_i * sign;  break;
                         case lex_FLOAT:     lex.v_i = static_cast<int>(lex.v_f * sign); break;  // add warning?
                         }
                         master.assign_item_data(val, lex);
                         break;
                    case lit_FLOAT_double:
                    case lit_FLOAT_float:
                    case lit_FLOAT_lex:
                    case lit_FLOAT_tstr:
                         switch (lex.type)
                         {
                         case lex_INTEGER:    lex.v_f = lex.v_i * sign;  break;
                         case lex_FLOAT:      lex.v_f = lex.v_f * sign;  break;
                         }
                         master.assign_item_data(val, lex);
                         break;
                    }
               }
          }
          virtual const char* show(const LexItem&) const { return "Number"; }
     };
};

void test_externparse()
{
     TParser pars;

     pars.assign(R"ptext(
             x = +3; hellow
             y = -5.6;
)ptext");
     pars.end_of_init();

     using namespace tlex;
     std::string varName, v2;
     TLex val;
     TState pres(0);
     double v_d;


     pres = pars.parse(pres, { varName, ThisSym("="), Number(v_d), ThisSym(";"), Word(val) });
     if (varName != "x" || v_d != 3)
     {
          printf("Error test_externparse\n");
     }

     pres = pars.parse(pres, { varName, ThisSym("="), Number(v_d), ThisSym(";") });
     const double diff = fabs(v_d + 5.6);
     if (varName != "y" || diff>1e-20)
     {
          printf("Error test_externparse\n");
     }

}


void test_read()
{
     TParser pars;

     pars.assign(R"ptext(
             var x : float = 3;
             var y : int = 4;
             var z : str = 5;
)ptext");
     pars.end_of_init();

     using namespace tlex;
     std::string varName;
     TLex val;
     TState pres(0);

     while (
          pres = pars.parse(pres, { Case({
                { ThisWord("var"), varName, ':', "str",   '=', Int(val), ';', Act("Ok, success str val")}
            //  , { "var", varName, ':', "int",   '=', Int(val), ';', Act("Ok, success int val")}
              , { "var", varName, ':', "float", '=', Int(val), ';', Act([&varName](const TLexItem& item) {printf("varName:%s\n", varName.c_str()); })}
               }) }))
     {
          printf("!");
     }

}

//#include "lextest.h"


void test_any_opt_end()
{
     TParser pars;
     pars.assign("hello = 42 ; world");
     pars.end_of_init();

     using namespace tlex;
     std::string w1, w2;
     TStr op;
     int num = 0;
     TState state(0);

     if (!pars.parse(state, { Word(w1), Opt({ '=', Int(num) }), ';' }))
     {
          printf("Error test_opt1\n");
     }

     if (!pars.parse(state, { Word(w1), Opt({ ThisWord("none"), '=', Int(num)}), '=', Int(num), ';'}))
     {
          printf("Error test_any0\n");
     }

     // Test Any without capture
     if (!pars.parse(state, { Any(w1), '=', Any(w2), ';' }))
     {
          printf("Error test_any1\n");
     }
     if (w1 != "hello" || w2 != "42")
     {
          printf("Error test_any2\n");
     }

     // Test Opt
     pars.restore_state(TState(0));

     if (!pars.parse({ Word(w1), Opt({ '=', Int(num) }), ';' }))
     {
          printf("Error test_opt1\n");
     }

     pars.restore_state(TState(0));
     pars.assign("x;");
     pars.end_of_init();
     if (!pars.parse({ Word(w1), Opt({ '=', Int(num) }), ';' }))
     {
          printf("Error test_opt2\n");
     }
     if (w1 != "x")
     {
          printf("Error test_opt3\n");
     }

     // Test End
     pars.restore_state(TState(0));
     pars.assign("first second third");
     pars.end_of_init();
     std::string f, s;
     if (!pars.parse({ Word(f), End(), Word(s) }))
     {
          printf("Error test_end1\n");
     }
     // s should not be assigned because parsing stops at End
     // f should be "first"
     if (f != "first")
     {
          printf("Error test_end2\n");
     }
     // next token is still "second"
     if (!pars.cur().is_this_word("second"))
     {
          printf("Error test_end3\n");
     }

     printf("test_any_opt_end passed\n");
}

void test_expected_error()
{
     TParser pars;
     pars.assign("x = ;");
     pars.end_of_init();


     using namespace tlex;
     std::string var;
     int val = 0;
     bool caught = false;
     try {
          pars.parse({ Word(var), Expected({ '=', ';'})});
     } catch (...) {
          caught = true;
     }
     if (caught)
     {
          printf("Error test_expected1\n");
     }

     caught = false;
     try {
          pars.parse(TState(0),{ Word(var), Expected({ '=', Int(val), '!'}), ';'});
     }
     catch (...) {
          caught = true;
     }
     if (!caught)
     {
          printf("Error test_expected2\n");
     }

     pars.restore_state(TState(0));
     pars.assign("y = 5");
     pars.end_of_init();
     caught = false;
     try {
          pars.parse({ Word(var), '=', Error("Missing semicolon") });
     } catch (...) {
          caught = true;
     }
     if (!caught)
     {
          printf("Error test_error1\n");
     }

     printf("test_expected_error passed\n");
}

// Продолжение разбора после Case/Expected/Opt (континуации-фреймы в match).
void test_construct_continuation()
{
     using namespace tlex;
     int errors = 0;

     // 1. Case + хвост: хвост матчируется, переменные заполняются
     {
          TParser p; p.assign("x = 5 ;"); p.end_of_init();
          std::string a; int v = 0;
          if (!p.parse({ Case({ { Word(a), '=' }, { Word(a), ':' } }), Int(v), ';' })
              || a != "x" || v != 5 || !p.is_eof())
          { printf("Error test_cont:1 Case+tail\n"); ++errors; }
     }
     // 2. Case: вторая альтернатива + хвост
     {
          TParser p; p.assign("x : 5 ;"); p.end_of_init();
          std::string a; int v = 0;
          if (!p.parse({ Case({ { Word(a), '=' }, { Word(a), ':' } }), Int(v), ';' }) || v != 5)
          { printf("Error test_cont:2 Case alt2\n"); ++errors; }
     }
     // 3. Case совпал, но хвост отказал: parse=false, переменные НЕ тронуты, позиция восстановлена
     {
          TParser p; p.assign("x = 5 !"); p.end_of_init();
          std::string a = "init"; int v = -1;
          if (p.parse({ Case({ { Word(a), '=' } }), Int(v), ';' })
              || v != -1 || a != "init" || p.m_currentIdx != 0)
          { printf("Error test_cont:3 no premature assign\n"); ++errors; }
     }
     // 4. Expected + хвост: префикс заполняется, хвост потребляется
     {
          TParser p; p.assign("x = 5 ;"); p.end_of_init();
          std::string a; int v = 0;
          if (!p.parse({ Word(a), Expected({ '=', Int(v) }), ';' })
              || a != "x" || v != 5 || !p.is_eof())
          { printf("Error test_cont:4 Expected+tail\n"); ++errors; }
     }
     // 5. Expected как cut внутри Case: несовпадение -> ошибка, вторая альтернатива не пробуется
     {
          TParser p; p.assign("a + 1"); p.end_of_init();
          std::string w; int v = 0; bool caught = false, second = false;
          try {
               p.parse({ Case({
                    { Word(w), Expected({'=', Int(v)}) },
                    { Word(w), '+', Int(v), Act([&](const TLexItem&){ second = true; }) }
               })});
          } catch(...) { caught = true; }
          if (!caught || second)
          { printf("Error test_cont:5 Expected cut in Case\n"); ++errors; }
     }
     // 6. Opt совпал, но хвост отказал: откат пробует путь без Opt; здесь оба пути падают
     {
          TParser p; p.assign("x = 5 !"); p.end_of_init();
          std::string a = "init"; int v = -1;
          if (p.parse({ Word(a), Opt({ '=', Int(v) }), ';' }) || v != -1 || a != "init")
          { printf("Error test_cont:6 Opt no premature assign\n"); ++errors; }
     }
     // 7. Откат через Opt: '=' потребляется хвостом, а не Opt
     {
          TParser p; p.assign("x = 5"); p.end_of_init();
          std::string a; int v = 0;
          if (!p.parse({ Word(a), Opt({ '=' }), '=', Int(v) }) || v != 5)
          { printf("Error test_cont:7 Opt backtrack\n"); ++errors; }
     }
     // 8. Вложенный Case + хвост
     {
          TParser p; p.assign("k1 v2 end"); p.end_of_init();
          std::string b;
          if (!p.parse({ Case({
                    { ThisWord("k1"), Case({ { ThisWord("v1") }, { ThisWord("v2") } }), Word(b) },
                    { ThisWord("k2") } }) })
              || b != "end")
          { printf("Error test_cont:8 nested Case\n"); ++errors; }
     }

     if (!errors)
          printf("test_construct_continuation passed\n");
}
void test_new_functions()
{
     // --- TLexem: get_this_word(std::string) ---
     {
          TLexem lex;
          lex.assign(U"hello world");
          lex.end_of_init();
          std::string kw = "hello";
          if (!lex.get_this_word(kw))
               printf("Error test_new:1  get_this_word(std::string)\n");
     }

     // --- TLexem: get_this_sym(std::string), get_this_sym(TStr) ---
     {
          TLexem lex;
          lex.assign(U"; ,");
          lex.end_of_init();
          std::string sc = ";";
          if (!lex.get_this_sym(sc))
               printf("Error test_new:2  get_this_sym(std::string)\n");
          TStr tc(U",");
          if (!lex.get_this_sym(tc))
               printf("Error test_new:3  get_this_sym(TStr)\n");
     }

     // --- TLexem: expected_this_sym(std::string) и expected_this_sym(TStr) ---
     {
          TLexem lex;
          lex.assign(U"= :");
          lex.end_of_init();

          std::string s1 = "=";
          try { lex.expected_this_sym(s1); }
          catch (...) { printf("Error test_new:4  expected_this_sym(std::string)\n"); }
          TStr s2(U":");
          try { lex.expected_this_sym(s2); }
          catch (...) { printf("Error test_new:5  expected_this_sym(TStr)\n"); }
     }

     // --- TParser: get_this_word(std::string) ---
     {
          TParser p;
          p.assign("alpha beta");
          p.end_of_init();

          std::string w = "alpha";
          if (!p.get_this_word(w))
               printf("Error test_new:6  TParser::get_this_word(std::string)\n");
     }

     // --- TParser: get_this_sym(std::string), get_this_sym(TStr) ---
     {
          TParser p;
          p.assign("< >");
          p.end_of_init();

          std::string s1 = "<";
          if (!p.get_this_sym(s1))
               printf("Error test_new:7  TParser::get_this_sym(std::string)\n");
          TStr s2(U">");
          if (!p.get_this_sym(s2))
               printf("Error test_new:8  TParser::get_this_sym(TStr)\n");
     }

     // --- TParser: expected_this_word(std::string) ---
     {
          TParser p;
          p.assign("var");
          p.end_of_init();
          std::string kw = "var";
          try { p.expected_this_word(kw); }
          catch (...) { printf("Error test_new:9  TParser::expected_this_word(std::string)\n"); }
     }

     // --- TParser: expected_this_sym(std::string), expected_this_sym(TStr) ---
     {
          TParser p;
          p.assign("; :");
          p.end_of_init();
          std::string s1 = ";";
          try { p.expected_this_sym(s1); }
          catch (...) { printf("Error test_new:10 TParser::expected_this_sym(std::string)\n"); }
          TStr s2(U":");
          try { p.expected_this_sym(s2); }
          catch (...) { printf("Error test_new:11 TParser::expected_this_sym(TStr)\n"); }
     }

     // --- match_item: Sym(TStr), Sym(std::string) в parse ---
     {
          using namespace tlex;
          TParser p;
          p.assign("= :");
          p.end_of_init();
          TStr      ts; std::string ss;
          if (!p.parse({ Sym(ts), Sym(ss) }))
               printf("Error test_new:12 Sym(TStr/std::string) in parse\n");
          if (ts != "=" || ss != ":")
               printf("Error test_new:13 Sym values wrong: ts='%s' ss='%s'\n",
                    ts.to_std_string().c_str(), ss.c_str());
     }

     // --- match_item: Int(TStr), Int(std::string) в parse ---
     {
          using namespace tlex;
          TParser p;
          p.assign("42 99");
          p.end_of_init();

          TStr ti; std::string si;
          if (!p.parse({ Int(ti), Int(si) }))
               printf("Error test_new:14 Int(TStr/std::string) in parse\n");
          if (ti != "42" || si != "99")
               printf("Error test_new:15 Int values wrong: ti='%s' si='%s'\n",
                    ti.to_std_string().c_str(), si.c_str());
     }

     // --- match_item: Float(TStr) в parse ---
     {
          using namespace tlex;
          TParser p;
          p.assign("3.14");
          p.end_of_init();

          TStr tf;
          if (!p.parse({ Float(tf) }))
               printf("Error test_new:16 Float(TStr) in parse\n");
          if (tf != "3.14")
               printf("Error test_new:17 Float(TStr) value wrong: '%s'\n",
                    tf.to_std_string().c_str());
     }

     // --- match_item: Str(TStr) в parse ---
     {
          using namespace tlex;
          TParser p;
          p.assign("'hello'");
          p.end_of_init();

          TStr ts;
          if (!p.parse({ Str(ts) }))
               printf("Error test_new:18 Str(TStr) in parse\n");
          if (ts != "hello")
               printf("Error test_new:19 Str(TStr) value wrong: '%s'\n",
                    ts.to_std_string().c_str());
     }

     // --- match_item: ThisSym(std::string), ThisSym(TStr) в parse ---
     {
          using namespace tlex;
          TParser p;
          p.assign("; :");
          p.end_of_init();

          std::string ss = ";";
          TStr        ts(U":");
          if (!p.parse({ ThisSym(ss), ThisSym(ts) }))
               printf("Error test_new:20 ThisSym(std::string/TStr) in parse\n");
     }

     // --- match_item: комбинированный шаблон с новыми типами ---
     {
          using namespace tlex;
          TParser p;
          p.assign("speed = 3.14 ; count = 42 ;");
          p.end_of_init();

          TStr   name1, name2;
          TStr   fval_s;
          TStr   ival_s;  int ival_i = 0;
          if (!p.parse({
               name1, ThisSym(std::string("=")), Float(fval_s), ';',
               name2, ThisSym(TStr(U"=")),        Int(ival_i),   Sym(ival_s)
               }))
               printf("Error test_new:21 combined pattern\n");
          if (name1 != "speed" || name2 != "count" || ival_i != 42)
               printf("Error test_new:22 combined pattern values wrong\n");
     }

     printf("test_new_functions: done\n");
}
//=============== test expan str ====================

// ============== Вспомогательные макросы ==============

#define TEST(name) do { std::cout << "TEST: " << name << std::endl; g_total++; } while(0)
#define EXPECT(cond, msg) do { \
    if (!(cond)) { \
        std::cout << "  FAIL: " << msg << std::endl; \
        g_failed++; \
    } else { \
        g_passed++; \
    } \
} while(0)

int g_total = 0;
int g_failed = 0;
int g_passed = 0;
// ============== Тесты ==============
void test_simple_mode_no_escape() {
     TEST("SIMPLE mode: no escape processing");
     TLexSession ses(false); // CStyle = false => SIMPLE
     ses.feed("\"hello\\n\"");
     //ses.lex.next_lex();
     const TLex& lx = ses.lex.cur();
     EXPECT(lx.type == lex_STRING, "should be string");
     EXPECT(lx.ok, "should be ok");
     std::string s = lx.v_s.to_std_string();
     EXPECT(s == "hello\\n", "backslash-n must be literal");
}

void test_cstyle_escape() {
     TEST("CSTYLE mode: classic escapes");
     TLexSession ses(true); // CStyle => CSTYLE
     ses.feed("\"a\\nb\\tc\\\"d\\\\e\"");
     const TLex& lx = ses.lex.cur();
     EXPECT(lx.type == lex_STRING, "should be string");
     std::string s = lx.v_s.to_std_string();
     // a newline, b tab, c " d \ e null
     //std::string expected = std::string("a\nb\tc\"d\\e\0", 10);
     // или явно:
     std::string expected;
     expected += 'a'; expected += '\n'; expected += 'b'; expected += '\t';
     expected += 'c'; expected += '"'; expected += 'd'; expected += '\\'; 
     expected += 'e'; 
     EXPECT(s == expected, "classic escapes decoded correctly");
     //EXPECT(s == std::string("a\nb\tc\"d\\e", 11) + '\0', "classic escapes decoded correctly");
}

void test_python_hex() {
     TEST("PYTHON mode: \\x escapes");
     TLexSession ses;
     ses.lex.set_string_mode(TLexem::StringMode::PYTHON);
     ses.feed("'\\x41\\x42\\x043\\xABCDE'");
     const TLex& lx = ses.lex.cur();
     std::string s = lx.v_s.to_std_string();
     // \x41 = 'A', \x42 = 'B', \x043 = 'C', \xABCDE должен быть обрезан до U+FFFD?
     // по нашему коду при превышении 0x10FFFF заменяется на U+FFFD.
     // \xABCDE = 0xABCDE, что < 0x10FFFF, так что это будет символ U+ABCDE (не BMP, 4 байта в UTF-8)
     // Проверим, что символы корректны.
     // Для простоты проверим первые три: AB, а последний пропустим через decode.
     EXPECT(s[0] == 'A', "\\x41 -> A");
     EXPECT(s[1] == 'B', "\\x42 -> B");
     EXPECT(s[2] == 'C', "\\x043 -> C");
     // оставшаяся часть: U+ABCDE в UTF-8: F2 AF 86 9E? вычислим.
     // 0xABCDE = 1010 1011 1100 1101 1110 -> по таблице: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
     // 0xABCDE в битах: 0001 0101 0111 1001 1011 1101 1110? Подождём, упростим: просто убедимся, что символ не равен 0.
     EXPECT(s.size() >= 7, "string at least 7 bytes (A,B,C + U+ABCDE)");
}

void test_cpp_octal() {
     TEST("CPP mode: octal escapes");
     TLexSession ses;
     ses.lex.set_string_mode(TLexem::StringMode::CPP);
     ses.feed("\"\\101\\377\"");
     const TLex& lx = ses.lex.cur();
     std::string s = lx.v_s.to_std_string();
     // \101 = 'A', \0 = '\0', \377 = 0xFF (U+00FF)

     EXPECT(s[0] == 'A', "\\101 -> A");
     EXPECT(lx.v_s.str[1] == U'\xFF', "\\377 -> U+00FF");
     //EXPECT(static_cast<unsigned char>(s[2]) == 0xFF, "\\377 -> 0xFF");
}

void test_raw_simple() {
     TEST("RAW string: R\"(simple)\"");
     TLexSession ses;
     ses.feed("R\"(hello\\n)\"");
     const TLex& lx = ses.lex.cur();
     EXPECT(lx.type == lex_STRING, "raw string recognized");
     EXPECT(lx.ok, "ok");
     std::string s = lx.v_s.to_std_string();
     EXPECT(s == "hello\\n", "raw string contains literal backslash-n");
}

void test_raw_with_delimiter() {
     TEST("RAW string: R\"delim(()) data\ndelim\"");
     TLexSession ses;
     ses.feed("R\"xyz(x(y)z)xyz\"");
     const TLex& lx = ses.lex.cur();
     std::string s = lx.v_s.to_std_string();
     EXPECT(s == "x(y)z", "raw string with delimiter");
}

void test_raw_multiline() {
     TEST("RAW string: multiline with position tracking");
     TLexSession ses;
     ses.feed("R\"(line1\nline2)\" after");
     const TLex& lx = ses.lex.cur();
     EXPECT(lx.type == lex_STRING, "raw string ok");
     // position: начало строки на первой строке, внутри есть \n, после закрытия кавычки позиция должна быть на следующей строке
     EXPECT(lx.lin == 1, "start line");
     // последующий next_lex должен выдать слово "after"
     ses.lex.next_lex();
     const TLex& w = ses.lex.cur();
     EXPECT(w.is_word() && w.v_s.to_std_string() == "after", "following token is 'after'");
}

void test_raw_unclosed_error() {
     TEST("RAW string: unclosed error");
     TLexSession ses;
     ses.feed("R\"(no end");
     const TLex& lx = ses.lex.cur();
     EXPECT(lx.type == lex_ERROR, "should be error");
     EXPECT(lx.v_i == lex_ERR_UNCLOSED_STRING, "error code unclosed string");
}

void test_unclosed_string_ex() {
     TEST("Unclosed string in CSTYLE mode");
     TLexSession ses(true);
     ses.feed("\"no end");
     const TLex& lx = ses.lex.cur();
     EXPECT(lx.type == lex_ERROR, "error type");
     EXPECT(lx.v_errMsg && strcmp(lx.v_errMsg, "Unclosed string literal") == 0, "error message");
}

void test_escape_unknown_ex() {
     TEST("Unknown escape \\p in CSTYLE");
     TLexSession ses(true);
     ses.feed("\"a\\pb\"");
     const TLex& lx = ses.lex.cur();
     std::string s = lx.v_s.to_std_string();
     EXPECT(s == "a\\pb", "backslash-p remains literal");
}

void test_u_escape() {
     TEST("Python \\u and \\U");
     TLexSession ses;
     ses.lex.set_string_mode(TLexem::StringMode::PYTHON);
     ses.feed("'\\u0041\\U0001F600'");
     const TLex& lx = ses.lex.cur();
     std::string s = lx.v_s.to_std_string();
     // \u0041 = 'A'
     // \U0001F600 = 😀 (U+1F600) в UTF-8: F0 9F 98 80
     EXPECT(s[0] == 'A', "\\u0041 -> A");
     EXPECT(s.size() >= 5, "emoji is 4 bytes + 'A' = 5");
     // проверка первого байта эмодзи
     EXPECT(static_cast<unsigned char>(s[1]) == 0xF0, "first byte of emoji");
}

void test_quote_char_stored() {
     TEST("v_i stores quote character code");
     TLexSession ses;
     ses.feed("'single'");
     const TLex& lx = ses.lex.cur();
     EXPECT(lx.v_i == static_cast<unsigned>('\''), "quote char is apostrophe");
}

void test_backward_compatibility_cstyle() {
     TEST("set_C_style(true) still works as CSTYLE");
     TLexSession ses;
     ses.lex.set_C_style(true); // должно дать CSTYLE
     ses.feed("\"\\n\"");
     const TLex& lx = ses.lex.cur();
     std::string s = lx.v_s.to_std_string();
     EXPECT(s == "\n", "newline decoded");
}

void test_invalid_hex_escape() {
     TEST("Invalid \\x with no digits");
     TLexSession ses;
     ses.lex.set_string_mode(TLexem::StringMode::CPP);
     ses.feed("\"\\xG\"");
     const TLex& lx = ses.lex.cur();
     // по нашей логике, если после \x нет hex-цифр, decoded остаётся '\\' и 'x' выводится отдельно? 
     // в коде: если cnt == 0, valid = false, тогда добавляется '\\' и next символ (который 'x')? 
     // Нужно уточнить: при входе в case 'x' мы уже съели '\\', и next = 'x'. Если цифр нет, мы попадаем в valid = false и в else ветку (некорректная последовательность) – там мы добавляем '\\' и текущий символ (next), т.е. 'x'. Получится "\\xG"? 
     // В текущей реализации при valid==false после switch мы идём в else блок, который пушит '\\' и curChar() (это 'x'?), и затем step_curChar(). Получится, что 'x' будет выведен, а 'G' останется следующим символом. Так что результат будет "\\xG". Проверим.
     std::string s = lx.v_s.to_std_string();
     EXPECT(s == "\\xG", "\\x with no hex digits leaves \\x and next char");
}

// ═══════════════════════════════════════════════════════════════════════════════
// Тесты QString-интеграции (через MockQString = class QString из mock_qstring.h)
// ═══════════════════════════════════════════════════════════════════════════════

// ── Вспомогательные ──────────────────────────────────────────────────────────
namespace {
int g_qs_total = 0, g_qs_passed = 0, g_qs_failed = 0;
} // namespace

#define QS_CHECK(cond, msg) do { \
    ++g_qs_total; \
    if (cond) { ++g_qs_passed; } \
    else { ++g_qs_failed; printf("  FAIL [%s:%d]  %s\n", __FILE__, __LINE__, msg); } \
} while(0)

#define QS_SECTION(name) printf("\n--- %s ---\n", name)

// ── 1. cast_to: TStr → std::string / std::u32string / QString ─────────────────
void test_cast_to()
{
    QS_SECTION("cast_to");
    {
        const TChar src[] = U"hello";
        TStr s(src, 5);

        QS_CHECK(s.cast_to<std::string>()    == "hello",   "cast_to<string>");
        QS_CHECK(s.cast_to<std::u32string>() == U"hello",  "cast_to<u32string>");
        QS_CHECK(s.cast_to<QString>()        == "hello",   "cast_to<QString> ASCII");
    }
    {
        // BMP Unicode: U+00E9 'é'
        const TChar src[] = { 0xE9, 0 };
        TStr s(src, 1);
        const QString q = s.cast_to<QString>();
        QS_CHECK(q.utf16Size() == 1,              "BMP: one UTF-16 unit");
        QS_CHECK(q.utf16At(0)  == 0x00E9u,        "BMP: correct code unit");
    }
    {
        // Surrogate pair: U+1F600 (😀) → [0xD83D, 0xDE00]
        const TChar src[] = { 0x1F600u, 0 };
        TStr s(src, 1);
        const QString q = s.cast_to<QString>();
        QS_CHECK(q.utf16Size() == 2,              "Surrogate: two UTF-16 units");
        QS_CHECK(q.utf16At(0)  == 0xD83Du,        "Surrogate: high surrogate");
        QS_CHECK(q.utf16At(1)  == 0xDE00u,        "Surrogate: low surrogate");
    }
}

// ── 2. assign(const QString&) ─────────────────────────────────────────────────
void test_qs_assign()
{
    QS_SECTION("assign(QString)");
    TParser p;
    p.assign(QString("alpha beta gamma"));
    p.end_of_init();

    using namespace tlex;
    std::string w1, w2, w3;
    QS_CHECK(p.parse({ Word(w1), Word(w2), Word(w3) }), "parse after assign(Q)");
    QS_CHECK(w1 == "alpha",  "first word");
    QS_CHECK(w2 == "beta",   "second word");
    QS_CHECK(w3 == "gamma",  "third word");
}

// ── 3. Word / Str / Sym / Int / Float / Any → захват в QString ────────────────
void test_qs_capture()
{
    QS_SECTION("capture to QString");
    using namespace tlex;

    // Word(QString&)
    {
        TParser p;
        p.assign("key = value");
        p.end_of_init();
        QString key, val;
        QS_CHECK(p.parse({ Word(key), ThisSym("="), Word(val) }), "Word(Q) parse");
        QS_CHECK(key == "key",   "Word(Q) key");
        QS_CHECK(val == "value", "Word(Q) val");
    }

    // Str(QString&) - строковый литерал в кавычках
    {
        TParser p;
        p.assign(R"(name = "hello world")");
        p.end_of_init();
        QString name, strval;
        QS_CHECK(p.parse({ Word(name), '=', Str(strval) }), "Str(Q) parse");
        QS_CHECK(name   == "name",        "Str(Q) name");
        QS_CHECK(strval == "hello world", "Str(Q) value");
    }

    // Int(QString&) - целое в виде строки
    {
        TParser p;
        p.assign("count = 42");
        p.end_of_init();
        QString key, ival;
        QS_CHECK(p.parse({ Word(key), '=', Int(ival) }), "Int(Q) parse");
        QS_CHECK(ival == "42", "Int(Q) value");
    }

    // Float(QString&) - число с точкой
    {
        TParser p;
        p.assign("pi = 3.14");
        p.end_of_init();
        QString key, fval;
        QS_CHECK(p.parse({ Word(key), '=', Float(fval) }), "Float(Q) parse");
        // Q::number через %g - "3.14"
        QS_CHECK(fval == "3.14", "Float(Q) value");
    }

    // Sym(QString&) - захват символа
    {
        TParser p;
        p.assign("a + b");
        p.end_of_init();
        QString a, op, b;
        QS_CHECK(p.parse({ Word(a), Sym(op), Word(b) }), "Sym(Q) parse");
        QS_CHECK(op == "+", "Sym(Q) op");
    }

    // Any(QString&) - любой токен
    {
        TParser p;
        p.assign("token");
        p.end_of_init();
        QString tok;
        QS_CHECK(p.parse({ Any(tok) }), "Any(Q) parse");
        QS_CHECK(tok == "token", "Any(Q) value");
    }
}

// ── 4. ThisWord / ThisSym с QString ───────────────────────────────────────────
void test_qs_this_match()
{
    QS_SECTION("ThisWord / ThisSym (QString)");
    using namespace tlex;

    // Успешное совпадение
    {
        TParser p;
        p.assign("begin x end");
        p.end_of_init();
        QString val;
        const QString kw_begin("begin"), kw_end("end");
        QS_CHECK(p.parse({ ThisWord(kw_begin), Word(val), ThisWord(kw_end) }),
                 "ThisWord(Q) success");
        QS_CHECK(val == "x", "ThisWord(Q) capture between keywords");
    }

    // Провал + откат - позиция должна остаться прежней
    {
        TParser p;
        p.assign("other x");
        p.end_of_init();
        const TState before = p.get_state();
        const QString kw("begin");
        QS_CHECK(!p.parse({ ThisWord(kw) }), "ThisWord(Q) expected fail");
        QS_CHECK(p.get_state().lexIdx == before.lexIdx, "position restored after fail");
    }

    // ThisSym(const QString&) - стандартный символ, не требует add_ex_exSym
    {
        TParser p;
        p.assign("a + b");
        p.end_of_init();
        QString a, b;
        const QString plus("+");
        QS_CHECK(p.parse({ Word(a), ThisSym(plus), Word(b) }), "ThisSym(Q) parse");
        QS_CHECK(a == "a" && b == "b", "ThisSym(Q) captures");
    }
}

// ── 5. is_ / get_ / expected_ для QString ────────────────────────────────────
void test_qs_methods()
{
    QS_SECTION("is_this_word / get_this_word / expected_this_word (Q)");
    using namespace tlex;

    {
        TParser p;
        p.assign("hello world");
        p.end_of_init();
        const QString hello("hello"), world("world"), other("other");

        QS_CHECK( p.is_this_word(hello),  "is_this_word match");
        QS_CHECK(!p.is_this_word(other),  "is_this_word no match");

        const TLex& lx = p.get_this_word(hello);
        QS_CHECK(!!lx,                    "get_this_word returns valid lex");
        QS_CHECK(lx.v_s == "hello",       "get_this_word lex content");

        const TLex& lx2 = p.get_this_word(world);
        QS_CHECK(!!lx2,                   "get_this_word second word");
    }

    QS_SECTION("is_this_sym / get_this_sym (Q)");
    {
        TParser p;
        p.assign("x + y");
        p.end_of_init();
        std::string dummy;
        p.parse({ Word(dummy) });           // skip 'x'

        const QString plus("+"), minus("-");
        QS_CHECK( p.is_this_sym(plus),    "is_this_sym match");
        QS_CHECK(!p.is_this_sym(minus),   "is_this_sym no match");

        const TLex& lx = p.get_this_sym(plus);
        QS_CHECK(!!lx,                    "get_this_sym returns valid");
    }

    QS_SECTION("expected_this_word / expected_this_sym (Q)");
    {
        TParser p;
        p.assign("end ;");
        p.end_of_init();
        const QString end_kw("end"), semi(";");

        const TLex& w = p.expected_this_word(end_kw);
        QS_CHECK(!!w,                     "expected_this_word success");
        const TLex& s = p.expected_this_sym(semi);
        QS_CHECK(!!s,                     "expected_this_sym success");
    }
}

// ── 6. Surrogate pair через match без лексера ─────────────────────────────────
void test_qs_surrogate()
{
    QS_SECTION("Surrogate pair - TQBridge::match");

    // U+1F600 (😀) → UTF-16: [0xD83D, 0xDE00]
    const TChar emoji_u32[] = { 0x1F600u, 0 };
    TStr tstr(emoji_u32, 1);

    // Строим QString с правильной суррогатной парой через fromUcs4
    const QString emoji_q = QString::fromUcs4(emoji_u32, 1);
    QS_CHECK(emoji_q.utf16Size() == 2,       "Surrogate: two code units");
    QS_CHECK(emoji_q.utf16At(0)  == 0xD83Du, "Surrogate: high");
    QS_CHECK(emoji_q.utf16At(1)  == 0xDE00u, "Surrogate: low");

    // Прямой вызов TQBridge<1>::match - без лексера
    QS_CHECK( TQBridge<1>::match(&emoji_q, tstr),  "match: Q emoji == TStr emoji");

    // Неправильная пара - не совпадает
    const TChar other_u32[] = { 0x1F601u, 0 };
    const QString other_q = QString::fromUcs4(other_u32, 1);
    QS_CHECK(!TQBridge<1>::match(&other_q, tstr),  "match: different emoji != ");

    // Проверяем cast_to туда-обратно: TStr → QString → (сравнение через match)
    const QString cast_q = tstr.cast_to<QString>();
    QS_CHECK( TQBridge<1>::match(&cast_q, tstr),   "match: cast_to roundtrip");
}

// ── 7. Несколько Qt-захватов + откат в одном parse ────────────────────────────
void test_qs_backtrack()
{
    QS_SECTION("Backtracking with Qt captures");
    using namespace tlex;

    TParser p;
    p.assign("x = 10 ; y = 20 ;");
    p.end_of_init();

    // Этот вариант не совпадёт (нет "str")
    {
        const TState before = p.get_state();
        QString name, val;
        const QString kw("str");
        QS_CHECK(!p.parse({ Word(name), ThisSym("="), Word(val), ThisWord(kw) }),
                 "backtrack: fail on ThisWord(Q) mismatch");
        // Позиция должна быть восстановлена
        QS_CHECK(p.get_state().lexIdx == before.lexIdx, "backtrack: state restored");
    }

    // Теперь правильный шаблон
    {
        QString k1, k2;
        int     v1 = 0, v2 = 0;
        QS_CHECK(p.parse({ Word(k1), '=', Int(v1), ';', Word(k2), '=', Int(v2), ';' }),
                 "correct pattern after backtrack");
        QS_CHECK(k1 == "x" && v1 == 10, "first pair");
        QS_CHECK(k2 == "y" && v2 == 20, "second pair");
    }
}

// ── Итоговый запуск всех Qt-тестов ───────────────────────────────────────────
void test_lexer_settings()
{
     SECTION("set_quote_as_sym");
     {
          // Кавычки как символы: строк нет, " и ' - просто символы
          TLexSession s;
          s.lex.set_quote_as_sym(true);
          s.feed(R"("hello" 'world')");
          auto toks = s.all_tokens();
          CHECK(toks.size() == 6, "quote_as_sym: 6 токенов");
          CHECK(toks[0].type == lex_SYMBOL && toks[0].v_i == '"',  "открывающая \"");
          CHECK(toks[1].type == lex_WORD,                          "hello - слово");
          CHECK(toks[2].type == lex_SYMBOL && toks[2].v_i == '"',  "закрывающая \"");
          CHECK(toks[3].type == lex_SYMBOL && toks[3].v_i == '\'', "открывающая '");
          CHECK(toks[4].type == lex_WORD,                          "world - слово");
          CHECK(toks[5].type == lex_SYMBOL && toks[5].v_i == '\'', "закрывающая '");
     }
     {
          // Без флага - всё как обычно
          TLexSession s;
          s.feed(R"("hello")");
          auto toks = s.all_tokens();
          CHECK(toks.size() == 1, "без флага: 1 токен-строка");
          CHECK(toks[0].type == lex_STRING, "тип lex_STRING");
     }

     SECTION("ex_word_symbols_first");
     {
          // @ и $ могут начинать слово
          TLexSession s;
          s.lex.ex_word_symbols_first("@$");
          s.feed("@set $undo normal");
          auto toks = s.all_tokens();
          CHECK(toks.size() == 3, "first: 3 слова");
          CHECK(str_of(toks[0]) == "@set",  "@set - одно слово");
          CHECK(str_of(toks[1]) == "$undo", "$undo - одно слово");
          CHECK(str_of(toks[2]) == "normal","normal без префикса");
     }
     {
          // Без настройки @ и $ - символы
          TLexSession s;
          s.feed("@set");
          auto toks = s.all_tokens();
          CHECK(toks.size() == 2, "без настройки: @-символ + слово");
          CHECK(toks[0].type == lex_SYMBOL, "@ - символ");
          CHECK(str_of(toks[1]) == "set",  "set - слово");
     }

     SECTION("ex_word_symbols (body)");
     {
          // Дефис и точка внутри слова: $test-string, v1.2
          TLexSession s;
          s.lex.ex_word_symbols_first("$");
          s.lex.ex_word_symbols("-.");
          s.feed("$test-string v1.2 standalone-");
          auto toks = s.all_tokens();
          // $test-string → одно слово
          CHECK(str_of(toks[0]) == "$test-string", "дефис внутри слова");
          // v1.2 → одно слово (. в теле)
          CHECK(str_of(toks[1]) == "v1.2",         "точка внутри слова");
          // standalone- : слово, затем - уже конец текста → тире как часть слова
          // (тире идёт после 'e' - is_letter('e')=true, затем '-' - is_letter('-')=true)
          CHECK(str_of(toks[2]) == "standalone-",  "тире на конце слова");
          CHECK(toks.size() == 3, "ровно 3 токена");
     }
     {
          // Одиночный дефис - по-прежнему символ (не начало слова)
          TLexSession s;
          s.lex.ex_word_symbols("-");
          s.feed("a - b");
          auto toks = s.all_tokens();
          CHECK(toks.size() == 3,                  "a - b: 3 токена");
          CHECK(str_of(toks[0]) == "a",            "a");
          CHECK(toks[1].type == lex_SYMBOL,        "- как символ между словами");
          CHECK(str_of(toks[2]) == "b",            "b");
     }
     {
          // ex_word_symbols state=false: убираем ранее добавленный символ
          // (state=false не трогает базовый алфавит a-zA-Z0-9_, только custom-добавления)
          TLexSession s;
          s.lex.ex_word_symbols(".-");   // добавляем . и - в тело слова
          s.lex.ex_word_symbols(".", false);  // потом отменяем .
          s.feed("aa.bb aa-bb");
          auto toks = s.all_tokens();
          // aa.bb → aa, ., bb (3 токена: . больше не часть слова)
          // aa-bb → aa-bb (1 токен: - остался в теле)
          CHECK(toks.size() == 4,                "state=false: 4 токена");
          CHECK(str_of(toks[0]) == "aa",         "aa перед точкой");
          CHECK(toks[1].type == lex_SYMBOL,      ". снова символ");
          CHECK(str_of(toks[2]) == "bb",         "bb после точки");
          CHECK(str_of(toks[3]) == "aa-bb",      "aa-bb - одно слово");
     }
}

void test_qstring_all()
{
    printf("\n════════════════════════════════\n");
    printf("  QString integration tests\n");
    printf("════════════════════════════════\n");

    test_cast_to();
    test_qs_assign();
    test_qs_capture();
    test_qs_this_match();
    test_qs_methods();
    test_qs_surrogate();
    test_qs_backtrack();

    printf("\nQString tests: total=%d  passed=%d  failed=%d\n",
           g_qs_total, g_qs_passed, g_qs_failed);

    g_total  += g_qs_total;
    g_passed += g_qs_passed;
    g_failed += g_qs_failed;
}

int main_string_test()
{
     test_act();
     test_externparse();
     test_read_s();
     test_tstr();
     test_expected_error();
     test_construct_continuation();

     test_any_opt_end();
     test_new_functions();
     test_python_hex();
     test_cpp_octal();
     test_cstyle_escape();
     test_backward_compatibility_cstyle();  //***
     test_simple_mode_no_escape();
     test_raw_simple();
     test_raw_with_delimiter();
     test_raw_multiline();
     test_raw_unclosed_error();
     test_unclosed_string_ex();
     test_escape_unknown_ex();
     test_u_escape();
     test_quote_char_stored();
     test_invalid_hex_escape();

     test_qstring_all();
     test_lexer_settings();   // ← новые тесты QString-интеграции

     std::cout << "\nTotal tests: " << g_total
          << ", Passed: " << g_passed
          << ", Failed: " << g_failed << std::endl;
     return (g_failed == 0) ? 0 : 1;
}

int main()
{
     main_string_test();
}
#endif
