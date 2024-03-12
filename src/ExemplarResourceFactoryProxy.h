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
#include "ResourceFactoryProxy.h"
#include "cIExemplarLoadHookServer.h"
#include <unordered_map>
#include <unordered_set>

static constexpr uint32_t GZCLSID_ExemplarFactoryProxy = 0xEDA309D9;
static constexpr uint32_t ExemplarTypeID = 0x6534284A;

class ExemplarResourceFactoryProxy final :
	public ResourceFactoryProxy,
	private cIExemplarLoadHookServer
{
public:

	ExemplarResourceFactoryProxy();

	bool QueryInterface(uint32_t riid, void** ppvObj) override;
	uint32_t AddRef() override;
	uint32_t Release() override;

	bool AddLoadNotification(cIExemplarLoadHookTarget* target) override;
	bool AddLoadNotification(
		cIExemplarLoadHookTarget* target,
		uint32_t requiredGroupID) override;
	bool AddLoadNotification(cIExemplarLoadHookTarget* target,
		uint32_t requiredGroupID,
		uint32_t requiredInstanceID) override;
	bool RemoveLoadNotification(cIExemplarLoadHookTarget* target) override;

	bool AddLoadErrorNotification(cIExemplarLoadErrorHookTarget* target) override;
	bool RemoveLoadErrorNotification(cIExemplarLoadErrorHookTarget* target) override;

private:

	void ResourceLoaded(
		const char* const originalFunctionName,
		uint32_t riid,
		void** ppvObj) override;

	void ResourceLoadError(
		const char* const originalFunctionName,
		uint32_t riid) override;

	void ResourceLoadError(
		const char* const originalFunctionName,
		uint32_t riid,
		const cGZPersistResourceKey& key) override;

	class ExemplarTGIFilter
	{
	public:

		ExemplarTGIFilter(uint32_t requestedGroupID, uint32_t requestedInstanceID);

		bool IsIncluded(const cGZPersistResourceKey& key) const;

	private:

		uint32_t groupID;
		uint32_t instanceID;
	};

	std::unordered_map<cIExemplarLoadHookTarget*, ExemplarTGIFilter> exemplarLoadTargets;
	std::unordered_set<cIExemplarLoadErrorHookTarget*> exemplarLoadErrorTargets;
};

