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
#include "cIExemplarLoadHookTarget.h"
#include "cIExemplarLoadErrorHookTarget.h"
#include "cRZCOMDllDirector.h"
#include "ExemplarLoggerBase.h"
#include <filesystem>
#include <memory>

class ExemplarLoadLoggingDllDirector
	: public cRZCOMDllDirector,
	  private cIExemplarLoadHookTarget,
	  private cIExemplarLoadErrorHookTarget
{
public:

	ExemplarLoadLoggingDllDirector();

	bool QueryInterface(uint32_t riid, void** ppvObj) override;

	uint32_t AddRef() override;

	uint32_t Release() override;

	uint32_t GetDirectorID() const override;

	bool OnStart(cIGZCOM* pCOM) override;

private:

	void ExemplarLoaded(
		const char* const originalFunctionName,
		const cGZPersistResourceKey& key,
		cISCResExemplar* resExemplar) override;

	void LoadError(
		const char* const originalFunctionName,
		uint32_t riid) override;

	void LoadError(
		const char* const originalFunctionName,
		uint32_t riid,
		const cGZPersistResourceKey& key) override;

	void SetLoggerFromCommandLine(const std::string_view& argName);

	std::filesystem::path logFilePath;
	std::unique_ptr<ExemplarLoggerBase> exemplarLogger;
};