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

