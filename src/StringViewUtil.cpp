///////////////////////////////////////////////////////////////////////////////
//
// This file is part of sc4-resource-loading-hooks, a DLL Plugin for SimCity 4
// that allows other DLLs to modify resources as the game loads them.
//
// Copyright (c) 2024 Nicholas Hayes
//
// This file is licensed under terms of the MIT License.
// See LICENSE.txt for more information.
//
///////////////////////////////////////////////////////////////////////////////

#include "StringViewUtil.h"
#include "boost/algorithm/string.hpp"

bool StringViewUtil::EqualsIgnoreCase(const std::string_view& lhs, const std::string_view& rhs)
{
	return lhs.length() == rhs.length()
		&& boost::iequals(lhs, rhs);
}

bool StringViewUtil::StartsWithIgnoreCase(const std::string_view& lhs, const std::string_view& rhs)
{
	return lhs.length() >  rhs.length()
		&& boost::istarts_with(lhs, rhs);
}
