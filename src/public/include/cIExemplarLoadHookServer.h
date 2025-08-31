/*
* The public header for the sc4-resource-loading-hooks
* cIExemplarLoadHookServer interface.
* This file is licensed under terms of the MIT License.
*
* Copyright (c) 2024, 2025 Nicholas Hayes
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#pragma once
#include "cIGZUnknown.h"

class cIExemplarLoadHookTarget;
class cIExemplarLoadErrorHookTarget;

static const uint32_t GZCLSID_cIExemplarLoadHookServer = 0xFD7BD8C4;
static const uint32_t GZIID_cIExemplarLoadHookServer = 0xFC1A55E1;

/**
 * @brief The class used to register for the exemplar load callbacks.
 */
class cIExemplarLoadHookServer : public cIGZUnknown
{
public:

	/**
	 * @brief Registers to receive a callback on all successful exemplar loads.
	 * @param target The object that implements cIExemplarLoadHookTarget.
	 * @return true if successful; otherwise, false.
	 */
	virtual bool AddLoadNotification(cIExemplarLoadHookTarget* target) = 0;

	/**
	 * @brief Registers to receive a callback on all successful exemplar loads
	 * with the specified group ID.
	 * @param target The object that implements cIExemplarLoadHookTarget.
	 * @param requiredGroupID The group ID to watch for.
	 * @return true if successful; otherwise, false.
	 */
	virtual bool AddLoadNotification(
		cIExemplarLoadHookTarget* target,
		uint32_t requiredGroupID) = 0;

	/**
	 * @brief Registers to receive a callback on successful exemplar loads
	 * with the specified group and instance IDs.
	 * @param target The object that implements cIExemplarLoadHookTarget.
	 * @param requiredGroupID The group ID to watch for.
	 * @param requiredInstanceID The instance ID to watch for.
	 * @return true if successful; otherwise, false.
	 */
	virtual bool AddLoadNotification(
		cIExemplarLoadHookTarget* target,
		uint32_t requiredGroupID,
		uint32_t requiredInstanceID) = 0;

	/**
	 * @brief Unregisters an object from the exemplar load callbacks.
	 * @param target The object that implements cIExemplarLoadHookTarget.
	 * @return true if successful; otherwise, false.
	 */
	virtual bool RemoveLoadNotification(cIExemplarLoadHookTarget* target) = 0;

	/**
	 * @brief Registers an object for the exemplar load error callbacks.
	 * @param target The object that implements cIExemplarLoadErrorHookTarget.
	 * @return true if successful; otherwise, false.
	 */
	virtual bool AddLoadErrorNotification(cIExemplarLoadErrorHookTarget* target) = 0;

	/**
	 * @brief Unregisters an object from the exemplar load error callbacks.
	 * @param target The object that implements cIExemplarLoadErrorHookTarget.
	 * @return true if successful; otherwise, false.
	 */
	virtual bool RemoveLoadErrorNotification(cIExemplarLoadErrorHookTarget* target) = 0;
};