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
#include "ResourceFactoryProxy.h"
#include "cIExemplarLoadHookServer.h"
#include "cIExemplarPatchingServer.h"
#include "ExemplarPatcher.h"
#include "IExemplarResourceFactoryProxy.h"
#include <unordered_map>
#include <unordered_set>

static constexpr uint32_t GZCLSID_ExemplarFactoryProxy = 0xEDA309D9;
static constexpr uint32_t ExemplarTypeID = 0x6534284A;

class ExemplarResourceFactoryProxy final :
	public ResourceFactoryProxy,
	private cIExemplarLoadHookServer,
	private cIExemplarPatchingServer,
	private IExemplarResourceFactoryProxy
{
public:

	ExemplarResourceFactoryProxy();

	// cIGZUnknown

	bool QueryInterface(uint32_t riid, void** ppvObj) override;
	uint32_t AddRef() override;
	uint32_t Release() override;

	// cIExemplarLoadHookServer

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

	// IExemplarResourceFactoryProxy

	void InitializeExemplarPatchData(bool debugLoggingEnabled) override;

	// cIExemplarPatchingServer

	void ScanForExemplarPatches();

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
	ExemplarPatcher exemplarPatcher;
};

