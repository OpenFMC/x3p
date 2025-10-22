/***************************************************************************
 *   Copyright by Johannes Herwig (NanoFocus AG) 2007                      *
 *   Copyright by Georg Wiora (NanoFocus AG) 2007                          *
 *                                                                         *
 *   This file is part of the openGPS (R)[TM] software library.            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License (LGPL)    *
 *   as published by the Free Software Foundation; either version 3 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the files "licence_LGPL-3.0.txt" and                   *
 *   "licence_GPL-3.0.txt".                                                *
 *                                                                         *
 *   openGPS is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Lesser General Public License for more details.                   *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 *   The name "openGPS" and the logo are registered as                     *
 *   European trade mark No. 006178354 for                                 *
 *   Physikalisch Technische Bundesanstalt (PTB)                           *
 *   http://www.ptb.de/                                                    *
 *                                                                         *
 *   More information about openGPS can be found at                        *
 *   http://www.opengps.eu/                                                *
 ***************************************************************************/

#include <opengps/iso5436_2.h>
#include <opengps/cxx/iso5436_2.hxx>
#include <opengps/cxx/iso5436_2_handle.hxx>

#include "iso5436_2_handle_c.hxx"
#include "point_iterator_c.hxx"
#include "point_vector_c.hxx"

#include "messages_c.hxx"

#include "../cxx/iso5436_2_container.hxx"
#include "../cxx/stdafx.hxx"

OGPS_ISO5436_2Handle ogps_OpenISO5436_2(
	const OGPS_Character* file,
	const OGPS_Character* temp)
{
	assert(file);

	return HandleExceptionRetval(nullptr, [&]() {
		auto instance{ std::make_unique<ISO5436_2>(file, temp ? temp : _T("")) };
		instance->Open();
		OGPS_ISO5436_2Handle h{ new OGPS_ISO5436_2 };
		h->instance = std::move(instance);
		return h;
	});
}

OGPS_ISO5436_2Handle ogps_CreateMatrixISO5436_2(
	const OGPS_Character* file,
	const OGPS_Character* temp,
	const Schemas::ISO5436_2::Record1Type& record1,
	const Schemas::ISO5436_2::Record2Type* record2,
	const Schemas::ISO5436_2::MatrixDimensionType& matrixDimension,
	bool useBinaryData)
{
	assert(file);

	return HandleExceptionRetval(nullptr, [&]() {
		auto instance{ std::make_unique<ISO5436_2>(file, temp ? temp : _T("")) };
		instance->Create(record1, record2, matrixDimension, useBinaryData);

		OGPS_ISO5436_2Handle h{ new OGPS_ISO5436_2 };
		h->instance = std::move(instance);
		return h;
	});
}

OGPS_ISO5436_2Handle ogps_CreateListISO5436_2(
	const OGPS_Character* file,
	const OGPS_Character* temp,
	const Schemas::ISO5436_2::Record1Type& record1,
	const Schemas::ISO5436_2::Record2Type* record2,
	size_t listDimension,
	bool useBinaryData)
{
	assert(file);

	return HandleExceptionRetval(nullptr, [&]() {
		auto instance{ std::make_unique<ISO5436_2>(file, temp ? temp : _T("")) };
		instance->Create(record1, record2, listDimension, useBinaryData);

		OGPS_ISO5436_2Handle h{ new OGPS_ISO5436_2 };
		h->instance = std::move(instance);
		return h;
	});
}

Schemas::ISO5436_2::ISO5436_2Type* ogps_GetDocument(const OGPS_ISO5436_2Handle handle)
{
	assert(handle && handle->instance);

	return HandleExceptionRetval(nullptr, [&]() {
		return handle->instance->GetDocument();
	});
}

void ogps_WriteISO5436_2(const OGPS_ISO5436_2Handle handle, int compressionLevel)
{
	assert(handle && handle->instance);

	HandleException([&]() {
		handle->instance->Write(compressionLevel);
	});
}

void ogps_CloseISO5436_2(OGPS_ISO5436_2Handle* handle)
{
	if (handle)
	{
		HandleException([&]() {
			delete *handle;
		});
		*handle = nullptr;
	}
}

OGPS_PointIteratorPtr ogps_CreateNextPointIterator(const OGPS_ISO5436_2Handle handle)
{
	assert(handle && handle->instance);

	return HandleExceptionRetval(nullptr, [&]() {
		auto instance{ handle->instance->CreateNextPointIterator() };

		OGPS_PointIteratorPtr iter{ new OGPS_PointIterator() };
		iter->instance = std::move(instance);
		return iter;
	});
}

OGPS_PointIteratorPtr ogps_CreatePrevPointIterator(const OGPS_ISO5436_2Handle handle)
{
	assert(handle && handle->instance);

	return HandleExceptionRetval(nullptr, [&]() {
		auto instance{ handle->instance->CreatePrevPointIterator() };

		OGPS_PointIteratorPtr iter{ new OGPS_PointIterator() };
		iter->instance = std::move(instance);
		return iter;
	});
}

void ogps_SetMatrixPoint(
	const OGPS_ISO5436_2Handle handle,
	size_t u,
	size_t v,
	size_t w,
	const OGPS_PointVectorPtr vector)
{
	assert(handle && handle->instance);

	HandleException([&]() {
		handle->instance->SetMatrixPoint(u, v, w, vector ? &vector->instance : nullptr);
	});
}

void ogps_GetMatrixPoint(
	const OGPS_ISO5436_2Handle handle,
	size_t u,
	size_t v,
	size_t w,
	OGPS_PointVectorPtr vector)
{
	assert(handle && handle->instance && vector);

	HandleException([&]() {
		handle->instance->GetMatrixPoint(u, v, w, vector->instance);
	});
}

void ogps_SetListPoint(
	const OGPS_ISO5436_2Handle handle,
	size_t index,
	const OGPS_PointVectorPtr vector)
{
	assert(handle && handle->instance && vector);

	HandleException([&]() {
		handle->instance->SetListPoint(index, vector->instance);
	});
}

void ogps_GetListPoint(
	const OGPS_ISO5436_2Handle handle,
	size_t index,
	OGPS_PointVectorPtr vector)
{
	assert(handle && handle->instance && vector);

	HandleException([&]() {
		handle->instance->GetListPoint(index, vector->instance);
	});
}

void ogps_GetMatrixCoord(
	const OGPS_ISO5436_2Handle handle,
	size_t u,
	size_t v,
	size_t w,
	OGPS_Double* x,
	OGPS_Double* y,
	OGPS_Double* z)
{
	assert(handle && handle->instance);

	HandleException([&]() {
		handle->instance->GetMatrixCoord(u, v, w, x, y, z);
	});
}

bool ogps_IsMatrixCoordValid(
	const OGPS_ISO5436_2Handle handle,
	size_t u,
	size_t v,
	size_t w)
{
	assert(handle && handle->instance);

	return HandleExceptionRetval(false, [&]() {
		return handle->instance->IsMatrixCoordValid(u, v, w);
	});
}

void ogps_GetListCoord(
	const OGPS_ISO5436_2Handle handle,
	size_t index,
	OGPS_Double* x,
	OGPS_Double* y,
	OGPS_Double* z)
{
	assert(handle && handle->instance);

	HandleException([&]() {
		handle->instance->GetListCoord(index, x, y, z);
	});
}

void ogps_AppendVendorSpecific(
	const OGPS_ISO5436_2Handle handle,
	const OGPS_Character* vendorURI,
	const OGPS_Character* filePath)
{
	assert(handle && handle->instance);
	assert(vendorURI && filePath);

	HandleException([&]() {
		handle->instance->AppendVendorSpecific(vendorURI, filePath);
	});
}

bool ogps_GetVendorSpecific(
	const OGPS_ISO5436_2Handle handle,
	const OGPS_Character* vendorURI,
	const OGPS_Character* fileName,
	const OGPS_Character* targetPath)
{
	assert(handle && handle->instance);
	assert(vendorURI && fileName && targetPath);

	return HandleExceptionRetval(false, [&]() {
		return handle->instance->GetVendorSpecific(vendorURI, fileName, targetPath);
	});
}

bool ogps_IsMatrix(const OGPS_ISO5436_2Handle handle)
{
	assert(handle && handle->instance);

	return HandleExceptionRetval(false, [&]() {
		return handle->instance->IsMatrix();
	});
}

void ogps_GetMatrixDimensions(const OGPS_ISO5436_2Handle handle,
	size_t* size_u,
	size_t* size_v,
	size_t* size_w)
{
	assert(handle && handle->instance);

	HandleException([&]() {
		handle->instance->GetMatrixDimensions(size_u, size_v, size_w);
	});
}

size_t ogps_GetListDimension(const OGPS_ISO5436_2Handle handle)
{
	assert(handle && handle->instance);

	return HandleExceptionRetval(0, [&]() {
		return handle->instance->GetListDimension();
	});
}
