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

#pragma once
#include "cIGZPersistResourceFactory.h"
#include "cGZPersistResourceKey.h"
#include "cRZAutoRefCount.h"

class ResourceFactoryProxy : public cIGZPersistResourceFactory
{
public:

	virtual ~ResourceFactoryProxy();

protected:

	ResourceFactoryProxy(uint32_t originalFactoryCLSID, uint32_t resourceTypeID);

	virtual void ResourceLoaded(
		const char* const originalFunctionName,
		uint32_t riid,
		void** ppvObj) = 0;

	virtual void ResourceLoadError(
		const char* const originalFunctionName,
		uint32_t riid) = 0;

	virtual void ResourceLoadError(
		const char* const originalFunctionName,
		uint32_t riid,
		const cGZPersistResourceKey& key) = 0;


	bool QueryInterface(uint32_t riid, void** ppvObj) override;
	uint32_t AddRef() override;
	uint32_t Release() override;

private:

	bool CreateInstance(
		uint32_t type,
		uint32_t riid,
		void** ppvObj,
		uint32_t unknown1,
		cIGZUnknown* unknown2) override;
	bool CreateInstance(
		cIGZPersistDBRecord& record,
		uint32_t riid,
		void** ppvObj,
		uint32_t unknown1,
		cIGZUnknown* unknown2) override;

	bool Read(cIGZPersistResource& resource, cIGZPersistDBRecord& record) override;
	bool Write(cIGZPersistResource const& resource, cIGZPersistDBRecord& record) override;

	uint32_t resourceTypeID;
	uint32_t refCount;
	cRZAutoRefCount<cIGZPersistResourceFactory> originalFactory;
};

