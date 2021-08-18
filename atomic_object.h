#ifndef ATOMIC_OBJECT_H
#define ATOMIC_OBJECT_H

#include <mutex>
#include <shared_mutex>

namespace my_std
{
	/**
	 * @brief atomic_object ԭ�Ӷ���
	 *		ͨ��get��const_get��ȡ������󣬼�����������������������
	 *		�������ӿں�ָ��һ��
	 * @tparam _Value Ҫ����Ķ�������
	 * @tparam _Mutex ������������ʹ�õ���������,��ʹ�ö�д��
	 *
	 * @example
	 *		{
	 *			atomic_object<std::vector<int>> p;
	 *			{
	 *				auto proxy = p.get();//��ʱ����
	 *				proxy->push_back(2);
	 *			}//��ʱ����
	 *		}
	*/
	template<typename _Value, typename _Mutex = std::mutex>
	class atomic_object
	{
		static_assert(
			std::is_class_v<_Value> || std::is_union_v<_Value>
			, "base type please using std atomic"
			);

		template<typename T, typename = void>
		struct _GetLocker
		{
			using locker = std::lock_guard<_Mutex>;
		};

		template<typename T>
		struct _GetLocker<T, std::void_t<decltype(&T::lock_shared)>>
		{
			using locker = std::shared_lock<_Mutex>;
		};

		using _locker = std::lock_guard<_Mutex>;
		using _shared_locker = typename _GetLocker<_Mutex>::locker;

		class _lock_helper
		{
			friend atomic_object;
			_locker locker;
			_Value & _m_Value;
			_lock_helper (_Mutex & mutex, _Value & value)
				:locker (mutex)
				, _m_Value (value)
			{}

		public:
			_Value * operator->()const
			{
				return &_m_Value;
			}
			_Value & operator*()const
			{
				return _m_Value;
			}
		};

		class _shared_lock_helper
		{
			friend atomic_object;
			_shared_locker locker;
			const _Value & _m_Value;

			_shared_lock_helper (_Mutex & mutex, const _Value & value)
				:locker (mutex)
				, _m_Value (value)
			{}
		public:
			const _Value * operator->()const
			{
				return &_m_Value;
			}
			const _Value & operator*()const
			{
				return _m_Value;
			}
		};
	public:
		//�����������а���mutable���ݺ;�̬��ȫ������ ��Ȼ���ܵ��¶��̴߳���
		_shared_lock_helper const_get ()const
		{
			return {
				_m_Mutex
				,_m_Value
			};
		}

		_lock_helper get ()
		{
			return {
				_m_Mutex
				,_m_Value
			};
		}

		template<typename ...Args>
		atomic_object (Args&&...args) :_m_Value (std::forward<Args> (args)...)
		{}
	private:
		mutable _Mutex _m_Mutex;
		_Value _m_Value;
	};
}

#endif // !ATOMIC_OBJECT_H
