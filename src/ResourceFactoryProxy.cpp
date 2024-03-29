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

#include "ResourceFactoryProxy.h"
#include "Logger.h"
#include "cGZPersistResourceKey.h"
#include "cIGZCOM.h"
#include "cIGZFrameWork.h"
#include "cRZCOMDllDirector.h"
#include "cIGZPersistDBRecord.h"

namespace
{
	cIGZPersistResourceFactory* CreateOriginalFactory(uint32_t originalFactoryCLSID)
	{
		cIGZFrameWork* const pFramework = RZGetFramework();

		cIGZCOM* const pCOM = pFramework->GetCOMObject();

		cIGZPersistResourceFactory* pFactory = nullptr;

		if (!pCOM->GetClassObject(
			originalFactoryCLSID,
			GZIID_cIGZPersistResourceFactory,
			reinterpret_cast<void**>(&pFactory)))
		{
			throw std::runtime_error("Failed to create the original resource factory.");
		}

		return pFactory;
	}
}

ResourceFactoryProxy::ResourceFactoryProxy(uint32_t originalFactoryCLSID, uint32_t resourceTypeID)
	: resourceTypeID(resourceTypeID),
	  refCount(0),
	  originalFactory(CreateOriginalFactory(originalFactoryCLSID))
{
}

ResourceFactoryProxy::~ResourceFactoryProxy()
{
}

bool ResourceFactoryProxy::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZIID_cIGZPersistResourceFactory)
	{
		*ppvObj = static_cast<cIGZPersistResourceFactory*>(this);
		AddRef();

		return true;
	}
	else if (riid == GZIID_cIGZUnknown)
	{
		*ppvObj = static_cast<cIGZUnknown*>(static_cast<cIGZPersistResourceFactory*>(this));
		AddRef();

		return true;
	}

	*ppvObj = nullptr;
	return false;
}

uint32_t ResourceFactoryProxy::AddRef()
{
	originalFactory->AddRef();

	++refCount;

	originalFactory->Release();

	return refCount;
}

uint32_t ResourceFactoryProxy::Release()
{
	uint32_t localRefCount = 0;

	originalFactory->AddRef();

	if (refCount == 1)
	{
		originalFactory->Release();

		delete this;
		localRefCount = 0;
	}
	else
	{
		localRefCount = refCount - 1;
		refCount = localRefCount;

		originalFactory->Release();
	}

	return localRefCount;
}

bool ResourceFactoryProxy::CreateInstance(
	uint32_t type,
	uint32_t riid,
	void** ppvObj,
	uint32_t unknown1,
	cIGZUnknown* unknown2)
{
	if (type == resourceTypeID)
	{
		bool result = originalFactory->CreateInstance(type, riid, ppvObj, unknown1, unknown2);

		if (result)
		{
			ResourceLoaded(__FUNCSIG__, riid, ppvObj);
		}
		else
		{
			ResourceLoadError(__FUNCSIG__, riid);
		}

		return result;
	}

	*ppvObj = nullptr;
	return false;
}

bool ResourceFactoryProxy::CreateInstance(
	cIGZPersistDBRecord& record,
	uint32_t riid,
	void** ppvObj,
	uint32_t unknown1,
	cIGZUnknown* unknown2)
{
	cGZPersistResourceKey key;
	record.GetKey(key);

	if (key.type == resourceTypeID)
	{
		bool result = originalFactory->CreateInstance(record, riid, ppvObj, unknown1, unknown2);

		if (result)
		{
			ResourceLoaded(__FUNCSIG__, riid, ppvObj);
		}
		else
		{
			cGZPersistResourceKey key;
			record.GetKey(key);

			ResourceLoadError(__FUNCSIG__, riid, key);
		}

		return result;
	}

	*ppvObj = nullptr;
	return false;
}

bool ResourceFactoryProxy::Read(cIGZPersistResource& resource, cIGZPersistDBRecord& record)
{
	bool result = originalFactory->Read(resource, record);

	Logger::GetInstance().WriteLineFormatted(
		LogLevel::Debug,
		__FUNCSIG__ ": result=%s",
		result ? "true" : "false");

	return result;
}

bool ResourceFactoryProxy::Write(cIGZPersistResource const& resource, cIGZPersistDBRecord& record)
{
	bool result = originalFactory->Write(resource, record);

	Logger::GetInstance().WriteLineFormatted(
		LogLevel::Debug,
		__FUNCSIG__ ": result=%s",
		result ? "true" : "false");

	return result;
}
