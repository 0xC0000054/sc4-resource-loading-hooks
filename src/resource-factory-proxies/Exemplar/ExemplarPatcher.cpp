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

#include "ExemplarPatcher.h"
#include "cIGZPersistDBSegment.h"
#include "cIGZPersistDBSegmentMultiPackedFiles.h"
#include "cIGZPersistResource.h"
#include "cIGZPersistResourceManager.h"
#include "cIGZPersistResourceKeyList.h"
#include "cIGZVariant.h"
#include "cISCProperty.h"
#include "cISCResExemplarCohort.h"
#include "GZServPtrs.h"
#include "Logger.h"
#include "PersistResourceKeyFilterByTypeAndGroup.h"

namespace
{
	static constexpr uint32_t kCohortTypeId = 0x05342861;
	static constexpr uint32_t kExemplarPatchGroupId = 0xb03697d1;  // GID of Cohort files
	static constexpr uint32_t kExemplarTypeId = 0x6534284a;
	static constexpr uint32_t kExemplarNamePropertyId = 0x20;
	static constexpr uint32_t kExemplarPatchTargetPropertyId = 0x0062e78a;

	struct ApplyPatchContext
	{
		cISCPropertyHolder* pTarget;
		bool logPatchedProperties;
		bool writtenExemplarPatchHeader;

		ApplyPatchContext(cISCPropertyHolder* pTarget, bool logPatchedProperties)
			: pTarget(pTarget),
			  logPatchedProperties(logPatchedProperties),
			  writtenExemplarPatchHeader(false)
		{
		}
	};

	void ApplyPatchCallback(cISCProperty* pProperty, void* pContext)
	{
		uint32_t id = pProperty->GetPropertyID();

		if (id != kExemplarPatchTargetPropertyId && id != kExemplarNamePropertyId)
		{
			auto context = static_cast<ApplyPatchContext*>(pContext);

			context->pTarget->AddProperty(pProperty, /*bSendMsg*/ false);  // `true` results in a crash

			if (context->logPatchedProperties)
			{
				Logger& logger = Logger::GetInstance();

				// The log lines are indented because ExemplarPatcher::ApplyPatches
				// writes the patched exemplar TGI before this function gets called.

				if (context->writtenExemplarPatchHeader)
				{
					// The source exemplar patch info was written, so add an extra indent level.
					logger.WriteLineFormatted(
						LogLevel::Info,
						"    Patched property 0x%08X",
						id);
				}
				else
				{
					logger.WriteLineFormatted(
						LogLevel::Info,
						"  Patched property 0x%08X",
						id);
				}
			}
		}
	}

	struct ExemplarPatchScanContext
	{
		cIGZPersistResourceManager* pResMan;
		ExemplarPatcher::PatchContainer patches;

		ExemplarPatchScanContext(cIGZPersistResourceManager* pResMan)
			: pResMan(pResMan), patches()
		{
		}
	};

	void ExemplarPatchScanCallback(cGZPersistResourceKey const& key, void* pContext)
	{
		auto context = static_cast<ExemplarPatchScanContext*>(pContext);

		cRZAutoRefCount<cISCResExemplarCohort> cohort;

		if (context->pResMan->GetResource(key, GZIID_cISCResExemplarCohort, cohort.AsPPVoid(), 0, nullptr))
		{
			cISCPropertyHolder* propHolder = cohort->AsISCPropertyHolder();

			auto property = propHolder->GetProperty(kExemplarPatchTargetPropertyId);

			if (property)
			{
				const cIGZVariant* variant = property->GetPropertyValue();
				const uint32_t reps = variant->GetCount();

				if (variant->GetType() != cIGZVariant::Type::Uint32Array)
				{
					Logger& logger = Logger::GetInstance();
					logger.WriteLineFormatted(LogLevel::Error,
						"Exemplar Patch Target property requires type Uint32Array: T:0x%08X G:0x%08X I:0x%08X.",
						key.type, key.group, key.instance);
				}
				else if (reps > 0)
				{
					if ((reps % 2) != 0)
					{
						Logger& logger = Logger::GetInstance();
						logger.WriteLineFormatted(LogLevel::Error,
							"Exemplar Patch Target property requires even number of values: T:0x%08X G:0x%08X I:0x%08X.",
							key.type, key.group, key.instance);
					}
					else
					{
						auto& patches = context->patches;
						const uint32_t* values = variant->RefUint32();

						for (uint32_t i = 1; i < reps; i += 2)
						{
							const cGZPersistResourceKey targetTgi(kExemplarTypeId, values[i - 1], values[i]);

							auto item = patches.find(targetTgi);

							if (item != patches.end())
							{
								cRZAutoRefCount<cISCPropertyHolder> containerPropertyHolder(
									propHolder,
									cRZAutoRefCount<cISCPropertyHolder>::kAddRef);

								item->second.push_back(std::move(containerPropertyHolder));
							}
							else
							{
								cRZAutoRefCount<cISCPropertyHolder> containerPropertyHolder(
									propHolder,
									cRZAutoRefCount<cISCPropertyHolder>::kAddRef);

								boost::container::deque<cRZAutoRefCount<cISCPropertyHolder>> queue;
								queue.push_back(std::move(containerPropertyHolder));

								patches.emplace(targetTgi, std::move(queue));
							}
						}
					}
				}
			}
		}
	}

	bool GetResourceFilePath(const cGZPersistResourceKey& key, cIGZString& path)
	{
		bool result = false;

		cIGZPersistResourceManagerPtr pResMan;

		if (pResMan)
		{
			cRZAutoRefCount<cIGZPersistDBSegment> pSegment;

			if (pResMan->FindDBSegment(key, pSegment.AsPPObj()))
			{
				cRZAutoRefCount<cIGZPersistDBSegmentMultiPackedFiles> pMultiPackedFile;

				if (pSegment->QueryInterface(GZIID_cIGZPersistDBSegmentMultiPackedFiles, pMultiPackedFile.AsPPVoid()))
				{
					// cIGZPersistDBSegmentMultiPackedFiles is a collection of DAT files in a specific folder
					// and its sub-folders.
					// Call its FindDBSegment method to get the actual file.

					cRZAutoRefCount<cIGZPersistDBSegment> pMultiPackedSegment;

					if (pMultiPackedFile->FindDBSegment(key, pMultiPackedSegment.AsPPObj()))
					{
						pMultiPackedSegment->GetPath(path);
						result = true;
					}
				}
				else
				{
					pSegment->GetPath(path);
					result = true;
				}
			}
		}

		return result;
	}

	bool GetResourceKeyAndNameFromExemplarPatch(
		cISCPropertyHolder* pExemplarPatch,
		cGZPersistResourceKey& key,
		cIGZString& filePath)
	{
		bool result = false;

		if (pExemplarPatch)
		{
			cRZAutoRefCount<cIGZPersistResource> pPersistResource;

			if (pExemplarPatch->QueryInterface(GZIID_cIGZPersistResource, pPersistResource.AsPPVoid()))
			{
				pPersistResource->GetKey(key);

				result = GetResourceFilePath(key, filePath);
			}
		}

		return result;
	}
}

ExemplarPatcher::ExemplarPatcher() : debugLoggingEnabled(false)
{
}

void ExemplarPatcher::LoadExemplarPatches()
{
	auto guard = sync.lock();

	cIGZPersistResourceManagerPtr pResMan;

	if (pResMan)
	{
		cRZAutoRefCount<cIGZPersistResourceKeyList> pResourceList;

		PersistResourceKeyFilterByTypeAndGroup* filter = new PersistResourceKeyFilterByTypeAndGroup(kCohortTypeId, kExemplarPatchGroupId);
		filter->AddRef();
		uint32_t res = pResMan->GetAvailableResourceList(pResourceList.AsPPObj(), filter);
		filter->Release();

		if (pResourceList->Size() > 0)
		{
			ExemplarPatchScanContext context(pResMan);

			pResourceList->EnumKeys(ExemplarPatchScanCallback, &context);

			patches.clear();
			patches = std::move(context.patches);
		}

		Logger& logger = Logger::GetInstance();
		logger.WriteLineFormatted(LogLevel::Info,
			"Loaded %u Exemplar patches targeting, in total, %u Exemplar files.",
			pResourceList->Size(),
			patches.size());

		if (debugLoggingEnabled)
		{
			pResourceList->EnumKeys([](auto key, void* pContext)
				{
					cRZBaseString path;

					if (GetResourceFilePath(key, path))
					{
						Logger& logger = Logger::GetInstance();
						logger.WriteLineFormatted(
							LogLevel::Info,
							"Exemplar patch T=0x%08X, G=0x%08X, I=0x%08X loaded from %s",
							key.type,
							key.group,
							key.instance,
							path.ToChar());
					}
				}, nullptr);
		}
	}
}

void ExemplarPatcher::ApplyPatches(const cGZPersistResourceKey& key, cISCResExemplar* pExemplar)
{
	auto guard = sync.lock();

	const auto item = patches.find(key);

	if (item != patches.end())
	{
		if (debugLoggingEnabled)
		{
			Logger& logger = Logger::GetInstance();

			cRZBaseString path;

			if (GetResourceFilePath(key, path))
			{
				logger.WriteLineFormatted(
					LogLevel::Info,
					"Patching T=0x%08X, G=0x%08X, I=0x%08X in %s",
					key.type,
					key.group,
					key.instance,
					path.ToChar());
			}
			else
			{
				logger.WriteLineFormatted(
					LogLevel::Info,
					"Patching T=0x%08X, G=0x%08X, I=0x%08X",
					key.type,
					key.group,
					key.instance);
			}
		}

		ApplyPatchContext context(pExemplar->AsISCPropertyHolder(), debugLoggingEnabled);

		for (auto& patch : item->second)
		{
			if (debugLoggingEnabled)
			{
				cGZPersistResourceKey key;
				cRZBaseString path;

				if (GetResourceKeyAndNameFromExemplarPatch(
					patch,
					key,
					path))
				{
					Logger& logger = Logger::GetInstance();
					logger.WriteLineFormatted(
						LogLevel::Info,
						"  Applying exemplar patch T=0x%08X, G=0x%08X, I=0x%08X from %s",
						key.type,
						key.group,
						key.instance,
						path.ToChar());

					context.writtenExemplarPatchHeader = true;
				}
				else
				{
					context.writtenExemplarPatchHeader = false;
				}
			}

			patch->EnumProperties(ApplyPatchCallback, &context);
		}
	}
}

void ExemplarPatcher::SetDebugLoggingEnabled(bool enabled)
{
	debugLoggingEnabled = enabled;
}
