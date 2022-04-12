#ifndef STL_EXCHANGE_IMPLEMENTATION
#define STL_EXCHANGE_IMPLEMENTATION


#define exchange(x,y) __exchange(x,y) 

// namespace std {

// template<class T, class U = T>
// constexpr // since C++20
// T exchange(T& obj, U&& new_value)
// {
//     T old_value = std::move(obj);
//     obj = std::forward<U>(new_value);
//     return old_value;
// }

// }

#endif