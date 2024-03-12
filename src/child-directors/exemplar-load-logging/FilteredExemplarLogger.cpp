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

#include "FilteredExemplarLogger.h"
#include "cGZPersistResourceKey.h"
#include "cIGZPersistResource.h"
#include "cIGZVariant.h"
#include "cISCProperty.h"
#include "cISCPropertyHolder.h"
#include "cISCResExemplar.h"
#include "cRZAutoRefCount.h"
#include "SCPropertyUtil.h"

static constexpr uint32_t ExemplarTypePropertyID = 0x00000010;

FilteredExemplarLogger::FilteredExemplarLogger(
	uint32_t type,
	const std::filesystem::path& logFilePath,
	bool debugLevel)
	: ExemplarResourceLoggerBase(/*logResourceLoadErrors*/false, logFilePath, debugLevel)
{
	exemplarTypeToLog = type;
}

void FilteredExemplarLogger::ExemplarLoaded(
	const char* const originalFunctionName,
	const cGZPersistResourceKey& key,
	cISCResExemplar* resExemplar)
{
	const cISCPropertyHolder* propertyHolder = resExemplar->AsISCPropertyHolder();

	if (propertyHolder)
	{
		uint32_t type = 0;

		if (SCPropertyUtil::GetPropertyValue(propertyHolder, ExemplarTypePropertyID, type))
		{
			if (type == exemplarTypeToLog)
			{
				LogExemplarTGI(originalFunctionName, key);
			}
		}
	}
}
