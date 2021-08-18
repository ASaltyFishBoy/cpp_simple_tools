#pragma once
#include <mutex>
#include <atomic>
#include <memory>

/**
 * @brief SingleCaseBase �������࣬��֤����ģʽ�Ͷ���ģʽ����ʹ��ͬһ������
 * @tparam Case ���������
*/
template<typename Case>
class SingleCaseBase
{
protected:
	inline static std::unique_ptr<Case> uptr;
};

/**
 * @brief LazySingleCase �����������ɴ��ι���
 * @tparam Case ���������
*/
template<typename Case>
class LazySingleCase :SingleCaseBase <Case>
{

	LazySingleCase () = delete;
public:
	//create,delete��֧�ֶ��̰߳�ȫ
	template<typename ...Args>
	static Case & create (Args&&...args)
	{
		if (SingleCaseBase<Case>::uptr)
		{
			return *SingleCaseBase<Case>::uptr;
		}
		SingleCaseBase <Case>::uptr = std::make_unique<Case> (std::forward<Args> (args)...);
		return *SingleCaseBase<Case>::uptr;
	}
	template<typename ...Args>
	static void reset ()
	{
		SingleCaseBase<Case>::uptr.reset ();
	}

	static Case & instance ()
	{
		return *SingleCaseBase<Case>::uptr;
	}
};


/**
 * @brief LazySingleCase ��������
 * @tparam Case ���������
*/
template<typename Case>
class HungrySingleCase :SingleCaseBase <Case>
{
private:
	//��̬����������˳���ǲ�ȷ���ģ���uptr����ʱ���ܱ�֤ptr�Ĵ����ԣ�������������
	//ʹ���������ʱ���뱣֤�������Ĭ�Ϲ���
	inline static std::atomic<Case *> ptr = nullptr;
	inline static std::mutex mutex = std::mutex ();

	HungrySingleCase () = delete;
public:

	static Case & instance ()
	{
		if (ptr)
		{
			return *ptr;
		}
		std::lock_guard<std::mutex> lock (mutex);
		return (ptr ?
			*ptr
			: *(ptr = (
				uptr ?
				uptr
				: SingleCaseBase <Case>::uptr = std::make_unique<Case> ()
				).get ())
			);
	}
};
