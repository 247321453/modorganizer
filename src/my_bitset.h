
#ifndef MYSTD_BITSET_TEST_H
#define MYSTD_BITSET_TEST_H
#include <cassert>  // assert
#include <cstddef>   // std::size_t
#include <string>   // std::string 
#include <cstring>   // std::memset
#include <exception>   
#include <iostream>    

#ifdef __cplusplus
 
namespace mystd{
  
	class  invalid_argument_1{};
 
	template<std::size_t N>
	class bitset{
	private:
		typedef bitset self;
		typedef std::size_t bit_size;
		typedef unsigned long ULONG;
		typedef unsigned long long ULLONG;
		typedef unsigned char UCHAR;
	public:
		static unsigned char bit_get_true(std::size_t index) // 1 - 8
		{
			assert(index >= 1 && index <= 8);
			return (unsigned char)1 << (index - 1) ;
		}

	    static unsigned char bit_get_false(std::size_t index)
		{
			assert(index >= 1 && index <= 8);
			return ~bit_get_true(index);
		}
 
		static void bit_set_false(unsigned char& val, std::size_t index) // index (1 - 8)
		{
			assert(index >= 1 && index <= 8);
			val &= bit_get_false(index);
		}

		static void bit_set_true( unsigned char& val, std::size_t index) // index (1 - 8) 
		{
			assert(index >= 1 && index <= 8);
			val |= bit_get_true(index);
		}
 
		static void* bit_set(void* ptr, std::size_t pos, bool val = true)
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
 
		static bool bit_read(void *ptr, std::size_t pos) 
		{
			assert(ptr != 0);
			unsigned char *pointer = (unsigned char*)ptr;
			std::size_t subpos = (pos + 7)/ 8 - 1;  
			std::size_t index = (pos  + 7) % 8 + 1;
			unsigned char tmp_val = (pointer[subpos] >> (index - 1) ) & (unsigned char)1;
			return tmp_val > 0;
		}

	private:
		class ref{
		private:
			typedef std::size_t bit_size;
			typedef ref self;
			UCHAR *head;
			bit_size position;
		public:
			ref(UCHAR* tmp_ptr,bit_size pos):head(tmp_ptr),position(pos)
			{
			}
		public:
			self& operator = (const self& tmp)
			{
				//pos以1开始计数,因此加1,以下都是如此
				bitset::bit_set(head,position+1, bitset::bit_read(tmp.head,tmp.position+1));
				return *this;
			}
			self& operator = (const bool& tmp)
			{
				assert(head != 0);
				bitset::bit_set(head,position + 1,tmp);
				return *this;
			}
			bool operator == (const bool& tmp) const 
			{
				assert(head != 0);
				return bitset::bit_read(head,position + 1) == tmp;
			}
      
      bool operator && (const bool& tmp) const 
			{
				assert(head != 0);
				return bitset::bit_read(head,position + 1) && tmp;
			}
      
      bool operator && (const self& tmp) const 
			{
				assert(head != 0);
				return bitset::bit_read(head, position + 1) && bitset::bit_read(tmp.head, tmp.position + 1);
			}
      
			bool operator != (const bool& tmp) const
			{
			    assert(head != 0);
				return !(*this == tmp);
			}
			bool operator == (const self& tmp) const 
			{
				assert(head != 0);
				return bitset::bit_read(head,position + 1) == 
					bitset::bit_read(tmp.head,tmp.position + 1);
			}
			bool operator != (const self& tmp) const 
			{
              return !(*this == tmp);
			}
			
			friend std::ostream& operator << (std::ostream& os,const self& tmp)
			{
				return os << bitset::bit_read(const_cast<UCHAR*>(tmp.head),tmp.position + 1);
			}
		};
 
	private:
		typedef ref reference;
		typedef const ref const_reference;
		static const bit_size SIZE = N;
		UCHAR *head;
	private:   //内部使用的一些函数
		bool read(void *ptr,bit_size pos) const   // 从0开始
		{ // mystd里的read以1开始计数，在这里转换下,以下同理可得
			return bitset::bit_read(ptr,pos+1);
		}
		void* set(void* ptr,bit_size pos,bool val = true) const  // 从0开始
		{ //从逻辑上讲，不应该用const，为了通用性，加了个const
		  // 也可以另外写一个非const版本
			return bitset::bit_set(ptr,pos+1,val);
		}
		bit_size get_pos(bit_size pos) const   // 从0开始
		{ // 举例说明，低位到高位假如有10位1000 0001 11 则最后一位getpos的结果为1,
			// 是第二个字节，getsub则是1
			return pos / 8;
		}
		bit_size get_sub(bit_size pos) const   // 从0开始
		{  // 
			return pos % 8;
		}
		// 计算有效位数,由低位到高位，如11010000有效位为4位
		bit_size get_num_count() const 
		{
			bit_size m_count = 0;
			for(bit_size i = size(); i >= 1; )
				if(test(--i))
					break;
				else
					++m_count;
			return size() - m_count;
		}
		void zero_last() //将最后剩余位清零
		{  //如，有4位1101,但是分配的是一个字节，最后的4位无用，清零
			bit_size cur_pos = get_pos(size());
			bit_size cur_sub = get_sub(size());
			++cur_sub;
			for(bit_size i = cur_sub; i < 8; ++i)
				set(&head[cur_pos],i,false);
		}
		void fill(bit_size first,bit_size last,bool val = false) 
		{  // 对区间[first,last)内的位进行填充
			assert(first >= 0 && last <= size());
			for(bit_size i = first; i < last; ++i)
				set(i,val);  // 成员函数
		}
		void copy_left(bit_size des,bit_size beg,bit_size end)
		{  // 区间[beg,end);
			assert(des >= 0 && des <= beg);
			if(des == beg)  // 移动0位的情况
				return ;
			while(beg != end)
				set(des++,test(beg++));
		}
		void copy_right(bit_size des,bit_size beg,bit_size end)
		{  // 向后复制,des为末端位置加 1
		   assert(des >= end && des <= size());
		   if(end == size())   // 移动0位的情况
			   return ;
		   while(end != beg)
			   set(--des,test(--end));
		}
	public:
		bitset()  
			try:head(0)   
		{  //计算分配的字节数，如SIZE 为10则分配2个字节就可以了，剩余6位。（16-10）
			// 以下同理可得
			head = new UCHAR[get_pos(size()) + 1];
			std::memset(head,0,get_pos(size()) + 1); // 
		}
		catch(...)
		{
#ifdef _DEBUG
    std::cerr<<"out of memory"<<std::endl;
#endif
		}
		explicit bitset(ULONG val)    // unsigned long 版本
			try:head(0)
		{
			head = new UCHAR[get_pos(size()) + 1];
			std::memset(head,0,get_pos(size()) + 1); // 
			for(bit_size i = 0; i < size() && i < sizeof(ULONG) * 8; ++i)
				set(i,read(&val,i));
		}
		catch(...)
		{
#ifdef _DEBUG
    std::cerr<<"out of memory"<<std::endl;
#endif
		}
		explicit bitset(const std::string& str,bit_size pos = 0,bit_size n = std::string::npos)
			try:head(0)
		{
			
			head = new UCHAR[get_pos(size()) + 1];
			std::memset(head,0,get_pos(size()) + 1); // 
			assert(n == std::string::npos || pos + n < str.size());
			if(pos + n > str.size())
				n = str.size() - pos;
			for(bit_size i = 0,j = pos + n; i < size() && j >= pos + 1; )
			{
				assert(str[j-1] == '0' || str[j-1] == '1');
#ifdef NDEBUG
				if(str[j-1] != '0' && str[j-1] != '1')
					throw mystd::invalid_argument_1();  //非法参数，抛出异常
#endif
				set(head,i++,str[--j] == '1');
			}
		}
		catch(...)
		{
#ifdef _DEBUG
    std::cerr<<"out of memory"<<std::endl;
#endif
		}
		 //复制构造函数
		 bitset(const self& temp):head(0)
		{
			head = new UCHAR[get_pos(size()) + 1];
			std::memcpy(head,temp.head,get_pos(size()) + 1);
		}
		self& operator = (const self& temp)  //复制赋值
		{    // 直接覆盖，这里不用先释放资源，因为类型一致，SIZE一样
			if(this == &temp)
				return *this;
			std::memcpy(head,temp.head,get_pos(size()) + 1);
			return *this;
		}
		~bitset()
		{
			delete [] head;
		}
	public:
		const_reference operator [] (bit_size pos) const 
		{
			assert(pos < size());
	        return reference(head, pos);
		}
	
		reference operator [] (bit_size pos)
		{
			assert(pos < size());
			return reference(head, pos);
		}
 
		bit_size size() const 
		{
			return SIZE;
		}
 
		bit_size count() const 
		{
			bit_size m_count = 0;
			for(bit_size i = 0; i < size(); ++i)
			{
			bit_size cur_pos = get_pos(i);
			bit_size cur_sub = get_sub(i);
			if(read(&head[cur_pos],cur_sub))
				++m_count;
			}
			return m_count;
		}
 
       self& set()
	   {
		   std::memset(head,0xFF,get_pos(size()) + 1);
		   // 将剩余的位清零
		   zero_last();
		   return *this;
	   }
       self& set(bit_size pos, bool val = true)
	   {
		   assert(pos < size());
		   bit_size cur_pos = get_pos(pos);
		   bit_size cur_sub = get_sub(pos);
		   set(&head[cur_pos],cur_sub,val);
		   return *this;
	   }
 
	   bool any() const 
	   {
	  for(bit_size i = 0; i <= get_pos(size()); ++i)
		  if(head[i])
			  return true;
	  return false;
	   }
	   bool none() const 
	   {
		   for(bit_size i = 0; i <= get_pos(size()); ++i)
			   if(head[i])
				   return false;
		   return true;
	   }
	   self& reset()
	   {
		 std::memset(head,0,get_pos(SIZE) + 1);
		 return *this;
	   }
	   self& reset(bit_size pos)
	   {
		   assert(pos < size());
		   bit_size cur_pos = get_pos(pos);
		   bit_size cur_sub = get_sub(pos);
		   set(&head[cur_pos],cur_sub,false);
		   return *this;
	   }
	   bool test(size_t pos) const
	   {
		   assert(pos < size());
           bit_size cur_pos = get_pos(pos);
		   bit_size cur_sub = get_sub(pos);
		   return read(&head[cur_pos],cur_sub);
	   }
 
	   self& flip()
	   {
		   bit_size pos = get_pos(size());
		   for(bit_size i = 0; i <= pos; ++i)
			   head[i] = ~head[i];
		   zero_last();
		   return *this;
	   }
	   self& filp(bit_size pos)
	   {
		   assert(pos < size());
		   if(test(pos))
			   set(pos,false);
		   else
			   set(pos,true);
		   return *this;
	   }
	   std::string to_string() const 
	   {
		   std::string str(size(),'0');   // 自动增长效率低，所以直接初始化
		   for(bit_size i = size(),j = 0; i >= 1; )
			   if(test(--i))
				   str[j++] = '1';
			   else
				   ++j; // 不做无用功，直接跳过，不用复制了
		   return str;
	   }
	   ULONG to_ulong() const 
	   { 
		   bit_size num_count = get_num_count();
		   assert(num_count <= sizeof(ULONG) * 8);
#ifdef NDEBUG
		   if(num_count > sizeof(ULONG) * 8) 
			   throw std::overflow_error("overflow error,please check it");
#endif
		   if(size() == sizeof(ULONG) * 8 || num_count == sizeof(ULONG) * 8)
		       return *(ULONG*)head;
		   ULONG tmp_val = 0;
		   for(bit_size i = 0; i < num_count; ++i)
			   set(&tmp_val,i,test(i));
		   return tmp_val;
	   }
	   ULLONG to_ullong() const   // c++11 功能
	   {
		   bit_size num_count = get_num_count();
		   assert(num_count <= sizeof(ULLONG) * 8);
#ifdef NDEBUG
		     if(num_count > sizeof(ULLONG) * 8) 
			   throw std::overflow_error("overflow error,please check it");
#endif
		   if(size() == sizeof(ULLONG)* 8 || num_count == sizeof(ULLONG) * 8)
		       return *(ULLONG*)head;
		   if(num_count <= sizeof(ULONG) * 8) 
			   return to_ulong();
		   ULLONG tmp_val = 0;
		   for(bit_size i = 0; i < num_count; ++i)
			   set(&tmp_val,i,test(i));
		   return tmp_val;
	   }
	   //下面是一些位操作符的重载
	   self& operator &= (const self& obj)
	   {
		  bit_size position = get_pos(size());
		  for(bit_size i = 0; i <= position ; ++i)
			  head[i] &= obj.head[i];
		  return *this;
	   }
	   self& operator |= (const self& obj)
	   {
		  bit_size position = get_pos(size());
		  for(bit_size i = 0; i <= position; ++i)
			  head[i] |= obj.head[i];
		  return *this;
	   }
	   self& operator ^= (const self& obj)
	   {
		  bit_size position = get_pos(size());
		  for(bit_size i = 0; i <= position; ++i)
			  head[i] ^= obj.head[i];
		  return *this;
	   }
	   self operator | (const self& obj) const 
	   {
		   self temp(*this);
		   temp |= obj;
		   return temp;
	   }
	   self operator & (const self& obj) const 
	   {
		   self temp(*this);
		   temp &= obj;
		   return temp;
	   }
	   self operator ^ (const self& obj) const
	   {
		   self temp(*this);
		   temp ^= obj;
		   return temp;
	   }
	   self operator ~ () const 
	   {
		   self temp(*this);
		   temp.flip();
		   return temp;
	   }
	   self& operator <<= (bit_size n)
	   {
		   if(n == 0)
			   return *this;
		   if(n >= size())
			  std::memset(head,0,get_pos(size()) + 1);
		   //相对向右复制覆盖
		   else
		   {
		   copy_right(size(),0,size()-n);  
		   fill(0,n,false); // 空出的位置填充0
		   }
		   return *this;
	   }
	  self& operator >>= (bit_size n)
	   {
		   if(n == 0) 
			   return *this;
		   if(n >= size())
			   std::memset(head,0,get_pos(size()) + 1);
		   else
		   {
		   copy_left(0,n,size());
		   fill(size()-n,size(),false);
		   }
		   return *this;
	   }
	  self operator << (bit_size n) const 
	  {
		  self temp(*this);
		  temp <<= n;  // 转调operator <<= 
		  return temp;
	  }
	  self operator >> (bit_size n) const 
	  {
		  self temp(*this);
		  temp >>= n;
		  return temp;
	  }
	  bool operator == (const self& obj) const 
	  {
		  bit_size pos = get_pos(size());
		  for(bit_size i = 0; i <= pos; ++i)
			  if(head[i] != obj.head[i])
				  return false;
		  return true;
	  }
	  bool operator != (const self& obj) const 
	  {
		  return !(*this == obj);
	  }
	   friend std::ostream& operator << (std::ostream& os,const self& obj)
	   {
		   for(bit_size i = obj.size(); i >0; )
			   os<<obj.test(--i);
		   return os;
	   }
	};
 
} // end of namespace mystd 
#endif //__cplusplus
#endif //MYSTD_BITSET_TEST_H
