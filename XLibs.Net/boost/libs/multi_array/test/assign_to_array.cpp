// Copyright 2002 The Trustees of Indiana University.

// Use, modification and distribution is subject to the Boost Software 
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  Boost.MultiArray Library
//  Authors: Ronald Garcia
//           Jeremy Siek
//           Andrew Lumsdaine
//  See http://www.boost.org/libs/multi_array for documentation.

// 
// assign_to_array.cpp - multi_array should be constructible from any other
// array type in the library.
// 
//
#include "generative_tests.hpp"
#include "boost/array.hpp"
#include "boost/multi_array.hpp"
#include "boost/cstdlib.hpp"
#include <algorithm>
#include <iostream>

bool equal(const int& a, const int& b)
{
  return a == b;
}

template <typename ArrayA, typename ArrayB>
bool equal(const ArrayA& A, const ArrayB& B)
{
  typename ArrayA::const_iterator ia;
  typename ArrayB::const_iterator ib = B.begin();
  for (ia = A.begin(); ia != A.end(); ++ia, ++ib)
    if (!equal(*ia, *ib))
      return false;
  return true;
}


template <typename Array>
void access(Array& A, const mutable_array_tag&) {

  assign(A);
  access(A,const_array_tag());
}

template <typename Array>
void access(Array& A, const const_array_tag&) {
  typedef boost::multi_array<int,3> array3;
  array3 acopy(A);
  BOOST_TEST(equal(acopy,A));
  ++tests_run;
}


int test_main(int,char*[]) {
  return run_generative_tests();
}
