#pragma once
#include "Module/base_tool/functional_ex.h"

namespace my_std
{
	template<typename _T, typename _TT>
	_T constCast (_TT && _t)
	{
		return const_cast<_T>(StdForward (_t));
	}

	template<typename _T, typename _TT>
	_T staticCast (_TT && _t)
	{
		return static_cast<_T>(StdForward (_t));
	}

	template<typename _T, typename _TT>
	_T dynamicCast (_TT && _t)
	{
		return dynamic_cast<_T>(StdForward (_t));
	}

	template<typename _T, typename _TT>
	_T reinterpretCast (_TT && _t)
	{
		return reinterpret_cast<_T>(StdForward (_t));
	}
}

DEFINE_TEMPLATE_FUNCTOR_WITH_NAMESPACE (my_std, constCast)
DEFINE_TEMPLATE_FUNCTOR_WITH_NAMESPACE (my_std, staticCast)
DEFINE_TEMPLATE_FUNCTOR_WITH_NAMESPACE (my_std, dynamicCast)
DEFINE_TEMPLATE_FUNCTOR_WITH_NAMESPACE (my_std, reinterpretCast)


#if __has_include(<memory>)
#include <memory>
namespace my_std
{
	template<typename Derived, typename Base, typename Del>
	std::unique_ptr<Derived, Del>
		static_pointer_cast (std::unique_ptr<Base, Del> && p)
	{
		auto d = static_cast<Derived *>(p.release ());
		return std::unique_ptr<Derived, Del> (d, std::move (p.get_deleter ()));
	}

	template<typename Derived, typename Base, typename Del>
	std::unique_ptr<Derived, Del>
		dynamic_pointer_cast (std::unique_ptr<Base, Del> && p)
	{
		if (Derived * result = dynamic_cast<Derived *>(p.get ()))
		{
			p.release ();
			return std::unique_ptr<Derived, Del> (result, std::move (p.get_deleter ()));
		}
		return std::unique_ptr<Derived, Del> (nullptr, p.get_deleter ());
	}
}

DEFINE_TEMPLATE_FUNCTOR_WITH_NAMESPACE (std, static_pointer_cast)
DEFINE_TEMPLATE_FUNCTOR_WITH_NAMESPACE (std, dynamic_pointer_cast)
DEFINE_TEMPLATE_FUNCTOR_WITH_NAMESPACE (std, const_pointer_cast)
DEFINE_TEMPLATE_FUNCTOR_WITH_NAMESPACE (std, reinterpret_pointer_cast)
#endif
