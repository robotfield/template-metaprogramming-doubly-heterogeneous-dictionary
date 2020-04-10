#include <iostream>
#include <string>
#include <cassert>

using namespace std;

// Type purely dependent on the value
// (i.e. entry_type<int,5> and entry_type<int,6> are different)
template<class TYPE, TYPE X>
struct entry_name{
	static constexpr TYPE value = X;
};

// Type which always contains 'false', but depends on T, so 
// Dependent_False_Type<T>::value isn't determined unless the
// template is instantiated.
template<class T>
struct Dependent_False_Type{
	static constexpr bool value = false;
};

// Base case of recursive structure RecList with the base case
// of the member function get_entry (which causes a compiler
// error if actually instantiated)
template<class... Args>
struct RecList{
	template<class B>
	inline void get_entry(){
		static_assert(Dependent_False_Type<B>::value,"Search for template key-value failed");
	}
};

template<class CMD, class T, class... Args>
struct RecList<CMD,T,Args...> : public RecList<Args...> {
	using parent_list = RecList<Args...>;
	
	// Data entry (to be determined at runtime)
	T entry;

	// The get_entry search function takes in a entry_name< , >
	// template key. If the key value matches this object's
	// key, then the get_entry instantiation returns this entry,
	// a reference to type T.  If the key value doesn't match,
	// the get_entry instantiation returns the parent class's
	// instantiation of get_entry with that key value, with its
	// return type determined by the return type the parent
	// get_entry.  Essentially, a compile-time linear search of 
	// the RecList.
	template<class OTHER, enable_if_t<OTHER::value != CMD::value,int> = 0 >
	inline auto get_entry() -> decltype(parent_list::template get_entry<OTHER>()){
		return parent_list::template get_entry<OTHER>();
	}
	template<class OTHER, enable_if_t<OTHER::value == CMD::value,int> = 0 >
	inline T& get_entry(){
		return entry;
	}

};

#define get_list_entry(LST,A) LST.get_entry<entry_name<decltype(A),A>>()

int main(){
	RecList<entry_name<char,'a'>,int,entry_name<int,5>,string> A;
	
	//A.get_entry<entry_name<char,'a'>>() = 1;
	get_list_entry(A,'a') = 1;
	
	//A.get_entry<entry_name<char,5>>() = "Hello";
	get_list_entry(A,5) = "Hello";
	//get_list_entry(A,7) = "Hello"; //Doesn't compile, there's no key value 7

	cout << get_list_entry(A,'a') << ' ' << get_list_entry(A,5) << endl;
	//Prints "1 Hello"

	return 0;
}
