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
#include "ExemplarResourceLoggerBase.h"

class ExemplarTypeLogger final : public ExemplarResourceLoggerBase
{
public:

	ExemplarTypeLogger(
		const std::filesystem::path& logFilePath,
		bool debugLevel);

private:

	void ExemplarLoaded(
		const char* const originalFunctionName,
		const cGZPersistResourceKey& key,
		cISCResExemplar* resExemplar) override;
};

