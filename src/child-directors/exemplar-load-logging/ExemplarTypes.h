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

#pragma once
#include <map>
#include <string>

namespace ExemplarTypes
{
	const char* const GetExemplarTypeName(uint32_t type);

	bool TryParseExemplarNumber(
		const std::string_view& value,
		uint32_t& exemplarType);
}