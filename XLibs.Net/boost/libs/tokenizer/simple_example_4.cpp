// � Copyright John R. Bandela 2001. 

// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all
// copies. This software is provided "as is" without express or
// implied warranty, and with no claim as to its suitability for any
// purpose.

// See http://www.boost.org/libs/tokenizer for documenation

// simple_example_4.cpp
#include<iostream>
#include<boost/tokenizer.hpp>
#include<string>

int main(){
   using namespace std;
   using namespace boost;
   string s = "This is,  a test";
   tokenizer<char_delimiters_separator<char> > tok(s);
   for(tokenizer<char_delimiters_separator<char> >::iterator beg=tok.begin(); beg!=tok.end();++beg){
       cout << *beg << "\n";
   }
   return 0;
}

