//  Boost string_algo library predicate_test.cpp file  ------------------//

//  Copyright Pavol Droba 2002-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// Include unit test framework
#include <boost/test/included/test_exec_monitor.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <functional>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/test/test_tools.hpp>

using namespace std;
using namespace boost;

void predicate_test()
{
    string str1("123xxx321");
    string str1_prefix("123");
    string str2("abc");
    string str3("");
    string str4("abc");
    vector<int> vec1( str1.begin(), str1.end() );

    // Basic tests
    BOOST_CHECK( starts_with( str1, string("123") ) );
    BOOST_CHECK( !starts_with( str1, string("1234") ) );

    BOOST_CHECK( istarts_with( "aBCxxx", "abc" ) );
    BOOST_CHECK( !istarts_with( "aBCxxx", "abcd" ) );

    BOOST_CHECK( ends_with( str1, string("321") ) );
    BOOST_CHECK( !ends_with( str1, string("123") ) );

    BOOST_CHECK( iends_with( "aBCxXx", "XXX" ) );
    BOOST_CHECK( !iends_with( "aBCxxX", "xXXX" ) );

    BOOST_CHECK( contains( str1, string("xxx") ) );
    BOOST_CHECK( !contains( str1, string("yyy") ) );

    BOOST_CHECK( icontains( "123XxX321", "xxx" ) );
    BOOST_CHECK( !icontains( "123xXx321", "yyy" ) );

    BOOST_CHECK( equals( str2, string("abc") ) );
    BOOST_CHECK( !equals( str1, string("yyy") ) );

    BOOST_CHECK( iequals( "AbC", "abc" ) );
    BOOST_CHECK( !iequals( "aBc", "yyy" ) );

    // multi-type comparison test
    BOOST_CHECK( starts_with( vec1, string("123") ) );
    BOOST_CHECK( ends_with( vec1, string("321") ) );
    BOOST_CHECK( contains( vec1, string("xxx") ) );
    BOOST_CHECK( equals( vec1, str1 ) );

    // overflow test
    BOOST_CHECK( !starts_with( str2, string("abcd") ) );
    BOOST_CHECK( !ends_with( str2, string("abcd") ) );
    BOOST_CHECK( !contains( str2, string("abcd") ) );
    BOOST_CHECK( !equals( str2, string("abcd") ) );

    // equal test
    BOOST_CHECK( starts_with( str2, string("abc") ) );
    BOOST_CHECK( ends_with( str2, string("abc") ) );
    BOOST_CHECK( contains( str2, string("abc") ) );
    BOOST_CHECK( equals( str2, string("abc") ) );

    //! Empty string test
    BOOST_CHECK( starts_with( str2, string("") ) );
    BOOST_CHECK( ends_with( str2, string("") ) );
    BOOST_CHECK( contains( str2, string("") ) );
    BOOST_CHECK( equals( str3, string("") ) );

    //! Container compatibility test
    BOOST_CHECK( starts_with( "123xxx321", "123" ) );
    BOOST_CHECK( ends_with( "123xxx321", "321" ) );
    BOOST_CHECK( contains( "123xxx321", "xxx" ) );
    BOOST_CHECK( equals( "123xxx321", "123xxx321" ) );
}

#define TEST_CLASS( Pred, YesInput, NoInput )\
{\
    BOOST_CHECK( all( string(YesInput), Pred ) );\
    BOOST_CHECK( !all( string(NoInput), Pred ) );\
}

void classification_test()
{
    TEST_CLASS( is_space(), "\n\r\t ", "..." );
    TEST_CLASS( is_alnum(), "ab129ABc", "_ab129ABc" );
    TEST_CLASS( is_alpha(), "abc", "abc1" );
    TEST_CLASS( is_cntrl(), "\n\t\r", "..." );
    TEST_CLASS( is_digit(), "1234567890", "abc" );
    TEST_CLASS( is_graph(), "123abc.,", "  \t" );
    TEST_CLASS( is_lower(), "abc", "Aasdf" );
    TEST_CLASS( is_print(), "abs", "\003\004asdf" );
    TEST_CLASS( is_punct(), ".,;\"", "abc" );
    TEST_CLASS( is_upper(), "ABC", "aBc" );
    TEST_CLASS( is_xdigit(), "ABC123", "XFD" );
    TEST_CLASS( is_any_of( string("abc") ), "aaabbcc", "aaxb" );
    TEST_CLASS( is_any_of( "abc" ), "aaabbcc", "aaxb" );
    TEST_CLASS( is_from_range( 'a', 'c' ), "aaabbcc", "aaxb" );

    TEST_CLASS( !is_classified(std::ctype_base::space), "...", "..\n\r\t " );
    TEST_CLASS( ( !is_any_of("abc") && is_from_range('a','e') ) || is_space(), "d e", "abcde" );
}

#undef TEST_CLASS

// test main 
int test_main( int, char*[] )
{
    predicate_test();
    classification_test();
    
    return 0;
}
