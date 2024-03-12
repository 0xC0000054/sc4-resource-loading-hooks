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

#include "ExemplarErrorLogger.h"
#include "cGZPersistResourceKey.h"


ExemplarErrorLogger::ExemplarErrorLogger(
	const std::filesystem::path& logFilePath,
	bool debugLevel)
	: ExemplarLoggerBase(logFilePath, debugLevel)
{
}

ExemplarLoggerOptions ExemplarErrorLogger::GetLoggerOptions() const
{
	return ExemplarLoggerOptions::LogExemplarLoadingErrors;
}

void ExemplarErrorLogger::ExemplarLoaded(
	const char* const originalFunctionName,
	const cGZPersistResourceKey& key,
	cISCResExemplar* resExemplar)
{
}
