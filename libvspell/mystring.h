#ifndef __MYSTRING_H__					// -*- tab-width: 2 mode: c++ -*-
#define __MYSTRING_H__

struct char_traits_strid:public std::char_traits<strid>
{
	typedef strid 		char_type; 
	typedef int 	        int_type;
	//typedef streampos 	pos_type;
	//typedef streamoff 	off_type;
	//typedef mbstate_t 	state_type;

	static void 
	assign(char_type& __c1, const char_type& __c2)
	{ __c1 = __c2; }

	static bool 
	eq(const char_type& __c1, const char_type& __c2)
	{ return __c1 == __c2; }

	static bool 
	lt(const char_type& __c1, const char_type& __c2)
	{ return __c1 < __c2; }

	static int 
	compare(const char_type* __s1, const char_type* __s2, size_t __n)
	{ return memcmp((const char*)__s1, (const char*)__s2, __n*sizeof(char_type)); }

	static size_t	length(const char_type* __s)
	{
		int i = 0;
		while (__s[i])
			i++;
		return i;
	}


	static const char_type* 
	find(const char_type* __s, size_t __n, const char_type& __a)
	{
		for (size_t i = 0;i < __n;i ++)
			if (__s[i] == __a)
				return &__s[i];
		return 0;
	}

	static char_type* 
	move(char_type* __s1, const char_type* __s2, size_t __n)
	{ return static_cast<char_type*>(memmove(__s1, __s2, __n*sizeof(char_type))); }

	static char_type* 
	copy(char_type* __s1, const char_type* __s2, size_t __n)
	{  return static_cast<char_type*>(memcpy(__s1, __s2, __n*sizeof(char_type))); }

	static char_type* 
	assign(char_type* __s, size_t __n, char_type __a)
	{
		for (size_t i = 0;i < __n;i ++)
			__s[i] = __a;
		return __s;
	}

	static char_type 
	to_char_type(const int_type& __c)
	{ return static_cast<char_type>(__c); }

	// To keep both the byte 0xff and the eof symbol 0xffffffff
	// from ending up as 0xffffffff.
	static int_type 
	to_int_type(const char_type& __c)
	{ return static_cast<int_type>(static_cast<unsigned char>(__c)); }

	static bool 
	eq_int_type(const int_type& __c1, const int_type& __c2)
	{ return __c1 == __c2; }

	static int_type 
	eof() { return static_cast<int_type>(EOF); }

	static int_type 
	not_eof(const int_type& __c)
	{ return (__c == eof()) ? 0 : __c; }
};
typedef std::basic_string<strid,char_traits_strid> strid_string;
#endif
