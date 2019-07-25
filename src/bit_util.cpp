#include "bit_util.h"

#ifdef __cplusplus
namespace mystd{
///////////////////////////
 
     unsigned char bit_get_true(std::size_t index) // 1 - 8
	{
		assert(index >= 1 && index <= 8);
		return (unsigned char)1 << (index - 1) ;
	}
	 unsigned char bit_get_false(std::size_t index)
	{
		assert(index >= 1 && index <= 8);
		return ~bit_get_true(index);
	}
 
	void bit_set_false(unsigned char& val, std::size_t index) // index (1 - 8)
	{
		assert(index >= 1 && index <= 8);
		val &= bit_get_false(index);
	}
	void bit_set_true( unsigned char& val, std::size_t index) // index (1 - 8) 
	{
		assert(index >= 1 && index <= 8);
		val |= bit_get_true(index);
	}
 
	void* bit_set(void* ptr, std::size_t pos, bool val = true)
	{
		assert(ptr != 0);
		unsigned char *pointer = (unsigned char*)ptr;
		std::size_t subpos = (pos + 7)/ 8 - 1; 
		std::size_t index = (pos + 7) % 8  + 1;
		if(val)
			bit_set_true(pointer[subpos],index);
		else
			bit_set_false(pointer[subpos],index);
		return ptr;
	}
 
	bool bit_read(void *ptr, std::size_t pos) 
	{
		assert(ptr != 0);
		unsigned char *pointer = (unsigned char*)ptr;
		std::size_t subpos = (pos + 7)/ 8 - 1;  
		std::size_t index = (pos  + 7) % 8 + 1;
		unsigned char tmp_val = (pointer[subpos] >> (index - 1) ) & (unsigned char)1;
		return tmp_val > 0;
	}
}
#endif //__cplusplus
