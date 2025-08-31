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

#include "ExemplarResourceFactoryProxy.h"
#include "cIExemplarLoadHookTarget.h"
#include "cIExemplarLoadErrorHookTarget.h"
#include "cIGZPersistResource.h"
#include "cISCResExemplar.h"
#include "cRZAutoRefCount.h"

static constexpr uint32_t GZCLSID_SCResExemplarFactory = 0x453429B3;

ExemplarResourceFactoryProxy::ExemplarResourceFactoryProxy()
	: ResourceFactoryProxy(GZCLSID_SCResExemplarFactory, ExemplarTypeID)
{
}

bool ExemplarResourceFactoryProxy::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZIID_cIExemplarLoadHookServer)
	{
		*ppvObj = static_cast<cIExemplarLoadHookServer*>(this);
		ResourceFactoryProxy::AddRef();

		return true;
	}

	return ResourceFactoryProxy::QueryInterface(riid, ppvObj);
}

uint32_t ExemplarResourceFactoryProxy::AddRef()
{
	return ResourceFactoryProxy::AddRef();
}

uint32_t ExemplarResourceFactoryProxy::Release()
{
	return ResourceFactoryProxy::Release();
}

bool ExemplarResourceFactoryProxy::AddLoadNotification(cIExemplarLoadHookTarget* target)
{
	// A group/instance ID of 0 is treated as including every value for that item.
	return AddLoadNotification(target, 0, 0);
}

bool ExemplarResourceFactoryProxy::AddLoadNotification(
	cIExemplarLoadHookTarget* target,
	uint32_t requestedGroupID)
{
	// A group/instance ID of 0 is treated as including every value for that item.
	return AddLoadNotification(target, requestedGroupID, 0);
}

bool ExemplarResourceFactoryProxy::AddLoadNotification(
	cIExemplarLoadHookTarget* target,
	uint32_t requestedGroupID,
	uint32_t requestedInstanceID)
{
	bool result = false;

	if (target)
	{
		result = exemplarLoadTargets.emplace(target, ExemplarTGIFilter(requestedGroupID, requestedInstanceID)).second;
	}

	return result;
}

bool ExemplarResourceFactoryProxy::RemoveLoadNotification(cIExemplarLoadHookTarget* target)
{
	bool result = false;

	if (target)
	{
		result = exemplarLoadTargets.erase(target) == 1;
	}

	return result;
}

bool ExemplarResourceFactoryProxy::AddLoadErrorNotification(cIExemplarLoadErrorHookTarget* target)
{
	bool result = false;

	if (target)
	{
		result = exemplarLoadErrorTargets.emplace(target).second;
	}

	return result;
}

bool ExemplarResourceFactoryProxy::RemoveLoadErrorNotification(cIExemplarLoadErrorHookTarget* target)
{
	bool result = false;

	if (target)
	{
		result = exemplarLoadErrorTargets.erase(target) == 1;
	}

	return result;
}

void ExemplarResourceFactoryProxy::ResourceLoaded(
	const char* const originalFunctionName,
	uint32_t riid,
	void** ppvObj)
{
	if (exemplarLoadTargets.size() > 0
		&& riid == GZIID_cIGZPersistResource
		&& *ppvObj)
	{
		cIGZPersistResource* pRes = static_cast<cIGZPersistResource*>(*ppvObj);
		cGZPersistResourceKey key;
		pRes->GetKey(key);

		cRZAutoRefCount<cISCResExemplar> resExemplar;

		if (pRes->QueryInterface(GZIID_cISCResExemplar, resExemplar.AsPPVoid()))
		{
			for (const auto& item : exemplarLoadTargets)
			{
				cIExemplarLoadHookTarget* const temp = item.first;
				const ExemplarTGIFilter& filter = item.second;

				if (temp && filter.IsIncluded(key))
				{
					temp->ExemplarLoaded(originalFunctionName, key, resExemplar);
				}
			}
		}
	}
}

void ExemplarResourceFactoryProxy::ResourceLoadError(
	const char* const originalFunctionName,
	uint32_t riid)
{
	if (exemplarLoadErrorTargets.size() > 0)
	{
		for (cIExemplarLoadErrorHookTarget* pTarget : exemplarLoadErrorTargets)
		{
			cIExemplarLoadErrorHookTarget* const temp = pTarget;

			if (temp)
			{
				temp->LoadError(originalFunctionName, riid);
			}
		}
	}
}

void ExemplarResourceFactoryProxy::ResourceLoadError(
	const char* const originalFunctionName,
	uint32_t riid,
	const cGZPersistResourceKey& key)
{
	if (exemplarLoadErrorTargets.size() > 0)
	{
		for (cIExemplarLoadErrorHookTarget* pTarget : exemplarLoadErrorTargets)
		{
			cIExemplarLoadErrorHookTarget* const temp = pTarget;

			if (temp)
			{
				temp->LoadError(originalFunctionName, riid, key);
			}
		}
	}
}

ExemplarResourceFactoryProxy::ExemplarTGIFilter::ExemplarTGIFilter(
	uint32_t requestedGroupID,
	uint32_t requestedInstanceID)
	: groupID(requestedGroupID),
	  instanceID(requestedInstanceID)
{
}

bool ExemplarResourceFactoryProxy::ExemplarTGIFilter::IsIncluded(const cGZPersistResourceKey& key) const
{
	bool result = true;

	// A group/instance ID of 0 is treated as including every value for that item.
	if (groupID != 0)
	{
		if (instanceID != 0)
		{
			result = key.group == groupID && key.instance == instanceID;
		}
		else
		{
			result = key.group == groupID;
		}
	}

	return result;
}
