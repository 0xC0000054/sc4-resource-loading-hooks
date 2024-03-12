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

#include "ExemplarLoadLoggingDllDirector.h"
#include "FileSystem.h"
#include "Logger.h"
#include "ExemplarErrorLogger.h"
#include "ExemplarTGILogger.h"
#include "ExemplarTypeLogger.h"
#include "ExemplarTypes.h"
#include "FilteredExemplarLogger.h"
#include "StringViewUtil.h"
#include "cIExemplarLoadHookServer.h"
#include "cIGZApp.h"
#include "cIGZCmdLine.h"
#include "cIGZCOM.h"
#include "cIGZFrameWork.h"
#include "cRZAutoRefCount.h"
#include "cRZBaseString.h"
#include <GZServPtrs.h>

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <Windows.h>
#include "wil/resource.h"
#include "wil/win32_helpers.h"

#include "EASTLConfigSC4.h"
#include "EASTL\vector.h"

static constexpr uint32_t kExemplarLoadLoggingDirectorID = 0xC6703C6C;

using namespace std::string_view_literals;

static constexpr std::string_view PluginLogFileName = "SC4ExemplarLoad.log"sv;

ExemplarLoadLoggingDllDirector::ExemplarLoadLoggingDllDirector()
{
	std::filesystem::path dllFolderPath = FileSystem::GetDllFolderPath();

	logFilePath = dllFolderPath;
	logFilePath /= PluginLogFileName;
}

bool ExemplarLoadLoggingDllDirector::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZIID_cIExemplarLoadHookTarget)
	{
		*ppvObj = static_cast<cIExemplarLoadHookTarget*>(this);
		AddRef();

		return true;
	}
	else if (riid == GZIID_cIExemplarLoadErrorHookTarget)
	{
		*ppvObj = static_cast<cIExemplarLoadErrorHookTarget*>(this);
		AddRef();

		return true;
	}

	return cRZCOMDllDirector::QueryInterface(riid, ppvObj);
}

uint32_t ExemplarLoadLoggingDllDirector::AddRef()
{
	return cRZCOMDllDirector::AddRef();
}

uint32_t ExemplarLoadLoggingDllDirector::Release()
{
	return cRZCOMDllDirector::Release();
}

uint32_t ExemplarLoadLoggingDllDirector::GetDirectorID() const
{
	return kExemplarLoadLoggingDirectorID;
}

bool ExemplarLoadLoggingDllDirector::OnStart(cIGZCOM* pCOM)
{
	cIGZFrameWork* const pFramework = pCOM->FrameWork();

	const cIGZCmdLine* pCmdLine = pFramework->CommandLine();

	cRZBaseString value;

	if (pCmdLine->IsSwitchPresent(cRZBaseString("exemplar-log"), value, true))
	{
		SetLoggerFromCommandLine(value.ToChar());

		if (exemplarLogger)
		{
			// We can put the cIExemplarLoadHookServer GetClassObject call in OnStart because this class
			// is a child director of ResourceLoadingHooksDllDirector, it will call our OnStart after it
			// adds the cIExemplarLoadHookServer class into the game.
			//
			// Other DLLs should put the cIExemplarLoadHookServer GetClassObject call in PreAppInit.
			// This is because there is no guarantee that SC4 will load you DLL and call its OnStart
			// method after the SC4ResourceLoadingHooks DLL.

			cRZAutoRefCount<cIExemplarLoadHookServer> exemplarHookServer;

			if (pCOM->GetClassObject(
				GZCLSID_cIExemplarLoadHookServer,
				GZIID_cIExemplarLoadHookServer,
				exemplarHookServer.AsPPVoid()))
			{
				const ExemplarLoggerOptions options = exemplarLogger->GetLoggerOptions();

				if ((options & ExemplarLoggerOptions::LogExemplarLoading) != ExemplarLoggerOptions::None)
				{
					exemplarHookServer->AddLoadNotification(this);
				}

				if ((options & ExemplarLoggerOptions::LogExemplarLoadingErrors) != ExemplarLoggerOptions::None)
				{
					exemplarHookServer->AddLoadErrorNotification(this);
				}
			}
		}
	}

	return true;
}

void ExemplarLoadLoggingDllDirector::ExemplarLoaded(
	const char* const originalFunctionName,
	const cGZPersistResourceKey& key,
	cISCResExemplar* resExemplar)
{
	exemplarLogger->ExemplarLoaded(
		originalFunctionName,
		key,
		resExemplar);
}

void ExemplarLoadLoggingDllDirector::LoadError(
	const char* const originalFunctionName,
	uint32_t riid)
{
	exemplarLogger->LoadError(
		originalFunctionName,
		riid);
}

void ExemplarLoadLoggingDllDirector::LoadError(
	const char* const originalFunctionName,
	uint32_t riid,
	const cGZPersistResourceKey& key)
{
	exemplarLogger->LoadError(
		originalFunctionName,
		riid,
		key);
}

void ExemplarLoadLoggingDllDirector::SetLoggerFromCommandLine(const std::string_view& argName)
{
	if (StringViewUtil::EqualsIgnoreCase(argName, "error"sv))
	{
		exemplarLogger = std::make_unique<ExemplarErrorLogger>(logFilePath, false);
	}
	else if (StringViewUtil::EqualsIgnoreCase(argName, "debug"sv))
	{
		exemplarLogger = std::make_unique<ExemplarTGILogger>(
			/*logResourceLoadErrors*/true,
			logFilePath,
			true);
	}
	else if (StringViewUtil::EqualsIgnoreCase(argName, "type"sv))
	{
		exemplarLogger = std::make_unique<ExemplarTypeLogger>(logFilePath, false);
	}
	else if (StringViewUtil::EqualsIgnoreCase(argName, "TGI"sv))
	{
		exemplarLogger = std::make_unique<ExemplarTGILogger>(
			/*logResourceLoadErrors*/true,
			logFilePath,
			false);
	}
	else
	{
		// Any other value is an integer that is used to filter for a specific
		// exemplar type.
		uint32_t type = 0;

		if (ExemplarTypes::TryParseExemplarNumber(argName, type))
		{
			exemplarLogger = std::make_unique<FilteredExemplarLogger>(type, logFilePath, false);
		}
		else
		{
			Logger::GetInstance().WriteLineFormatted(
				LogLevel::Error,
				"Invalid exemplar type argument '%s', expected an exemplar type number.",
				std::string(argName).c_str());
		}
	}
}
