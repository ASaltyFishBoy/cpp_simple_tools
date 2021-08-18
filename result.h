#pragma once
#include <type_traits>
#include <exception>
namespace my_std
{
	/**
	 * @brief 在无有效值时调用了获取行为将报此异常
	*/
	class bad_result_access :public std::exception
	{
	public:
		bad_result_access () :std::exception ("Bad result access")
		{}
	};

	/**
	 * @brief 错误码包装器
	 * @valid 有效值
	 * @Type 错误码类型，默认从有效值进行推导
	*/
	template<auto valid, typename Type = decltype(valid)>
	class ValidCodeWrapper
	{
	public:
		using code_type = Type;
		void to_valid (code_type & _code)const
		{
			_code = valid;
		}
		bool operator()(const code_type & _code)const
		{
			return _code == valid;
		}
	};

	/**
	 * @brief Result 用于包装返回值和其错误码，若放入有效码时必须携带将要返回的值类型，否则会抛出异常
	 * @tparam CodeChecker 错误码校验器
	 * @tparam Value 返回值类型
	*/
	template<typename CodeChecker, typename Value>
	class Result
	{
	public:
		using code_type = typename CodeChecker::code_type;
	private:
		code_type _code;
		unsigned char _value[sizeof (Value)];

		void _release ()
		{
			if (has_value ())
			{
				value ().~Value ();
			}
		}

		void _set_value (Value value)
		{
			new (reinterpret_cast<Value *>(_value))Value (std::move (value));
		}
	public:
		Result (code_type code) :_code (std::move (code))
		{
			if (!CodeChecker ()(code))
			{
				return;
			}
			throw bad_result_access ();
		}

		Result (code_type code, Value value)
			:_code (std::move (code))
		{
			if (has_value ())
			{
				_set_value (std::move (value));
			}
		}

		template<typename = std::void_t<decltype (&CodeChecker::to_valid)>>
		Result (Value value)
		{
			CodeChecker ().to_valid (_code);
			_set_value (std::move (value));
		}

		Result (const Result & other)
			:_code (other._code)
		{
			if (has_value ())
			{
				_set_value (other.value ());
			}
		}

		Result (Result && other)
			:_code (std::move (other._code))
		{
			if (has_value ())
			{
				_set_value (std::move (other.value ()));
			}
		}

		~Result ()
		{
			_release ();
		}

		constexpr void recode (code_type code)
		{
			if (!CodeChecker ()(code))
			{
				_release ();
				return;
			}
			throw bad_result_access ();
		}

		void reset (code_type code, Value value)
		{
			if (CodeChecker ()(code))
			{
				if (has_value ())
				{
					this->value () = std::move (value);
				}
				else
				{
					_set_value (std::move (value));
				}
			}
			else
			{
				_release ();
			}
			_code = std::move (code);
		}

		template<typename = std::void_t<decltype (&CodeChecker::to_valid)>>
		void reset (Value value)
		{
			CodeChecker ().to_valid (_code);
			value () = std::move (value);
		}

		Result & operator=(Result result)
		{
			reset (std::move (result._code), std::move (result.value ()));
			return *this;
		}

		Value & value ()
		{
			if (!has_value ())
			{
				throw bad_result_access ();
			}
			return *reinterpret_cast<Value *>(_value);
		}

		const Value & value () const
		{
			if (!has_value ())
			{
				throw bad_result_access ();
			}
			return *reinterpret_cast<const Value *>(_value);
		}

		operator const code_type & ()const
		{
			return _code;
		}

		operator bool ()const
		{
			return has_value ();
		}

		bool has_value ()const
		{
			return CodeChecker ()(_code);
		}
	};

}
