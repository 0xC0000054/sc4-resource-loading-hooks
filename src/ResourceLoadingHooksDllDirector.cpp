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

#include "version.h"
#include "FileSystem.h"
#include "Logger.h"
#include "ExemplarLoadLoggingDllDirector.h"
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

static bool CreateExemplarLoadHookServer(uint32_t riid, void** ppvObj)
{
	bool result = false;

	if (riid == GZIID_cIExemplarLoadHookServer)
	{
		// The load hook server interface is implemented by the resource factory proxy.
		// Grab a reference to the resource factory proxy instance that the game's
		// resource manager created and call QueryInterface on it.

		cIGZPersistResourceManagerPtr pResMan;

		if (pResMan)
		{
			cRZAutoRefCount<cIGZPersistResourceFactory> pFactory;

			if (pResMan->FindObjectFactory(ExemplarTypeID, pFactory.AsPPObj()))
			{
				result = pFactory->QueryInterface(riid, ppvObj);
			}
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
		AddCls(GZCLSID_cIExemplarLoadHookServer, CreateExemplarLoadHookServer);

		// A child director that is used to test the services provided by
		// this director.
		AddDirector(&exemplarLoadLoggingDirector);

		std::filesystem::path dllFolderPath = FileSystem::GetDllFolderPath();

		std::filesystem::path logFilePath = dllFolderPath;
		logFilePath /= PluginLogFileName;

		Logger& logger = Logger::GetInstance();
		logger.Init(logFilePath, LogLevel::Info, false);
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

		// The resource factory proxies must be registered before starting the
		// child DLL directors because they will add listeners in OnStart.
		exemplarLoadLoggingDirector.OnStart(pCOM);

		return true;
	}

private:

	ExemplarLoadLoggingDllDirector exemplarLoadLoggingDirector;
};

cRZCOMDllDirector* RZGetCOMDllDirector() {
	static ResourceLoadingHooksDllDirector sDirector;
	return &sDirector;
}