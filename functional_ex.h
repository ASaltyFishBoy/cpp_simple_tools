#ifndef FUNCTIONAL_EX
#define FUNCTIONAL_EX

#include <functional>
#include "type_traits_ex.h"
#include "utility_ex.h"

/*
 * @brief DEFINE_FUNCTOR_WITH_NAMESPACE 将一个可调用对象定义为全局仿函数，通常用于规避模板函数指针的类型指定
 * @param _NAMESPACE 此可调用对象的命名空间（从全局域开始）
 * @param _TEMPLATE_FUNCTION 此对象名
*/
#define DEFINE_FUNCTOR_WITH_NAMESPACE(_NAMESPACE,_TEMPLATE_FUNCTION)\
namespace my_std {\
    namespace functors{\
			namespace datail{\
			    struct _TEMPLATE_FUNCTION{\
		            template<typename ...Args>\
	                static decltype(auto) imple(Args&&...args){\
						return _NAMESPACE::_TEMPLATE_FUNCTION(StdForward(args)...);\
					}\
					template<typename ...Args>\
					decltype(auto) operator()(Args&&...args)const{\
						return imple(StdForward(args)...);\
					}\
					template<typename R,typename ...Args>\
					operator R(*)(Args...)()const{\
						return (R(*)(Args...))(imple);\
					}\
				};\
			}\
			constexpr auto _TEMPLATE_FUNCTION = datail::_TEMPLATE_FUNCTION();\
    }\
}

/*
 * @brief DEFINE_FUNCTOR 将一个全局可调用对象定义为全局仿函数，通常用于规避模板函数指针的类型指定
 * @param _TEMPLATE_FUNCTION 此对象名
*/
#define DEFINE_FUNCTOR(_TEMPLATE_FUNCTION) DEFINE_FUNCTOR_WITH_NAMESPACE(,_TEMPLATE_FUNCTION)

/*
 * @brief DEFINE_FUNCTOR_WITH_NAMESPACE 将一个可调用模板对象定义为全局模板仿函数，通常用于规避模板函数指针的类型指定
 * @param _NAMESPACE 此可调用模板对象的命名空间（从全局域开始）
 * @param _TEMPLATE_FUNCTION 此对象名
*/
#define DEFINE_TEMPLATE_FUNCTOR_WITH_NAMESPACE(_NAMESPACE,_TEMPLATE_FUNCTION)\
namespace my_std {\
    namespace functors{\
			namespace datail{\
				template<typename ... _Args>\
			    struct _TEMPLATE_FUNCTION{\
		            template<typename ...Args>\
	                static decltype(auto) imple(Args&&...args){\
						return _NAMESPACE::_TEMPLATE_FUNCTION<_Args...>(StdForward(args)...);\
					}\
					template<typename ...Args>\
					decltype(auto) operator()(Args&&...args)const{\
						return imple(StdForward(args)...);\
					}\
					template<typename R, typename = std::void_t<decltype (R (imple))>>\
					operator R()const \
					{\
						return R(imple); \
					}\
				};\
			}\
			template<typename ...Args>\
			constexpr auto _TEMPLATE_FUNCTION = datail::_TEMPLATE_FUNCTION<Args...> (); \
	}\
}

/*
 * @brief DEFINE_FUNCTOR_WITH_NAMESPACE 将一个全局可调用模板对象定义为全局模板仿函数，通常用于规避模板函数指针的类型指定
 * @param _TEMPLATE_FUNCTION 此对象名
*/
#define DEFINE_TEMPLATE_FUNCTOR(_TEMPLATE_FUNCTION) DEFINE_TEMPLATE_FUNCTOR_WITH_NAMESPACE(,_TEMPLATE_FUNCTION)

namespace my_std
{
	template<typename T>
	class unique_function;

	/**
	 * @brief unique_function 不可拷贝的函数对象，接口和std::function一致，target懒得写，函数表懒得写
	 * @tparam _Result 返回值
	 * @tparam ..._Args 入参类型
	*/
	template<typename _Result, typename ..._Args>
	class unique_function<_Result (_Args...)>
	{
		struct _FunctionBase
		{
			virtual ~_FunctionBase () = default;
			virtual _Result operator()(_Args...)const = 0;
		};
		template<typename _Functor>
		struct _Function :_FunctionBase
		{
			mutable std::remove_cv_t<_Functor> _functor;
			_Function (_Functor _functor) :_functor (std::move (_functor))
			{}
			virtual _Result operator()(_Args... args)const override
			{
				return _functor (StdForward (args)...);
			}
		};

		void * _pointer;
		bool _is_function_pointer;

	public:
		unique_function (const std::nullptr_t & = nullptr) noexcept :_pointer (nullptr)
		{}

		unique_function (const unique_function & other)noexcept = delete;

		unique_function (unique_function && other) noexcept
		{
			_is_function_pointer = other._is_function_pointer;
			_pointer = std::move (other._pointer);
			other._pointer = nullptr;
		}

		unique_function & operator=(const std::nullptr_t &) noexcept
		{
			if (_pointer && !_is_function_pointer)
			{
				delete static_cast<_FunctionBase *>(_pointer);
			}
			return *this;
		}

		unique_function & operator=(unique_function && other) noexcept
		{
			*this = nullptr;
			_is_function_pointer = other._is_function_pointer;
			_pointer = std::move (other._pointer);
			other._pointer = nullptr;
			return *this;
		}

		~unique_function ()
		{
			*this = nullptr;
		}

		template<typename Function
			, std::enable_if_t<
			std::is_convertible_v<Function, _Result (*)(_Args...)>
			, bool>
			= false
		>
			unique_function (Function && other)
			: _pointer (static_cast<_Result (*)(_Args...)> (StdForward (other)))
			, _is_function_pointer (true)
		{}

		template<typename Function
			, std::enable_if_t<
			!std::is_convertible_v<Function, _Result (*)(_Args...)>
			, bool>
			= false
		>
			unique_function (Function other)
			: _pointer (new _Function< std::remove_reference_t<Function>> (std::move (other)))
			, _is_function_pointer (false)
		{}

		operator bool ()const
		{
			return _pointer;
		}

		void swap (unique_function & other) noexcept
		{
			std::swap (_is_function_pointer, other._is_function_pointer);
			std::swap (_pointer, other._pointer);
		}

		_Result operator()(_Args...args)const
		{
			if (!_pointer)
			{
				throw std::bad_function_call ();
			}

			if (_is_function_pointer)
			{
				return static_cast<_Result (*)(_Args...)>(_pointer) (StdForward (args)...);
			}
			else
			{
				return (*static_cast<_FunctionBase *>(_pointer))(StdForward (args)...);
			}
		}

	};

	template< class R, class... ArgTypes >
	bool operator==(const unique_function<R (ArgTypes...)> & f,
		std::nullptr_t)
	{
		return !f;
	}
	template< class R, class... ArgTypes >
	bool operator==(std::nullptr_t,
		const unique_function<R (ArgTypes...)> & f) noexcept
	{
		return !f;
	}
	template< class R, class... ArgTypes >
	bool operator!=(const unique_function<R (ArgTypes...)> & f,
		std::nullptr_t) noexcept
	{
		return f;
	}
	template< class R, class... ArgTypes >
	bool operator!=(std::nullptr_t,
		const unique_function<R (ArgTypes...)> & f) noexcept
	{
		return f;
	}

	template <typename _Fx>
	unique_function (_Fx &&)->unique_function<
		typename my_std::function_traits<std::remove_reference_t<_Fx>>::function_type
	>;


	template<typename T>
	class function_ref;

	/**
	 * @brief function_ref 函数引用对象，不会开辟堆空间，需要自行保护函数对象生存期，接口和std::function一致
	 * @tparam _Result 返回值
	 * @tparam ..._Args 入参类型
	*/
	template<typename _Result, typename ..._Args>
	class function_ref<_Result (_Args...)>
	{

		const void * _pointer;
		_Result (*_function_invoke)(const void *, _Args...);

	public:
		function_ref (const std::nullptr_t & = nullptr)noexcept :_pointer (nullptr)
		{}

		function_ref (const function_ref &) noexcept = default;
		function_ref (function_ref && other) noexcept = default;
		function_ref & operator= (const function_ref &) noexcept = default;

		function_ref & operator= (function_ref && other) noexcept = default;

		function_ref & operator=(const std::nullptr_t &) noexcept
		{
			_pointer = nullptr;
			return *this;
		}

		template<typename Function
			, std::enable_if_t<std::is_convertible_v<Function, _Result (*)(_Args...)>, bool>
			= false
		>
			function_ref (const Function & other)
			:_function_invoke (
				[] (const void * pointer, _Args... args)
		{
			return static_cast<_Result (*)(_Args...)>(pointer)(StdForward (args)...);
		})
			, _pointer (static_cast<_Result (*)(_Args...)>(other))
		{}

		template<typename Function
			, std::enable_if_t<!std::is_convertible_v<Function, _Result (*)(_Args...)>, bool>
			= false
		>
			function_ref (const Function & other)
			:_function_invoke ([] (const void * pointer, _Args... args)
		{
			return (*static_cast<const Function *>(pointer))(StdForward (args)...);
		})
			, _pointer (&other)
		{}

		operator bool ()const
		{
			return _pointer;
		}

		void swap (function_ref & other) noexcept
		{
			std::swap (_function_invoke, other._function_invoke);
			std::swap (_pointer, other._pointer);
		}

		_Result operator()(_Args...args)const
		{
			if (!_pointer)
			{
				throw std::bad_function_call ();
			}
			return _function_invoke (_pointer, StdForward (args)...);
		}

	};

	template< class R, class... ArgTypes >
	bool operator==(const function_ref<R (ArgTypes...)> & f,
		std::nullptr_t)
	{
		return !f;
	}
	template< class R, class... ArgTypes >
	bool operator==(std::nullptr_t,
		const function_ref<R (ArgTypes...)> & f) noexcept
	{
		return !f;
	}
	template< class R, class... ArgTypes >
	bool operator!=(const function_ref<R (ArgTypes...)> & f,
		std::nullptr_t) noexcept
	{
		return f;
	}
	template< class R, class... ArgTypes >
	bool operator!=(std::nullptr_t,
		const function_ref<R (ArgTypes...)> & f) noexcept
	{
		return f;
	}

	template <class _Fx>
	function_ref (_Fx &&)->function_ref<
		typename my_std::function_traits<std::remove_reference_t<_Fx>>::function_type
	>;

	//入左值时是函数引用，注意生存期，入右值时存储
	template<typename Function>
	class scope_guard
	{
	public:
		using value_type = std::conditional_t<
			std::is_rvalue_reference_v<Function>
			, std::remove_reference_t<Function>
			, Function
		>;
		scope_guard (const Function & function) :_m_GuardFunction (StdForward (function))
		{}
		scope_guard (Function && function) :_m_GuardFunction (StdForward (function))
		{}
		~scope_guard ()
		{
			_m_GuardFunction ();
		}
	private:
		value_type _m_GuardFunction;
	};

	template<typename T>
	scope_guard (T &&)->scope_guard<T &&>;

	template<typename T>
	class rvalue_reference_wrapper
	{
		T && _t;
	public:
		rvalue_reference_wrapper (T && _t) :_t (std::move (_t))
		{}

		rvalue_reference_wrapper (const rvalue_reference_wrapper & other) :_t (std::move (other._t))
		{}

		rvalue_reference_wrapper & operator= (const rvalue_reference_wrapper & other)
		{
			_t = std::move (other._t);
		}

		operator T && () const
		{
			return get ();
		}

		T && get () const
		{
			return std::move (_t);
		}
	};

	template<typename T>
	constexpr rvalue_reference_wrapper<std::remove_reference_t <T>> rref (std::remove_reference_t <T> && t)
	{
		return { std::move (t) };
	}
}

#endif // !FUNCTIONAL_EX