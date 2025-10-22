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

#include <opengps/point_iterator.h>

#include "point_iterator_c.hxx"
#include "point_vector_c.hxx"

#include "messages_c.hxx"

#include <opengps/cxx/point_iterator.hxx>

#include "../cxx/stdafx.hxx"

bool ogps_HasNextPoint(const OGPS_PointIteratorPtr iterator)
{
	assert(iterator && iterator->instance);

	return HandleExceptionRetval(false, [&]() {
		return iterator->instance->HasNext();
	});
}

bool ogps_HasPrevPoint(const OGPS_PointIteratorPtr iterator)
{
	assert(iterator && iterator->instance);

	return HandleExceptionRetval(false, [&]() {
		return iterator->instance->HasPrev();
	});
}

bool ogps_MoveNextPoint(OGPS_PointIteratorPtr iterator)
{
	assert(iterator && iterator->instance);

	return HandleExceptionRetval(false, [&]() {
		return iterator->instance->MoveNext();
	});
}

bool ogps_MovePrevPoint(OGPS_PointIteratorPtr iterator)
{
	assert(iterator && iterator->instance);

	return HandleExceptionRetval(false, [&]() {
		return iterator->instance->MovePrev();
	});
}

void ogps_ResetNextPointIterator(OGPS_PointIteratorPtr iterator)
{
	assert(iterator && iterator->instance);

	HandleException([&]() {
		iterator->instance->ResetNext();
	});
}

void ogps_ResetPrevPointIterator(OGPS_PointIteratorPtr iterator)
{
	assert(iterator && iterator->instance);

	HandleException([&]() {
		iterator->instance->ResetPrev();
	});
}

void ogps_GetCurrentPoint(
	const OGPS_PointIteratorPtr iterator,
	OGPS_PointVectorPtr vector)
{
	assert(iterator && iterator->instance && vector);

	HandleException([&]() {
		iterator->instance->GetCurrent(vector->instance);
	});
}

void ogps_GetCurrentCoord(
	const OGPS_PointIteratorPtr iterator,
	OGPS_PointVectorPtr vector)
{
	assert(iterator && iterator->instance && vector);

	HandleException([&]() {
		iterator->instance->GetCurrentCoord(vector->instance);
	});
}

void ogps_SetCurrentPoint(
	const OGPS_PointIteratorPtr iterator,
	const OGPS_PointVectorPtr vector)
{
	assert(iterator && iterator->instance);

	HandleException([&]() {
		iterator->instance->SetCurrent(vector ? &vector->instance : nullptr);
	});
}

bool ogps_GetMatrixPosition(
	const OGPS_PointIteratorPtr iterator,
	size_t* u,
	size_t* v,
	size_t* w)
{
	assert(iterator && iterator->instance);

	return HandleExceptionRetval(false, [&]() {
		return iterator->instance->GetPosition(u, v, w);
	});
}

bool ogps_GetListPosition(
	const OGPS_PointIteratorPtr iterator,
	size_t* index)
{
	assert(iterator && iterator->instance);

	return HandleExceptionRetval(false, [&]() {
		return iterator->instance->GetPosition(index);
	});
}

void ogps_FreePointIterator(OGPS_PointIteratorPtr* iterator)
{
	if (*iterator)
	{
		HandleException([&]() {
			delete* iterator;
		});
		*iterator = nullptr;
	}
}
