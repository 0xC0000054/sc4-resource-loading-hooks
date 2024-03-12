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

// This class is a GZCOM DLL that logs the TGI of every exemplar the game loads
// to the debugger console.
// It is a simplified version of the code in ExemplarLoadLoggingDllDirector.cpp


#include "cIGZCOM.h"
#include "cIGZFrameWork.h"
#include "cIGZVariant.h"
#include "cISCProperty.h"
#include "cISCPropertyHolder.h"
#include "cRZAutoRefCount.h"
#include "cRZCOMDllDirector.h"
#include "SCPropertyUtil.h"

#include "cIExemplarLoadHookServer.h"
#include "cIExemplarLoadHookTarget.h"

#include <Windows.h>

namespace
{
	void PrintLineToDebugOutput(const char* const line)
	{
		OutputDebugStringA(line);
		OutputDebugStringA("\n");
	}
}

// This must be unique for every plugin. Generate a random 32-bit integer and use it.
// DO NOT REUSE DIRECTOR IDS EVER.
static const uint32_t kLogExemplarTGIDirectorDirectorID = 0x79EB066F;

class LogExemplarTGIDllDirector : public cRZCOMDllDirector, cIExemplarLoadHookTarget
{
	LogExemplarTGIDllDirector()
	{
	}

	uint32_t GetDirectorID() const
	{
		return kLogExemplarTGIDirectorDirectorID;
	}

	bool QueryInterface(uint32_t riid, void** ppvObj)
	{
		if (riid == GZIID_cIExemplarLoadHookTarget)
		{
			*ppvObj = static_cast<cIExemplarLoadHookTarget*>(this);
			AddRef();

			return true;
		}

		return cRZCOMDllDirector::QueryInterface(riid, ppvObj);
	}

	uint32_t AddRef()
	{
		return cRZCOMDllDirector::AddRef();
	}

	uint32_t Release()
	{
		return cRZCOMDllDirector::Release();
	}

	void ExemplarLoaded(
		const char* const originalFunctionName,
		const cGZPersistResourceKey& key,
		cISCResExemplar* resExemplar)
	{
		constexpr uint32_t ExemplarTypePropertyID = 0x00000010;

		char buffer[1024]{};

		// Get a reference to the exemplar property holder.

		cISCPropertyHolder* properties = resExemplar->AsISCPropertyHolder();

		// If the exemplar has an ExemplarType property, its value will be
		// logged to the debugger console.

		uint32_t exemplarType = 0;

		if (properties && SCPropertyUtil::GetPropertyValue(properties, ExemplarTypePropertyID, exemplarType))
		{
			std::snprintf(
				buffer,
				sizeof(buffer),
				"T=0x%08X, G=0x%08X, I=0x%08X, ExemplarType=0x%X",
				key.type,
				key.group,
				key.instance,
				exemplarType);
		}
		else
		{
			std::snprintf(
				buffer,
				sizeof(buffer),
				"T=0x%08X, G=0x%08X, I=0x%08X",
				key.type,
				key.group,
				key.instance);
		}

		PrintLineToDebugOutput(buffer);
	}

	bool OnStart(cIGZCOM* pCOM)
	{
		cIGZFrameWork* const pFramework = pCOM->FrameWork();

		if (pFramework->GetState() < cIGZFrameWork::kStatePreAppInit)
		{
			pFramework->AddHook(this);
		}
		else
		{
			PreAppInit();
		}

		return true;
	}

	bool PreAppInit()
	{
		cIGZCOM* const pCOM = GZCOM();

		cRZAutoRefCount<cIExemplarLoadHookServer> exemplarHookServer;

		// Get an instance of the cIExemplarLoadHookServer class
		// from GZCOM.
		if (pCOM->GetClassObject(
			GZCLSID_cIExemplarLoadHookServer,
			GZIID_cIExemplarLoadHookServer,
			exemplarHookServer.AsPPVoid()))
		{
			// Register the class for notifications when SC4 loads an exemplar.
			//
			// This overload of AddLoadNotification will call our callback for every
			// exemplar that the game successfully loads.
			// AddLoadNotification also has other overloads that allow the notifications
			// to be filtered to a group ID or group and instance IDs.
			// The latter would be used if you are only interested in a specific exemplar.
			exemplarHookServer->AddLoadNotification(this);
		}

		return true;
	}
};