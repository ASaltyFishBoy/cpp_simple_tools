#pragma once

#include <variant>
#include "functional_ex.h"

namespace my_std
{
	template<typename _T>
	using forward_object
		= std::variant<
		std::reference_wrapper<const _T>,
		my_std::rvalue_reference_wrapper<std::remove_const_t<_T>>
		>;
}