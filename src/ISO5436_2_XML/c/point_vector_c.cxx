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

#include <opengps/point_vector.h>
#include <opengps/cxx/point_vector.hxx>
#include <opengps/cxx/data_point.hxx>

#include "data_point_c.hxx"
#include "point_vector_c.hxx"
#include "messages_c.hxx"

#include "../cxx/stdafx.hxx"

_OGPS_POINT_VECTOR::~_OGPS_POINT_VECTOR()
{
	delete x;
	delete y;
	delete z;
}

OGPS_PointVectorPtr ogps_CreatePointVector(void)
{
	return new OGPS_PointVector;
}

void ogps_FreePointVector(OGPS_PointVectorPtr* vector)
{
	if (vector)
	{
		HandleException([&]() {
			delete *vector;
		});
		*vector = nullptr;
	}
}

template<typename T>
static inline void ogps_SetXT(OGPS_PointVectorPtr vector, T value)
{
	assert(vector);

	HandleException([&]() {
		vector->instance.SetX(value);
	});
}

void ogps_SetInt16X(
	OGPS_PointVectorPtr vector,
	OGPS_Int16 value)
{
	ogps_SetXT<OGPS_Int16>(vector, value);
}

void ogps_SetInt32X(
	OGPS_PointVectorPtr vector,
	OGPS_Int32 value)
{
	ogps_SetXT<OGPS_Int32>(vector, value);
}

void ogps_SetFloatX(
	OGPS_PointVectorPtr vector,
	OGPS_Float value)
{
	ogps_SetXT<OGPS_Float>(vector, value);
}

void ogps_SetDoubleX(
	OGPS_PointVectorPtr vector,
	OGPS_Double value)
{
	ogps_SetXT<OGPS_Double>(vector, value);
}

template<typename T>
static inline void ogps_SetYT(OGPS_PointVectorPtr vector, T value)
{
	assert(vector);

	HandleException([&]() {
		vector->instance.SetY(value);
	});
}

void ogps_SetInt16Y(
	OGPS_PointVectorPtr vector,
	OGPS_Int16 value)
{
	ogps_SetYT<OGPS_Int16>(vector, value);
}

void ogps_SetInt32Y(
	OGPS_PointVectorPtr vector,
	OGPS_Int32 value)
{
	ogps_SetYT<OGPS_Int32>(vector, value);
}

void ogps_SetFloatY(
	OGPS_PointVectorPtr vector,
	OGPS_Float value)
{
	ogps_SetYT<OGPS_Float>(vector, value);
}

void ogps_SetDoubleY(
	OGPS_PointVectorPtr vector,
	OGPS_Double value)
{
	ogps_SetYT<OGPS_Double>(vector, value);
}

template<typename T>
static inline void ogps_SetZT(OGPS_PointVectorPtr vector, T value)
{
	assert(vector);

	HandleException([&]() {
		vector->instance.SetZ(value);
	});
}

void ogps_SetInt16Z(
	OGPS_PointVectorPtr  vector,
	OGPS_Int16 value)
{
	ogps_SetZT<OGPS_Int16>(vector, value);
}

void ogps_SetInt32Z(
	OGPS_PointVectorPtr vector,
	OGPS_Int32 value)
{
	ogps_SetZT<OGPS_Int32>(vector, value);
}

void ogps_SetFloatZ(
	OGPS_PointVectorPtr vector,
	OGPS_Float value)
{
	ogps_SetZT<OGPS_Float>(vector, value);
}

void ogps_SetDoubleZ(
	OGPS_PointVectorPtr vector,
	OGPS_Double value)
{
	ogps_SetZT<OGPS_Double>(vector, value);
}

void ogps_GetXYZ(
	const OGPS_PointVectorPtr vector,
	OGPS_Double* x,
	OGPS_Double* y,
	OGPS_Double* z)
{
	assert(vector);

	HandleException([&]() {
		vector->instance.GetXYZ(x, y, z);
	});
}

void ogps_SetXYZ(
	const OGPS_PointVectorPtr vector,
	OGPS_Double x,
	OGPS_Double y,
	OGPS_Double z)
{
	assert(vector);

	HandleException([&]() {
		vector->instance.SetXYZ(x, y, z);
	});
}

OGPS_DataPointPtr ogps_GetX(OGPS_PointVectorPtr vector)
{
	assert(vector);

	if (!vector->x)
	{
		HandleException([&]() {
			auto p{ vector->instance.GetX() };
			vector->x = new OGPS_DataPoint;
			vector->x->instance = p;
		});
	}

	return vector->x;
}

OGPS_DataPointPtr ogps_GetY(OGPS_PointVectorPtr  vector)
{
	assert(vector);

	if (!vector->y)
	{
		HandleException([&]() {
			auto p{ vector->instance.GetY() };
			vector->y = new OGPS_DataPoint;
			vector->y->instance = p;
		});
	}

	return vector->y;
}

OGPS_DataPointPtr ogps_GetZ(OGPS_PointVectorPtr vector)
{
	assert(vector);

	if (!vector->z)
	{
		HandleException([&]() {
			auto p{ vector->instance.GetZ() };
			vector->z = new OGPS_DataPoint;
			vector->z->instance = p;
		});
	}

	return vector->z;
}

bool ogps_IsValidPoint(const OGPS_PointVectorPtr vector)
{
	assert(vector);

	return HandleExceptionRetval(false, [&]() {
		return vector->instance.IsValid();
	});
}

OGPS_DataPointType ogps_GetPointTypeX(const OGPS_PointVectorPtr vector)
{
	assert(vector);

	return HandleExceptionRetval(OGPS_MissingPointType, [&]() {
		return vector->instance.GetX()->GetPointType();
	});
}

template<typename T>
static inline T ogps_GetXT(const OGPS_PointVectorPtr vector)
{
	assert(vector);

	return HandleExceptionRetval(0, [&]() {
		T value{};
		vector->instance.GetX(&value);
		return value;
	});
}

OGPS_Int16 ogps_GetInt16X(const OGPS_PointVectorPtr vector)
{
	return ogps_GetXT<OGPS_Int16>(vector);
}

OGPS_Int32 ogps_GetInt32X(const OGPS_PointVectorPtr vector)
{
	return ogps_GetXT<OGPS_Int32>(vector);
}

OGPS_Float ogps_GetFloatX(const OGPS_PointVectorPtr vector)
{
	return ogps_GetXT<OGPS_Float>(vector);
}

OGPS_Double ogps_GetDoubleX(const OGPS_PointVectorPtr vector)
{
	return ogps_GetXT<OGPS_Double>(vector);
}

OGPS_DataPointType ogps_GetPointTypeY(const OGPS_PointVectorPtr vector)
{
	assert(vector);

	return HandleExceptionRetval(OGPS_MissingPointType, [&]() {
		return vector->instance.GetY()->GetPointType();
	});
}

template<typename T>
static inline T ogps_GetYT(const OGPS_PointVectorPtr vector)
{
	assert(vector);

	return HandleExceptionRetval(0, [&]() {
		T value{};
		vector->instance.GetY(&value);
		return value;
	});
}

OGPS_Int16 ogps_GetInt16Y(const OGPS_PointVectorPtr vector)
{
	return ogps_GetYT<OGPS_Int16>(vector);
}

OGPS_Int32 ogps_GetInt32Y(OGPS_PointVectorPtr vector)
{
	return ogps_GetYT<OGPS_Int32>(vector);
}

OGPS_Float ogps_GetFloatY(OGPS_PointVectorPtr vector)
{
	return ogps_GetYT<OGPS_Float>(vector);
}

OGPS_Double ogps_GetDoubleY(const OGPS_PointVectorPtr vector)
{
	return ogps_GetYT<OGPS_Double>(vector);
}

OGPS_DataPointType ogps_GetPointTypeZ(const OGPS_PointVectorPtr vector)
{
	assert(vector);

	return HandleExceptionRetval(OGPS_MissingPointType, [&]() {
		return vector->instance.GetZ()->GetPointType();
	});
}

template<typename T>
static inline T ogps_GetZT(const OGPS_PointVectorPtr vector)
{
	assert(vector);

	return HandleExceptionRetval(0, [&] {
		T value{};
		vector->instance.GetZ(&value);
		return value;
	});
}

OGPS_Int16 ogps_GetInt16Z(const OGPS_PointVectorPtr vector)
{
	return ogps_GetZT<OGPS_Int16>(vector);
}

OGPS_Int32 ogps_GetInt32Z(const OGPS_PointVectorPtr vector)
{
	return ogps_GetZT<OGPS_Int32>(vector);
}

OGPS_Float ogps_GetFloatZ(const OGPS_PointVectorPtr vector)
{
	return ogps_GetZT<OGPS_Float>(vector);
}

OGPS_Double ogps_GetDoubleZ(const OGPS_PointVectorPtr vector)
{
	return ogps_GetZT<OGPS_Double>(vector);
}
