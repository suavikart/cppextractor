#include "lexer.h"

using namespace tlex;

namespace {
int encode_utf8(char32_t cp, char* out) {
    // простая реализация, чтобы код компилировался
    if (cp < 0x80) {
        out[0] = static_cast<char>(cp);
        return 1;
    } else if (cp < 0x800) {
        out[0] = static_cast<char>(0xC0 | (cp >> 6));
        out[1] = static_cast<char>(0x80 | (cp & 0x3F));
        return 2;
    } else if (cp < 0x10000) {
        out[0] = static_cast<char>(0xE0 | (cp >> 12));
        out[1] = static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        out[2] = static_cast<char>(0x80 | (cp & 0x3F));
        return 3;
    } else if (cp <= 0x10FFFF) {
        out[0] = static_cast<char>(0xF0 | (cp >> 18));
        out[1] = static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
        out[2] = static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        out[3] = static_cast<char>(0x80 | (cp & 0x3F));
        return 4;
    }
    return 0; // error
}
} // namespace

std::string TStr::to_std_string() const
{
     std::string res;
     res.reserve(length * 2);
     for (int i = 0; i < length; ++i)
     {
          char32_t cp = str[i];
          if (cp < 0x80) { res += static_cast<char>(cp); continue; }
          char buf[4];
          int n = encode_utf8(cp, buf); // используем уже существующую encode_utf8
          res.append(buf, n);
     }
     return res;
}

void TStr::to_std_string(std::string& res) const
{
     for (int i = 0; i < length; ++i)
     {
          char32_t cp = str[i];
          if (cp < 0x80) { res += static_cast<char>(cp); continue; }
          char buf[4];
          int n = encode_utf8(cp, buf);
          res.append(buf, n);
     }
}

bool TStr::equ(const char* b) const
{
     int i = 0;
     for (; i < length && b[i]; ++i)
          if (str[i] != static_cast<char32_t>(static_cast<unsigned char>(b[i]))) return false;
     return b[i] == 0 && i == length;
}

bool TStr::equ(const std::string& s) const { return equ(s.c_str()); }

bool TStr::equ(const TStr& s) const
{
     if (length != s.length) return false;
     for (int i = 0; i < length; ++i)
          if (str[i] != s.str[i]) return false;
     return true;
}

bool TStr::equ(const char32_t* b) const
{
     int i = 0;
     for (; i < length && b[i]; ++i)
          if (str[i] != b[i]) return false;
     return b[i] == 0 && i == length;
}

// Сравнивает ровно length символов начиная с p (без нулевого терминатора).
// Используется в load_symbol, где p - указатель в середину текста.
bool TStr::equ_prefix(const TChar* p) const
{
     for (int i = 0; i < length; ++i)
          if (str[i] != p[i]) return false;
     return true;
}

TLex TLexem::get_word()
{
     if (m_current.is_word())
     {
          TLex lx = m_current;
          next_lex();
          return lx;
     }
     return Fail();
}
bool TLexem::get_this_word(const char* word)
{
     if (m_current.is_this_word(word))
     {
          next_lex();
          return true;
     }
     return false;
}
bool TLexem::get_this_word(const TStr& w)
{
     if (m_current.is_this_word(w))
     {
          next_lex();
          return true;
     }
     return false;
}
bool TLexem::get_this_word(const std::string& w)
{
     if (m_current.is_this_word(w))
     {
          next_lex();
          return true;
     }
     return false;
}
TLex TLexem::get_sym()
{
     if (m_current.is_sym())
     {
          TLex lx = m_current;
          next_lex();
          return lx;
     }
     return Fail();
}
bool TLexem::get_this_sym(int code)
{
     if (m_current.is_this_sym(code))
     {
          next_lex();
          return true;
     }
     return false;
}
TLex TLexem::get_this_sym(const char* sym)
{
     if (m_current.is_this_sym(sym))
     {
          TLex lx = m_current;
          next_lex();
          return lx;
     }
     return Fail();
}
bool TLexem::get_this_sym(const std::string& sym)
{
     if (m_current.is_this_sym(sym))
     {
          next_lex();
          return true;
     }
     return false;
}
bool TLexem::get_this_sym(const TStr& sym)
{
     if (m_current.is_this_sym(sym))
     {
          next_lex();
          return true;
     }
     return false;
}

TLex TLexem::get_str()
{
     if (m_current.is_str())
     {
          TLex lx = m_current;
          next_lex();
          return lx;
     }
     return Fail();
}

TLex TLexem::get_float()
{
     if (m_current.is_float())
     {
          TLex lx = m_current;
          next_lex();
          return lx;
     }
     if (m_current.is_int())
     {
          TLex lx = m_current;
          next_lex();
          lx.v_f = lx.v_i;
          lx.type = lex_FLOAT;
          return lx;
     }
     return Fail();
}
TLex TLexem::get_int()
{
     if (m_current.is_int())
     {
          TLex lx = m_current;
          next_lex();
          return lx;
     }
     return Fail();
}
TLex TLexem::get_number()
{
     int sign = 1;
     if (!get_this_sym('+'))
          if (get_this_sym('-'))
               sign = -1;

     if (m_current.type == lex_INTEGER || m_current.type == lex_FLOAT)
     {
          TLex lx = m_current;
          if (m_current.type == lex_INTEGER)
          {
               lx.v_f = double(m_current.v_i) * sign;
               lx.v_i = m_current.v_i * sign;
          }
          else
          {
               lx.v_f = m_current.v_f * sign;
               lx.v_i = int(m_current.v_f) * sign;
          }
          next_lex();
          return lx;
     }
     return Fail();
}
TLex TLexem::get_eof()
{
     if (m_current.is_eof())
          return m_current;
     return Fail();
}

TLex TLexem::expected_word()
{
     if (m_current.type == lex_WORD)
     {
          TLex lx = m_current;
          next_lex();
          return lx;
     }
     m_error->error(m_current, parse_EXPECTED, "Expected word");
     return Fail();
}
bool TLexem::expected_this_word(const char* word)
{
     if (m_current.type == lex_WORD && m_current.v_s.equ(word))
     {
          next_lex();
          return true;
     }
     m_error->error(m_current, parse_EXPECTED, "Expected word '", word, "'");
     return false;
}
bool TLexem::expected_this_word(const std::string& w)
{
     if (m_current.type == lex_WORD && m_current.v_s.equ(w))
     {
          next_lex();
          return true;
     }
     m_error->error(m_current, parse_EXPECTED, "Expected word '", w, "'");
     return false;
}
bool TLexem::expected_this_word(const TStr& w)
{
     if (m_current.type == lex_WORD && m_current.v_s.equ(w))
     {
          next_lex();
          return true;
     }
     m_error->error(m_current, parse_EXPECTED, "Expected word '", w, "'");
     return false;
}

TLex TLexem::expected_sym()
{
     if (m_current.type == lex_SYMBOL)
     {
          TLex lx = m_current;
          next_lex();
          return lx;
     }
     m_error->error(m_current, parse_EXPECTED, "Expected symbol");
     return Fail();
}
bool TLexem::expected_this_sym(int code)
{
     if (m_current.type == lex_SYMBOL && m_current.v_i == code)
     {
          next_lex();
          return true;
     }
     m_error->error(m_current, parse_EXPECTED, "Expected symbol '", code, ",");
     return false;
}
bool TLexem::expected_this_sym(const char* sym)
{
     if (m_current.type == lex_SYMBOL && m_current.v_s.equ(sym))
     {
          next_lex();
          return true;
     }
     m_error->error(m_current, parse_EXPECTED, "Expected symbol '", sym, ",");
     return false;
}
bool TLexem::expected_this_sym(const std::string& sym)
{
     if (m_current.type == lex_SYMBOL && m_current.v_s.equ(sym))
     {
          next_lex();
          return true;
     }
     m_error->error(m_current, parse_EXPECTED, "Expected symbol '", sym, "'");
     return false;
}
bool TLexem::expected_this_sym(const TStr& sym)
{
     if (m_current.type == lex_SYMBOL && m_current.v_s.equ(sym))
     {
          next_lex();
          return true;
     }
     m_error->error(m_current, parse_EXPECTED, "Expected symbol '", sym, "'");
     return false;
}
TLex TLexem::expected_str()
{
     if (m_current.type == lex_STRING)
     {
          TLex lx = m_current;
          next_lex();
          return lx;
     }
     m_error->error(m_current, parse_EXPECTED, "Expected string");
     return Fail();
}
TLex TLexem::expected_float()
{
     if (m_current.type == lex_FLOAT)
     {
          TLex lx = m_current;
          next_lex();
          return lx;
     }
     m_error->error(m_current, parse_EXPECTED, "Expected float");
     return Fail();
}
TLex TLexem::expected_int()
{
     if (m_current.type == lex_INTEGER)
     {
          TLex lx = m_current;
          next_lex();
          return lx;
     }
     m_error->error(m_current, parse_EXPECTED, "Expected int");
     return Fail();
}
TLex TLexem::expected_number()
{
     if (TLex lx = get_number())
          return lx;
     m_error->error(m_current, parse_EXPECTED, "Expected number");
     return Fail();
}
TLex TLexem::expected_eof()
{
     if (is_eof())
     {
          return m_current;
     }
     m_error->error(m_current, parse_EXPECTED, "Expected eof");
     return Fail();
}
//==================================================================================
#define MAX_DOUBLE_EXP 308
#define MIN_DOUBLE_EXP (-307)

namespace {

const long str_minDouble[2] = { 1,0 };
#define str_MAX_DOUBLE_BASE  1.7976931348623158
#define str_MAX_DOUBLE       1.7976931348623158E+308
#define str_MIN_DOUBLE       (*reinterpret_cast<const double*>(str_minDouble))

//==================================================================================
// только для функции  void ImpLexem::loadConstant()
//==================================================================================
enum { MAX_MANTISSA = 25 };
enum
{
     MAX_NUM_LEN = 50      /* Максимальное число значимых цифр    */
     , MAX_LONG_LEN = 10     /* Максимальное число цифр в целом     */
       //MAX_FORWARD_CHAR = Lexem::MAX_FORWARD_CHAR
};

double powerof10[9] = { 1.e1, 1.e2, 1.e4, 1.e8, 1.e16, 1.e32,
                        1.e64,1.e128,1.e256 };
double powFor8digit[] = { 1.0, 1.e1, 1.e2, 1.e3, 1.e4, 1.e5, 1.e6, 1.e7, 1.e8, 1.e9, 1.e10 };

double mantissa_denum[MAX_MANTISSA] = {
   1e-1,  1e-2,  1e-3,  1e-4,  1e-5,  1e-6,  1e-7,  1e-8,  1e-9,  1e-10,
   1e-11, 1e-12, 1e-13, 1e-14, 1e-15, 1e-16, 1e-17, 1e-18, 1e-19, 1e-20,
   1e-21, 1e-22, 1e-23, 1e-24, 1e-25
};

} // namespace

// (ISLETTER удалён: таблица символов теперь per-instance в TLexem::m_isletter)

void TLexem::ex_word_symbols_first(const char* s)
{
     for (; *s; ++s)
     {
          const unsigned char c = static_cast<unsigned char>(*s);
          if (c < 128)
          {
               m_isletter.letter[c] = true;   // может начинать слово
               m_isletter.alpha[c]  = true;   // и стоять внутри слова
          }
     }
}

void TLexem::ex_word_symbols(const char* s, bool state)
{
     for (; *s; ++s)
     {
          const unsigned char c = static_cast<unsigned char>(*s);
          if (c < 128)
          {
               m_isletter.alpha[c] = state;
               if (!state)
                    m_isletter.letter[c] = false; // если убираем из тела - убираем и из первых
          }
     }
}
// true if correct
bool TLexem::check_uint_postfix(const char* errStr, int errCode)
{
     bool hasPostfix = false;
     TChar ch;

     while ((ch = curChar())
          && (ch == 'l' || ch == 'L' || ch == 'u' || ch == 'U')
          )
     {
          m_current.v_s.expand();
          step_curChar();
          hasPostfix = true;
     }

     if (hasPostfix && is_error_postfix(errStr, errCode))
          return false;
     return true;
}

bool TLexem::load_hex_bin_constant()
{
     unsigned long intResult = 0;
     /*
      * Load heximal
      */

     switch (curChar())
     {
     case 'x': case 'X':
     {
          m_current.v_s.expand();
          step_curChar();
          bool hasDigits = false;
          bool isDig, isLow;
          TChar ch = curChar();

          while ((isDig = is_digit(ch))
               || (isLow = (ch >= 'a' && ch <= 'f'))
               || (ch >= 'A' && ch <= 'F')
               )
          {
               if (isDig)
                    intResult = (intResult << 4) + ch - '0';
               else
                    if (isLow)
                         intResult = (intResult << 4) + ch - 'a' + 10;
                    else intResult = (intResult << 4) + ch - 'A' + 10;
               hasDigits = true;
               m_current.v_s.expand();
               step_curChar();
               ch = curChar();
          }

          if (!check_uint_postfix("Error heximal postfix", lex_ERR_HEXIMAL_POSTFIX))
               return true;

          if (hasDigits)
          {
               m_current.type = lex_INTEGER;
               m_current.ok = true;
               m_current.v_i = intResult;
          }
          else
          {
               m_current.type = lex_ERROR;
               m_current.ok = false;
               m_current.v_errMsg = "Expected heximal digits";
               m_current.v_f = 0.0;
               m_current.v_i = lex_ERR_HEXIMAL_EMPTY; // error code
          }
          return true;
     }
     break;

     case 'b': case 'B':
     {
          m_current.v_s.expand();
          step_curChar();
          bool hasDigits = false;
          TChar ch;

          while ((ch = curChar()) == '0' || ch == '1')
          {
               intResult = (intResult << 1) + (ch - '0');
               m_current.v_s.expand();
               step_curChar();
               hasDigits = true;
          }

          if (!check_uint_postfix("Error binary postfix", lex_ERR_BINARY_POSTFIX))
               return true;

          if (hasDigits)
          {
               m_current.type = lex_INTEGER;
               m_current.ok = true;
               m_current.v_i = intResult;
          }
          else
          {
               m_current.type = lex_ERROR;
               m_current.ok = false;
               m_current.v_errMsg = "Expected binary digits";
               m_current.v_f = 0.0;
               m_current.v_i = lex_ERR_HEXIMAL_EMPTY; // error code
          }
          return true;
     }
     break;
     }

     return false;
}

void TLexem::load_constant()
{
     m_current.set_pos(m_pos);
     unsigned long intResult = 0;
     int         value = 0;             /* successful parse      */
     int         sign;
     bool        isInt = true;
     int         i;

     m_current.v_s.assign(m_text.const_char(m_currentIdx), 0);

     bool CStyle = m_stringMode == StringMode::CSTYLE
                || m_stringMode == StringMode::CPP
                || m_stringMode == StringMode::RAW;
     /*
      * Load heximal
      */
     if (CStyle && curChar() == '0')
     {
          m_current.v_s.expand();   // включаем '0' в строку
          step_curChar();
          if (load_hex_bin_constant())
               return;
     }

     /*
      *  Load first number
      */

     int mantissaCount = 0;
     int mantissaStart = m_currentIdx;

     while (is_digit(curChar()))
     {
          ++mantissaCount;

          if (mantissaCount > MAX_LONG_LEN)   /* number too long */
               isInt = false;                 /* This real value */

          m_current.v_s.expand();
          step_curChar();
     }

     int  exp = 0;               /* exponent              */
     TChar ch;
     unsigned long newVal = 0;

     ch = curChar();
     // '.' - десятичная точка ТОЛЬКО если за ней цифра, e/E (экспонента) или f/F (суффикс).
     // "1." в конце клаузы (пробел/перевод строки/символ) остаётся INTEGER.  (паритет со старым лексером)
     // nextChar(1) читается ТОЛЬКО при ch=='.': если число стоит в конце текста,
     // curChar() - это терминатор, и безусловный nextChar(1) читал бы за границей буфера.
     const bool dot_is_float_sep = (ch == '.')
          && [this]{ const TChar dotNext = nextChar(1);
                     return is_digit(dotNext) || dotNext=='e' || dotNext=='E' || dotNext=='f' || dotNext=='F'; }();
     if (isInt
          && !dot_is_float_sep
          && (ch != 'e' && ch != 'E'))
     {
          for (i = 0; i < mantissaCount; i++)
          {
               newVal = intResult * 10u + static_cast<unsigned char>(m_text.str[mantissaStart + i]) - '0';

               if (newVal < intResult)  /* Overflow ? */
               {
                    isInt = false;
                    break;
               }

               intResult = newVal;
          }

          if (isInt)
          {
               m_current.type = lex_INTEGER;
               m_current.ok = true;
               m_current.v_i = intResult;
               if (CStyle)
               {
                    ch = curChar();
                    bool hasPostfix = false;
                    bool isFloat = false;
                    while ((ch = curChar())
                         && (ch == 'l' || ch == 'L' || ch == 'u' || ch == 'U' || ch == 'f' || ch == 'F')
                         )
                    {
                         if (ch == 'f' || ch == 'F')
                              isFloat = true;

                         m_current.v_s.expand();
                         step_curChar();
                         hasPostfix = true;
                    }

                    if (hasPostfix && is_error_postfix("Error nubler postfix", lex_ERR_NUMBER_BAD_POSTFIX))
                         return;

                    if (isFloat)
                    {
                         m_current.type = lex_FLOAT;
                         m_current.ok = true;
                         m_current.v_f = static_cast<double>(intResult);
                         return;
                    }
               }
               return;

          }
     }

     /*
      * Read in fractional part of number, until an 'E' is reached.
      * count digits after decimal point.
      */
      // читаем по 4 цифр, переводим их вначале в int, потом все разом в double
     double      result = 0;

     for (i = 0; i < mantissaCount; i += 4)
     {
          int e = i + 4;
          int digitCount = 0;

          if (e > mantissaCount)
               e = mantissaCount;

          intResult = 0;
          for (int j = i; j < e; ++j)
          {
               intResult = intResult * 10 + static_cast<unsigned char>(m_text.str[mantissaStart + j]) - '0';
               ++digitCount;
          }

          if (i)
               result *= powFor8digit[digitCount];
          result += static_cast<double>(intResult);
          value = 1;
     }

     if (curChar() == '.'
          && ( is_digit(nextChar(1)) || nextChar(1)=='e' || nextChar(1)=='E'
            || nextChar(1)=='f' || nextChar(1)=='F' ))   // десятичная точка: за ней цифра/e/E/f/F
     {

          m_current.v_s.expand();
          step_curChar();
          int start = m_current.v_s.length;
          int k = 0;

          while (is_digit(ch = curChar()))
          {
               m_current.v_s.expand();
               step_curChar();
               value = 1;
               // --exp;
               ++k;
          }

          //
          // сейчас пропустим 0 после точки и запомним насколько сдвинулись в powOfmantissa
          //
          int expOfmantissa = 0; // копиться будет положительное число, но реально это числоотрицательное!
          while (start < m_current.v_s.length && m_current.v_s.str[start] == '0')
          {
               ++start;
               ++expOfmantissa;
               --k;
          }

          double powOfmantissa = 1.0;
          if (expOfmantissa)
          {
               for (int i = 0; i < 9; ++i)
               {
                    const int p = 1 << i;
                    const int m = expOfmantissa & p;
                    if (m)
                         powOfmantissa *= powerof10[i];
               }
               powOfmantissa = 1.0 / powOfmantissa;
          }

          /*******************************
            Мы отсчитываем по 4 цифры с конца мантиссы и преобразовываем их в целое число
            только потом их сразу переводим в дробное для максимального уменьшения погрешности
            По 4 цифры потому, что в 16-разрядной версии может переполнится целое число.
           *******************************/
          double mantissa = 0.0;//, denum = 10.0;

          const int e = m_current.v_s.length;
          enum { MAX_COLLECT_DIGITS = 4 };

          if (start < e)
             for (i = e;; i -= MAX_COLLECT_DIGITS)
             {
                  int fNum = i - MAX_COLLECT_DIGITS;
                  int digNum = MAX_COLLECT_DIGITS;
                  if (fNum < start)
                  {
                       digNum = i - start;
                       fNum = start;
                  }
             
                  intResult = 0;
                  for (int j = 0; j < digNum; ++j)
                       intResult = intResult * 10 + (m_current.v_s.str[fNum + j] - '0');
             
                  mantissa += static_cast<double>(intResult);
                  mantissa *= mantissa_denum[digNum - 1];
             
                  if (fNum <= start)
                       break;
             }

          if (expOfmantissa)
               mantissa *= powOfmantissa;
          result += mantissa;
          //i = e;
     }

     /*
      *	Read in explicit exponent and calculate real exponent.
      */
     if (value && ((ch = curChar()) == 'E' || ch == 'e'))
     {
          m_current.v_s.expand();
          step_curChar();

          if ((sign = (curChar() == '-')) != 0 || curChar() == '+')
          {
               m_current.v_s.expand();
               step_curChar();
          }

          int j, count; /* Don t let exponent overflow */
          for (j = count = 0; is_digit(ch = curChar()); ++j)
          {
               count *= 10;
               count += ch - '0';   // проверить на переполнение! $$$
               m_current.v_s.expand();
               step_curChar();
          }

          if (sign != 0)
               exp -= count;
          else exp += count;
     }

     /*
      * ADJUST NUMBER BY POWERS OF TEN SPECIFIED BY FORMAT AND EXPONENT.
      */
     if (result != 0.0)
     {
          // check owerflow
          int countPow = 0;
          int correctExp = exp;
          double base10Pow = 1.0;
          while (fabs(result) * base10Pow > 2.0)
          {
               ++correctExp;
               base10Pow *= 0.1;
          }
          while (fabs(result) * base10Pow < 1.0)
          {
               --correctExp;
               base10Pow *= 10.0;
          }

          if (correctExp > MAX_DOUBLE_EXP || (correctExp == MAX_DOUBLE_EXP && fabs(result) * base10Pow > str_MAX_DOUBLE_BASE))
          {
               // процерка переполнений
               m_current.type = lex_ERROR;
               m_current.ok = false;
               m_current.v_errMsg = "Double owerflow";
               m_current.v_f = result;
               m_current.v_i = lex_ERR_DOUBLE_OWERLOW; // error code
               return;
          }
          else
               if (correctExp < MIN_DOUBLE_EXP)
               {
                    m_current.type = lex_ERROR;
                    m_current.ok = false;
                    m_current.v_errMsg = "Double underflow";
                    m_current.v_f = result;
                    m_current.v_i = lex_ERR_DOUBLE_UNDERFLOW;
                    return;
               }
               else
                    if (exp < 0)
                    {
                         exp = -exp;
                         for (int count = 0; exp; count++, exp >>= 1)
                              if (exp & 1)
                              {
                                   if (result < DBL_MIN/*str_MIN_DOUBLE*/ * powerof10[count])
                                   {
                                        result = 0;
                                        break;
                                   }
                                   else result /= powerof10[count];
                              }
                    }
                    else
                    {
                         for (int count = 0; exp; count++, exp >>= 1)
                              if (exp & 1)
                              {
                                   if (result > DBL_MAX/*str_MAX_DOUBLE*/ / powerof10[count])
                                   {
                                        result = DBL_MAX;//str_MAX_DOUBLE;
                                        break;
                                   }
                                   else result *= powerof10[count];
                              }
                    }
     }

     m_current.type = lex_FLOAT;
     m_current.ok = true;
     m_current.v_f = result;
     if (CStyle)
     {
          bool hasPostfix = false;
          while ((ch = curChar()) == 'f' || ch == 'F'
               || ch == 'l' || ch == 'L')
          {
               m_current.v_s.expand();
               step_curChar();
               hasPostfix = true;
          }
          if (hasPostfix && is_error_postfix("Error nubler postfix", lex_ERR_NUMBER_BAD_POSTFIX))
               return;
     }
}

bool TLexem::is_error_postfix(const char* errStr, int errCode)
{
     TChar ch = curChar();
     if (is_digit(curChar()) || is_letter(curChar()))
     {
          m_current.type = lex_ERROR;
          m_current.ok = false;
          m_current.v_f = 0;
          m_current.v_i = errCode;
          m_current.v_errMsg = errStr;
          return true;
     }
     return false;
}

bool TLexem::try_load_raw_string() {
     if (m_currentIdx >= m_text.length) return false;
     const TChar* p = m_text.const_char(m_currentIdx);
     // Проверяем префикс R" (допустим, только R" без u8 и т.п.)
     if (p[0] != U'R' || p[1] != U'\"')
          return false;

     // Ищем открывающую скобку после кавычки
     int delimStart = m_currentIdx + 2; // позиция сразу после R"
     int parenPos = -1;
     for (int i = delimStart; i < m_text.length; ++i) {
          TChar ch = m_text.str[i];
          if (ch == U'(') {
               parenPos = i;
               break;
          }
          // разделитель не может содержать пробельные символы, скобки, обратный слеш
          if (ch == U')' || ch == U'\\' || ch == U' ' || ch == U'\t' || ch == U'\n' || ch == U'\r')
               return false; // некорректный delimiter
     }
     if (parenPos == -1) return false;

     int delimLen = parenPos - delimStart;
     int contentStart = parenPos + 1;

     // Ищем закрывающую последовательность )delimiter"
     int searchPos = contentStart;
     while (searchPos < m_text.length) {
          if (m_text.str[searchPos] == U')' &&
              searchPos + 1 + delimLen < m_text.length &&
              m_text.str[searchPos + 1 + delimLen] == U'\"')
          {
               // проверяем совпадение разделителя
               bool match = true;
               for (int k = 0; k < delimLen; ++k) {
                    if (m_text.str[searchPos + 1 + k] != m_text.str[delimStart + k]) {
                         match = false;
                         break;
                    }
               }
               if (match) {
                    // успех – формируем токен строки
                    int contentLen = searchPos - contentStart;
                    m_current.set_pos(m_pos);
                    std::u32string* buf = m_stringStorage.alloc(contentLen);
                    buf->assign(m_text.str + contentStart, contentLen);
                    m_current.v_s.assign(buf->data(), contentLen);
                    m_current.type = lex_STRING;
                    m_current.ok = true;
                    // Продвигаем позицию за закрывающую кавычку
                    m_currentIdx = searchPos + 2 + delimLen; // ')', delimiter, '"'
                    // TODO: обновить m_pos с учётом переводов строк в содержимом
                    return true;
               }
          }
          ++searchPos;
     }

     // Если не нашли закрывающую – ошибка
     m_current.type = lex_ERROR;
     m_current.v_errMsg = "Unclosed raw string literal";
     m_current.ok = false;
     m_current.v_i = lex_ERR_UNCLOSED_STRING;
     m_currentIdx = m_text.length; // поглощаем остаток
     return true;
}

void TLexem::load_string()
{
     m_current.set_pos(m_pos);
     const TChar stringChar = curChar();
     m_current.v_i = static_cast<unsigned>(stringChar);
     step_curChar();

     // Начинаем в режиме «in-place»: v_s указывает прямо в m_text.
     // При первой встрече '\\' (CStyle) переключаемся на decode «на лету»,
     // копируя уже накопленные символы в новый буфер TStringStorage.
     // Один проход по строке, нет предварительного сканирования.

     m_current.v_s.assign(m_text.const_char(m_currentIdx), 0);
     std::u32string* buf = nullptr;   // nullptr - in-place режим
     bool closed = false;

     // Нужно ли обрабатывать escape?
     bool const handleEscape = (m_stringMode == StringMode::CSTYLE ||
                                m_stringMode == StringMode::PYTHON ||
                                m_stringMode == StringMode::CPP);


     while (m_currentIdx < m_text.length)   // ← индексный guard вместо curChar()!=0
     {
          const TChar ch = curChar();

          if (ch == stringChar)
          {
               step_curChar();
               closed = true;
               break;
          }

          if (handleEscape && ch == U'\\')
          {
               // первый escape – переключаемся на буфер
               if (!buf)
               {
                    // Верхняя оценка длины результата для reserve():
                    // уже накопленный in-place фрагмент + символы до закрывающей кавычки.
                    int approxLen = m_current.v_s.length;
                    for (int si = m_currentIdx; si < m_text.length; ++si)
                    {
                         if (m_text.str[si] == stringChar) break;
                         ++approxLen;
                    }
                    buf = m_stringStorage.alloc(approxLen);

                    // Копируем уже накопленный in-place фрагмент в buf
                    const TChar* inplaceStart = m_current.v_s.str;
                    for (int i = 0; i < m_current.v_s.length; ++i)
                         buf->push_back(inplaceStart[i]);
               }

               // Обработка escape-последовательности (универсальная для C/Python/C++)
               step_curChar(); // пропускаем '\\'
               if (m_currentIdx >= m_text.length)
               {
                    // Строка закончилась после '\' – считаем незакрытой
                    break;
               }

               TChar next = curChar();
               TChar decoded = U'\\';   // по умолчанию
               int additionalSteps = 0; // сколько ещё символов потребить (кроме уже съеденного '\\')
               bool valid = true;

               switch (next)
               {
               case U'n':  decoded = U'\n'; break;
               case U'r':  decoded = U'\r'; break;
               case U't':  decoded = U'\t'; break;
               case U'0':  decoded = U'\0'; break;
               case U'\\': decoded = U'\\'; break;
               case U'\'': decoded = U'\''; break;
               case U'\"': decoded = U'\"'; break;

               case U'x':
               {
                    // \xHH... все последующие hex-цифры
                    int pos = m_currentIdx + 1; // позиция после 'x'
                    unsigned long val = 0;
                    int cnt = 0;
                    while (pos < m_text.length)
                    {
                         TChar d = m_text.str[pos];
                         if (d >= U'0' && d <= U'9')
                              val = (val << 4) | (d - U'0');
                         else if (d >= U'a' && d <= U'f')
                              val = (val << 4) | (d - U'a' + 10);
                         else if (d >= U'A' && d <= U'F')
                              val = (val << 4) | (d - U'A' + 10);
                         else break;
                         ++cnt;
                         ++pos;
                    }
                    if (cnt == 0) { valid = false; break; }
                    if (val > 0x10FFFFUL) val = 0xFFFD; // замена
                    decoded = static_cast<TChar>(val);
                    additionalSteps = 0 + cnt; // 'x' + цифры
                    break;
               }
               case U'u':
               {
                    // \uHHHH ровно 4
                    unsigned long val = 0;
                    int i = 0;
                    for (; i < 4; ++i)
                    {
                         int idx = m_currentIdx + 1 + i;
                         if (idx >= m_text.length) break;
                         TChar d = m_text.str[idx];
                         if (d >= U'0' && d <= U'9')
                              val = (val << 4) | (d - U'0');
                         else if (d >= U'a' && d <= U'f')
                              val = (val << 4) | (d - U'a' + 10);
                         else if (d >= U'A' && d <= U'F')
                              val = (val << 4) | (d - U'A' + 10);
                         else break;
                    }
                    if (i == 4 && val <= 0x10FFFFUL)
                    {
                         decoded = static_cast<TChar>(val);
                         additionalSteps = 4; // 'u' + 4 цифры
                    }
                    else valid = false;
                    break;
               }
               case U'U':
               {
                    // \UHHHHHHHH ровно 8
                    unsigned long long val = 0;
                    int i = 0;
                    for (; i < 8; ++i)
                    {
                         int idx = m_currentIdx + 1 + i;
                         if (idx >= m_text.length) break;
                         TChar d = m_text.str[idx];
                         if (d >= U'0' && d <= U'9')
                              val = (val << 4) | (d - U'0');
                         else if (d >= U'a' && d <= U'f')
                              val = (val << 4) | (d - U'a' + 10);
                         else if (d >= U'A' && d <= U'F')
                              val = (val << 4) | (d - U'A' + 10);
                         else break;
                    }
                    if (i == 8 && val <= 0x10FFFFUL)
                    {
                         decoded = static_cast<TChar>(val);
                         additionalSteps = 8; // 'U' + 8 цифр
                    }
                    else valid = false;
                    break;
               }
               default:
               {
                    if (next >= U'0' && next <= U'7')
                    {
                         // восьмеричное 1..3 цифры
                         unsigned octal_val = next - U'0';
                         additionalSteps = 0; // уже 'next' съеден
                         for (int k = 1; k < 3; ++k)
                         {
                              int idx = m_currentIdx + k;
                              if (idx >= m_text.length) break;
                              TChar d = m_text.str[idx];
                              if (d >= U'0' && d <= U'7')
                              {
                                   octal_val = (octal_val << 3) | (d - U'0');
                                   ++additionalSteps;
                              }
                              else break;
                         }
                         if (octal_val > 255) octal_val = 255;
                         decoded = static_cast<TChar>(octal_val);
                    }
                    else
                    {
                         // неизвестный escape – оставляем оба символа
                         buf->push_back(U'\\');
                         buf->push_back(next);
                         step_curChar(); // next
                         continue; // идём к следующему символу строки
                    }
                    break;
               }
               } // switch

               if (valid)
               {
                    buf->push_back(decoded);
                    step_curChar();
                    // продвигаемся по escape-символам
                    for (int s = 0; s < additionalSteps; ++s)
                         if (m_currentIdx < m_text.length) step_curChar();
               }
               else
               {
                    // некорректная последовательность – оставляем '\\' и всё, что набрали?
                    // безопасный вариант: вывести '\\', а затем все символы, которые мы уже потребили?
                    buf->push_back(U'\\');
                    // мы уже шагнули с '\\', сейчас позиция на next – съедим и его
                    if (m_currentIdx < m_text.length) {
                         buf->push_back(curChar());
                         step_curChar();
                    }
     }
          }
          else
          {
               // обычный символ
               if (buf)
                    buf->push_back(ch);
               else
                    m_current.v_s.expand();
               step_curChar();
          }
     }

     if (!closed)
     {
          m_current.type = lex_ERROR;
          m_current.v_errMsg = "Unclosed string literal";
          m_current.ok  = false;
          m_current.v_f = 0;
          m_current.v_i = lex_ERR_UNCLOSED_STRING;
          m_current.v_errMsg = "Unclosed string literal";
          return;
     }

     if (buf)
          m_current.v_s.assign(buf->data(), static_cast<int>(buf->size()));
     // else: v_s уже указывает на in-place фрагмент m_text

     m_current.type = lex_STRING;
     m_current.ok = true;
}

int TLexem::match_ex_symbol_len(const TChar* buf, int len) const
{
     if (len <= 0) return 0;
     const TChar firstCh = buf[0];
     const int sz = static_cast<int>(m_exSymbols.size());
     for (int idx = 0; idx < sz; )
     {
          const TExSym& e = m_exSymbols[idx];
          const TChar eCh = e.buff[0];
          if (eCh > firstCh) break;                       // отсортировано - дальше не подойдут
          if (eCh < firstCh) { idx = e.nextGroup; continue; }
          const int groupEnd = idx + e.groupCount;
          for (int k = idx; k < groupEnd; ++k)             // длинные раньше коротких -> первый матч самый длинный
          {
               const TExSym& s = m_exSymbols[k];
               if (s.sym.length <= len && s.sym.equ_prefix(buf))
                    return s.sym.length;
          }
          break;
     }
     return 0;
}

void TLexem::load_symbol()
{
     m_current.set_pos(m_pos);
     const auto* p = m_text.const_char(m_currentIdx);
     const TChar firstCh = p[0];   // первый символ входа

     // m_exSymbols отсортирован лексикографически; внутри группы с одинаковой
     // первой буквой длинные символы идут раньше коротких.
     // nextGroup/groupCount заполнены в end_of_init().
     const int sz = static_cast<int>(m_exSymbols.size());
     for (int idx = 0; idx < sz; )
     {
          const TExSym& e = m_exSymbols[idx];
          const TChar eCh = e.buff[0];

          if (eCh > firstCh)
               break;   // символы отсортированы: дальше уже не подойдут

          if (eCh < firstCh)
          {
               // вся группа не совпадает - прыгаем на следующую группу
               idx = e.nextGroup;
               continue;
          }

          // eCh == firstCh: проверяем только groupCount элементов этой группы
          const int groupEnd = idx + e.groupCount;
          for (int k = idx; k < groupEnd; ++k)
          {
               const TExSym& s = m_exSymbols[k];
               if (s.sym.equ_prefix(p))
               {
                    m_current.type = lex_SYMBOL;
                    m_current.ok   = true;
                    m_current.v_s  = s.sym;
                    m_current.v_i  = s.code;
                    for (int j = 0; j < s.sym.length; ++j)
                         step_curChar();
                    return;
               }
          }
          // группа проверена, совпадений нет
          break;
     }

     // Расширенный символ не найден - однобуквенный символ
     m_current.type = lex_SYMBOL;
     m_current.ok   = true;
     m_current.v_s.assign(p, 1);
     m_current.v_i  = curChar();
     step_curChar();
}

void TLexem::load_word()
{
     m_current.set_pos(m_pos);
     auto ch = curChar();
     m_current.v_s.assign(m_text.const_char(m_currentIdx), 0);

     // UTF-8: continuation bytes (0x80..0xBF) pass is_letter(), so
     // multi-byte sequences are accumulated whole into one lex_WORD token.
     while (is_letter(ch))
     {
          step_curChar();
          m_current.v_s.expand();
          ch = curChar();
     }
     m_current.type = lex_WORD;
     m_current.ok = true;
     m_current.v_i = 0;
}

void TLexem::next_lex()
{
     m_current.ok = false;
     bool doRep = true;
     while (doRep)
     {
          doRep = false;
          TChar c;
          // пропуск пробельных элементов
          while (m_currentIdx < m_text.length
               && ((c = curChar()) == '\t'
                    || c == '\r'
                    || c == ' '
                    || (c == '\n' && m_skipEOL)
                    )
               )
               step_curChar();

          if (!m_endOfLineComment.empty()
               && m_endOfLineComment.equ_prefix(m_text.const_char(m_currentIdx)))
          {
               doRep = true;
               m_currentIdx += m_endOfLineComment.length;
               while (m_currentIdx < m_text.length)
               {
                    if (curChar() == '\n')
                    {
                         step_curChar();
                         break;
                    }
                    step_curChar();
               }
          }
          else
               if (!m_openComment.empty()
                    && m_openComment.equ_prefix(m_text.const_char(m_currentIdx)))
               {
                    doRep = true;
                    m_currentIdx += m_openComment.length;
                    while (m_currentIdx < m_text.length)
                    {
                         if (m_closeComment.equ_prefix(m_text.const_char(m_currentIdx)))
                         {
                              m_currentIdx += m_closeComment.length;
                              break;
                         }
                         step_curChar();
                    }
               }
     }
     if (m_currentIdx >= m_text.length)
     {
          m_current.type = lex_EOF;
          m_current.ok = true;
          return;
     }

     // ---------- проверка сырой строки (R") ----------
     if (m_text.str[m_currentIdx] == U'R' && m_text.str[m_currentIdx+1] == U'\"') {
          if (try_load_raw_string())
               return;
     }

     // ---------- основная диспетчеризация ----------
     switch (m_text.str[m_currentIdx])
     {
     case '\n':
          step_curChar();
          m_current.type = lex_ENDOFLINE;
          m_current.ok = true;
          break;
     case '0': case '1': case '2': case '3': case '4':
     case '5': case '6': case '7': case '8': case '9':
          load_constant();
          break;
     case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
     case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
     case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
     case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
     case 'Y': case 'Z':
     case '_':
     case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
     case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
     case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
     case 's': case 't': case 'u': case 'v': case 'w': case 'x':
     case 'y': case 'z':
          load_word();
          break;

     case '"': case '\'':
          if (m_quoteAsSym)
               load_symbol();
          else
               load_string();
          break;
     default:
          // UTF-8 leading bytes (0xC0..0xFF) start a multi-byte word sequence.
          // Continuation bytes (0x80..0xBF) should not appear here in well-formed
          // UTF-8 (they are consumed inside load_word), but we route them to
          // load_word() as well to avoid crashing on malformed input.
          // Extended first-word chars (ex_word_symbols_first) land here too.
          {
          const TChar cc = m_text.str[m_currentIdx];
          if (cc < 128 && m_isletter.letter[cc])
               load_word();
          else
               load_symbol();
          }
     }
}

int TLexem::add_ex_exSym(const TChar *exSym, int code) // <= >= и т.д.
{
     // Считаем длину; MAX_SYMBOL-1 - максимум (последний слот под нулевой терминатор)
     int len = 0;
     for (; exSym[len]; ++len);
     if (len == 0)
     {
          m_error->error(TPos(0, 0), lex_ERR_ADD_EMPTY_SYMBOL, "add_ex_exSym: Add empty symnol [", code,"]");
          return -1;
     }
     if (len > MAX_SYMBOL - 1)
     {
          m_error->error(TPos(0, 0), lex_ERROR_ADD_TOO_LONG_SYMBOL, "add_ex_exSym: Add too long symbol(", exSym, ")");
          return -1;
     }

     const int curIdx = static_cast<int>(m_exSymbols.size());
     m_exSymbols.push_back(TExSym(code));
     TExSym& s = m_exSymbols[curIdx];
     copy_chars(s.buff, exSym, len);
     s.buff[len] = U'\0';         // явный нулевой терминатор
     s.sym.length = len;
     return code;
}
int TLexem::add_ex_exSym(const char* exSym, int code)
{
     int len = 0;
     for (; exSym[len]; ++len);
     if (len == 0)
     {
          m_error->error(TPos(0, 0), lex_ERR_ADD_EMPTY_SYMBOL, "add_ex_exSym: Add empty symnol [", code, "]");
          return -1;
     }
     if (len > MAX_SYMBOL - 1)
     {
          m_error->error(TPos(0, 0), lex_ERROR_ADD_TOO_LONG_SYMBOL, "add_ex_exSym: Add too long symbol(", exSym, ")");
          return -1;
     }

     const int curIdx = static_cast<int>(m_exSymbols.size());
     m_exSymbols.push_back(TExSym(code));
     TExSym& s = m_exSymbols[curIdx];
     // copy_chars(TChar*, const char*, int) определён в классе - копирует побайтно (ASCII)
     copy_chars(s.buff, exSym, len);
     s.buff[len] = U'\0';         // явный нулевой терминатор
     s.sym.length = len;
     return code;
}


int TLexem::get_sym_code(const TChar* s) const// <= >= и т.д.
{
     for (auto& i : m_exSymbols)
          if (i.sym.equ(s))
               return i.code;
     return -1;
}

int  TLexem::get_sym_code(const char* s) const
{
     for (auto& i : m_exSymbols)
          if (i.sym.equ(s))
               return i.code;
     return -1;
}

const TChar* TLexem::get_sym_str(int code) const
{
     for (auto& i : m_exSymbols)
          if (i.code == code)
               return i.sym.const_char(0);
     return nullptr;
}

void TLexem::end_of_init()
{
     // Сортируем лексикографически (длинные префиксы идут раньше коротких в группе)
     std::stable_sort(m_exSymbols.begin(), m_exSymbols.end());

     // Заполняем nextGroup и groupCount для ускорения поиска в load_symbol().
     // Группа - все символы с одинаковой первой буквой (buff[0]).
     // После сортировки они идут подряд.
     const int sz = static_cast<int>(m_exSymbols.size());
     int i = 0;
     while (i < sz)
     {
          const TChar firstCh = m_exSymbols[i].sym.length > 0 ? m_exSymbols[i].buff[0] : U'\0';
          int j = i + 1;
          while (j < sz)
          {
               const TChar ch = m_exSymbols[j].sym.length > 0 ? m_exSymbols[j].buff[0] : U'\0';
               if (ch != firstCh) break;
               ++j;
          }
          const int count = j - i;
          for (int k = i; k < j; ++k)
          {
               m_exSymbols[k].groupCount = count;
               m_exSymbols[k].nextGroup  = j;
          }
          i = j;
     }

     m_endOfLineComment.assign(endOfLineComment);
     m_openComment.assign(openComment);
     m_closeComment.assign(closeComment);
     next_lex();
}


const char32_t TTextStorage::cp1251_to_unicode[256] = {
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
    0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
    0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
    0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,
    0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F,
    0x0402, 0x0403, 0x201A, 0x0453, 0x201E, 0x2026, 0x2020, 0x2021, 0x20AC, 0x2030, 0x0409, 0x2039, 0x040A, 0x040C, 0x040B, 0x040F,
    0x0452, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014, 0xFFFD, 0x2122, 0x0459, 0x203A, 0x045A, 0x045C, 0x045B, 0x045F,
    0x00A0, 0x040E, 0x045E, 0x0408, 0x00A4, 0x0490, 0x00A6, 0x00A7, 0x0401, 0x00A9, 0x0404, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x0407,
    0x00B0, 0x00B1, 0x0406, 0x0456, 0x0491, 0x00B5, 0x00B6, 0x00B7, 0x0451, 0x2116, 0x0454, 0x00BB, 0x0458, 0x0405, 0x0455, 0x0457,
    0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417, 0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
    0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427, 0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
    0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437, 0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
    0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447, 0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F,
};

const char32_t TTextStorage::koi8r_to_unicode[256] = {
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
    0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
    0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
    0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,
    0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F,
    0x2500, 0x2502, 0x250C, 0x2510, 0x2514, 0x2518, 0x251C, 0x2524, 0x252C, 0x2534, 0x253C, 0x2580, 0x2584, 0x2588, 0x258C, 0x2590,
    0x2591, 0x2592, 0x2593, 0x2320, 0x25A0, 0x2219, 0x221A, 0x2248, 0x2264, 0x2265, 0x00A0, 0x2321, 0x00B0, 0x00B2, 0x00B7, 0x00F7,
    0x2550, 0x2551, 0x2552, 0x0451, 0x2553, 0x2554, 0x2555, 0x2556, 0x2557, 0x2558, 0x2559, 0x255A, 0x255B, 0x255C, 0x255D, 0x255E,
    0x255F, 0x2560, 0x2561, 0x0401, 0x2562, 0x2563, 0x2564, 0x2565, 0x2566, 0x2567, 0x2568, 0x2569, 0x256A, 0x256B, 0x256C, 0x00A9,
    0x044E, 0x0430, 0x0431, 0x0446, 0x0434, 0x0435, 0x0444, 0x0433, 0x0445, 0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E,
    0x043F, 0x044F, 0x0440, 0x0441, 0x0442, 0x0443, 0x0436, 0x0432, 0x044C, 0x044B, 0x0437, 0x0448, 0x044D, 0x0449, 0x0447, 0x044A,
    0x042E, 0x0410, 0x0411, 0x0426, 0x0414, 0x0415, 0x0424, 0x0413, 0x0425, 0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E,
    0x041F, 0x042F, 0x0420, 0x0421, 0x0422, 0x0423, 0x0416, 0x0412, 0x042C, 0x042B, 0x0417, 0x0428, 0x042D, 0x0429, 0x0427, 0x042A,
};

// ==================== Реализация приватных конвертеров ====================

void TTextStorage::from_ascii_bytes(const char* data, size_t len) {
     m_text32.clear();
     m_text32.reserve(len);
     for (size_t i = 0; i < len; ++i) {
          unsigned char b = static_cast<unsigned char>(data[i]);
          m_text32.push_back(b < 0x80 ? static_cast<char32_t>(b) : 0xFFFD);
     }
}

void TTextStorage::from_cp_bytes(const char* data, size_t len, const char32_t* table) {
     m_text32.clear();
     m_text32.reserve(len);
     for (size_t i = 0; i < len; ++i) {
          unsigned char b = static_cast<unsigned char>(data[i]);
          m_text32.push_back(table[b]);
     }
}

void TTextStorage::from_utf8_bytes(const char* data, size_t len) {
     m_text32.clear();
     m_text32.reserve(len);                     // грубая оценка
     const uint8_t* s = reinterpret_cast<const uint8_t*>(data);

     size_t i = 0;
     if (len >= 3 && s[0] == 0xEF && s[1] == 0xBB && s[2] == 0xBF) i = 3;  // пропустить UTF-8 BOM
     for (; i < len;) {
          uint8_t byte = s[i];

          if (byte < 0x80) {                      // 1 байт
               m_text32.push_back(byte);
               ++i;
               continue;
          }

          if ((byte & 0xE0) == 0xC0) {            // 2 байта
               if (i + 1 >= len || (s[i + 1] & 0xC0) != 0x80) {
                    m_text32.push_back(0xFFFD);
                    ++i;
                    continue;
               }
               char32_t ch = ((byte & 0x1F) << 6) | (s[i + 1] & 0x3F);
               if (ch < 0x80) ch = 0xFFFD;         // overlong
               m_text32.push_back(ch);
               i += 2;
               continue;
          }

          if ((byte & 0xF0) == 0xE0) {            // 3 байта
               if (i + 2 >= len ||
                    (s[i + 1] & 0xC0) != 0x80 ||
                    (s[i + 2] & 0xC0) != 0x80) {
                    m_text32.push_back(0xFFFD);
                    ++i;
                    continue;
               }
               char32_t ch = ((byte & 0x0F) << 12) |
                    ((s[i + 1] & 0x3F) << 6) |
                    (s[i + 2] & 0x3F);
               if (ch < 0x800 || (ch >= 0xD800 && ch <= 0xDFFF))
                    ch = 0xFFFD;                    // overlong / surrogate
               m_text32.push_back(ch);
               i += 3;
               continue;
          }

          if ((byte & 0xF8) == 0xF0) {            // 4 байта
               if (i + 3 >= len ||
                    (s[i + 1] & 0xC0) != 0x80 ||
                    (s[i + 2] & 0xC0) != 0x80 ||
                    (s[i + 3] & 0xC0) != 0x80) {
                    m_text32.push_back(0xFFFD);
                    ++i;
                    continue;
               }
               char32_t ch = ((byte & 0x07) << 18) |
                    ((s[i + 1] & 0x3F) << 12) |
                    ((s[i + 2] & 0x3F) << 6) |
                    (s[i + 3] & 0x3F);
               if (ch < 0x10000 || ch > 0x10FFFF)
                    ch = 0xFFFD;
               m_text32.push_back(ch);
               i += 4;
               continue;
          }

          // неверный стартовый байт
          m_text32.push_back(0xFFFD);
          ++i;
     }
}


void TTextStorage::assign(const char* data, TEncoding code) {
     if (!data || !*data) {
          m_text32.clear();
          return;
     }
     size_t len = std::strlen(data);

     switch (code) {
     case TEncoding::ASCII:   from_ascii_bytes(data, len); break;
     case TEncoding::UTF8:    from_utf8_bytes(data, len); break;
     case TEncoding::Win1251: from_cp_bytes(data, len, cp1251_to_unicode); break;
     case TEncoding::KOI8R:   from_cp_bytes(data, len, koi8r_to_unicode); break;
     }
}

void TTextStorage::assign(const std::string& data, TEncoding code) {
     if (data.empty()) {
          m_text32.clear();
          return;
     }
     const char* ptr = data.data();
     size_t len = data.size();

     switch (code) {
     case TEncoding::ASCII:   from_ascii_bytes(ptr, len); break;
     case TEncoding::UTF8:    from_utf8_bytes(ptr, len); break;
     case TEncoding::Win1251: from_cp_bytes(ptr, len, cp1251_to_unicode); break;
     case TEncoding::KOI8R:   from_cp_bytes(ptr, len, koi8r_to_unicode); break;
     }
}

void TTextStorage::assign(const char32_t* data, int length) {
     if (!data || length <= 0) {
          m_text32.clear();
          return;
     }
     m_text32.assign(data, data + length);
}

void TTextStorage::assign_from_utf8(const char* utf8_data) {
     if (!utf8_data || !*utf8_data) {
          m_text32.clear();
          return;
     }
     from_utf8_bytes(utf8_data, std::strlen(utf8_data));
}

void TTextStorage::assign_from_utf8(const std::string& utf8_data) {
     if (utf8_data.empty()) {
          m_text32.clear();
          return;
     }
     from_utf8_bytes(utf8_data.data(), utf8_data.size());
}


///////////////////////////

// Статический декодер: преобразует null-terminated UTF-8 строку в массив char32_t.
// Результат записывается в out[0..maxOut-1], нулевой терминатор добавляется.
// Возвращает число записанных символов (без терминатора).
// Динамической памяти не использует.
namespace {
int utf8_to_char32_static(const char* utf8, TChar* out, int maxOut)
{
     const uint8_t* s = reinterpret_cast<const uint8_t*>(utf8);
     int n = 0;
     while (*s && n < maxOut)
     {
          uint8_t b = s[0];
          char32_t cp;
          if (b < 0x80)
          {
               cp = b; s += 1;
          }
          else if ((b & 0xE0) == 0xC0 && (s[1] & 0xC0) == 0x80)
          {
               cp = ((b & 0x1F) << 6) | (s[1] & 0x3F);
               if (cp < 0x80) cp = 0xFFFD;
               s += 2;
          }
          else if ((b & 0xF0) == 0xE0 && (s[1] & 0xC0) == 0x80 && (s[2] & 0xC0) == 0x80)
          {
               cp = ((b & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F);
               if (cp < 0x800 || (cp >= 0xD800 && cp <= 0xDFFF)) cp = 0xFFFD;
               s += 3;
          }
          else if ((b & 0xF8) == 0xF0 && (s[1] & 0xC0) == 0x80 && (s[2] & 0xC0) == 0x80 && (s[3] & 0xC0) == 0x80)
          {
               cp = ((b & 0x07) << 18) | ((s[1] & 0x3F) << 12) | ((s[2] & 0x3F) << 6) | (s[3] & 0x3F);
               if (cp < 0x10000 || cp > 0x10FFFF) cp = 0xFFFD;
               s += 4;
          }
          else
          {
               cp = 0xFFFD; s += 1;
          }
          out[n++] = cp;
     }
     out[n] = U'\0';
     return n;
}
} // namespace

int TLexem::add_ex_exSym_utf8(const char* utf8Sym, int code)
{
     // Декодируем прямо в стековый буфер - никакой динамической памяти
     TChar tmp[MAX_SYMBOL];
     utf8_to_char32_static(utf8Sym, tmp, MAX_SYMBOL - 1);
     return add_ex_exSym(tmp, code);
}


// ============================================================
//  Минимальный фреймворк
// ============================================================
namespace {
int g_total = 0;
int g_passed = 0;
int g_failed = 0;
} // namespace

#include <string>


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

#if 0

// ============================================================
//  Тесты
// ============================================================

// ------------------------------------------------------------
void test_empty_string()
{
     SECTION("пустая строка");

     {
          TLexSession s;
          s.feed("\"\"");
          TLex lx = s.cur();
          CHECK(lx.type == lex_STRING, "тип lex_STRING");
          CHECK(lx.v_i == '"', "кавычка - двойная");
          CHECK(lx.v_s.length == 0, "длина == 0");
          CHECK(str_of(lx) == "", "содержимое пустое");
     }

     {
          TLexSession s;
          s.feed("''");
          TLex lx = s.cur();
          CHECK(lx.type == lex_STRING, "тип lex_STRING (одинарная)");
          CHECK(lx.v_i == '\'', "кавычка - одинарная");
          CHECK(lx.v_s.length == 0, "длина == 0");
     }
}

// ------------------------------------------------------------
void test_simple_strings()
{
     SECTION("простые строки без escape");

     {
          TLexSession s;
          s.feed("\"hello\"");
          TLex lx = s.cur();
          CHECK(lx.type == lex_STRING, "тип lex_STRING");
          CHECK(str_of(lx) == "hello", "содержимое 'hello'");
          CHECK(lx.v_s.length == 5, "длина 5");
     }
     {
          TLexSession s;
          s.feed("\"x\"");
          TLex lx = s.cur();
          CHECK(lx.type == lex_STRING, "тип lex_STRING (один символ)");
          CHECK(str_of(lx) == "x", "содержимое 'x'");
          CHECK(lx.v_s.length == 1, "длина 1");
     }
     {
          TLexSession s;
          s.feed("\"hello world\"");
          TLex lx = s.cur();
          CHECK(lx.type == lex_STRING, "тип lex_STRING (пробелы)");
          CHECK(str_of(lx) == "hello world", "содержимое с пробелом");
     }
     {
          TLexSession s;
          s.feed("\"abc 123!@#\"");
          TLex lx = s.cur();
          CHECK(lx.type == lex_STRING, "тип lex_STRING (смешанное)");
          CHECK(str_of(lx) == "abc 123!@#", "содержимое смешанное");
     }
     {
          std::string long_str(100, 'A');
          std::string src = "\"" + long_str + "\"";
          TLexSession s;
          s.feed(src.c_str());
          TLex lx = s.cur();
          CHECK(lx.type == lex_STRING, "тип lex_STRING (длинная)");
          CHECK(str_of(lx) == long_str, "содержимое длинной строки");
          CHECK(lx.v_s.length == 100, "длина 100");
     }
}

// ------------------------------------------------------------
void test_string_position()
{
     SECTION("позиция токена строки");

     {
          TLexSession s;
          s.feed("\"hi\"");
          TLex lx = s.cur();
          CHECK(lx.col == 1 && lx.lin == 1, "позиция (1,1)");
     }
     {
          TLexSession s;
          s.feed("abc \"hi\"");
          auto toks = s.all_tokens();
          CHECK(toks.size() >= 2, "есть два токена");
          if (toks.size() >= 2) {
               CHECK(toks[1].type == lex_STRING, "второй - STRING");
               CHECK(toks[1].col == 5, "col строки == 5");
               CHECK(toks[1].lin == 1, "lin строки == 1");
          }
     }
}

// ------------------------------------------------------------
void test_escape_sequences()
{
     SECTION("escape-последовательности (CStyle=true)");

     struct EscCase {
          const char* src;
          const char* expected;
          int         expLen;
     };

     EscCase cases[] = {
         { R"("\n")",     "\n",   1 },
         { R"("\r")",     "\r",   1 },
         { R"("\t")",     "\t",   1 },
         { "\"\\\\\"",   "\\",   1 },
         { "\"\\\"\"",   "\"",   1 },
         { R"('\'')",     "'",    1 },
         { R"("\n\t")",   "\n\t", 2 },
         { R"("a\nb")",   "a\nb", 3 },
         { R"("\n\n\n")", "\n\n\n", 3 },
     };

     for (auto& c : cases)
     {
          TLexSession s(/*cstyle=*/true);
          s.feed(c.src);
          TLex lx = s.cur();
          CHECK(lx.type == lex_STRING, std::string("тип STRING: ").append(c.src).c_str());
          CHECK(lx.v_s.length == c.expLen, std::string("длина: ").append(c.src).c_str());

          bool contentOk = true;
          for (int i = 0; i < c.expLen && i < lx.v_s.length; ++i)
          {
               if (lx.v_s.str[i] != static_cast<char32_t>(static_cast<unsigned char>(c.expected[i])))
               {
                    contentOk = false; break;
               }
          }
          CHECK(contentOk, std::string("содержимое: ").append(c.src).c_str());
     }
}

// ------------------------------------------------------------
void test_escape_null()
{
     SECTION("escape \\0 - нулевой символ внутри строки");

     TLexSession s(/*cstyle=*/true);
     s.feed(R"("a\0b")");
     TLex lx = s.cur();
     CHECK(lx.type == lex_STRING, "тип STRING");
     CHECK(lx.v_s.length == 3, "длина 3 (нуль - не терминатор строки)");
     if (lx.v_s.length == 3) {
          CHECK(lx.v_s.str[0] == U'a', "str[0] == 'a'");
          CHECK(lx.v_s.str[1] == U'\0', "str[1] == U'\\0'");
          CHECK(lx.v_s.str[2] == U'b', "str[2] == 'b'");
     }
}

// ------------------------------------------------------------
void test_escape_unknown()
{
     SECTION("неизвестный escape - оба символа остаются");

     TLexSession s(/*cstyle=*/true);
     s.feed(R"("\z")");
     TLex lx = s.cur();
     CHECK(lx.type == lex_STRING, "тип STRING");
     CHECK(lx.v_s.length == 2, "длина 2 (backslash + z)");
     if (lx.v_s.length == 2) {
          CHECK(lx.v_s.str[0] == U'\\', "str[0] == '\\\\'");
          CHECK(lx.v_s.str[1] == U'z', "str[1] == 'z'");
     }
}

// ------------------------------------------------------------
void test_no_escape_without_cstyle()
{
     SECTION("escape НЕ декодируется без CStyle");

     // В не-CStyle режиме '\' остаётся как есть - 4 символа: a \ n b
     TLexSession s(/*cstyle=*/false);
     s.feed("\"a\\nb\"");
     TLex lx = s.cur();
     CHECK(lx.type == lex_STRING, "тип STRING");
     CHECK(lx.v_s.length == 4, "длина 4 (no escape decoding)");
     if (lx.v_s.length == 4) {
          CHECK(lx.v_s.str[1] == U'\\', "str[1] == '\\\\'");
          CHECK(lx.v_s.str[2] == U'n', "str[2] == 'n'");
     }
}

// ------------------------------------------------------------
void test_escaped_quote_terminates_correctly()
{
     SECTION("экранированная кавычка не завершает строку");

     TLexSession s(/*cstyle=*/true);
     s.feed("\"say \\\"hi\\\"\"");
     TLex lx = s.cur();
     CHECK(lx.type == lex_STRING, "тип STRING");
     CHECK(str_of(lx) == "say \"hi\"", "содержимое: say \"hi\"");
}

// ------------------------------------------------------------
void test_multiple_strings_in_sequence()
{
     SECTION("несколько строк подряд");

     TLexSession s;
     s.feed("\"one\" \"two\" \"three\"");
     auto toks = s.all_tokens();
     CHECK(toks.size() == 3, "три токена");
     if (toks.size() == 3) {
          CHECK(toks[0].type == lex_STRING, "токен 0 - STRING");
          CHECK(str_of(toks[0]) == "one", "токен 0 содержимое");
          CHECK(toks[1].type == lex_STRING, "токен 1 - STRING");
          CHECK(str_of(toks[1]) == "two", "токен 1 содержимое");
          CHECK(toks[2].type == lex_STRING, "токен 2 - STRING");
          CHECK(str_of(toks[2]) == "three", "токен 2 содержимое");
     }
}

// ------------------------------------------------------------
void test_string_followed_by_other_tokens()
{
     SECTION("строка среди других токенов");

     TLexSession s;
     s.feed("key \"value\" 42");
     auto toks = s.all_tokens();
     CHECK(toks.size() == 3, "три токена");
     if (toks.size() == 3) {
          CHECK(toks[0].type == lex_WORD, "токен 0 - WORD");
          CHECK(toks[1].type == lex_STRING, "токен 1 - STRING");
          CHECK(str_of(toks[1]) == "value", "строка == 'value'");
          CHECK(toks[2].type == lex_INTEGER, "токен 2 - INTEGER");
     }
}

// ------------------------------------------------------------
void test_string_single_quotes()
{
     SECTION("строки в одинарных кавычках");

     TLexSession s;
     s.feed("'hello'");
     TLex lx = s.cur();
     CHECK(lx.type == lex_STRING, "тип STRING");
     CHECK(lx.v_i == '\'', "кавычка одинарная");
     CHECK(str_of(lx) == "hello", "содержимое 'hello'");
}

// ------------------------------------------------------------
void test_unclosed_string()
{
     SECTION("незакрытая строка - доходим до EOF");

     {
          TLexSession s;
          s.feed("\"unclosed");
          TLex lx = s.cur();
          CHECK(lx.type == lex_STRING, "тип STRING (незакрытая)");
          CHECK(str_of(lx) == "unclosed", "содержимое до EOF");
          s.lex.next_lex();
          CHECK(s.lex.is_eof(), "после незакрытой строки - EOF");
     }
     {
          TLexSession s;
          s.feed("\"");
          TLex lx = s.cur();
          CHECK(lx.type == lex_STRING, "тип STRING (только открывающая кавычка)");
          CHECK(lx.v_s.length == 0, "длина 0");
     }
     {
          TLexSession s(/*cstyle=*/true);
          s.feed("\"abc\\n");
          TLex lx = s.cur();
          if (lx)
               printf("тип STRING (незакрытая с escape) не сработала ошибка\n");
     }
}

// ------------------------------------------------------------
void test_string_with_only_escape()
{
     SECTION("строка только из escape-символов");

     TLexSession s(/*cstyle=*/true);
     s.feed(R"("\n\t\r")");
     TLex lx = s.cur();
     CHECK(lx.type == lex_STRING, "тип STRING");
     CHECK(lx.v_s.length == 3, "длина 3");
     if (lx.v_s.length == 3) {
          CHECK(lx.v_s.str[0] == U'\n', "str[0] == \\n");
          CHECK(lx.v_s.str[1] == U'\t', "str[1] == \\t");
          CHECK(lx.v_s.str[2] == U'\r', "str[2] == \\r");
     }
}

// ------------------------------------------------------------
void test_string_reuse_after_reassign()
{
     SECTION("повторный assign - хранилище очищается, новый документ независим");

     TLexSession s(/*cstyle=*/true);

     s.feed("\"first\"");
     TLex lx1 = s.cur();
     std::string content1 = str_of(lx1);  // копируем до переназначения

     s.feed("\"second\"");                 // feed повторно - reinit внутри
     TLex lx2 = s.cur();

     // content1 - копия std::string, не зависит от хранилища
     CHECK(content1 == "first", "сохранённое содержимое первого");
     CHECK(lx2.type == lex_STRING, "второй токен - STRING");
     CHECK(str_of(lx2) == "second", "содержимое второго");
}

// ------------------------------------------------------------
void test_string_storage_pointer_stability()
{
     SECTION("указатели из хранилища стабильны после выдачи нескольких строк");

     TLexSession s(/*cstyle=*/true);
     s.feed(R"("a\nb" "c\td" "e\\f")");
     auto toks = s.all_tokens();

     CHECK(toks.size() == 3, "три строковых токена");
     if (toks.size() == 3) {
          CHECK(toks[0].type == lex_STRING, "токен 0 STRING");
          CHECK(toks[1].type == lex_STRING, "токен 1 STRING");
          CHECK(toks[2].type == lex_STRING, "токен 2 STRING");
          CHECK(toks[0].v_s.length == 3, "токен 0 длина 3");
          CHECK(toks[1].v_s.length == 3, "токен 1 длина 3");
          CHECK(toks[2].v_s.length == 3, "токен 2 длина 3");
          // a \n b
          CHECK(toks[0].v_s.str[0] == U'a', "t0[0]=='a'");
          CHECK(toks[0].v_s.str[1] == U'\n', "t0[1]=='\\n'");
          CHECK(toks[0].v_s.str[2] == U'b', "t0[2]=='b'");
          // c \t d
          CHECK(toks[1].v_s.str[0] == U'c', "t1[0]=='c'");
          CHECK(toks[1].v_s.str[1] == U'\t', "t1[1]=='\\t'");
          CHECK(toks[1].v_s.str[2] == U'd', "t1[2]=='d'");
          // e \\ f
          CHECK(toks[2].v_s.str[0] == U'e', "t2[0]=='e'");
          CHECK(toks[2].v_s.str[1] == U'\\', "t2[1]=='\\\\'");
          CHECK(toks[2].v_s.str[2] == U'f', "t2[2]=='f'");
     }
}

// ------------------------------------------------------------
void test_string_equ()
{
     SECTION("TStr::equ для строк из лексера");

     TLexSession s;
     s.feed("\"hello\"");
     TLex lx = s.cur();
     CHECK(lx.v_s.equ("hello"), "equ(char*)");
     CHECK(lx.v_s.equ(std::string("hello")), "equ(std::string)");
     CHECK(!lx.v_s.equ("hell"), "!equ(короткий)");
     CHECK(!lx.v_s.equ("helloo"), "!equ(длинный)");
     CHECK(!lx.v_s.equ("Hello"), "!equ(другой регистр)");
}

// ------------------------------------------------------------
void test_very_long_escape_string()
{
     SECTION("длинная строка с многочисленными escape");

     std::string src = "\"";
     for (int i = 0; i < 50; ++i) src += "a\\n";
     src += "\"";

     TLexSession s(/*cstyle=*/true);
     s.feed(src.c_str());
     TLex lx = s.cur();
     CHECK(lx.type == lex_STRING, "тип STRING (длинная с escape)");
     CHECK(lx.v_s.length == 100, "длина 100 (50 * 'a' + 50 * '\\n')");
     if (lx.v_s.length == 100) {
          bool ok = true;
          for (int i = 0; i < 100; i += 2) {
               if (lx.v_s.str[i] != U'a') { ok = false; break; }
               if (lx.v_s.str[i + 1] != U'\n') { ok = false; break; }
          }
          CHECK(ok, "чередование 'a' и '\\n'");
     }
}

// ------------------------------------------------------------
void test_adjacent_escape_at_end()
{
     SECTION("escape в конце строки перед закрывающей кавычкой");

     TLexSession s(/*cstyle=*/true);
     s.feed("\"test\\n\"");
     TLex lx = s.cur();
     CHECK(lx.type == lex_STRING, "тип STRING");
     CHECK(lx.v_s.length == 5, "длина 5 (test + \\n)");
     if (lx.v_s.length == 5)
          CHECK(lx.v_s.str[4] == U'\n', "последний символ \\n");
}

// ------------------------------------------------------------
void test_string_quote_char_stored()
{
     SECTION("v_i хранит символ-кавычку");

     {
          TLexSession s;
          s.feed("\"test\"");
          CHECK(s.cur().v_i == '"', "двойная кавычка в v_i");
     }
     {
          TLexSession s;
          s.feed("'test'");
          CHECK(s.cur().v_i == '\'', "одинарная кавычка в v_i");
     }
}

void test_expected_str()
{
     //TLex expected_str()
     TLexem lex;
     lex.assign(U"  'hello world'");
     lex.end_of_init();
     TLex lx = lex.expected_str();
     if (!lx || !lx.v_s.equ("hello world"))
          printf("Error");
}

#include <iostream>
int main0()
{
     test_empty_string();
     test_simple_strings();
     test_string_position();
     test_escape_sequences();
     test_escape_null();
     test_escape_unknown();
     test_no_escape_without_cstyle();
     test_escaped_quote_terminates_correctly();
     test_multiple_strings_in_sequence();
     test_string_followed_by_other_tokens();
     test_string_single_quotes();
     test_unclosed_string();
     test_string_with_only_escape();
     test_string_reuse_after_reassign();
     test_string_storage_pointer_stability();
     test_string_equ();
     test_very_long_escape_string();
     test_adjacent_escape_at_end();
     test_string_quote_char_stored();

     printf("\n=== Итог: %d / %d прошли", g_passed, g_total);
     if (g_failed)
          printf(", %d ПРОВАЛИЛИСЬ", g_failed);
     printf(" ===\n");

     return g_failed ? 1 : 0;
}

#endif

