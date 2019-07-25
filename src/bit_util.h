#ifndef MYSTD_BITSET_UTIL_TEST_H
#define MYSTD_BITSET_UTIL_TEST_H
#include <cassert>  // assert
#include <cstddef>   // std::size_t
#include <string>   // std::string 
#include <cstring>   // std::memset
#include <exception>   
#include <iostream>    

#ifdef __cplusplus
namespace mystd{
///////////////////////////
 
  unsigned char bit_get_true(std::size_t index);
  
	unsigned char bit_get_false(std::size_t index);
 
	void bit_set_false(unsigned char& val, std::size_t index);
  
	void bit_set_true( unsigned char& val, std::size_t index);
 
	void* bit_set(void* ptr, std::size_t pos, bool val = true);
  
	bool bit_read(void *ptr, std::size_t pos);
}
    
#endif //__cplusplus

#endif //MYSTD_BITSET_UTIL_TEST_H
