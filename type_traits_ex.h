#ifndef TYPE_TRAITS_EX_H
#define TYPE_TRAITS_EX_H

#include <type_traits>
#include "utility_ex.h"

/**
 * @brief DEFINE_HAS_NESTED_TYPE 定义是否存在内部类型的模板
 * @param _NTYPE 类型名
*/
#define DEFINE_HAS_NESTED_TYPE(_NTYPE) \
namespace my_std{\
    namespace nested_traits{\
        template<typename _Tp, typename = void>				\
        struct has_type_##_NTYPE						\
            : std::false_type{\
        };\
        template<typename _Tp>						\
        struct has_type_##_NTYPE<_Tp, std::void_t<typename _Tp::_NTYPE>>		\
                    : std::true_type {\
        };\
    }\
}

/**
 * @brief DEFINE_HAS_NESTED_MEMBER 定义是否存在某名称成员的模板
 * @param _NMEMBER 成员名
*/
#define DEFINE_HAS_NESTED_MEMBER(_NMEMBER) \
namespace my_std{\
    namespace nested_traits{\
		/*是否存在某成员*/\
        template<typename _Tp, typename = void>				\
        struct has_member_##_NMEMBER			\
			: std::false_type\
		{};\
        template<typename _Tp>						\
        struct has_member_##_NMEMBER<_Tp, std::void_t<decltype(&_Tp::_NMEMBER)>>		\
			: std::true_type \
		{};\
\
        template<typename _Tp>\
        inline constexpr bool has_member_##_NMEMBER##_v = has_member_##_NMEMBER<_Tp>::value;\
\
		/*是否存在某成员对象*/\
        template<typename _Tp, typename = void>				\
        struct has_member_##_NMEMBER##_object			\
			: std::false_type\
		{};\
        template<typename _Tp>						\
        struct has_member_##_NMEMBER##_object<\
			_Tp\
			, std::void_t<decltype(std::declval<_Tp>()._NMEMBER)>\
		>: std::true_type \
		{};\
\
        template<typename _Tp>\
        inline constexpr bool has_member_##_NMEMBER##_object_v \
			= has_member_##_NMEMBER##_object<_Tp>::value;\
\
		/*是否存在某成员函数*/\
        template<typename _Tp, typename = void>				\
        struct has_member_##_NMEMBER##_function			\
			:std::conditional_t<\
				has_member_##_NMEMBER##_v <_Tp> && !has_member_##_NMEMBER##_object_v<_Tp> \
				,std::true_type\
				, std::false_type\
			> \
		{};\
\
        template<typename _Tp>\
        inline constexpr bool has_member_##_NMEMBER##_function_v \
			= has_member_##_NMEMBER##_function<_Tp>::value;\
	}\
}


DEFINE_HAS_NESTED_MEMBER (value)

#include <tuple>
namespace my_std
{
	template<typename T>
	struct is_tuple : nested_traits::has_member_value<std::tuple_size<T>>
	{};

	/**
	 * @brief is_tuple 检查类型是否可用与标准库中对tuple的类型检测
	 * @tparam T 被检测类型
	*/
	template<typename T>
	inline constexpr bool is_tuple_v = is_tuple<T>::value;
}

namespace my_std
{
	/**
	 * @brief traits_t 用于提取类型中的type类型
	 * @tparam T 被处理类型
	*/
	template<typename T>
	using traits_t = typename T::type;

	/**
	 * @brief traits_v 用于提取类型中的value数据
	 * @tparam T 被处理类型
	*/
	template<typename T>
	inline constexpr auto traits_v = T::value;

	namespace datail
	{
		template <size_t I, typename T, typename... Args>
		struct find_type_imple
		{};

		template <size_t I, typename T, typename U, typename... Args>
		struct find_type_imple <I, T, U, Args...>
			:std::conditional_t<std::is_same_v<T, U>, std::integral_constant<std::size_t, I >, find_type_imple<I + 1, T, Args...>>
		{};
	}


	/**
	 * @brief find_type 查询某个类型第一次出现的位置
	 * @tparam T 被查询类型
	 * @tparam ...Arga 查询列表
	*/
	template<typename T, typename ...Args>
	struct find_type : datail::find_type_imple<0, T, Args...>
	{};

	template<typename T, typename ...Args>
	inline constexpr auto find_type_v = traits_v<find_type<T, Args...>>;

	/**
	 * @brief find_type 查询某个类型是否在某个类型列表中出现过
	 * @tparam T 被查询类型
	 * @tparam ...Arga 查询列表
	*/
	template <typename T, typename... Args>
	struct can_find : nested_traits::has_member_value<find_type<T, Args...>>
	{};

	template<typename T, typename ...Args>
	inline constexpr auto can_find_v = traits_v<can_find<T, Args...>>;


	/**
	 * @brief template_traits 对某个纯类型模板参数模板进行拆解分析（值模板参数可以转换为类型模板参数）
	 * @tparam Template 被拆解类型
	*/
	template<typename Template>
	struct template_traits;

	template<template<typename...>typename Template, typename ...Args>
	struct template_traits < Template<Args...> >
	{
		/**
		 * @brief type 被处理的类型
		*/
		using type = Template<Args...>;

		/**
		 * @brief size 拆解后模板参数的个数
		*/
		static constexpr std::size_t size = sizeof... (Args);

		/**
		 * @brief template_type 用其他的类型填充进原模板
		 * @tparam ..._Args 将进行填充的类型
		*/
		template<typename ..._Args>
		using template_type = Template<_Args...>;

		/**
		 * @brief apply 将原类型列表填充进其他的模板
		 * @tparam _Template 指定的模板
		*/
		template<template<typename...>typename _Template>
		using apply = _Template<Args...>;

		/**
		 * @brief arg 获得第I个模板参数类型
		 * @tparam I 指定下标
		*/
		template<std::size_t I>
		using arg = traits_t<std::tuple_element < I, std::tuple<Args...>>>;
	};

	namespace datail
	{
		template<auto t>
		struct can_compile_time_calculate_helper
		{};

		template<typename T, typename = void>
		struct can_compile_time_calculate_imple :std::false_type
		{};


		template<typename T>
		struct can_compile_time_calculate_imple<T
			, std::void_t<can_compile_time_calculate_helper<T::get ()>>
		> :std::true_type
		{};
	}

	/**
	 * @brief can_compile_time_calculate 检测某表达式是否编译期可解
	 * @tparam T 入参类型，要求其中有static get函数，get函数中实现想求解的表达式
	*/
	template<typename T>
	struct can_compile_time_calculate :datail::can_compile_time_calculate_imple<T>
	{};

	template<typename T>
	inline constexpr static bool can_compile_time_calculate_v
		= can_compile_time_calculate<T>::value;


	/**
	 * @brief remove_cv 相较std的remove_cv，此类型将额外移除成员函数的后缀cv限定
	 * @tparam T 被处理类型
	*/
	template< class T > struct remove_cv :std::remove_cv<T>
	{};
	template< class T, class ...Args> struct remove_cv<T (Args...)const>
	{
		using type = T (Args...);
	};
	template< class T, class ...Args> struct remove_cv<T (Args...)volatile>
	{
		using type = T (Args...);
	};

	template< class T, class ...Args> struct remove_cv<T (Args...)const volatile>
	{
		using type = T (Args...);
	};


	template< class T > struct remove_const :std::remove_const<T>
	{};
	template< class T, class ...Args> struct remove_const<T (Args...)const >
	{
		using type = T (Args...);
	};


	template< class T > struct remove_volatile :std::remove_volatile<T>
	{};
	template< class T, class ...Args> struct remove_volatile<T (Args...)volatile >
	{
		using type = T (Args...);
	};

	template< class T >
	using remove_cv_t = typename remove_cv<T>::type;
	template< class T >
	using remove_const_t = typename remove_const<T>::type;
	template< class T >
	using remove_volatile_t = typename remove_volatile<T>::type;

	template< class T >
	using remove_cvref_t = std::remove_reference_t<remove_cv_t<T>>;
}

#endif // TYPE_TRAITS_EX_H


#if __has_include(<algorithm>)
#ifndef ALGORITHM_TRAITS
#define ALGORITHM_TRAITS
#include <algorithm>


namespace my_std
{
	namespace datail
	{
		template<typename, typename = void>
		struct is_container_helper : std::false_type
		{};
		template<typename T>
		struct is_container_helper <
			T
			, std::void_t < decltype (*std::begin (std::declval<T> ())), decltype (++std::begin (std::declval<T> ()) != std::end (std::declval<T> ())) >
		>
			: std::true_type
		{};
	}

	/**
	 * @brief is_container 判断是否是容器，
	 *			判断条件：能作为std::begin,std::end入参，std::begin的结果可++可取*，++结果可和std::end进行比较
	 * @tparam T 被检测类型
	*/
	template<typename T>
	struct is_container : datail::is_container_helper <T>
	{};

	template<typename T>
	inline constexpr bool is_container_v = traits_v<is_container<T>>;
}
#endif
#endif

#if __has_include(<memory>)
#ifndef MEMORY_TRAITS
#define MEMORY_TRAITS
#include<memory>

namespace my_std
{
	/**
	 * @brief is_pointer 相较std的is_pointer，此类型将额外处理智能指针类型
	 * @tparam T 被处理类型
	*/
	template<typename T>
	struct is_pointer : std::conditional_t <
		std::is_same_v<std::remove_cv_t<T>, T>
		, std::is_pointer<T>, is_pointer<std::remove_cv_t<T> >
	>
	{};
	template<typename T>
	struct is_pointer<std::shared_ptr<T>> : std::true_type
	{};
	template<typename T>
	struct is_pointer<std::unique_ptr<T>> : std::true_type
	{};

	template<typename T>
	inline constexpr bool is_pointer_v = traits_v<is_pointer<T>>;

	/**
	 * @brief remove_pointer 相较std的remove_pointer，此类型将额外处理智能指针类型
	 * @tparam T 被处理类型
	*/
	template<typename T>
	struct remove_pointer : std::conditional_t <
		std::is_same_v<std::remove_cv_t<T>, T>
		, std::remove_pointer<T>, remove_pointer<std::remove_cv_t<T> >
	>
	{};
	template<typename T>
	struct remove_pointer<std::shared_ptr<T>>
	{
		using type = T;
	};
	template<typename T>
	struct remove_pointer<std::unique_ptr<T>>
	{
		using type = T;
	};

	template<typename T>
	using remove_pointer_t = traits_t<remove_pointer<T>>;
}

#endif
#endif

#if __has_include(<functional>)
#ifndef FUNCTIONAL_TRAITS
#define FUNCTIONAL_TRAITS
#include<functional>

/**
 * @brief DEFINE_HAS_NESTED_MEMBER_INVOKE 定义检查某类型是否存在某可调用对象的模板
 * @param _NMEMBER 可调用对象名
*/
#define DEFINE_HAS_NESTED_MEMBER_INVOKE(_NMEMBER) \
namespace my_std{\
    namespace nested_traits{\
        template<typename _Tp, typename = void>				\
        struct has_member_invoke_##_NMEMBER			\
        : false_type{\
            template<typename ...Args>\
            static void invoke(Args&&...){}\
        };\
        template<typename _Tp>						\
        struct has_member_invoke_##_NMEMBER<_Tp, std::void_t<decltype(&_Tp::_NMEMBER)>>		\
        : true_type {\
            template<typename Class,typename Function,typename ...Args>\
            static void invoke(Class&& class,Function&& function, Args&&... args){\
                std::invoke(StdForward(function),StdForward(class)._NMEMBER,StdForward(args)...);\
            }\
        };\
        template<typename _Tp>\
        inline constexpr bool has_member_##_NMEMBER##_v = has_member_##_NMEMBER<_Tp>::value;\
    }\
}

/**
 * @brief HAS_MEMBER_INVOKE 检查某类型是否存在某可调用对象，若存在，则调用，若不存在则无行为
 * @param _CLASS 被检测对象
 * @param _NMEMBER 检测是否存在的可调用对象名
 * @param ... 以指定参数调用
*/
#define HAS_MEMBER_INVOKE(_CLASS,_NMEMBER,...) \
(my_std::nested_traits::has_member_invoke_##_NMEMBER<decltype(_CLASS)>::invoke((_CLASS),__VA_ARGS__))

namespace my_std
{
	namespace datail
	{
		template <typename T, typename = void>
		struct functor_helper
		{
			using type = T;
		};
		template <typename T>
		struct functor_helper<T, std::void_t<decltype(&T::operator())>>
		{
			using type = decltype(&T::operator());
		};
	}

	/**
	 * @brief function_traits 解析函数类型
	 * @tparam T 被解析函数，可为函数，函数指针，函数引用，成员函数指针，仿函数，
	*/
	template <typename T>
	struct function_traits
		: std::enable_if_t <
		!std::is_same_v<
		my_std::traits_t<datail::functor_helper<my_std::remove_cv_t<my_std::remove_pointer_t<T>>>>, T
		>
		, function_traits<
		my_std::traits_t<datail::functor_helper<my_std::remove_cv_t<my_std::remove_pointer_t<T>>>>
		>> {};

	template <typename Result, typename ...Args>
	struct function_traits<Result && (Args&&...) >
	{
		/**
		 * @brief result_type 返回值类型
		*/
		using result_type = Result &&;

		/**
		 * @brief function_type 函数类型
		*/
		using function_type = Result && (Args&&...);

		/**
		 * @brief args_size 参数数量
		*/
		inline constexpr static size_t args_size = sizeof...(Args);

		/**
		 * @brief apply 将所有的入参参数类型放入指定模板
		 * @tparam _Template 指定的模板
		*/
		template<template<typename...> typename _Template>
		using apply = _Template<Args&&...>;

		/**
		 * @brief arg 获得第I个入参参数类型
		 * @tparam I 指定下标
		*/
		template<size_t i>
		using arg = std::tuple_element_t<i, std::tuple<Args&&...>>;
	};

	template <typename Result, typename ...Args>
	struct function_traits<Result (Args&&...) >
	{
		using result_type = Result;

		using function_type = Result (Args&&...);

		inline constexpr static size_t args_size = sizeof...(Args);

		template<template<typename...> typename _Template>
		using apply = _Template<Args&&...>;

		template<size_t i>
		using arg = std::tuple_element_t<i, std::tuple<Args&&...>>;
	};

	template <typename Result, typename ...Args>
	struct function_traits<Result && (Args...) >
	{
		using result_type = Result &&;

		using function_type = Result && (Args...);

		inline constexpr static size_t args_size = sizeof...(Args);

		template<template<typename...> typename _Template>
		using apply = _Template<Args...>;

		template<size_t i>
		using arg = std::tuple_element_t<i, std::tuple<Args...>>;
	};

	template <typename Result, typename ...Args>
	struct function_traits<Result (Args...) >
	{
		using result_type = Result;

		using function_type = Result (Args...);

		inline constexpr static size_t args_size = sizeof...(Args);

		template<template<typename...> typename _Template>
		using apply = _Template<Args...>;

		template<size_t i>
		using arg = std::tuple_element_t<i, std::tuple<Args...>>;
	};

	template <typename Class, typename T>
	struct function_traits<T Class:: *> : function_traits< my_std::remove_cv_t<T>>
	{
		/**
		 * @brief class_type 成员函指的来源类
		*/
		using class_type = Class;

		/**
		 * @brief member_function_type 成员函指类型
		*/
		using member_function_type = T Class:: *;
	};

	namespace datail
	{
		template<typename T, typename = void>
		struct is_function_helper : std::false_type
		{};
		template<typename T>
		struct is_function_helper<T, std::void_t<typename function_traits<T>::function_type>> : std::true_type
		{};

	}

	/**
	 * @brief is_function 相较std的is_function，此类型将额外函数指针，函数引用，成员函指，仿函数
	 * @tparam T 被处理类型
	*/
	template<typename T>
	struct is_function : datail::is_function_helper<T>
	{};

	template<typename T>
	inline constexpr bool is_function_v = traits_v<is_function<T>>;
}
#endif
#endif

