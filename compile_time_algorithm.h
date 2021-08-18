#pragma once
#include "compile_time_container.h"

namespace my_std
{
	namespace compile_time
	{
		namespace datail
		{
			template<typename itType, int i>
			struct next_imple :
				std::conditional_t < i == 0
				, itType
				, next_imple <typename itType::next, i - 1>> {};

			template<typename it, typename itE, size_t I>
			struct distance_imple :
				std::conditional_t<std::is_same_v<it, itE>
				, std::integral_constant<size_t, I>
				, distance_imple< typename it::next, itE, I + 1 >
				>
			{};

			template<typename itBegin, typename itEnd, template<typename>class Template>
			struct for_each_run_imple
			{
				template<typename ...Args>
				constexpr static void run (Args&& ...args)
				{
					Template<traits_t<itBegin>>::run (StdForward (args)...);
					for_each_run_imple<typename itBegin::next, itEnd, Template>::run (StdForward (args)...);
				}
			};
			template<typename itBegin, template<typename>class Template>
			struct for_each_run_imple<itBegin, itBegin, Template>
			{
				template<typename ...Args>
				constexpr static void run (Args&& ...)
				{}
			};
		}

		/**
		 * @brief next 对于模板迭代器的next，和std::next类似
		 * @tparam itType 迭代器类型
		*/
		template<typename itType, int i = 1>
		using next = datail::next_imple<itType, i>;

		/**
		 * @brief distance 对于模板迭代器的distance，和std::distance类似
		 * @tparam it begin
		 * @tparam itE end
		*/
		template<typename it, typename itE>
		inline constexpr size_t distance = traits_v<datail::distance_imple<it, itE, 0>>;

		/**
		 * @brief for_each_run 对于模板迭代器的for_each，和std::for_each类似
		 *			此运行内容为运行期而非编译期
		 * @tparam it begin
		 * @tparam itE end
		 * @tparam Template 运行函数，要求入参为迭代器内容时内部有静态run函数
		*/
		template<typename itBegin, typename itEnd, template<typename>class Template, typename ...Args>
		constexpr void for_each_run (Args&&...args)
		{
			datail::for_each_run_imple<itBegin, itEnd, Template>::run (StdForward (args)...);
		}

		/**
		 * @brief default_compare 默认比较行为，要求两个类中存在constexpr的可比较value参数
		 * @tparam A
		 * @tparam B
		*/
		template<typename A, typename B>
		struct default_compare
		{
			inline static constexpr bool value = traits_v<A> < traits_v<B>;
		};

		namespace list
		{
			namespace datail
			{
				template<typename itBegin, typename itEnd, template<typename, typename>class Compare>
				struct list_sort_imple
				{
				private:
					template<typename FirstNode, typename SecondNode>
					struct sort_merge
					{
					private:
						using biger = std::conditional_t < !traits_v<Compare<traits_t<FirstNode>, traits_t<SecondNode>>>, FirstNode, SecondNode >;
						using smaller = std::conditional_t < traits_v<Compare<traits_t<FirstNode>, traits_t<SecondNode>>>, FirstNode, SecondNode >;

					public:
						using type = node<traits_t<smaller>, traits_t<sort_merge<typename smaller::next, biger>>>;
					};

					template<typename FirstNode>
					struct sort_merge<FirstNode, void>
					{
						using type = FirstNode;
					};
					template<typename SecondNode>
					struct sort_merge<void, SecondNode>
					{
						using type = SecondNode;
					};

					template<typename Center>
					struct center_next
					{
						using type = typename Center::next;
					};

					template<>
					struct center_next<void>
					{
						using type = void;
					};

					template<typename FastPointer, typename SlowPointer>
					struct get_center
					{
						template<typename _FastPointer, typename _SlowPointer, typename = void>
						struct get_center_next
						{
							using type = _SlowPointer;
						};

						template<typename _FastPointer, typename _SlowPointer>
						struct get_center_next<_FastPointer, _SlowPointer, std::enable_if_t<!std::is_void_v<typename _FastPointer::next>>>
						{
							using type = traits_t<get_center<typename _FastPointer::next, SlowPointer>>;
						};

						using type = traits_t<get_center_next<typename FastPointer::next, typename SlowPointer::next>>;
					};

					template<typename SlowPointer>
					struct get_center<void, SlowPointer>
					{
						using type = SlowPointer;

					};

					using itCenter = traits_t<get_center<itBegin, itBegin>>;

					using firstList = typename list_sort_imple<itBegin, itCenter, Compare>::type;
					using secondList = typename list_sort_imple<typename center_next<itCenter>::type, itEnd, Compare>::type;
				public:
					using type = typename sort_merge<firstList, secondList>::type;
				};

				//切到只有两个的时候
				template<typename itBegin, template<typename, typename>class Compare>
				struct list_sort_imple < itBegin, std::enable_if_t < !std::is_void_v<typename itBegin::next>, typename itBegin::next >, Compare>
				{
					using type = std::conditional_t <
						Compare<traits_t<itBegin>, traits_t<typename itBegin::next>>::value
						, node<traits_t<itBegin>, node<traits_t<typename itBegin::next>>>
						, node<traits_t<typename itBegin::next>, node<traits_t<itBegin>>>
					>;
				};

				//匹配到自己
				template<typename itBegin, template<typename, typename>class Compare>
				struct list_sort_imple<itBegin, itBegin, Compare>
				{
				private:
					template<typename Begin>
					struct isVoidHelp
					{
						using type = node<traits_t<Begin>>;
					};

					template<>
					struct isVoidHelp<void>
					{
						using type = void;
					};
				public:
					using type = typename isVoidHelp<itBegin>::type;
				};

				//无可以匹配的链
				template<typename itBegin, template<typename, typename>class Compare>
				struct list_sort_imple<itBegin, std::enable_if_t <std::is_void_v<typename itBegin::next>, typename itBegin::next >, Compare>
				{
					using type = itBegin;
				};

			}

			/**
			 * @brief sort 相当于对模板使用的std::sort
			 * @tparam itBegin
			 * @tparam itEnd
			 * @tparam Compare
			*/
			template<typename itBegin, typename itEnd, template<typename, typename>typename Compare = default_compare>
			using sort = container<traits_t<datail::list_sort_imple<itBegin, itEnd, Compare>>>;
		}
	}
}
