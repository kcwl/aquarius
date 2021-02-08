#pragma once
#include <functional>
#include <tuple>

template<class connect>
struct function_traits;

template<class Ret, class... Args>
struct function_traits<Ret(Args...)>
{
public:
	enum { arity = sizeof...(Args) };

	typedef Ret function_type(Args...);
	using return_type = Ret;
	using stl_function_type = std::function<function_type>;
	typedef Ret (*pointer)(Args...);

	template<size_t I>
	class args
	{
		static_assert(I < arity, "index is out of range, index must less than sizeof Args");
		using type = class std::tuple_element<I, std::tuple<Args...>>::type;
	};

	//typedef std::tuple<std::remove_cv_t<std::remove_reference_t<Args>>...> tuple_type;
};

//函数指针.
template<class Ret, class... Args>
struct function_traits<Ret(*)(Args...)> : public function_traits<Ret(Args...)> {};

//std::function.
template <class Ret, class... Args>
struct function_traits<std::function<Ret(Args...)>> : public function_traits<Ret(Args...)> {};

//声明class
template <class ReturnType, class ClassType, class... Args>
struct function_traits<ReturnType(ClassType::*)(Args...)> : function_traits<ReturnType(Args...)> {};

template <class ReturnType, class ClassType, class... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const> : function_traits<ReturnType(Args...)> {};

//函数对象.
template<typename Callable>
struct function_traits : public function_traits<decltype(&Callable::operator())> {};

template <class Function>
typename function_traits<Function>::stl_function_type to_function(Function&& lambda)
{
	return static_cast<typename function_traits<Function>::stl_function_type>(std::forward<Function>(lambda));
}

template<class Function>
typename function_traits<Function>::stl_function_type to_function(const Function& lambda)
{
	return static_cast<typename function_traits<Function>::stl_function_type>(lambda);
}

template<class T>
struct is_string : std::false_type {};

template<>
struct is_string<std::string> : std::true_type {};

template<>
struct is_string<const char*> : std::true_type{};

template<typename _Ty, typename _ = void>
struct is_container : std::false_type
{
};

template<class _Ty>
struct is_container<_Ty, std::void_t<typename _Ty::value_type,
	typename _Ty::size_type,
	typename _Ty::allocator_type,
	typename _Ty::iterator,
	typename _Ty::const_iterator,
	decltype(std::declval<_Ty>().size()),
	decltype(std::declval<_Ty>().begin()),
	decltype(std::declval<_Ty>().end()),
	decltype(std::declval<_Ty>().cbegin()),
	decltype(std::declval<_Ty>().cend())>> : std::true_type{};

template<class _Ty>
constexpr static bool is_container_v = is_container<std::remove_cvref_t<_Ty>>::value;
