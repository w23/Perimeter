/*=============================================================================
    Copyright (c) 2004 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

//  This is a compile only test for verifying, whether the multi_pass<>
//  iterator works ok with an input iterator, which returns a value_type and not 
//  a reference from its dereferencing operator.

#include <cstdio>
#include <fstream>
#include <iterator>

#include <boost/spirit/core.hpp>
#include <boost/spirit/iterator/multi_pass.hpp>

using namespace boost::spirit;
using namespace std;

int main () 
{
    // create a sample file
    {
        ofstream out("./input_file.txt");
        out << 1.0 << "," << 2.0;
    }

    int result = 0;

    // read in the values from the sample file
    {
        ifstream in("./input_file.txt"); // we get our input from this file

        typedef char char_t;
        typedef multi_pass<istreambuf_iterator<char_t> > iterator_t;

        typedef skip_parser_iteration_policy<space_parser> iter_policy_t;
        typedef scanner_policies<iter_policy_t> scanner_policies_t;
        typedef scanner<iterator_t, scanner_policies_t> scanner_t;

        typedef rule<scanner_t> rule_t;

        iter_policy_t iter_policy(space_p);
        scanner_policies_t policies(iter_policy);
        iterator_t first(make_multi_pass(std::istreambuf_iterator<char_t>(in)));
        scanner_t scan(first, make_multi_pass(std::istreambuf_iterator<char_t>()),
            policies);

        rule_t n_list = real_p >> *(',' >> real_p);
        match<> m = n_list.parse(scan);

        result = !m ? 1 : 0;
    }
    
    unlink("./input_file.txt");
    return result;
}
