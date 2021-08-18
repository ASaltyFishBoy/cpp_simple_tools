#pragma once
#ifndef UTILITY_EX_H
#define UTILITY_EX_H

#include <utility>

#define StdForward(x) (std::forward<decltype(x)>(x))

namespace my_std
{
	/**
	 * @brief immediate_move 立即移动，对象将会保证失效，无论有没有接受赋值的对象
	 * @param t 将移动的类型
	 * @return 移除cvref的返回值
	*/
	template<typename T>
	std::remove_volatile_t <std::remove_const_t<std::remove_reference_t<T>>> immediate_move (T & t)
	{
		return std::move (t);
	}

	namespace datail
	{
		template<bool _AreAllArray, typename ..._T>
		struct get_string_type_helper;


		template<typename ..._T>
		struct get_string_type_helper<false, _T...>
		{
			using type = std::basic_string <
				std::remove_cv_t <
				std::remove_pointer_t<
				std::decay_t<std::tuple_element_t<0, std::tuple<std::remove_reference_t <_T>...>>>
				>>>;
		};

		template<typename ..._T>
		struct get_string_type_helper<true, _T...>
		{
			using type = decltype((StdForward (std::declval<_T> ()) + ...));
		};

		template<typename ..._T>
		struct get_string_type :get_string_type_helper<
			(std::is_class_v <std::remove_reference_t <_T>> || ...)
			, _T...
		>
		{};
	}

	template<typename T>
	constexpr size_t string_length (const T & t)
	{
		if constexpr (std::is_array_v<std::remove_reference_t<T>>)
		{
			return std::size (t) - 1;
		}
		else if constexpr (std::is_pointer_v<std::remove_reference_t<T>>)
		{
			return std::strlen (StdForward (t));
		}
		else
		{
			return std::size (t);
		}
	}

	template<typename ..._Args>
	constexpr auto string_concat (_Args&&...args)
	{
		if constexpr (sizeof...(args) == 0)
		{
			return "";
		}
		else
		{
			typename datail::get_string_type<_Args&&...>::type string;
			string.reserve ((string_length (args) + ...) + 1);
			((string += StdForward (args)), ...);
			return string;
		}
	}

}

#endif // UTILITY_EX_H
