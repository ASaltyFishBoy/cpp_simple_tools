#ifndef RESOURCE_HANDLE
#define RESOURCE_HANDLE

#include <optional>
#include "type_traits_ex.h"

namespace my_std
{
	/**
	 * @brief nullable_pointer_wrapper 用于给unique_ptr的Deleter包装pointer，只能处理可赋值对象
	 * @tparam _T 包装的底层类型
	 * @tparam value 无效值，不填的话会将底层类型再用std optional包裹
	*/
	template<typename _T, auto _value = std::nullopt>
	struct nullable_pointer_wrapper
	{
		inline constexpr static bool using_opt
			= std::is_same_v<decltype(_value), std::remove_cv_t<decltype(std::nullopt)>>;
		using value_type = std::conditional_t<
			using_opt
			, std::optional<_T>
			, _T
		>;

	private:
		value_type _t;
	public:
		nullable_pointer_wrapper (const std::nullptr_t & = nullptr) : _t (_value)
		{};
		nullable_pointer_wrapper (_T _t) :_t (std::move (_t))
		{}

		constexpr operator bool () const noexcept
		{
			return (_t == _value);
		}

		constexpr bool operator==(const nullable_pointer_wrapper & other) const noexcept
		{
			return _t == other._t;
		}

		constexpr bool operator!=(const nullable_pointer_wrapper & other) const noexcept
		{
			return !(*this == other);
		}

		constexpr bool operator==(const std::nullptr_t &) const noexcept
		{
			return !*this;
		}
		constexpr bool operator!=(const std::nullptr_t &) const noexcept
		{
			return *this;
		}

		friend constexpr bool operator==(const std::nullptr_t &, const nullable_pointer_wrapper & p) noexcept
		{
			return !*this;
		}
		friend constexpr bool operator!=(const std::nullptr_t &, const nullable_pointer_wrapper & p) noexcept
		{
			return *this;
		}

		constexpr _T & value () noexcept
		{
			if constexpr (using_opt)
				return _t.value ();
			else
				return _t;
		}
		constexpr const _T & value ()const noexcept
		{
			if constexpr (using_opt)
				return _t.value ();
			else
				return _t;
		}

		constexpr explicit operator _T & () noexcept
		{
			return value ();
		}
		constexpr explicit operator const _T & ()const noexcept
		{
			return value ();
		}

		constexpr _T & operator*()noexcept
		{
			return value ();
		}
		constexpr const _T & operator*()const noexcept
		{
			return value ();
		}
		constexpr _T * operator->() noexcept
		{
			return &value ();
		}
		constexpr const _T * operator->()const noexcept
		{
			return &value ();
		}
	};


	/**
	 * @brief deleter_wrapper 用于包装unique_ptr的Deleter,让其在有效时自动执行特定函数，只能处理可赋值对象
	 * @tparam func 希望自动执行的函数
	 * @tparam value 无效值，不填的话会将底层类型再用std optional包裹
	*/
	template<auto func, auto value = std::nullopt>
	class deleter_wrapper
	{

	private:
		inline constexpr static auto args_size = my_std::function_traits<decltype(func)>::args_size;

		template<size_t I>
		struct helper
		{
			using type =
				nullable_pointer_wrapper<
				typename my_std::function_traits<decltype(func)>::template arg<0>
				, value>;
		};
		template<>
		struct helper<0>
		{
			using type = nullable_pointer_wrapper<decltype(value), value>;
		};

	public:
		using pointer = my_std::traits_t<helper<args_size>>;

		void operator()(pointer & _n) const noexcept
		{
			if constexpr (args_size == 0)
			{
				func ();
			}
			else
			{
				func (_n.value ());
			}
		}
	};
}

#endif // !RESOURCE_HANDLE 
