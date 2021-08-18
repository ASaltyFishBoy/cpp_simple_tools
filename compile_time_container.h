#pragma once
#include "type_traits_ex.h"
namespace my_std
{
	/**
	 * @brief 编译期容器，由于c++函数式的相关特性，对编译期vector的某值进行修改是个On复杂度操作，故有修改需求优先list
	*/
	namespace compile_time
	{
		/**
		 * @brief vector 代表编译期的vector容器及相关操作
		*/
		namespace vector
		{
			/**
			 * @brief iterator vector容器的迭代器
			 * @tparam I 下标索引
			 * @tparam Vector 来源容器
			*/
			template<size_t I, typename Vector>
			struct iterator
			{
				using type = typename Vector::template arg<I>;

				/*template<typename Arg>
				using change_to = Vector::template change < I, Arg>;*/
			};

			/**
			 * @brief container vector容器
			 * @tparam ...Args 容器内数据
			*/
			template<typename ...Args>
			struct container
			{
				using begin = iterator<0, container<Args...>>;
				using end = iterator<sizeof...(Args), container<Args...>>;

				template<typename Arg>
				using push_back = container<Args..., Arg>;

				template<typename Arg>
				using push_front = container<Arg, Args...>;

				//此实现比较复杂，暂时没必要
				/*
				template<size_t I, typename Arg>
				using change = container<>;*/

				/**
				 * @brief apply	将所有数据放入另一个模板
				 * @tparam Template 接收所有数据的模板
				*/
				template<template<typename ...>typename Template>
				using apply = Template<Args...>;
			};
		}

		/**
		 * @brief list 代表编译期的list容器及相关操作
		 *			由于c++函数式的相关特性，无法实现编译期双向链表
		*/
		namespace list
		{
			template<typename Type, typename Next>
			struct node;

			template<typename Node = void>
			struct container;

			namespace datail
			{
				template<typename Vector, typename Node>
				struct to_vector_imple
				{
					using type = traits_t<to_vector_imple<typename Vector::template push_back<traits_t<Node>>, typename Node::next>>;
				};

				template<typename Vector>
				struct to_vector_imple<Vector, void>
				{
					using type = Vector;
				};

				template<typename ...Args>
				struct make_list_imple
				{
					using type = void;
				};

				template<typename Head, typename ...Args>
				struct make_list_imple<Head, Args...>
				{
					using type = node<Head, traits_t<make_list_imple<Args...>>>;
				};

				template<typename Node, typename Arg>
				struct change_next_imple
				{
					static_assert(std::is_same_v<node<traits_t<Arg>, typename Arg::Next>, Arg> || std::is_void_v<Arg>, "error change");
					using type = node<traits_t<Node>, Arg>;
				};
			}

			/**
			 * @brief 由数组形式创建链表形式
			 * @tparam ...Args 类型参数列表
			*/
			template<typename ...Args>
			using create = container<traits_t<datail::make_list_imple<Args...>>>;

			/**
			 * @brief 由列表形式转换数组形式
			 * @tparam List 列表
			*/
			template<typename List>
			using to_vector = traits_t<datail::to_vector_imple<vector::container<>, typename List::begin>>;

			/**
			 * @brief node 代表编译期的节点
			 * @tparam Type 此节点中储存的类型
			 * @tparam Next 此节点的下一个节点
			*/
			template<typename Type, typename Next = void>
			struct node
			{
				using type = Type;
				using next = Next;

				template<typename Arg>
				using change = node<Arg, Next>;

				template<typename Arg>
				using change_next = traits_t<datail::change_next_imple<node<Type, Next>, Arg>>;
			};

			/**
			 * @brief container list容器
			 * @tparam Node 头节点
			*/
			template<typename Node>
			struct container
			{
				using begin = Node;
				using end = void;

				/**
				 * @brief apply	将所有数据放入另一个模板
				 * @tparam Template 接收所有数据的模板
				*/
				template<template<typename ...>typename Template>
				using apply = typename list::to_vector<container<Node>>::template apply<Template>;

				template<typename Arg>
				using push_front = container<node<Arg, Node>>;
			};
		}
	}
}
