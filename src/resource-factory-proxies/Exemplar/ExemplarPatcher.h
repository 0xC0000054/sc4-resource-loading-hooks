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
#include "cISCPropertyHolder.h"
#include "cISCResExemplar.h"
#include "cRZAutoRefCount.h"
#include "PersistResourceKeyBoostHash.h"

#include "boost/container/deque.hpp"
#include "boost/unordered/unordered_flat_map.hpp"

#include <mutex>

class ExemplarPatcher
{
public:

	using PatchContainer = boost::unordered_flat_map<const cGZPersistResourceKey, boost::container::deque<cRZAutoRefCount<cISCPropertyHolder>>>;

	ExemplarPatcher();

	void LoadExemplarPatches();

	void ApplyPatches(const cGZPersistResourceKey& key, cISCResExemplar* pExemplar);

	void SetDebugLoggingEnabled(bool enabled);

private:

	PatchContainer patches;
	std::mutex mutex;
	bool debugLoggingEnabled;
};