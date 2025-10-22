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

#include "binary_msb_point_vector_writer_context.hxx"
#include "point_vector_iostream.hxx"

#include <opengps/cxx/exceptions.hxx>

#include "environment.hxx"

#include "stdafx.hxx"

template<typename T, size_t TSize>
inline void BinaryMSBPointVectorWriterContext::WriteT(T value)
{
	assert(value);

	CheckStreamAndThrowException();

	static_assert(sizeof(value) == TSize, "value has incorrect byte size");
	unsigned char buffer[TSize] = {};
	GetStream()->write(reinterpret_cast<const char*>(Environment::ByteSwap(value, buffer)), TSize);

	CheckIsGoodAndThrowException();
}

void BinaryMSBPointVectorWriterContext::Write(OGPS_Int16 value)
{
	WriteT<OGPS_Int16, _OPENGPS_BINFORMAT_INT16_SIZE>(value);
}

void BinaryMSBPointVectorWriterContext::Write(OGPS_Int32 value)
{
	WriteT<OGPS_Int32, _OPENGPS_BINFORMAT_INT32_SIZE>(value);
}

void BinaryMSBPointVectorWriterContext::Write(OGPS_Float value)
{
	WriteT<OGPS_Float, _OPENGPS_BINFORMAT_FLOAT_SIZE>(value);
}

void BinaryMSBPointVectorWriterContext::Write(OGPS_Double value)
{
	WriteT<OGPS_Double, _OPENGPS_BINFORMAT_DOUBLE_SIZE>(value);
}
