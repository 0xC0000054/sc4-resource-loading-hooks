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
	  // CreateOriginalFactory returns a pointer that has already had its reference count incremented.
	  // The cRZAutoRefCount<cIGZPersistResourceFactory>(cIGZPersistResourceFactory*) constructor
	  // takes ownership of the pointer without modifying its reference count.
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

	return cRZBaseUnknown::QueryInterface(riid, ppvObj);
}

uint32_t ResourceFactoryProxy::AddRef()
{
	return cRZBaseUnknown::AddRef();
}

uint32_t ResourceFactoryProxy::Release()
{
	return cRZBaseUnknown::Release();
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
