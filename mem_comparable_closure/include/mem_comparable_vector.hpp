#ifndef MEM_COMPARABLE_VECTOR_HPP
#define MEM_COMPARABLE_VECTOR_HPP

#include "mem_comparable_closure.hpp"
#include <vector>

namespace mem_comparable_closure{
  template<class T, class Alloc>
  struct concepts::is_specialized<std::vector<T, Alloc>> : std::true_type{};

  struct VectorCompareIterator{
    const void * next_obj;
    mem_compare_continuation_fn_t  continuation_fn;

    std::size_t next_element;
    std::size_t size;
  };
  
  namespace algorithm {
    namespace{
      template <class T, class Alloc>
	typename std::enable_if<concepts::is_trivial<T>::value
				and not std::is_same<T,bool>::value,MemCompareInfo>::type
	continue_vector_mem_compare_info(IteratorStack& stack,
					 const void* obj){
	auto self = static_cast< const std::vector<T,Alloc>*>( obj);
	auto it = stack.get_last<VectorCompareIterator>();
	if ( it.next_element == self->size()  ){
	  auto it = stack.pop_last<VectorCompareIterator>();
	  return MemCompareInfo{
	    .next_obj = it.next_obj,
	      .continuation_fn = it.continuation_fn,
	      .obj  = nullptr,
	      .size =0
	      };
	}else {
	  assert(it.next_element == 0);
	  it.next_element = self->size();
	  return MemCompareInfo{
	    .next_obj = obj,
	      .continuation_fn = continue_vector_mem_compare_info<T,Alloc>,
	      .obj  = static_cast<const void*>(self->data()),
	      .size = sizeof(T)*self->size()
	      };
	};
      };

      template <class T, class Alloc>
	typename std::enable_if<!concepts::is_trivial<T>::value
				,MemCompareInfo>::type
	continue_vector_mem_compare_info(IteratorStack& stack,
					 const void* obj){
	auto self = static_cast< const std::vector<T,Alloc>*>( obj);
	auto it = stack.get_last<VectorCompareIterator>();
	if ( it.next_element == self->size()  ){
	  auto it = stack.pop_last<VectorCompareIterator>();
	  return MemCompareInfo{
	    .next_obj = it.next_obj,
	      .continuation_fn = it.continuation_fn,
	      .obj  = nullptr,
	      .size =0
	      };
	}else {
	  std::size_t this_element = it.next_element;
	  it.next_element = it.next_element+1;
	  return get_mem_compare_info( self->data()+this_element,
				       obj,
				       continue_vector_mem_compare_info<T,Alloc>,
				       stack
				       );
	};
      };
    }
    
    template<class T, class Alloc>
    MemCompareInfo get_mem_compare_info(const std::vector<T, Alloc>* vec,
					const void* next_obj,
					mem_compare_continuation_fn_t continuation_fn,
				        IteratorStack& stack){
      new (stack.get_new<VectorCompareIterator>()) VectorCompareIterator{
	.next_obj = next_obj,  
	  .continuation_fn = continuation_fn,
	  .next_element=0,
	  .size=vec->size()};
      
      auto it = stack.get_last<VectorCompareIterator>();
      assert(vec);
      return MemCompareInfo{
	.next_obj = static_cast<const void*>(vec),
	  .continuation_fn = continue_vector_mem_compare_info<T,Alloc>,
	  .obj  = static_cast<const void*>(&(it.size)),
	  .size =sizeof(std::size_t)
	  };
      
    };
    
  };//detail
    
}// mem_comparable_closure













#endif //MEM_COMPARABLE_VECTOR_HPP
