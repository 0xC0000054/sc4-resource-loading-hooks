/*
* The public header for the sc4-resource-loading-hooks
* cIExemplarLoadErrorHookTarget interface.
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
#include "cGZPersistResourceKey.h"
#include "cIGZUnknown.h"

static const uint32_t GZIID_cIExemplarLoadErrorHookTarget = 0x12C54BC2;

/**
 * @brief Defines callbacks that are executed on a failed exemplar load.
 */
class cIExemplarLoadErrorHookTarget : public cIGZUnknown
{
public:

	/**
	 * @brief A callback that is executed on a failed exemplar load.,
	 * @param originalFunctionName The name of the resource factory function that
	 *  loaded the exemplar, used for debugging.
	 * @param riid The requested interface ID for the type.
	 */
	virtual void LoadError(
		const char* const originalFunctionName,
		uint32_t riid) = 0;

	/**
	 * @brief A callback that is executed on a failed exemplar load.,
	 * @param originalFunctionName The name of the resource factory function that
	 *  loaded the exemplar, used for debugging.
	 * @param riid The requested interface ID for the type.
	 * @param key The type, group, and instance (TGI) information for the exemplar.
	 */
	virtual void LoadError(
		const char* const originalFunctionName,
		uint32_t riid,
		const cGZPersistResourceKey& key) = 0;
};
