#ifndef __EXTL_H__INCLUDED
#define __EXTL_H__INCLUDED

/*
 * \file: extl.h
 * \authors: suavik
 *
 * Расширение работы с шаблонами
 */

#include <initializer_list>
#include <type_traits>

#include "Util/prelude.h"

namespace extlNS
{
/*!
 * Список типов<br>
 * пример:<pre> typedef TypeList<int,unsigned,int,double> List;</pre>
 * param T... перечисление типов, из которых будет создан список
 * return Head - первый элемент
 * return Tail - хвост списка
 * return SIZE - размер списка
 * warning При создании списка через variadic шаблон заканчивается TypeList<>, а при создании вручную обычно заканчивают TypeList<EmptyClass>
 */
//---------------------------------
template<typename ...T> struct TypeList {};

template<>
struct TypeList<>
{
     enum { SIZE = 0, COUNT = 0 };
     typedef TypeList<> Type;
};
/// Пустой класс. Используется, как значение по умолчанию, терминальный для списка и т.д.
typedef TypeList<> EmptyClass;

template<typename H>
struct TypeList<H, TypeList<> >
{
     typedef H              Head;
     typedef TypeList<>     Tail;
     enum { SIZE = 1 };
};

template<typename H, typename T, typename ...N>
struct TypeList<H, TypeList<T, N...> >
{
     typedef H                 Head;
     typedef TypeList<T, N...> Tail;
     enum { SIZE = 1 + Tail::SIZE };
};

template<typename H, typename ...T>
struct TypeList<H, T...>
{
     typedef H              Head;
     typedef TypeList<T...> Tail;
     enum { SIZE = 1 + Tail::SIZE };
};

/*!
 * Выборка типа из списка TypeList<...> по индексу
 * param N индекс в списке начиная с 0
 * param TL список
 * return Type тип N-го элемента
 */
template<int N,class TL> struct TypeOf        { typedef typename TypeOf<N-1, typename TL::Tail>::Type Type; };
template<class TL>       struct TypeOf<0, TL> { typedef typename TL::Head                             Type; };

template<class T>         struct IsList                 { enum{ OK = 0 }; };
template<class H,class T> struct IsList<TypeList<H,T> > { enum{ OK = 1 }; }; // normalized
template<class T>         struct IsList<TypeList<T> >   { enum{ OK = 2 }; }; // once
template<>                struct IsList<TypeList<> >    { enum{ OK = 3 }; }; // empty
template<class ...T>      struct IsList<TypeList<T...> >{ enum{ OK = 4 }; }; // variadic

/*!
* Выборка типо по флагу
* param N флаг 0 или !=0
* param T0, T1 типы
* return Type равен первому типу, если флаг не равен 0 и второму, если равер 0
*/
template<bool N, class T0, class T1>
using SelectType = std::conditional<N, T0, T1>;

/*!
* Преобразование списка типа TypeList<t1,t2,t3,t4> в список вида TypeList<t1,TypeList<t2,...> >
* Исправление некорректно закрытых списоков.
*/
template<class ...T>  struct VariadicToList
{
     typedef TypeList<T...> Lst;
     typedef typename VariadicToList<Lst>::Type Type;
};
template<>  struct VariadicToList<>
{
     typedef EmptyClass Type;
};
template<>  struct VariadicToList< TypeList<> >
{
     typedef EmptyClass Type;
};

template<typename T, typename Subst>
struct RecalcVariadic // Запускает рекурсию VariadicToList только если параметр - список
{
   typedef typename VariadicToList<T>::Type NType;
   enum
   {
      LIST_TYPE = (IsList<T>::OK ? 1 : 0)
   };
   typedef typename SelectType< LIST_TYPE, NType, Subst >::type Type;
};

template<class T>  struct VariadicToList<T>
{
   typedef TypeList<T, TypeList<> > Type;
};

template<class H, class T>
struct VariadicToList<TypeList<H, T> >
{
private:
   typedef typename RecalcVariadic<H, H >::Type NHead;
   typedef typename RecalcVariadic<T, TypeList<T, EmptyClass> >::Type NTail;
public:
   typedef TypeList< NHead, NTail> Type;
};

template<class H, class ...T>
struct VariadicToList < TypeList<H, T...> >
{
private:
   typedef TypeList<H, T...> Lst;
   typedef typename RecalcVariadic<H, H >::Type NHead;
   typedef typename RecalcVariadic<typename Lst::Tail, TypeList<typename Lst::Tail, EmptyClass> >::Type NTail;
public:
   typedef TypeList< NHead, NTail> Type;
};

/*! объединение списков
 * example MergeList<TypeList<int,float>,TypeList<int,unsigned,int,double> >::Type === TypeList<int,float, int,unsigned,int,double>
 */
template<class TList, class V>struct MergeList
{
     typedef TypeList< typename TList::Head
                     , typename MergeList< typename TList::Tail , V >::Type
                     > Type;
};

/*
 * В случае объединения списков заданных явно, сигнатура списка не TypeList<1,TypeList<2,...>> а TypeList<1,2,3...>
 * Поэтому список нуждается в пересборке.
 */

template<class V>         struct MergeList<EmptyClass, V >{ typedef TypeList<typename V::Head, typename VariadicToList<typename V::Tail>::Type> Type; };
template<class V>         struct MergeList<V, EmptyClass >{ typedef TypeList<typename V::Head, typename VariadicToList<typename V::Tail>::Type> Type; };
template<>      struct MergeList<EmptyClass, EmptyClass  >{ typedef EmptyClass Type; };

template<typename X> struct ConvMapToList                                      { typedef X Type; };
template<>           struct ConvMapToList<TypeList<> >                         { typedef TypeList<> Type; };
template<>           struct ConvMapToList<TypeList<TypeList<> > >              { typedef TypeList<> Type; };
template<>           struct ConvMapToList<TypeList<TypeList<>, TypeList<> > >  { typedef TypeList<> Type; };
template<class T>    struct ConvMapToList<TypeList<TypeList<>, T > >           { typedef typename ConvMapToList<T>::Type Type; };

template<typename H, typename T, typename NT>
struct ConvMapToList< TypeList< TypeList<H, T>, NT > >
{
     typedef typename ConvMapToList<TypeList<H, TypeList<T, NT> > >::Type Type;
};

template<typename H, typename T>
struct ConvMapToList<TypeList<H, T> >
{
     typedef TypeList<H, typename ConvMapToList<T>::Type> Type;
};

// Зарезервированные коды классов-опций
enum
{
     OPT_CHECKER = 1
   //--- далее только предложение
   , OPT_PRINT      =  2
   , OPT_CHAR       =  3
   , OPT_DEBUG      =  4 // константа степени отладки 0..3
   , OPT_WARNING    =  5 // константы сообщения об ошибках (печати) 0..3
   , OPT_THROWCLASS =  6 // тип класса-исключения
   , OPT_THROWGEN   =  7 // функции выдачи исключания и флага необходимости исключения
   , OPT_PARAMTYPE  =  8 // тип для контейнеров передачи параметра или возврата: const T& или T
};

/*! Проверка на равенство двух типов
 * param N, M - сравниваемые типы
 * return OK == 1 если типы равны, иначе 0
 */
template<class N, class M> struct TypeEQ      { enum{ OK = 0 }; };
template<class N>          struct TypeEQ<N, N>{ enum{ OK = 1 }; };

/// преобразование типа в константный
template<class E> struct ToConst          { typedef const E Type; };
template<class E> struct ToConst<const E> { typedef const E Type; };

/*! Проверка что тип принадлежит указанному списку
 * param E искомый тип
 * param T... список типов
 * return OK == 1 если тип E находится в списке T...
 */
template<class E, typename ...T>struct TypeIs{ enum { OK = 0 }; };
template<class E, class H, typename ...T>struct TypeIs<E,H,T...>
{
     enum
     {
          OK = (TypeEQ<typename ToConst<E>::Type, typename ToConst<H>::Type>::OK
            || TypeIs<E,T...>::OK ) ? 1 : 0
     };
};

// Пролверка, что тип находится в списке
template<class T,class ...ARGS>
struct FindType
{
     enum  { OK = 0 };
};

template<class T,class E,class ...ARGS>
struct FindType<T,E,ARGS...>
{
     enum  { OK = TypeEQ<T,E>::OK | FindType<T,ARGS...>::OK };
};

/// проверка, что указанный тип атомарный, или константный атомарный
template<class E>struct IsAtomic
{
     enum
     {
          OK = TypeIs<E, char, unsigned char, signed char
                       , short, unsigned short
                       , int, unsigned int
                       , long, unsigned long
                       , long long, unsigned long long
                       , float, double, bool
                       , wchar_t
                     >::OK
     };
};

/// Класс, определяющий эффективную передачу параметра: по значению или по константной ссылке
template<class E>
struct ParamType
{
private:
     enum
     {
          IsSimple =(std::is_reference_v<E>
                  || IsAtomic<E>::OK
                  || /*IsComparedType<E>::OK &&*/ sizeof( E ) <= sizeof( double ) // enum-ы и какие-то мелкие скалярные типы
                  ) ? 1 : 0
     };
     template<int ISSIMPLE, class T>
     struct ParType {};

     template<class T> struct ParType<0, T> { typedef const T &Type; };
     template<class T> struct ParType<1, T> { typedef T        Type; };
public:
     typedef typename ParType< IsSimple, E >::Type Type;
};


/*!
 * Класс, который превращает TypeList или variadic список в структуру-дерево. Обычно дерево представлено списком и
 * поэтому название для удобства FixList, хотя в принципе это может быть дерево произвольной вложенности.<br>
 * Избыточные объявления созданы для удобства использования: можно применять все интуитивно понятные сокращения и комбинации.<br>
 * Типы переменных головы и хвоста HeadType и NextType - чтобы избегать путаницы с типами головы и хвоста TypeList
 */
template<typename ...T> struct FixList;

//private:
template<typename H>            struct ToFixList
{
     enum{ LIST_TYPE = IsList<H>::OK ? 1 : 0 };
     typedef typename SelectType< LIST_TYPE, FixList<H>, H >::type Type;
};
template<> struct ToFixList < EmptyClass >                      { typedef EmptyClass Type; };
template<> struct ToFixList < TypeList<EmptyClass> >            { typedef EmptyClass Type; };
template<> struct ToFixList < TypeList<EmptyClass,EmptyClass> > { typedef EmptyClass Type; };

template<> struct FixList<>            {};
template<> struct FixList<TypeList<> > {};
template<> struct FixList<TypeList<>, TypeList<> > {};

template<typename H, typename ...T>
struct FixList<TypeList<H, T...> >
{
     private: typedef typename VariadicToList<TypeList<H, T...> >::Type    TList;
     public:
     typedef typename ToFixList<typename TList::Head>::Type      HeadType;
     typedef typename ToFixList<typename TList::Tail>::Type      NextType;
     typedef typename ParamType<H>::Type                       HeadParamType; // по-хорошему, надо не от H, а от HeadType, но internal error
     HeadType car;
     NextType cdr;
     FixList(){}
     template<typename ...X>
     FixList( HeadParamType h, X... res ) : car( h ), cdr( res... ) {}
};

template<typename H>
struct FixList<TypeList<H, TypeList<> > >
{
     private: typedef typename VariadicToList<TypeList<H, TypeList<> > >::Type    TList;
     public:
     typedef typename ToFixList<typename TList::Head>::Type      HeadType;
     typedef EmptyClass                                          NextType;
     HeadType car;
     NextType cdr;
     FixList(){}
     FixList( typename ParamType<HeadType>::Type h ) : car( h ) {}
};

// пустой мы перегрузили, перегружаем вариант, который содержит по меньшей мере один параметр

template<typename H, typename... T>
struct FixList<H, T...>
{
     private:
     typedef typename VariadicToList<T... >::Type      TListTail;
     public:
     typedef typename ToFixList<H>::Type          HeadType;
     typedef typename ToFixList<TListTail>::Type  NextType;
     HeadType car;
     NextType cdr;
     FixList() {}
     template<typename ...X>
     FixList( typename ParamType<HeadType>::Type h, X... res ) : car( h ), cdr( res... ) {}
};

template <int CHECK, bool THROW>
using Checker_default = ::Checker_default<CHECK, THROW>;

} // namespace extlNS

struct extl
{
// просто флаг ошибки (для всех монад)
struct Fail{};

//---------------------------------

public:
/*!
 * Разворачивание произвольного дерева в линейный список
 */
template<typename T>
struct MapToList
{
private:     typedef typename extlNS::VariadicToList<T>::Type   MPlainList;
public:      typedef typename extlNS::ConvMapToList<MPlainList>::Type Type;
};
/*!
 * Обертка для самого простого случая класса-опции, когда задан один тип (который может быть и списком типов).
 * При извлечении из списка сам класс OptType будет отброшен и останется только Type. 
 * Пример:<pre>
 * DefineOpt<MY_OPT, OptType<int>, TypeList<> >::Type                       i; // по умолчанию переменная i
 * DefineOpt<MY_OPT, OptType<int>, TypeList<OptType<float,MY_OPT> > >::Type f; // f будет типа float
 * </pre>
 */
template<typename T, int CODE = 0>
struct OptType
{
     enum { OPT_CODE = CODE };
     typedef T Type;
};
/*!
 * Обертка для одиночных констант. 
 * parav V передаваемое значение.
 * param CODE идет вторым, чтобы создавать простые опции по умолчанию, где код неважен.
 * return VAL значение передоваемой константы
 */
template<int V, int CODE = 0>
struct OptVal
{
     enum { OPT_CODE = CODE };
     enum { VAL      = V    };
};

//--------------------------------------------------- private -------------------------------------------
//private:
template<typename T>        struct UnOptType                { typedef T Type; };
template<typename T,int N>  struct UnOptType<OptType<T,N> > { typedef T Type; };

//---------------------------------

template<int OPT_CODE, class DEFAULT, class OptList> 
struct FindOpt { typedef typename DEFAULT::Error_Type_List Type; };

template<int OPT_CODE, class DEFAULT>
struct FindOpt< OPT_CODE, DEFAULT, extlNS::TypeList<> >  { typedef typename UnOptType<DEFAULT>::Type Type;   };

template<int OPT_CODE, class DEFAULT, class H>
struct FindOpt < OPT_CODE, DEFAULT, extlNS::TypeList<extlNS::TypeList<H> > > { typedef typename FindOpt< OPT_CODE, DEFAULT, extlNS::TypeList<H> >::Type Type;  Type x3; };

template<int OPT_CODE, class DEFAULT>
struct FindOpt < OPT_CODE, DEFAULT, extlNS::TypeList<extlNS::TypeList<> > > { typedef typename UnOptType<DEFAULT>::Type Type;  Type x4; };

template<int OPT_CODE, class DEFAULT,class H>
struct FindOpt< OPT_CODE, DEFAULT, extlNS::TypeList<H> > {
   typedef typename extlNS::SelectType < OPT_CODE == H::OPT_CODE
                                 , H
                                 , DEFAULT
                                 > ::type OpType;
     typedef typename UnOptType<OpType>::Type Type;

     enum
     {
          _OPT_CODE = OPT_CODE
     ,     _H_OC = H::OPT_CODE
     };
     H v0;
     DEFAULT v1;
     Type x5;
};

template< int OPT_CODE, class DEFAULT, class H, class T>
struct FindOpt< OPT_CODE, DEFAULT, extlNS::TypeList<H, T> >
{
     typedef typename
            FindOpt< OPT_CODE
            , typename extlNS::SelectType< OPT_CODE == H::OPT_CODE
                                        , H
                                        , DEFAULT
                                        >::type
            , typename extlNS::SelectType< OPT_CODE == H::OPT_CODE
                                        , extlNS::TypeList<>
                                        , T
                                        >::type
                   >::Type  OpType;
     typedef typename UnOptType<OpType>::Type Type;
     int x6;
};

//--------------------------------------------------- public -------------------------------------------
public:
/*!
Класс настройки шаблона от многих шаблонных параметров.<br>
Мотивация: иметь возможность передавать шаблону параметры в произвольном порядке в любом количестве, при этом не упомянутые пораметры 
будут иметь значение по умолчанию.<br>
Термины:
 # Пользовательский шаблон - некоторый шаблон, который нуждается в настройке: передаче ему разных типов и констант (обычно больше 3)
     template<class X, typename ...T>struct MyClass {};
 # Опция: тип (класс), который если не определен снаружи в списке шаблона, имеет умолчальный тип.<br>
 # Список опций - список параметров в пользовательском шаблоне, который содержит те опции, которые необходимо перегрузить.

Список классов-опций  передается при объявлении шаблона пользовательского класса. В случае указания Опции, нужный тип будет заменен указанным.<br>
Чтобы задать дополнительную опцию, создаем класс, в котором содержится константа OPT_CODE со значением нужной опции.
Допустимо использование списка списков любой вложенности.
Помимо константы-идентификатора задаются необходимые функции в нужном классе. пример:
<pre>

struct Opt1Default { static const char*get() { return "hello"; } }; // класс по умолчанию

// в классе ParserBase<> объявлена
template<class STRC, UT, typename ...OptionsList>
{
typedef typename DefineOpt< OPT_1, Opt1Default, OptionsList >::Type Opt1_get; // Задаем имя опционального типа внутри класса с опциями

... 
const char *n = Opt1_get::get(); // Если опция не была перегружена, значение будет "hello";
..
};

// создаем свой класс-опцию:
struct Opt1 { enum{ OPT_CODE = OPT_1 }; static const char*get() { return "world"; } }; // этот класс перегрузит умолчальное значение
// при создании класса передаем свой класс:
ParserBase<strc,int, Opt1>

// теперь класс подхватит опцию Opt1 и будет звать новую функцию get:
// const char *t = Opt1_get::get();
</pre>
Рекомендуется использовать коды выше 10, хотя никаких принципиальных ограничений на значения нет. Пусть первые 10
используются преимущество для системных целей.
Зарезервированные коды:
     # 0 - под умолчальные значения
     # 1 - чекер
param OPT_CODE уникальная константа идентификатор опции
param DEFAULT тип заданный по умолчанию, если его не будет в списке
param OptList список типов-опций, которые будут перегружать умолчальные значения
return Type тип с опцией
*/
template<int OPT_CODE, class DEFAULT, class OptList>
struct DefineOpt 
{
     typedef typename FindOpt<OPT_CODE, DEFAULT, typename MapToList<OptList>::Type>::Type Type;
};

//---------------------------------
// Оболочка для массива фиксированного размера
template<
    typename T,
    int S,
    typename CHECKER = extlNS::Checker_default<1, true>,
    typename PType = typename extlNS::ParamType<T>::Type
>
struct /*[[deprecated ("use dim")]]*/ TDim /* = dim<T, S>*/
{
     enum { MAXSIZE = S };

     T d[ S==0 ? 1 : S ];
     int count = 0;

           T & operator [] ( int i )       { return d[ i ]; }
     PType     operator [] ( int i ) const { return d[ i ]; }

     TDim() {}
     explicit TDim( PType p )             { for ( T &i : d ) i = p; count = S; }
     TDim( const T *b, const T *e )       { for ( const T *i = b; i != e; ++i ) push_back( *i ); }
     TDim( const TDim &p )                 { for ( int i = 0; i < S; ++i ) d[ i ] = p.d[ i ]; count = p.count; }
     TDim( std::initializer_list<T> lst ) { for ( const T&i : lst ) push_back(i); }

     TDim &operator =( const TDim &p )     { for ( int i = 0; i < p.count; ++i ) d[ i ] = p.d[ i ]; count = p.count; return *this; }
     TDim &operator =( std::initializer_list<T> lst) { clear();  for ( const T&i : lst ) push_back( i ); return *this;  }

     TDim &operator = (PType e) { d[ 0 ] = e; count = 1; return *this; }
     TDim &operator , (PType e) { push_back( e ); return *this; }
     typedef T* iterator;
           T *begin()        { return d; }
           T *end  ()        { return d + count; }
     const T *begin() const  { return d; }
     const T *end  () const  { return d + count; }
     unsigned size () const  { return static_cast<unsigned>(count); }
     void     clear()        { count = 0; }
     void     set  ( PType x ) { for ( int i = 0; i < S; ++i ) d[ i ] = x; count = S; }

     void push_back( PType p )       { Checker_default_ASSERT( count<S ) d[ count++ ] = p; }
     void reserve  ( int sz, PType v){ for ( int i = count; i < sz; ++i ) push_back(v); }

        T &operator() ( int i )      { Checker_default_ASSERT( i >= 0 && i<count ) return d[ i ]; }
     PType operator() ( int i )const { Checker_default_ASSERT( i >= 0 && i<count ) return d[ i ]; }

     void remove( int i ) { Checker_default_ASSERT( i >= 0 && i < count ) count--; for ( int j = i; j < count; ++j ) d[ j ] = d[ j + 1 ]; }

     bool operator==(const TDim&p) const
     {
          if ( count == p.count )
          {
               for ( int i = 0; i < count; ++i )
                    if ( d[ i ] != p.d[ i ] )
                         return false;
               return true;
          }
          return false;
     }
     bool operator!=(const TDim&a) const { return !operator==(a); }

     void swap( TDim &x ) { TDim temp = *this; *this = x; x = temp; }
     int  find( const T &x ) const { for ( int i = 0; i < count; ++i ) if ( d[ i ] == x )return i; return -1; }
};

//----------------------------------------------------

/*!
 * Класс для возврата группы параметров с флагом успешности, аналог монады Maybe в Haskel<br>
 * Сейчас монадические законы не выполняются, когда будет нужно, допишу, чтобы maybe от maybe не включалась, только данные
 */
template<typename... T>
struct /*[[deprecated ("Use std::optional")]]*/ Maybe
{
     typedef extlNS::FixList< T... > Data;
     Data data;
     bool isOk;
     Maybe() { isOk = false; }
     Maybe( Fail ) { isOk = false; }

//#ifdef _MSC_VER //TODO: gcc ???
     template<typename ...E>
//#endif
     Maybe( typename extlNS::ParamType<typename Data::HeadType>::Type h, E ... d ) : data( h, d... ) { isOk = true; }
     Maybe( const Data &d ) : data( d ) { isOk = true; }

     operator bool() const { return isOk; }

private:
     operator double() const = delete;
     operator int   () const = delete;
     operator float () const = delete;
     operator const void*() const = delete;
};
};// extl


#endif
