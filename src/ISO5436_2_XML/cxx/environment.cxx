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

#include "environment.hxx"
#include "stdafx.hxx"

std::unique_ptr<Environment> Environment::m_Instance;

const Environment* Environment::GetInstance()
{
	if (!m_Instance)
	{
		m_Instance = CreateInstance();
	}

	return m_Instance.get();
}

void Environment::Reset()
{
	m_Instance.reset();
}

Environment::Environment() = default;

Environment::~Environment() = default;

bool Environment::IsLittleEndian()
{
	unsigned char test[2] = { 0, 1 };

	OGPS_Int16 value{};
	ByteSwap(test, value);

	assert(value == 1 || value == 256);

	return (value == 1);
}

inline static void ByteSwap16(const unsigned char* src, unsigned char* dst)
{
	assert(src && dst);
	dst[0] = src[1];
	dst[1] = src[0];
}

inline static void ByteSwap32(const unsigned char* src, unsigned char* dst)
{
	assert(src && dst);
	dst[0] = src[3];
	dst[1] = src[2];
	dst[2] = src[1];
	dst[3] = src[0];
}

inline static void ByteSwap64(const unsigned char* src, unsigned char* dst)
{
	assert(src && dst);
	dst[0] = src[7];
	dst[1] = src[6];
	dst[2] = src[5];
	dst[3] = src[4];
	dst[4] = src[3];
	dst[5] = src[2];
	dst[6] = src[1];
	dst[7] = src[0];
}

unsigned char* Environment::ByteSwap(OGPS_Int16 value, unsigned char* dst)
{
	auto src{ reinterpret_cast<unsigned char*>(&value) };
	ByteSwap16(src, dst);
	return dst;
}

void Environment::ByteSwap(const unsigned char* src, OGPS_Int16& value)
{
	auto dst{ reinterpret_cast<unsigned char*>(&value) };
	ByteSwap16(src, dst);
}

unsigned char* Environment::ByteSwap(OGPS_Int32 value, unsigned char* dst)
{
	auto src{ reinterpret_cast<unsigned char*>(&value) };
	ByteSwap32(src, dst);
	return dst;
}

void Environment::ByteSwap(const unsigned char* src, OGPS_Int32& value)
{
	auto dst{ reinterpret_cast<unsigned char*>(&value) };
	ByteSwap32(src, dst);
}

unsigned char* Environment::ByteSwap(OGPS_Float value, unsigned char* dst)
{
	auto src{ reinterpret_cast<unsigned char*>(&value) };
	ByteSwap32(src, dst);
	return dst;
}

void Environment::ByteSwap(const unsigned char* src, OGPS_Float& value)
{
	auto dst{ reinterpret_cast<unsigned char*>(&value) };
	ByteSwap32(src, dst);
}

unsigned char* Environment::ByteSwap(OGPS_Double value, unsigned char* dst)
{
	auto src{ reinterpret_cast<unsigned char*>(&value) };
	ByteSwap64(src, dst);
	return dst;
}

void Environment::ByteSwap(const unsigned char* src, OGPS_Double& value)
{
	assert(src && value);
	auto dst{ reinterpret_cast<unsigned char*>(&value) };
	ByteSwap64(src, dst);
}
