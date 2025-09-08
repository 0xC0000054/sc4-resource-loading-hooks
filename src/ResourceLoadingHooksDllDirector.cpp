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

#include "version.h"
#include "FileSystem.h"
#include "Logger.h"
#include "ExemplarLoadLogger.h"
#include "ExemplarResourceFactoryProxy.h"
#include "cIGZApp.h"
#include "cIGZCmdLine.h"
#include "cIGZCOM.h"
#include "cIGZFrameWork.h"
#include "cIGZPersistResourceManager.h"
#include "cRZAutoRefCount.h"
#include "cRZBaseString.h"
#include "cRZCOMDllDirector.h"
#include "cISC4App.h"
#include <GZServPtrs.h>

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>

static constexpr uint32_t kResourceLoadingHooksDirectorID = 0x512B0315;

using namespace std::string_view_literals;

static constexpr std::string_view PluginLogFileName = "SC4ResourceLoadingHooks.log"sv;

static cIGZUnknown* CreateExemplarResourceProxy()
{
	// The cast is required for the compiler to differentiate between
	// the base classes that derive from cIGZUnknown.
	return static_cast<cIGZPersistResourceFactory*>(new ExemplarResourceFactoryProxy());
}

static bool QueryForExemplarResourceFactoryProxyInterface(uint32_t riid, void** ppvObj)
{
	bool result = false;

	// The interface is implemented by the exemplar resource factory proxy.
	// Grab a reference to the exemplar resource factory proxy instance that
	// the game's resource manager created and call QueryInterface on it.

	cIGZPersistResourceManagerPtr pResMan;

	if (pResMan)
	{
		cRZAutoRefCount<cIGZPersistResourceFactory> pFactory;

		if (pResMan->FindObjectFactory(ExemplarTypeID, pFactory.AsPPObj()))
		{
			result = pFactory->QueryInterface(riid, ppvObj);
		}
	}

	return result;
}

class ResourceLoadingHooksDllDirector : public cRZCOMDllDirector
{
public:

	ResourceLoadingHooksDllDirector()
	{
		AddCls(GZCLSID_ExemplarFactoryProxy, CreateExemplarResourceProxy);
		AddCls(GZCLSID_cIExemplarLoadHookServer, QueryForExemplarResourceFactoryProxyInterface);
		AddCls(GZCLSID_cIExemplarPatchingServer, QueryForExemplarResourceFactoryProxyInterface);

		std::filesystem::path dllFolderPath = FileSystem::GetDllFolderPath();

		std::filesystem::path logFilePath = dllFolderPath;
		logFilePath /= PluginLogFileName;

		Logger& logger = Logger::GetInstance();
		logger.Init(logFilePath, LogLevel::Info);
		logger.WriteLogFileHeader("SC4ResourceLoadingHooks v" PLUGIN_VERSION_STR);
	}

	uint32_t GetDirectorID() const
	{
		return kResourceLoadingHooksDirectorID;
	}

	void RegisterResourceFactoryProxies()
	{
		Logger& logger = Logger::GetInstance();

		cIGZPersistResourceManagerPtr pResMan;

		if (pResMan->RegisterObjectFactory(
			GZCLSID_ExemplarFactoryProxy,
			ExemplarTypeID,
			nullptr))
		{
			logger.WriteLine(
				LogLevel::Info,
				"Registered the exemplar factory proxy.");
		}
		else
		{
			logger.WriteLine(
				LogLevel::Error,
				"Failed to register the exemplar factory proxy.");
		}
	}

	bool OnStart(cIGZCOM* pCOM)
	{
		RegisterResourceFactoryProxies();

		mpFrameWork->AddHook(this);

		return true;
	}

	bool PostAppInit()
	{
		bool exemplarPatchDebugLoggingEnabled = false;

		cIGZCmdLine* pCmdLine = mpFrameWork->CommandLine();

		if (pCmdLine)
		{
			exemplarPatchDebugLoggingEnabled = pCmdLine->IsSwitchPresent(cRZBaseString("exemplar-patch-debug-logging"));
			exemplarLoadLogger.Init(*pCmdLine, mpCOM);
		}

		cRZAutoRefCount<IExemplarResourceFactoryProxy> pProxy;

		if (QueryForExemplarResourceFactoryProxyInterface(GZIID_IExemplarResourceFactoryProxy, pProxy.AsPPVoid()))
		{
			pProxy->InitializeExemplarPatchData(exemplarPatchDebugLoggingEnabled);
		}
		else
		{
			Logger& logger = Logger::GetInstance();
			logger.WriteLine(
				LogLevel::Error,
				"Failed to initialize the exemplar patch system.");
		}

		return true;
	}

private:

	ExemplarLoadLogger exemplarLoadLogger;
};

cRZCOMDllDirector* RZGetCOMDllDirector() {
	static ResourceLoadingHooksDllDirector sDirector;
	return &sDirector;
}