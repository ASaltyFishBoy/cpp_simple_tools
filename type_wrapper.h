#ifndef TYPE_WRAPPER
#define TYPE_WRAPPER

#include <type_traits>

namespace my_std
{
	namespace datail
	{
		template<typename _T, typename = void>
		struct type_wrapper_imple
		{
			_T value;

			template<typename ...Args>
			type_wrapper_imple (Args&&... args) :value (std::forward<Args> (args)...)
			{}

			constexpr operator _T & () &
			{
				return value;
			}

			constexpr operator const _T & () const &
			{
				return value;
			}

			constexpr operator _T && () &&
			{
				return std::move (value);
			}

			constexpr _T & operator*()
			{
				return value;
			}
			constexpr _T const & operator*() const
			{
				return value;
			}

		};

		template<typename _T>
		struct type_wrapper_imple<_T, std::enable_if_t<std::is_class_v<_T>>>
			:type_wrapper_imple <_T, bool>
		{
			using type_wrapper_imple <_T, bool>::type_wrapper_imple;

			constexpr _T * operator->()
			{
				return &this->value;
			}
			constexpr _T const * operator->() const
			{
				return &this->value;
			}
		};
	}

	/**
	 * @brief type_wrapper 对某类型进行包装，提供类似指针的接口和转换函数
	 * @tparam _T
	*/
	template<typename _T>
	struct type_wrapper :datail::type_wrapper_imple<_T>
	{
		using datail::type_wrapper_imple <_T>::type_wrapper_imple;
	};
}

#endif // !TYPE_WRAPPER
