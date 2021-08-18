#pragma once
#include "type_traits_ex.h"
namespace my_std
{
	/**
	 * @brief ����������������c++����ʽ��������ԣ��Ա�����vector��ĳֵ�����޸��Ǹ�On���ӶȲ����������޸���������list
	*/
	namespace compile_time
	{
		/**
		 * @brief vector ��������ڵ�vector��������ز���
		*/
		namespace vector
		{
			/**
			 * @brief iterator vector�����ĵ�����
			 * @tparam I �±�����
			 * @tparam Vector ��Դ����
			*/
			template<size_t I, typename Vector>
			struct iterator
			{
				using type = typename Vector::template arg<I>;

				/*template<typename Arg>
				using change_to = Vector::template change < I, Arg>;*/
			};

			/**
			 * @brief container vector����
			 * @tparam ...Args ����������
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

				//��ʵ�ֱȽϸ��ӣ���ʱû��Ҫ
				/*
				template<size_t I, typename Arg>
				using change = container<>;*/

				/**
				 * @brief apply	���������ݷ�����һ��ģ��
				 * @tparam Template �����������ݵ�ģ��
				*/
				template<template<typename ...>typename Template>
				using apply = Template<Args...>;
			};
		}

		/**
		 * @brief list ��������ڵ�list��������ز���
		 *			����c++����ʽ��������ԣ��޷�ʵ�ֱ�����˫������
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
			 * @brief ��������ʽ����������ʽ
			 * @tparam ...Args ���Ͳ����б�
			*/
			template<typename ...Args>
			using create = container<traits_t<datail::make_list_imple<Args...>>>;

			/**
			 * @brief ���б���ʽת��������ʽ
			 * @tparam List �б�
			*/
			template<typename List>
			using to_vector = traits_t<datail::to_vector_imple<vector::container<>, typename List::begin>>;

			/**
			 * @brief node ��������ڵĽڵ�
			 * @tparam Type �˽ڵ��д��������
			 * @tparam Next �˽ڵ����һ���ڵ�
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
			 * @brief container list����
			 * @tparam Node ͷ�ڵ�
			*/
			template<typename Node>
			struct container
			{
				using begin = Node;
				using end = void;

				/**
				 * @brief apply	���������ݷ�����һ��ģ��
				 * @tparam Template �����������ݵ�ģ��
				*/
				template<template<typename ...>typename Template>
				using apply = typename list::to_vector<container<Node>>::template apply<Template>;

				template<typename Arg>
				using push_front = container<node<Arg, Node>>;
			};
		}
	}
}
