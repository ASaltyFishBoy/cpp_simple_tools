#pragma once
#include <mutex>
#include <atomic>
#include <memory>

/**
 * @brief SingleCaseBase 单例基类，保证懒汉模式和饿汉模式都将使用同一个对象
 * @tparam Case 保存的类型
*/
template<typename Case>
class SingleCaseBase
{
protected:
	inline static std::unique_ptr<Case> uptr;
};

/**
 * @brief LazySingleCase 懒汉单例，可带参构造
 * @tparam Case 保存的类型
*/
template<typename Case>
class LazySingleCase :SingleCaseBase <Case>
{

	LazySingleCase () = delete;
public:
	//create,delete不支持多线程安全
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
 * @brief LazySingleCase 饿汉单例
 * @tparam Case 保存的类型
*/
template<typename Case>
class HungrySingleCase :SingleCaseBase <Case>
{
private:
	//静态变量的析构顺序是不确定的，故uptr析构时不能保证ptr的存在性，不对它做处理
	//使用这个对象时必须保证对象可以默认构造
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
