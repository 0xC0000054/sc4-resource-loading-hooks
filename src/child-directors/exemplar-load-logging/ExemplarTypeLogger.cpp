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
