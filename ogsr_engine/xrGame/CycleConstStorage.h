#ifndef CYCLE_CONST_STORAGE_H
#define CYCLE_CONST_STORAGE_H

template<class T,int size> 
class CCycleConstStorage
{
	T array[size];
	u16 first;
public:
	IC CCycleConstStorage()
	{
		first=0;
	}
	IC void fill_in(const T& val)
	{
		std::fill(array,array+size,val);
	}
	IC void push_back(T& val)
	{
		array[first]=val;
		first=(++first)%size;
	}
	IC T& operator [] (int i)
	{
		return array[(first+i)%size];
	}
};
#endif