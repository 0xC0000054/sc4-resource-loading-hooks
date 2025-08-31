/*
* The public header for the sc4-resource-loading-hooks
* cIExemplarLoadHookTarget interface.
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
#include "cISCResExemplar.h"
#include <cstdint>

static const uint32_t GZIID_cIExemplarLoadHookTarget = 0x80590E91;

/**
 * @brief Defines the callback that is executed on a successful exemplar load.
 */
class cIExemplarLoadHookTarget : public cIGZUnknown
{
public:

	/**
	 * @brief The callback that is executed on a successful exemplar load.,
	 * @param originalFunctionName The name of the resource factory function that
	 *  loaded the exemplar, used for debugging.
	 * @param key The type, group, and instance (TGI) information for the exemplar.
	 * @param resExemplar The exemplar data.
	 */
	virtual void ExemplarLoaded(
		const char* const originalFunctionName,
		const cGZPersistResourceKey& key,
		cISCResExemplar* resExemplar) = 0;
};