/*=============================================================================
    Copyright (c) 2003 Jonathan de Halleux (dehalleux@pelikhan.com)
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

///////////////////////////////////////////////////////////////////////////////
// Test suite for assign_key_actor
///////////////////////////////////////////////////////////////////////////////

#include <boost/spirit.hpp>
#include <map>
#include <cstring>
#include <iostream>
#include <boost/spirit/actor/assign_key_actor.hpp>
#include <boost/spirit/actor/assign_actor.hpp>

#include <boost/test/unit_test.hpp>


void assign_key_single_argument_test()
{
    using namespace boost::unit_test_framework;
    using namespace boost::spirit;

    const char* cp = "(one,0),(two,1),(three,2)";
    const char* cp_first = cp;
    const char* cp_last = cp + strlen(cp);
    const char* cp_i[] = {"one","two","three"};
    int i;
    typedef std::map<int,std::string> map_string_type;
    map_string_type c;
    map_string_type::const_iterator it_find;
    std::string str;

    scanner<char const*> scan( cp_first, cp_last );
    match<> hit;

    hit = list_p(
            confix_p(
                '(',
                    (*alpha_p)[ assign(str)]
                    >>ch_p(',')
                    >> int_p[ assign_key_a(c,str)]
                    ,
                ')'
                )
         ,
        ch_p(',')
        ).parse(scan);
    BOOST_CHECK(hit);
    BOOST_CHECK_EQUAL(scan.first, scan.last);
    BOOST_CHECK_EQUAL( c.size(), static_cast<map_string_type::size_type>(3));
    for (i=0;i<3;++i)
    {
        it_find = c.find(i);
        BOOST_CHECK( it_find != c.end() );
        BOOST_CHECK_EQUAL( i,it_find->first);
        BOOST_CHECK_EQUAL( cp_i[i],it_find->second);
    }

}

void assign_key_two_arguments_test()
{
    using namespace boost::unit_test_framework;
    using namespace boost::spirit;

    const char* cp = "(0,one),(1,two),(2,three)";
    const char* cp_first = cp;
    const char* cp_last = cp + strlen(cp);
    const char* cp_i[] = {"one","two","three"};
    int i;
    typedef std::map<std::string, int> map_string_type;
    map_string_type c;
    map_string_type::const_iterator it_find;
    std::string str;

    scanner<char const*> scan( cp_first, cp_last );
    match<> hit;

    hit = list_p(
        confix_p(
        '(',
            int_p[ assign_a(i)]
            >>ch_p(',')
            >> (*alpha_p)[ assign_key_a(c,i)]
            ,
                ')'
                )
                ,
                ch_p(',')
                ).parse(scan);

    BOOST_CHECK(hit);
    BOOST_CHECK_EQUAL(scan.first, scan.last);
    BOOST_CHECK_EQUAL( c.size(), static_cast<map_string_type::size_type>(3));

    for (i=0;i<3;++i)
    {
         it_find = c.find(cp_i[i]);
         BOOST_CHECK( it_find != c.end() );
         BOOST_CHECK_EQUAL( i,it_find->second);
         BOOST_CHECK_EQUAL( cp_i[i],it_find->first);
    }
    scan.first = cp;

}

void assign_key_action_test(boost::unit_test_framework::test_suite* test)
{
    test->add( BOOST_TEST_CASE(&assign_key_single_argument_test ) );
    test->add( BOOST_TEST_CASE(&assign_key_two_arguments_test ) );
}

