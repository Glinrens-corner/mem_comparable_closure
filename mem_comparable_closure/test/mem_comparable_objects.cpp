#include "doctest.h"
#include "mem_comparable_closure.hpp"
#include "mem_comparable_vector.hpp"
#include <tuple>
namespace {
  
  template<class Fun1_t, class Fun2_t>
  bool test_identical(Fun1_t& fun1, Fun2_t& fun2 , std::size_t& counter){
    using namespace mem_comparable_closure;
    constexpr auto is_null = [](const void* ptr)->bool {return ! ptr;}; 
    
    if (! std::is_same<Fun1_t, Fun2_t>::value ) return false;
    
    auto stack1 = algorithm::IteratorStack{};
    auto stack2 = algorithm::IteratorStack{};
    MemCompareInfo info1 = get_mem_compare_info(&fun1,nullptr,nullptr,stack1);
    MemCompareInfo info2 = get_mem_compare_info(&fun2,nullptr,nullptr,stack2);
 
    while (counter>0) {
      if ( is_null( info1.next_obj) or is_null(info2.next_obj) ){
#ifndef NDEBUG
	if (is_null(info1.next_obj)){
	  assert(stack1.get_size() == 0);
	  assert(! info1.continuation_fn);
	    
	};
	if (is_null(info2.next_obj)){
	  assert(stack2.get_size() == 0);
	  assert(!info2.continuation_fn);
	};
#endif
	if ( not ( is_null(info1.next_obj) and is_null(info2.next_obj))) return false;
	return true;
      } else if (is_null(info1.obj) or is_null(info2.obj) ) {
#ifndef NDEBUG
	if (is_null(info1.obj)){
	  assert(info1.size == 0);
	};
	if (is_null(info2.next_obj)){
	  assert(info2.size == 0);
	};
#endif
	if ( not ( is_null(info1.obj) and is_null(info2.obj))) return false;
	assert(  info1.continuation_fn);
	assert(  info1.next_obj);  
	info1 = info1.continuation_fn(stack1, info1.next_obj );
	  
	assert(  info2.continuation_fn);
	assert(  info2.next_obj);
	info2 = info2.continuation_fn(stack2, info2.next_obj );
	
      }else {
	if (! detail::is_identical_object( info1,info2)) return false;
	assert(  info1.continuation_fn);
	assert(  info1.next_obj);  
	info1 = info1.continuation_fn(stack1, info1.next_obj );
	  
	assert(  info2.continuation_fn);
	assert(  info2.next_obj);
	info2 = info2.continuation_fn(stack2, info2.next_obj );
      };
      --counter;
    }
    return false;
  }
}





TEST_CASE("vector" ){
  using namespace mem_comparable_closure;


  auto vec1 = std::vector<int>{};
  auto vec2 = std::vector<int>{};
  std::size_t counter = 100;
  CHECK(test_identical(vec1,vec2, counter));
  CHECK(counter == 99);

}

struct myStruct{
  int i=0;
  float j = 1.6;
  bool k = true;
  decltype(auto) get_member_access()const{
    return std::make_tuple(&(this->i),&(this->j),&(this->k));
  }
};

template<>
struct mem_comparable_closure::concepts::is_member_accessible<myStruct> : std::true_type{};

TEST_CASE("struct" ){

  
  using namespace mem_comparable_closure;
    


  auto struct1 = myStruct{};
  auto struct2 = myStruct{};
  std::size_t counter = 100;
  CHECK(test_identical(struct1,struct2, counter));
  CHECK(counter == 97);
  myStruct struct3{0,1.7,true};
  counter = 100;
  CHECK_FALSE(test_identical(struct1,struct3, counter));
}
