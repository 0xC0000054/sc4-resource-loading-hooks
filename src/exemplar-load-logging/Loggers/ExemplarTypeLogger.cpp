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

#include "ExemplarTypeLogger.h"
#include "ExemplarTypes.h"
#include "cGZPersistResourceKey.h"
#include "cIGZPersistResource.h"
#include "cIGZVariant.h"
#include "cISCProperty.h"
#include "cISCPropertyHolder.h"
#include "cISCResExemplar.h"
#include "cRZAutoRefCount.h"
#include "SCPropertyUtil.h"

static constexpr uint32_t ExemplarTypePropertyID = 0x00000010;

ExemplarTypeLogger::ExemplarTypeLogger(
	const std::filesystem::path& logFilePath,
	bool debugLevel)
	: ExemplarResourceLoggerBase(/*logResourceLoadErrors*/false, logFilePath, debugLevel)
{
}

void ExemplarTypeLogger::ExemplarLoaded(
	const char* const originalFunctionName,
	const cGZPersistResourceKey& key,
	cISCResExemplar* resExemplar)
{
	uint32_t exemplarType = 0;

	const cISCPropertyHolder* propertyHolder = resExemplar->AsISCPropertyHolder();
	bool wroteExemplarType = false;

	if (propertyHolder)
	{
		if (SCPropertyUtil::GetPropertyValue(propertyHolder, ExemplarTypePropertyID, exemplarType))
		{
			if (IsDebugLevel())
			{
				WriteLineFormatted(
					"%s: T=0x%08X G=0x%08X, I=0x%08X, ExemplarType=%s",
					originalFunctionName,
					key.type,
					key.group,
					key.instance,
					ExemplarTypes::GetExemplarTypeName(exemplarType));
			}
			else
			{
				WriteLineFormatted(
					"T=0x%08X G=0x%08X, I=0x%08X, ExemplarType=%s",
					key.type,
					key.group,
					key.instance,
					ExemplarTypes::GetExemplarTypeName(exemplarType));
			}

			wroteExemplarType = true;
		}
	}

	if (!wroteExemplarType)
	{
		LogExemplarTGI(originalFunctionName, key);
	}
}
