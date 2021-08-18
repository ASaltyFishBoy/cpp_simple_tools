#pragma once
#ifndef DEEP_EQUAL_H
#define DEEP_EQUAL_H

#include "type_traits_ex.h"

namespace my_std
{
	namespace datail
	{
		struct DeepEqualImple
		{
			template<typename T>
			struct TupleCompareHelper;
			template<auto ...I>
			struct TupleCompareHelper<std::index_sequence<I...>>
			{
				template < typename Equal, typename A, typename B>
				static bool compare (const A & a, const B & b)
				{
					return (Equal::compare (std::get<I> (a), std::get<I> (b)) && ...);
				}
			};

			template < typename Equal, typename A, typename B>
			static bool compare (const A & a, const B & b)
			{
				if constexpr (my_std::is_container_v<A> && my_std::is_container_v<B>)
				{
					return std::equal (
						std::begin (a), std::end (a)
						, std::begin (b), std::end (b)
						, Equal::template compare<typename A::value_type, typename B::value_type>);
				}
				else if constexpr (my_std::is_tuple_v<A> && my_std::is_tuple_v<B>)
				{
					if constexpr (std::tuple_size_v<A> == std::tuple_size_v<B>)
					{
						return TupleCompareHelper<std::make_index_sequence<std::tuple_size_v<A>> () >::template compare<Equal> (a, b);
					}
					else
					{
						return false;
					}
				}
				else if constexpr (my_std::is_pointer_v<A> && my_std::is_pointer_v<B> && !std::is_same_v<void *, A> && !std::is_same_v<void *, B>)
				{
					return a == b || (a && b && Equal::compare (*a, *b));
				}
				else
				{
					return a == b;
				}
			}
		};

		template<size_t depth>
		struct DeepEqual
		{
			template<typename A, typename B>
			static bool compare (const A & a, const B & b)
			{
				return DeepEqualImple::compare < DeepEqual < depth - 1 >> (a, b);
			}

			template<typename A, typename B>
			bool operator() (const A & a, const B & b) const
			{
				return compare (a, b);
			}

		};
		template<>
		struct DeepEqual<0>
		{
			template<typename A, typename B>
			static bool compare (const A & a, const B & b)
			{
				return a == b;
			}
			template<typename A, typename B>
			bool operator() (const A & a, const B & b) const
			{
				return compare (a, b);
			}
		};


		struct UnlimitedDeepEqual
		{
			template<typename A, typename B>
			static bool compare (const A & a, const B & b)
			{
				return DeepEqualImple::compare<UnlimitedDeepEqual> (a, b);
			}

			template<typename A, typename B>
			bool operator() (const A & a, const B & b) const
			{
				return compare (a, b);
			}
		};
	}

	/**
	 * @brief deep_equal_limited 限制深度的深比较，将对tuple，指针，容器进行多层迭代的拆分比较
	*/
	template <size_t depth = 0>
	constexpr auto deep_equal_limited = datail::DeepEqual<depth> ();

	/**
	 * @brief deep_equal_limited 深比较，将对tuple，指针，容器进行多层迭代的拆分比较
	*/
	constexpr auto deep_equal = datail::UnlimitedDeepEqual ();
}

#endif // DEEP_EQUAL_H
