#ifndef SHARED_REFERENCE
#define SHARED_REFERENCE

#include <memory>
namespace my_std
{
	/**
	 * @brief 主动管理生存期的对象，与智能指针近似，与其区别为一般来说表达不能为空的对象，可表述为智能引用
	 *			但要注意移动之后的可用性和引用对象变更
	*/
	namespace datail
	{
		template<typename T>
		class shared_ref_base
		{
		protected:
			std::shared_ptr<T> ptr;
		public:
			shared_ref_base (std::shared_ptr<T> ptr)
				:ptr (ptr ? std::move (ptr) : throw std::bad_weak_ptr ())
			{};

			operator const std::shared_ptr<T> & ()const &
			{
				return ptr;
			}

			operator std::shared_ptr<T> && () &&
			{
				return std::move (ptr);
			}
			operator std::weak_ptr<T> () const
			{
				return ptr;
			}
		};

		template<typename T, typename Deleter>
		class unique_ref_base
		{
		protected:
			std::unique_ptr<T, Deleter> ptr;
		public:
			unique_ref_base (std::unique_ptr<T, Deleter> ptr)
				:ptr (ptr ? std::move (ptr) : throw std::bad_weak_ptr ())
			{};

			operator std::unique_ptr<T, Deleter> && () &&
			{
				return std::move (ptr);
			}
		};
	}

	template<typename T>
	class shared_ref :public datail::shared_ref_base<T>
	{
	public:
		using datail::shared_ref_base<T>::shared_ref_base;

		T * operator->() const
		{
			return &*this->ptr;
		}

		operator T & () const
		{
			return get ();
		}

		T & get ()const
		{
			return this->ptr.get ();
		}
	};

	template<typename T>
	class shared_ref<T[]> :public datail::shared_ref_base<T[]>
	{
	public:
		using datail::shared_ref_base<T[]>::shared_ref_base;
		operator T * () const
		{
			return get ();
		}

		T * get ()const
		{
			return this->ptr.get ();
		}

		T & operator[](std::size_t i) const
		{
			return this->ptr[i];
		}
	};

	template<typename T, typename Deleter = std::default_delete<T>>
	class unique_ref :public datail::unique_ref_base<T, Deleter>
	{
	public:
		using datail::unique_ref_base<T, Deleter>::unique_ref_base;

		T * operator->() const
		{
			return &*this->ptr;
		}

		operator T & () const
		{
			return get ();
		}

		T & get ()const
		{
			return *this->ptr;
		}
	};

	template<typename T, typename Deleter>
	class unique_ref<T[], Deleter> :public datail::unique_ref_base<T[], Deleter>
	{
	public:
		using datail::unique_ref_base<T[], Deleter>::unique_ref_base;

		operator T * () const
		{
			return get ();
		}

		T * get ()const
		{
			return this->ptr.get ();
		}

		T & operator[](std::size_t i) const
		{
			return this->ptr[i];
		}
	};


}


#endif // !SHARED_REFERENCE

