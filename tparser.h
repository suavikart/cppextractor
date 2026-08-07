#pragma once

#include "lexer.h"

#include <functional>
#include <cstring>
#include <cmath>
#include <climits>

namespace tlex {

struct TParser;

// Описатели потока лексем,
typedef enum
{
       lit_NONE
     , lit_WORD_stdstr
     , lit_WORD_lex
     , lit_WORD_tstr
     , lit_THISWORD_cchar
     , lit_THISWORD_clex
     , lit_THISWORD_cstdstr
     , lit_THISWORD_ctstr
     , lit_SYM_int
     , lit_SYM_lex
     , lit_THISSYM_clex
     , lit_THISSYM_cchar
     , lit_THISSYM_int
     , lit_INT_int
     , lit_INT_lex
     , lit_INT_stdstr
     , lit_INT_tstr
     , lit_FLOAT_float
     , lit_FLOAT_double
     , lit_FLOAT_lex
     , lit_FLOAT_tstr
     , lit_STR_lex
     , lit_STR_stdstr
     , lit_STR_tstr
     , lit_SYM_tstr
     , lit_SYM_stdstr
     , lit_THISSYM_cstdstr
     , lit_THISSYM_ctstr
     , lit_EOF
     , lit_EOF_lex

     , lit_CASE
     , lit_ACT_class
     , lit_EXTERNPARSE
     , lit_WARNING

     , lit_ANY
     , lit_ANY_stdstr
     , lit_ANY_tstr
     , lit_ANY_lex
     , lit_OPT
     , lit_EXPECTED
     , lit_END
     , lit_ERROR_MSG

     // ── Внешний строковый тип (QString при QString_ENABLE=1) ──────────────
     , lit_WORD_qstr        // захват: Word(QString&)
     , lit_STR_qstr         // захват: Str(QString&)
     , lit_SYM_qstr         // захват: Sym(QString&)
     , lit_INT_qstr         // захват: Int(QString&) - запись через Q::number
     , lit_FLOAT_qstr       // захват: Float(QString&) - запись через Q::number
     , lit_ANY_qstr         // захват: Any(QString&)
     , lit_THISWORD_cqstr   // сравнение: ThisWord(const QString&)
     , lit_THISSYM_cqstr    // сравнение: ThisSym(const QString&)
} lit_TYPE;
struct LexItem;
struct IParseAct;
using TItemList = std::initializer_list<LexItem>;
using TCaseList = std::initializer_list<TItemList>;
struct TErrorInfo
{
     const char* msg = "";
     int         code = 0;
};

struct TIdx
{
     struct Fail {};
     int  m_lexIdx;
     bool ok;
     TIdx()        { ok = false; m_lexIdx = -1;  }
     TIdx(int idx) { ok = true;  m_lexIdx = idx;   }
     TIdx(Fail)    { ok = false; m_lexIdx = -1;  }
     TIdx(const TIdx &idx) : ok(idx.ok), m_lexIdx(idx.m_lexIdx) {}
     operator bool() const { return ok; }
private:
     TIdx(bool);
};
struct IParse
{
     virtual ~IParse() = default;
     virtual TIdx match_item      (const TParser& master, const LexItem &self, int idx) const = 0;
     virtual void assign_item_data(const TParser& master, LexItem& dest, int cidx) const = 0;
     virtual const char* show     (const LexItem&) const { return ""; }
};

typedef union {
     TLex           * m_lex         ;
     const TLex     * m_clex        ;
     std::string    * m_stdstr      ;
     const std::string* m_cstdstr   ;
     const char     * m_constcharptr;
     TStr           * m_tstr        ;
     const TStr     * m_ctstr       ;
     int            * m_int         ;
     float          * m_float       ;
     double         * m_double      ;
     int              m_code        ;
     const TCaseList* m_caseList    ;
     const TItemList* m_itemList    ;
     const IParseAct* m_actClass    ;
     const IParse   * m_externParse ;
     const TErrorInfo* m_errInfo;
     // Внешний строковый тип (указатель на неполный тип - допустимо в union)
     QString        * m_qstr ;      // для захвата (Word/Str/Sym/Int/Float/Any)
     const QString  * m_cqstr;      // для сравнения (ThisWord/ThisSym) - const
} lit_Data;


struct TLexItem
{
     lit_TYPE          dtype = lit_NONE;
     lit_Data          dest  = {nullptr};
     const IExtString* m_ext = nullptr;  // не-null только для qstr-вариантов
};
struct IParseAct
{
     virtual ~IParseAct() = default;
     virtual void call(const TLexItem&) const = 0;
};
struct Act : public IParseAct
{
     std::function<void(const TLexItem&)> action;
     TLexItem                             msg;
     
     template<typename Func>
     Act(Func&& f) {
          action = std::forward<Func>(f);
     }
     Act(const char* m)
     {
          msg.dtype = lit_THISSYM_ctstr;
          msg.dest.m_constcharptr = m;
     }
     virtual void call(const TLexItem&it) const
     { 
          if (msg.dtype == lit_THISSYM_ctstr)
          {
               printf("Act[");
               if (msg.dest.m_constcharptr)
                    printf("%s", msg.dest.m_constcharptr);
               printf("]");
          }
          else action(it); 
     }
};

struct Word : public TLexItem
{
     Word() {}
     Word(std::string&w) { dtype = lit_WORD_stdstr; dest.m_stdstr = &w; }
     Word(TLex       &w) { dtype = lit_WORD_lex;    dest.m_lex    = &w; }
     Word(TStr       &w) { dtype = lit_WORD_tstr;   dest.m_tstr   = &w; }
     template<class Q, class = if_qstr<Q>>
     Word(Q& w) { dtype = lit_WORD_qstr; dest.m_qstr = &w; m_ext = QB::template ext<Q>(); }
};
struct Sym : public TLexItem
{
     Sym()               {}
     Sym(int& code)      { dtype = lit_SYM_int;    dest.m_int    = &code; }
     Sym(TLex &d)        { dtype = lit_SYM_lex;    dest.m_lex    = &d;    }
     Sym(TStr&        d) { dtype = lit_SYM_tstr;   dest.m_tstr   = &d;    }
     Sym(std::string& d) { dtype = lit_SYM_stdstr; dest.m_stdstr = &d;    }
     template<class Q, class = if_qstr<Q>>
     Sym(Q& d) { dtype = lit_SYM_qstr; dest.m_qstr = &d; m_ext = QB::template ext<Q>(); }
};
struct Int : public TLexItem
{
     Int() {}
     Int(int  &v) { dtype = lit_INT_int;    dest.m_int    = &v; }
     Int(TLex &v) { dtype = lit_INT_lex;    dest.m_lex    = &v; }
     Int(std::string& v) { dtype = lit_INT_stdstr; dest.m_stdstr = &v; }
     Int(TStr&        v) { dtype = lit_INT_tstr;   dest.m_tstr   = &v; }
     template<class Q, class = if_qstr<Q>>
     Int(Q& v) { dtype = lit_INT_qstr; dest.m_qstr = &v; m_ext = QB::template ext<Q>(); }
};
struct Float : public TLexItem
{
     Float() {}
     Float(float & v) { dtype = lit_FLOAT_float;  dest.m_float  = &v; }
     Float(double& v) { dtype = lit_FLOAT_double; dest.m_double = &v; }
     Float(TLex  & v) { dtype = lit_FLOAT_lex;    dest.m_lex    = &v; }
     Float(TStr  & v) { dtype = lit_FLOAT_tstr;   dest.m_tstr   = &v; }
     template<class Q, class = if_qstr<Q>>
     Float(Q& v) { dtype = lit_FLOAT_qstr; dest.m_qstr = &v; m_ext = QB::template ext<Q>(); }
};
struct Str : public TLexItem
{
     Str() {}
     Str(std::string & v) { dtype = lit_STR_stdstr; dest.m_stdstr = &v; }
     Str(TLex        & v) { dtype = lit_STR_lex;    dest.m_lex    = &v; }
     Str(TStr        & v) { dtype = lit_STR_tstr;   dest.m_tstr   = &v; }
     template<class Q, class = if_qstr<Q>>
     Str(Q& v) { dtype = lit_STR_qstr; dest.m_qstr = &v; m_ext = QB::template ext<Q>(); }
};
struct Eof : public TLexItem
{
     Eof()        { dtype = lit_EOF; }
     Eof(TLex &v) { dtype = lit_EOF_lex; dest.m_lex = &v; }
};
struct ThisWord : public TLexItem
{
     ThisWord() {}
     ThisWord(const char        *w) { dtype = lit_THISWORD_cchar;   dest.m_constcharptr = w;  }
     ThisWord(const std::string &w) { dtype = lit_THISWORD_cstdstr; dest.m_cstdstr      = &w; }
     ThisWord(const TLex        &w) { dtype = lit_THISWORD_clex;    dest.m_clex         = &w; }
     ThisWord(const TStr        &w) { dtype = lit_THISWORD_ctstr;   dest.m_ctstr        = &w; }
     template<class Q, class = if_qstr<Q>>
     ThisWord(const Q& w)
     { dtype = lit_THISWORD_cqstr; dest.m_cqstr = &w; m_ext = QB::template ext<Q>(); }
};
struct ThisSym : public TLexItem
{
     ThisSym() {}
     ThisSym(int       code) { dtype = lit_THISSYM_int;    dest.m_code         = code; }
     ThisSym(const TLex & v) { dtype = lit_THISSYM_clex;   dest.m_clex         = &v;   }
     ThisSym(const char * v) { dtype = lit_THISSYM_cchar;  dest.m_constcharptr = v;    }
     ThisSym(const std::string& v) { dtype = lit_THISSYM_cstdstr; dest.m_cstdstr = &v; }
     ThisSym(const TStr       & v) { dtype = lit_THISSYM_ctstr;   dest.m_ctstr   = &v; }
     template<class Q, class = if_qstr<Q>>
     ThisSym(const Q& v)
     { dtype = lit_THISSYM_cqstr; dest.m_cqstr = &v; m_ext = QB::template ext<Q>(); }
};
struct Case : public TLexItem
{
     Case() {}
     Case(const TCaseList& d)
     {
          dtype = lit_CASE;
          dest.m_caseList = &d;
     }
};

using String  = Str;
using Integer = Int;
using Symbol  = Sym;
using TS = ThisSym;

struct Any : public TLexItem
{
     Any()               { dtype = lit_ANY; }
     Any(TLex& lex)      { dtype = lit_ANY_lex;    dest.m_lex    = &lex; }
     Any(TStr& tstr)     { dtype = lit_ANY_tstr;   dest.m_tstr   = &tstr; }
     Any(std::string& s) { dtype = lit_ANY_stdstr; dest.m_stdstr = &s; }
     template<class Q, class = if_qstr<Q>>
     Any(Q& s) { dtype = lit_ANY_qstr; dest.m_qstr = &s; m_ext = QB::template ext<Q>(); }
};

struct Opt : public TLexItem
{
     Opt() { dtype = lit_OPT; dest.m_itemList = nullptr; } // empty optional
     Opt(const TItemList& lst) { dtype = lit_OPT; dest.m_itemList = &lst; }
};

struct Expected : public TLexItem
{
     Expected(const TItemList& lst) { dtype = lit_EXPECTED; dest.m_itemList = &lst; }
};

struct End : public TLexItem
{
     End()         { dtype = lit_END;  dest.m_int = nullptr; }
     End(int& res) { dtype = lit_END;  dest.m_int = &res; res = 0; }
};

struct ErrorMsg : public TLexItem
{
     TErrorInfo errorInfo;

     ErrorMsg(const char* msg, int code = parse_EXPECTED)
     {
          errorInfo.msg = msg;
          errorInfo.code = code;
          dtype = lit_ERROR_MSG;
          dest.m_errInfo = &errorInfo;
     }
};

using Error = ErrorMsg;

struct LexItem : public TLexItem
{
     LexItem() {}
     // Копирование через base - захватывает все поля включая m_ext
     LexItem(TLexItem       & w) { static_cast<TLexItem&>(*this) = w; }
     LexItem(const TLexItem & w) { static_cast<TLexItem&>(*this) = w; }
     LexItem(std::string    & w)        { dtype = lit_WORD_stdstr;   dest.m_stdstr       = &w; }
     LexItem(TStr           & w)        { dtype = lit_WORD_tstr;     dest.m_tstr         = &w; }
     LexItem(const char     * w)        { dtype = lit_THISWORD_cchar;dest.m_constcharptr =  w; }
     LexItem(char             w)        { dtype = lit_THISSYM_int;   dest.m_code         =  w; }
     LexItem(int            & i)        { dtype = lit_INT_int;       dest.m_int          = &i; }
     LexItem(float          & f)        { dtype = lit_FLOAT_float;   dest.m_float        = &f; }
     LexItem(double         & d)        { dtype = lit_FLOAT_double;  dest.m_double       = &d; }

     LexItem(const Act      & act)      { dtype = lit_ACT_class;     dest.m_actClass     = &act; }
     LexItem(const IParseAct& act)      { dtype = lit_ACT_class;     dest.m_actClass     = &act; }
     LexItem(const Case& d)             { dtype = lit_CASE;          dest                = d.dest; }
     LexItem(const IParse& externParse) { dtype = lit_EXTERNPARSE;   dest.m_externParse = &externParse; }
     LexItem( const Error &err )        { dtype = lit_ERROR_MSG;     dest.m_errInfo = err.dest.m_errInfo;}
     // Для Any/Opt/Expected/End используем полное копирование базы -
     // чтобы m_ext (и любые будущие поля TLexItem) не терялись.
     LexItem( const Any& a )            { static_cast<TLexItem&>(*this) = a; }
     LexItem( const Opt& o )            { static_cast<TLexItem&>(*this) = o; }
     LexItem( const Expected& e )       { static_cast<TLexItem&>(*this) = e; }
     LexItem( const End& e )            { static_cast<TLexItem&>(*this) = e; }
};

struct TState
{
     bool ok = false;
     int  lexIdx = 0;
     TState() {}
     TState(Fail) { ok = false; }
     explicit TState(int i) { lexIdx = i; ok = true; }
     operator bool() const { return ok; }
private:
     operator int() const;
     TState(bool);
};


struct TParser
{
     enum { ENABLE_TRACE = 0
          , ENABLE_SHOW_LIST = 0
     };

     const TLex        fail;
     std::vector<TLex> m_stream;
     int               m_streamSize;
     int               m_currentIdx;
     TLexem            m_lexem;
     TTextStorage      m_textStorage;

     void clear()
     {
          m_stream.clear();
          m_streamSize = 0;
          m_currentIdx = 0;
     }

     TParser() :fail(Fail())
     {
          clear();
     }

     TState get_state()                   const { return TState{m_currentIdx}; }
     void   restore_state(const TState& state)  { if(state) m_currentIdx = state.lexIdx; }
     void set_end_ofLine_comment(const char* c)  { m_lexem.set_end_ofLine_comment(c); }
     void set_open_comment      (const char* c)  { m_lexem.set_open_comment      (c); }
     void set_close_comment     (const char* c)  { m_lexem.set_close_comment     (c); }

     void set_error_handler(TParseError& errorHand)       { m_lexem.set_error_handler( errorHand ); }

     int  add_ex_exSym     (const TChar* exSym, int code) { return m_lexem.add_ex_exSym(exSym, code); } // <= >= и т.д.
     int  get_sym_code     (const TChar* exSym) const     { return m_lexem.get_sym_code(exSym); }
     const TChar* get_sym_str(int code)         const     { return m_lexem.get_sym_str(code); }
     void end_of_init      ();
     void assign           (const TChar* src, int length); 
     void assign           (const char * src, TTextStorage::TEncoding enc);
     void assign           (const char * src);
     // Загрузка из внешнего строкового типа (QString и совместимые).
     // Тело компилируется лениво - только при вызове в Qt-TU.
     template<class Q, class = if_qstr<Q>>
     void assign(const     Q& src) { QB::template assign_src<Q>(*this, src); }
     void assign(const TChar *src) 
     { 
        m_lexem.assign(src);
        m_currentIdx = 0;
     }

     void set_C_style      (bool c)          { m_lexem.set_C_style(c); }
     void set_quote_as_sym (bool asSym)      { m_lexem.set_quote_as_sym(asSym); }
     void ex_word_symbols_first(const char* s)             { m_lexem.ex_word_symbols_first(s); }
     void ex_word_symbols  (const char* s, bool state = true) { m_lexem.ex_word_symbols(s, state); }

           TLex& cur()       { return m_stream[m_currentIdx]; }
     const TLex& cur() const { return m_stream[m_currentIdx]; }
     void next() 
     { 
          if (m_currentIdx < m_streamSize)
               ++m_currentIdx;
     }
     int lexem_count() const
     {
          return static_cast<int>(m_stream.size());
     }
     const TLex& lexem(int idx) const
     {
          if (idx >= 0 && idx < static_cast<int>(m_stream.size()))
               return m_stream[idx];
          return fail;
     }

     bool is_word  () const { return cur().is_word();    }
     bool is_this_word(const char*        word) const { return cur().is_this_word(word); }
     bool is_this_word(const TStr&        word) const { return cur().is_this_word(word); }
     bool is_this_word(const std::string& word) const { return cur().is_this_word(word); }
     bool is_sym   () const { return cur().type == lex_SYMBOL;  }
     bool is_this_sym(int code)               const { return cur().is_this_sym(code); }
     bool is_this_sym(const char* sym)        const { return cur().is_this_sym(sym);  }
     bool is_this_sym(const TStr& sym)        const { return cur().is_this_sym(sym);  }
     bool is_this_sym(const std::string& sym) const { return cur().is_this_sym(sym);  }
     bool is_str   () const { return cur().type == lex_STRING;  }
     bool is_float () const { return cur().type == lex_FLOAT || cur().type==lex_INTEGER; }
     bool is_int   () const { return cur().type == lex_INTEGER; }
     bool is_eof   () const { return cur().type == lex_EOF;     }

     // is_this_word / is_this_sym для внешнего строкового типа (QString).
     // Сравнение без выделения памяти - UTF-16 декодируется inline в TQBridge::match.
     template<class Q, class = if_qstr<Q>>
     bool is_this_word(const Q& w) const
     { return cur().is_word() && QB::template match<Q>(&w, cur().v_s); }

     template<class Q, class = if_qstr<Q>>
     bool is_this_sym(const Q& w) const
     { return cur().is_sym() && QB::template match<Q>(&w, cur().v_s); }
     bool is_number(int pos) const
     {
          const int size = static_cast<int>(m_stream.size());
          if (pos >= 0 && pos < size )
          {
               const auto t = m_stream[pos].type;
               if (t == lex_FLOAT || t == lex_INTEGER)
                    return true;
               if (t == lex_SYMBOL)
               {
                    const auto s = m_stream[pos].v_i;
                    if ((s == '+' || s == '-') && pos < size-1 )
                    {
                         const auto t2 = m_stream[pos+1].type;
                         if (t2 == lex_FLOAT || t2 == lex_INTEGER)
                              return true;
                    }
               }
          }
          return false;
     }

     bool is_number() const 
     { 
          return   is_number(m_currentIdx);
     }

     const TLex &get_word     ();
     const TLex &get_this_word(const char *word);
     const TLex &get_this_word(const TStr &word);
     const TLex &get_this_word(const std::string &word);
     const TLex &get_sym      ();
     const TLex &get_this_sym (int code);
     const TLex &get_this_sym (const char* sym);
     const TLex &get_this_sym (const std::string& word);
     const TLex &get_this_sym (const TStr& word);
     const TLex &get_str      ();
     const TLex &get_float    ();
     const TLex &get_int      ();
           TLex  get_number   (); // может быть создан из разных лексем
     const TLex &get_eof      ();

     // get_this_word / get_this_sym для внешнего строкового типа (QString).
     template<class Q, class = if_qstr<Q>>
     const TLex& get_this_word(const Q& w)
     {
          if (cur().is_word() && QB::template match<Q>(&w, cur().v_s))
               { const TLex& r = cur(); next(); return r; }
          return fail;
     }
     template<class Q, class = if_qstr<Q>>
     const TLex& get_this_sym(const Q& w)
     {
          if (cur().is_sym() && QB::template match<Q>(&w, cur().v_s))
               { const TLex& r = cur(); next(); return r; }
          return fail;
     }

     const TLex &expected_word       ();
     const TLex &expected_this_word  (const char *word);
     const TLex &expected_this_word  (const TStr &word);
     const TLex &expected_this_word  (const std::string &word);
     const TLex &expected_sym        ();
     const TLex &expected_this_sym   (int code);
     const TLex &expected_this_sym   (const char* sym);
     const TLex& expected_this_sym   (const TStr& sym);
     const TLex& expected_this_sym   (const std::string& sym);
     const TLex &expected_str        ();
     const TLex &expected_float      ();
     const TLex &expected_int        ();
           TLex  expected_number     ();
     const TLex &expected_eof        ();

     // expected_this_word / expected_this_sym для внешнего строкового типа (QString).
     template<class Q, class = if_qstr<Q>>
     const TLex& expected_this_word(const Q& w)
     {
          const TLex& lx = get_this_word(w);
          if (!lx)
               m_lexem.m_error->error(cur(), parse_EXPECTED, "Expected word (QString)");
          return lx;
     }
     template<class Q, class = if_qstr<Q>>
     const TLex& expected_this_sym(const Q& w)
     {
          const TLex& lx = get_this_sym(w);
          if (!lx)
               m_lexem.m_error->error(cur(), parse_EXPECTED, "Expected symbol (QString)");
          return lx;
     }

protected:
     struct TMatchContext
     {
          bool ok = false;
          int  thisIdx = -1;
          TMatchContext* prevCtx = nullptr;
          TMatchContext* nextCtx = nullptr;
          LexItem w;
          // ── Континуация (в терминах Prolog: goto_if_true последней инструкции блока) ──
          // Заполняется только у контекста-фрейма конструкции (Case/Opt/Expected):
          TItemList::iterator nextUp;    // первый итем родительского списка за конструкцией
          TItemList::iterator nextUpEnd; // и end родительского списка
          // У всех контекстов:
          TMatchContext* frame  = nullptr; // фрейм объемлющей конструкции (nullptr на верхнем уровне)
          bool           strict = false;  // true внутри Expected: несовпадение итема = ошибка (cut)

          operator bool() const { return ok; }
          TMatchContext() {}

          static TMatchContext Fail() { return TMatchContext();  }
     };
     TIdx match_item(const LexItem& w, int idx) const;
     TState   match         (TItemList::iterator list, const TItemList::iterator&end, TMatchContext& context, int depth);
     void on_end_of_parce(TMatchContext& context);
     void trace_items(TMatchContext* start) const;
public:
     void   assign_item_data(LexItem& dest, int lexidx) const;
     void   assign_item_data(LexItem& dest, const TLex& sour) const;
     TState parse           (TState startIdx, const TItemList &list);
     TState parse           (const TItemList& list); // от текущего состояния и переход на новое, если удачно распарсили
};

} // namespace tlex
