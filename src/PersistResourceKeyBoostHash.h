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

#pragma once
#include "cGZPersistResourceKey.h"
#include "boost/functional/hash.hpp"

// a hash implementation of cGZPersistResourceKey for use in boost collections
template<> struct boost::hash<const cGZPersistResourceKey>
{
	size_t operator()(const cGZPersistResourceKey& key) const noexcept
	{
		size_t seed = 0;

		boost::hash_combine(seed, key.type);
		boost::hash_combine(seed, key.instance);
		boost::hash_combine(seed, key.group);

		return seed;
	}
};

template<> struct std::equal_to<const cGZPersistResourceKey>
{
	constexpr bool operator()(const cGZPersistResourceKey& lhs, const cGZPersistResourceKey& rhs) const
	{
		return lhs.instance == rhs.instance && lhs.group == rhs.group && lhs.type == rhs.type;
	}
};