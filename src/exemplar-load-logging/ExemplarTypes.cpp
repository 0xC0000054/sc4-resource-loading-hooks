/*
 * This file is part of sc4-resource-loading-hooks, a DLL Plugin for SimCity 4
 * that allows other DLLs to modify resources as the game loads them.
 *
 * Copyright (C) 2024, 2025 Nicholas Hayes
 *
 * sc4-resource-loading-hooks is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * sc4-resource-loading-hooks is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with sc4-resource-loading-hooks.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "ExemplarTypes.h"
#include "StringViewUtil.h"
#include <charconv>

static const std::map<uint32_t, std::string> ExemplarTypeMap
{
	{ 0x00000000, "Other/Unknown" },
	{ 0x00000001, "Tuning" },
	{ 0x00000002, "Buildings" },
	{ 0x00000003, "RCI" },
	{ 0x00000004, "Developer" },
	{ 0x00000005, "Simulator" },
	{ 0x00000006, "Road" },
	{ 0x00000007, "Bridge" },
	{ 0x00000008, "MiscNetwork" },
	{ 0x00000009, "NetworkIntersection" },
	{ 0x0000000A, "Rail" },
	{ 0x0000000B, "Highway" },
	{ 0x0000000C, "PowerLine" },
	{ 0x0000000D, "Terrain" },
	{ 0x0000000E, "Ordinances" },
	{ 0x0000000F, "Flora" },
	{ 0x00000010, "LotConfigurations" },
	{ 0x00000011, "Foundations" },
	{ 0x00000012, "Advice" },
	{ 0x00000013, "Lighting" },
	{ 0x00000014, "Cursor" },
	{ 0x00000015, "LotRetainingWalls" },
	{ 0x00000016, "Vehicles" },
	{ 0x00000017, "Pedestrians" },
	{ 0x00000018, "Aircraft" },
	{ 0x00000019, "Watercraft" },
	{ 0x0000001E, "Prop" },
	{ 0x0000001F, "Construction" },
	{ 0x00000020, "Automata Tuning" },
	{ 0x00000021, "Type 21" },
	{ 0x00000022, "Disaster" },
	{ 0x00000023, "Data view" },
	{ 0x00000024, "Crime" },
	{ 0x00000025, "Audio" },
	{ 0x00000026, "My Sim Template" },
	{ 0x00000027, "TerrainBrush" },
	{ 0x00000028, "Misc Catalog" },
};

namespace
{
	bool TryParseNumber(
		const std::string_view& value,
		uint32_t& outValue)
	{
		const char* begin = value.data();
		const char* const end = begin + value.size();
		int base = 10;

		// std::from_chars can't handle hexadecimal numbers with the 0x
		// prefix, so we have to skip it and set the number base to 16.
		if (StringViewUtil::StartsWithIgnoreCase(value, "0x"))
		{
			begin += 2;
			base = 16;
		}

		const auto result = std::from_chars(begin, end, outValue, base);

		return result.ec == std::errc{} && result.ptr == end;
	}
}

const char* const ExemplarTypes::GetExemplarTypeName(uint32_t type)
{
	const auto& item = ExemplarTypeMap.find(type);

	if (item != ExemplarTypeMap.end())
	{
		return item->second.c_str();
	}

	return "Unknown";
}

bool ExemplarTypes::TryParseExemplarNumber(
	const std::string_view& value,
	uint32_t& exemplarType)
{
	uint32_t outValue = 0;

	if (TryParseNumber(value, outValue))
	{
		if (ExemplarTypeMap.contains(outValue))
		{
			exemplarType = outValue;
			return true;
		}
	}

	exemplarType = 0;
	return false;
}
