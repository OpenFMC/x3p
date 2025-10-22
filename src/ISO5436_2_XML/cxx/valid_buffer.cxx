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

#include <stdlib.h>
#include <string.h>

#include "valid_buffer.hxx"
#include "point_buffer.hxx"
#include "stdafx.hxx"

#include <opengps/cxx/exceptions.hxx>

ValidBuffer::ValidBuffer(std::shared_ptr<PointBuffer> value)
	:PointValidityProvider{ value }
{
}

ValidBuffer::~ValidBuffer()
{
	Reset();
}

void ValidBuffer::Allocate()
{
	assert(!m_ValidityBuffer && m_RawSize == 0 && GetPointBuffer());

	const auto size{ GetPointBuffer()->GetSize() };

	if (size > 0)
	{
		size_t rawSize{ size / 8 };

		if (rawSize * 8 < size)
		{
			++rawSize;
		}

		AllocateRaw(rawSize);
	}
}

void ValidBuffer::AllocateRaw(size_t rawSize)
{
	assert(!m_ValidityBuffer);

	m_ValidityBuffer = std::make_unique<unsigned char[]>(rawSize);

	if (!m_ValidityBuffer)
	{
		throw Exception(
			OGPS_ExGeneral,
			_EX_T("Attempt to allocate memory failed."),
			_EX_T("Could not allocate memory using malloc. This usually means there has not been left enough virtual memory on your system to fulfill the request. You may close some applications currently running and then try again or upgrade your computer hardware."),
			_EX_T("ValidBuffer::AllocateRaw"));
	}

	memset(m_ValidityBuffer.get(), 255, rawSize);
	m_RawSize = rawSize;
}

void ValidBuffer::Reset()
{
	m_ValidityBuffer.reset();

	m_RawSize = 0;
}

bool ValidBuffer::IsAllocated() const
{
	return m_ValidityBuffer != nullptr;
}

void ValidBuffer::SetValid(size_t index, bool value)
{
	assert(index < GetPointBuffer()->GetSize());

	/*
	 * Do not parse if the specific value equals true and no buffer has been allocated yet,
	 * since then everything is assumed to be valid by default.
	 * In other words: the validity does not need to be explicitly tracked.
	 */
	if (!value || m_ValidityBuffer)
	{
		if (!m_ValidityBuffer)
		{
			Allocate();
		}

		const size_t bytePosition{ index / 8 };
		const size_t bitPosition{ index % 8 };

		const auto bitValue{ static_cast<unsigned char>(static_cast<unsigned char>(1) << bitPosition) };

		if (value)
		{
			m_ValidityBuffer[bytePosition] |= bitValue;
		}
		else
		{
			m_ValidityBuffer[bytePosition] &= ~bitValue;
		}
	}
}

bool ValidBuffer::IsValid(size_t index) const
{
	if (!m_ValidityBuffer)
	{
		return true;
	}

	assert(index < GetPointBuffer()->GetSize());

	const size_t bytePosition{ index / 8 };
	const size_t bitPosition{ index % 8 };

	const auto bitValue{ static_cast<unsigned char>(static_cast<unsigned char>(1) << bitPosition) };

	const auto rawByte = &m_ValidityBuffer[bytePosition];

	return ((*rawByte & bitValue) != 0);
}

void ValidBuffer::Read(std::basic_istream<char>& stream)
{
	bool success{};

	// get length of file:
	stream.seekg(0, std::ios::end);
	if (!stream.fail())
	{
		const auto length{ stream.tellg() };
		if (!stream.fail())
		{
			stream.seekg(0, std::ios::beg);
			if (!stream.fail())
			{
				AllocateRaw(length);

				// read data as a block
				stream.read(reinterpret_cast<char*>(m_ValidityBuffer.get()), length);
				if (!stream.fail())
				{
					success = true;
				}
			}
		}
	}

	if (!success)
	{
		Reset();

		throw Exception(
			OGPS_ExGeneral,
			_EX_T("Could not read binary point validity file contained in the X3P archive."),
			_EX_T("Verify that the X3P file is not corrupted using the zip utility of your choice and try again."),
			_EX_T("OpenGPS::ValidBuffer::Read"));
	}
}

void ValidBuffer::Write(std::ostream& stream)
{
	assert(m_ValidityBuffer);

	stream.write(reinterpret_cast<const char*>(m_ValidityBuffer.get()), m_RawSize);

	if (stream.fail())
	{
		throw Exception(
			OGPS_ExGeneral,
			_EX_T("Failed to write to the point validity stream."),
			_EX_T("Check for filesystem permissions and enough space."),
			_EX_T("OpenGPS::ValidBuffer::Write"));
	}
}

bool ValidBuffer::HasInvalidMarks() const
{
	if (m_ValidityBuffer)
	{
		const auto max_index{ GetPointBuffer()->GetSize() };
		for (size_t index = 0; index < max_index; ++index)
		{
			if (!IsValid(index))
			{
				return true;
			}
		}
	}
	return false;
}

Int16ValidBuffer::Int16ValidBuffer(std::shared_ptr<PointBuffer> value)
	:ValidBuffer(value)
{
	assert(value && value->GetPointType() == OGPS_Int16PointType);
}

void Int16ValidBuffer::SetValid(size_t index, bool value)
{
	ValidBuffer::SetValid(index, value);

	if (!value)
	{
		constexpr OGPS_Int16 invalid{ 0 };
		GetPointBuffer()->Set(index, invalid);
	}
}

Int32ValidBuffer::Int32ValidBuffer(std::shared_ptr<PointBuffer> value)
	:ValidBuffer(value)
{
	assert(value && value->GetPointType() == OGPS_Int32PointType);
}

void Int32ValidBuffer::SetValid(size_t index, bool value)
{
	ValidBuffer::SetValid(index, value);

	if (!value)
	{
		constexpr OGPS_Int32 invalid{ 0 };
		GetPointBuffer()->Set(index, invalid);
	}
}
