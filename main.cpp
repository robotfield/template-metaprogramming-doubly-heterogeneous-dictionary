#include <iostream>
#include <string>
#include <cassert>

using namespace std;

template<class TYPE, TYPE X>
struct command_name{
	static constexpr TYPE value = X;
};

template<class... Args>
struct RecList{
	template<class B>
	inline void get_entry(){assert(false);}
};

template<class CMD, class T, class... Args>
struct RecList<CMD,T,Args...> : public RecList<Args...> {
	using child_list = RecList<Args...>;
	
	T entry;

	template<class OTHER, enable_if_t<OTHER::value != CMD::value,int> = 0 >
	inline auto get_entry() -> decltype(child_list::template get_entry<OTHER>()){
		return child_list::template get_entry<OTHER>();
	}
	template<class OTHER, enable_if_t<OTHER::value == CMD::value,int> = 0 >
	inline T& get_entry(){
		return entry;
	}
};

#define get_list_entry(LST,A) LST.get_entry<command_name<decltype(A),A>>()

int main(){
	RecList<command_name<char,'a'>,int,command_name<int,5>,string> A;
	
	//A.get_entry<command_name<char,'a'>>() = 1;
	get_list_entry(A,'a') = 1;
	
	//A.get_entry<command_name<char,5>>() = "Hello";
	get_list_entry(A,5) = "Hello";

	cout << get_list_entry(A,'a') << ' ' << get_list_entry(A,5) << endl;
	//Prints "1 Hello"

	return 0;
}
