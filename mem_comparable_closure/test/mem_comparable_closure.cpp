#include "doctest.h"

#include "mem_comparable_closure.hpp"
#include <type_traits>
#include <array>

enum class MyEnum: int{
  a,b,c};
  
template<>
struct  ::mem_comparable_closure::concepts::is_trivial<MyEnum> : std::true_type{};

class MyMemberAccessibleClass{
  std::tuple<const int*, const bool*> get_member_access(){
    return std::tuple<const int*, const bool*>(&(this->i), &(this->f));
  };
  
private:
  int i=127;
  bool f=false;
};

template<>
struct ::mem_comparable_closure::concepts::is_member_accessible<MyMemberAccessibleClass> : std::true_type{};

TEST_CASE("concepts"){
  using  mem_comparable_closure::concepts::is_transparent;
  
  class MyIntransparentStruct {};
  CHECK_FALSE(is_transparent< MyIntransparentStruct>::value);

  SUBCASE("is_trivial"){
    enum class MyIntransparentEnum: int{
      a,b,c};
    CHECK_FALSE(is_transparent< MyIntransparentEnum>::value);
    CHECK(is_transparent< MyEnum>::value);
  };

  SUBCASE("is_member_accessible"){
    
    CHECK(is_transparent< MyMemberAccessibleClass>::value);
  }
};



TEST_CASE("IteratorStack"){
  using  mem_comparable_closure::algorithm::IteratorStack;
  SUBCASE("basic test"){
    IteratorStack stack{};
    
    REQUIRE(stack.get_size()==0 );
    new (stack.get_new<int>()) int{5};
    CHECK(stack.get_size() >= sizeof(int));
    CHECK( stack.get_last<int>()== 5);
    stack.pop_last<int>();
    CHECK(stack.get_size() == 0);
  };
  SUBCASE("multiple"){
    IteratorStack stack{};

    new (stack.get_new<long>()) long{5};
    new (stack.get_new<int>()) int{4};
    new (stack.get_new<short>()) short{3};
    CHECK( stack.get_last<short>()== 3);
    stack.pop_last<short>();
    CHECK( stack.get_last<int>()== 4);
    stack.pop_last<int>();
    CHECK( stack.get_last<long>()== 5);
    stack.pop_last<long>();
  };
  
  SUBCASE("basic test"){
    IteratorStack stack{};
    
    constexpr std::size_t stack_init_max_size = IteratorStack::get_init_max_size();
    using array_t = std::array<char , stack_init_max_size>;
    new (stack.get_new<int>()) int{4};
    stack.get_new<array_t>();
    CHECK( stack.get_size()>=(stack_init_max_size+sizeof(int)));
    new (stack.get_new<int>()) int{16};
    CHECK( stack.get_last<int>()== 16);
    stack.pop_last<int>();
    stack.pop_last<array_t>();
    CHECK( stack.get_last<int>()== 4);
    stack.pop_last<int>();
    CHECK(stack.get_size() == 0 );
  };
}



TEST_CASE("Closure"){
  using namespace mem_comparable_closure;

  SUBCASE("creation"){
    using closure_t = Closure<
      FunctionSignature<int,int,int> >;

    SUBCASE("closure_from_fp"){
      int(*fp)(int,int)  = [](int a,int b)->int{return static_cast<char>(a);};
      auto closure = closure_from_fp(fp);
      CHECK(std::is_same<decltype(closure), closure_t>::value);
    };
    using closure_t = Closure<
      FunctionSignature<int,int,int> >;
    
    auto closure = ClosureMaker<int,int,int>::make([](int a, int b){return a; } );
    CHECK(std::is_same<decltype(closure), closure_t>::value);
    CHECK(closure(1,2 ) == 1);
    auto new_closure = closure.bind(2);
    CHECK(std::is_trivially_copyable<decltype(closure)>::value );
    CHECK(std::is_trivially_copyable<decltype(new_closure)>::value );
    CHECK(new_closure(3) == 2);

  };
  SUBCASE("Metaprogramming Errors"){ 
    // we would need to check that this gives an compiler error
    // test::check_transparency<int&> h{};
    // this however does not.
    using G  = test::check_transparency<int,bool>::type ;
    G g = true;
    CHECK(g);
    // Compiler Errors
    //    auto closure2 = ClosureMaker<int,int&,int>::make([](int a, int b )->int{return b;});
    //     int& is not transparent
    //    auto closure3 = ClosureMaker<int,int,int>::make([](int& a, int b )->int{return b;});
    //     int& is not transparent
    // it s ok as long as you don't try to bind anything to it.
    auto closure4 = ClosureMaker<int,int,int&>::make([](int a, int& b )->int{return a;});
    auto closure4_1 = closure4.bind(1);
    // auto closure4_2 = closure4_1.bind(100);
  };
					    
  SUBCASE("check equality"){
    int (*fn)(int,int) = [](int a, int b ) -> int { return a;};
    auto closure1 =ClosureMaker<int,int,int>::make(fn).bind(2).as_fun();
    auto closure2=ClosureMaker<int,int,int>::make(fn).bind(2).as_fun();;
    auto closure3=ClosureMaker<int,int,int>::make(fn).bind(3).as_fun();

    SUBCASE("home"){
      using  mem_comparable_closure::algorithm::IteratorStack;
      auto stack = IteratorStack();
      
      auto closure1 =ClosureMaker<int,int,int, int>::make([](int a, int b, int c ) -> int { return a;}).bind(2).bind(3).as_fun();
      int counter = 1;
      auto info = closure1.get_mem_compare_info(nullptr, nullptr, stack);
      REQUIRE(static_cast<bool>(info.next_obj));
      while ( true){
	if (! info.next_obj ){
	  break;
	}else {
	  counter +=1;
	};
	info = info.continuation_fn(stack, info.next_obj );
      };
      CHECK(counter ==3);
      
    };
    CHECK_FALSE(is_updated( closure1,closure2) );
    CHECK(is_identical(closure1,closure2));
    //    REQUIRE(mem_info1.size==mem_info2.size );
    //    CHECK(std::memcmp(mem_info1.obj, mem_info2.obj, mem_info2.size) == 0);
    
    CHECK_FALSE(is_identical(closure2,closure3));
    CHECK(is_updated( closure2,closure3) );
    //    REQUIRE(mem_info2.size==mem_info3.size );
    //    CHECK(std::memcmp(mem_info2.obj, mem_info3.obj, mem_info2.size) != 0);
  };

}
