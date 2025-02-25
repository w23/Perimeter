#include <boost/config.hpp>

#if defined(BOOST_MSVC)
#pragma warning(disable: 4786)  // identifier truncated in debug info
#pragma warning(disable: 4710)  // function not inlined
#pragma warning(disable: 4711)  // function selected for automatic inline expansion
#pragma warning(disable: 4514)  // unreferenced inline removed
#endif

//
//  bind_eq_test.cpp - boost::bind equality operator
//
//  Copyright (c) 2004 Peter Dimov
//
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.
//

#include <boost/bind.hpp>
#include <boost/ref.hpp>

#if defined(BOOST_MSVC) && (BOOST_MSVC < 1300)
#pragma warning(push, 3)
#endif

#include <iostream>

#if defined(BOOST_MSVC) && (BOOST_MSVC < 1300)
#pragma warning(pop)
#endif

#include <boost/detail/lightweight_test.hpp>

struct X
{
    int i_;

    explicit X(int i): i_(i)
    {
    }

    bool operator==(X const & rhs) const
    {
        return i_ == rhs.i_;
    }
};

// f_*

int f_0()
{
    return 0;
}

int f_1(X)
{
    return 0;
}

int f_2(X, X)
{
    return 0;
}

int f_3(X, X, X)
{
    return 0;
}

int f_4(X, X, X, X)
{
    return 0;
}

int f_5(X, X, X, X, X)
{
    return 0;
}

int f_6(X, X, X, X, X, X)
{
    return 0;
}

int f_7(X, X, X, X, X, X, X)
{
    return 0;
}

int f_8(X, X, X, X, X, X, X, X)
{
    return 0;
}

int f_9(X, X, X, X, X, X, X, X, X)
{
    return 0;
}

// fv_*

void fv_0()
{
}

void fv_1(X)
{
}

void fv_2(X, X)
{
}

void fv_3(X, X, X)
{
}

void fv_4(X, X, X, X)
{
}

void fv_5(X, X, X, X, X)
{
}

void fv_6(X, X, X, X, X, X)
{
}

void fv_7(X, X, X, X, X, X, X)
{
}

void fv_8(X, X, X, X, X, X, X, X)
{
}

void fv_9(X, X, X, X, X, X, X, X, X)
{
}

template<class F> void test_eq(F f1, F f2)
{
    BOOST_TEST(f1 == f2);
    BOOST_TEST(!(f1 != f2));
}

template<class F> void test_ne(F f1, F f2)
{
    BOOST_TEST(f1 != f2);
    BOOST_TEST(!(f1 == f2));
}

// 0

template<class F> void test_0(F f)
{
    test_eq( boost::bind(f), boost::bind(f) );
}

// 1

template<class F, class V> void test_1_(F f, V v1, V v2)
{
    test_eq( boost::bind(f, v1), boost::bind(f, v1) );
    test_ne( boost::bind(f, v1), boost::bind(f, v2) );
}

template<class F> void test_1(F f)
{
    test_eq( boost::bind(f, _1), boost::bind(f, _1) );

    test_1_( f, X(1), X(2) );

    X a(0), b(0);
    test_1_( f, boost::ref(a), boost::ref(b) );
}

// 2

template<class F, class V> void test_2_(F f, V v1, V v2)
{
    test_eq( boost::bind(f, v1, v1), boost::bind(f, v1, v1) );
    test_ne( boost::bind(f, v1, v1), boost::bind(f, v1, v2) );
    test_ne( boost::bind(f, v1, v1), boost::bind(f, v2, v1) );
}

template<class F> void test_2(F f)
{
    test_eq( boost::bind(f, _1, _2), boost::bind(f, _1, _2) );

    test_2_( f, X(1), X(2) );

    X a(0), b(0);
    test_2_( f, boost::ref(a), boost::ref(b) );
}

// 3

template<class F, class V> void test_3_(F f, V v1, V v2)
{
    test_eq( boost::bind(f, v1, v1, v1), boost::bind(f, v1, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1), boost::bind(f, v1, v1, v2) );
    test_ne( boost::bind(f, v1, v1, v1), boost::bind(f, v1, v2, v1) );
    test_ne( boost::bind(f, v1, v1, v1), boost::bind(f, v2, v1, v1) );
}

template<class F> void test_3(F f)
{
    test_eq( boost::bind(f, _1, _2, _3), boost::bind(f, _1, _2, _3) );

    test_3_( f, X(1), X(2) );

    X a(0), b(0);
    test_3_( f, boost::ref(a), boost::ref(b) );
}

// 4

template<class F, class V> void test_4_(F f, V v1, V v2)
{
    test_eq( boost::bind(f, v1, v1, v1, v1), boost::bind(f, v1, v1, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1), boost::bind(f, v1, v1, v1, v2) );
    test_ne( boost::bind(f, v1, v1, v1, v1), boost::bind(f, v1, v1, v2, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1), boost::bind(f, v1, v2, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1), boost::bind(f, v2, v1, v1, v1) );
}

template<class F> void test_4(F f)
{
    test_eq( boost::bind(f, _1, _2, _3, _4), boost::bind(f, _1, _2, _3, _4) );

    test_4_( f, X(1), X(2) );

    X a(0), b(0);
    test_4_( f, boost::ref(a), boost::ref(b) );
}

// 5

template<class F, class V> void test_5_(F f, V v1, V v2)
{
    test_eq( boost::bind(f, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v1, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v1, v1, v2) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v1, v2, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v2, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1), boost::bind(f, v1, v2, v1, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1), boost::bind(f, v2, v1, v1, v1, v1) );
}

template<class F> void test_5(F f)
{
    test_eq( boost::bind(f, _1, _2, _3, _4, _5), boost::bind(f, _1, _2, _3, _4, _5) );

    test_5_( f, X(1), X(2) );

    X a(0), b(0);
    test_5_( f, boost::ref(a), boost::ref(b) );
}

// 6

template<class F, class V> void test_6_(F f, V v1, V v2)
{
    test_eq( boost::bind(f, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v1, v1, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v1, v1, v1, v2) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v1, v1, v2, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v1, v2, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v2, v1, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v2, v1, v1, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1), boost::bind(f, v2, v1, v1, v1, v1, v1) );
}

template<class F> void test_6(F f)
{
    test_eq( boost::bind(f, _1, _2, _3, _4, _5, _6), boost::bind(f, _1, _2, _3, _4, _5, _6) );

    test_6_( f, X(1), X(2) );

    X a(0), b(0);
    test_6_( f, boost::ref(a), boost::ref(b) );
}

// 7

template<class F, class V> void test_7_(F f, V v1, V v2)
{
    test_eq( boost::bind(f, v1, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v1, v1, v1, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v1, v1, v1, v1, v2) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v1, v1, v1, v2, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v1, v1, v2, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v1, v2, v1, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v2, v1, v1, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v2, v1, v1, v1, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1, v1), boost::bind(f, v2, v1, v1, v1, v1, v1, v1) );
}

template<class F> void test_7(F f)
{
    test_eq( boost::bind(f, _1, _2, _3, _4, _5, _6, _7), boost::bind(f, _1, _2, _3, _4, _5, _6, _7) );

    test_7_( f, X(1), X(2) );

    X a(0), b(0);
    test_7_( f, boost::ref(a), boost::ref(b) );
}

// 8

template<class F, class V> void test_8_(F f, V v1, V v2)
{
    test_eq( boost::bind(f, v1, v1, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v1, v1, v1, v1, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v1, v1, v1, v1, v1, v2) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v1, v1, v1, v1, v2, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v1, v1, v1, v2, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v1, v1, v2, v1, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v1, v2, v1, v1, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v2, v1, v1, v1, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v2, v1, v1, v1, v1, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1, v1, v1), boost::bind(f, v2, v1, v1, v1, v1, v1, v1, v1) );
}

template<class F> void test_8(F f)
{
    test_eq( boost::bind(f, _1, _2, _3, _4, _5, _6, _7, _8), boost::bind(f, _1, _2, _3, _4, _5, _6, _7, _8) );

    test_8_( f, X(1), X(2) );

    X a(0), b(0);
    test_8_( f, boost::ref(a), boost::ref(b) );
}

// 9

template<class F, class V> void test_9_(F f, V v1, V v2)
{
    test_eq( boost::bind(f, v1, v1, v1, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v1, v1, v1, v1, v1, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v1, v1, v1, v1, v1, v1, v2) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v1, v1, v1, v1, v1, v2, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v1, v1, v1, v1, v2, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v1, v1, v1, v2, v1, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v1, v1, v2, v1, v1, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v1, v2, v1, v1, v1, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v1, v2, v1, v1, v1, v1, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1, v1, v1, v1), boost::bind(f, v1, v2, v1, v1, v1, v1, v1, v1, v1) );
    test_ne( boost::bind(f, v1, v1, v1, v1, v1, v1, v1, v1, v1), boost::bind(f, v2, v1, v1, v1, v1, v1, v1, v1, v1) );
}

template<class F> void test_9(F f)
{
    test_eq( boost::bind(f, _1, _2, _3, _4, _5, _6, _7, _8, _9), boost::bind(f, _1, _2, _3, _4, _5, _6, _7, _8, _9) );

    test_9_( f, X(1), X(2) );

    X a(0), b(0);
    test_9_( f, boost::ref(a), boost::ref(b) );
}

int main()
{
    // 0

    test_0( f_0 );
    test_0( fv_0 );

    // 1

    test_1( f_1 );
    test_1( fv_1 );

    // 2

    test_2( f_2 );
    test_2( fv_2 );

    // 3

    test_3( f_3 );
    test_3( fv_3 );

    // 4

    test_4( f_4 );
    test_4( fv_4 );

    // 5

    test_5( f_5 );
    test_5( fv_5 );

    // 6

    test_6( f_6 );
    test_6( fv_6 );

    // 7

    test_7( f_7 );
    test_7( fv_7 );

    // 8

    test_8( f_8 );
    test_8( fv_8 );

    // 9

    test_9( f_9 );
    test_9( fv_9 );

    return boost::report_errors();
}
