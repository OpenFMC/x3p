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

#include <opengps/data_point.h>

#include "data_point_c.hxx"
#include "messages_c.hxx"

#include "../cxx/data_point_impl.hxx"
#include "../cxx/stdafx.hxx"

OGPS_DataPointType ogps_GetPointType(const OGPS_DataPointPtr dataPoint)
{
	assert(dataPoint && dataPoint->instance);

	return HandleExceptionRetval(OGPS_MissingPointType, [&]() {
		return dataPoint->instance->GetPointType();
	});
}

template<typename T>
static inline T opgs_GetT(const OGPS_DataPointPtr dataPoint)
{
	assert(dataPoint && dataPoint->instance);

	return HandleExceptionRetval(0, [&]() {
		T v{};
		dataPoint->instance->Get(&v);
		return v;
	});
}

OGPS_Int16 ogps_GetInt16(const OGPS_DataPointPtr dataPoint)
{
	return opgs_GetT<OGPS_Int16>(dataPoint);
}

OGPS_Int32 ogps_GetInt32(const OGPS_DataPointPtr dataPoint)
{
	return opgs_GetT<OGPS_Int32>(dataPoint);
}

OGPS_Float ogps_GetFloat(const OGPS_DataPointPtr dataPoint)
{
	return opgs_GetT<OGPS_Float>(dataPoint);
}

OGPS_Double ogps_GetDouble(const OGPS_DataPointPtr dataPoint)
{
	return opgs_GetT<OGPS_Double>(dataPoint);
}

template<typename T>
static inline void opgs_SetT(OGPS_DataPointPtr dataPoint, T value)
{
	assert(dataPoint && dataPoint->instance);

	HandleException([&]() {
		dataPoint->instance->Set(value);
	});
}

void ogps_SetInt16(
	const OGPS_DataPointPtr dataPoint,
	OGPS_Int16 value)
{
	opgs_SetT<OGPS_Int16>(dataPoint, value);
}

void ogps_SetInt32(
	const OGPS_DataPointPtr dataPoint,
	OGPS_Int32 value)
{
	opgs_SetT<OGPS_Int32>(dataPoint, value);
}

void ogps_SetFloat(
	const OGPS_DataPointPtr dataPoint,
	OGPS_Float value)
{
	opgs_SetT<OGPS_Float>(dataPoint, value);
}

void ogps_SetDouble(
	const OGPS_DataPointPtr dataPoint,
	OGPS_Double value)
{
	opgs_SetT<OGPS_Double>(dataPoint, value);
}
