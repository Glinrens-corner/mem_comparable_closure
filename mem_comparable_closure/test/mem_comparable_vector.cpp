#include "doctest.h"
#include "mem_comparable_vector.hpp"








TEST_CASE("vector" ){
  using namespace mem_comparable_closure;


  auto vec = std::vector<int>{};
  auto stack = algorithm::IteratorStack{};
  algorithm::get_mem_compare_info(&vec,nullptr,nullptr, stack);



}
