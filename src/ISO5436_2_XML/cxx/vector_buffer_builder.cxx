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

#include "vector_buffer_builder.hxx"
#include "vector_buffer.hxx"

#include "inline_validity.hxx"

#include "point_buffer_impl.hxx"

#include <opengps/cxx/exceptions.hxx>

#include "stdafx.hxx"

bool VectorBufferBuilder::BuildBuffer()
{
	assert(!m_Buffer);

	m_Buffer = std::make_shared<VectorBuffer>();

	return true;
}

bool VectorBufferBuilder::BuildX(OGPS_DataPointType dataType, size_t size)
{
	assert(m_Buffer);

	bool success{};
	m_Buffer->SetX(CreatePointBuffer(dataType, size, success));
	return success;
}

bool VectorBufferBuilder::BuildY(OGPS_DataPointType dataType, size_t size)
{
	assert(m_Buffer);

	bool success{};
	m_Buffer->SetY(CreatePointBuffer(dataType, size, success));
	return success;
}

bool VectorBufferBuilder::BuildZ(OGPS_DataPointType dataType, size_t size)
{
	assert(m_Buffer);
	assert(dataType != OGPS_MissingPointType);

	bool success{};
	m_Buffer->SetZ(CreatePointBuffer(dataType, size, success));
	return success;
}

bool VectorBufferBuilder::BuildValidityProvider(bool allowInvalidPoints)
{
	assert(m_Buffer);
	assert(m_Buffer->GetZ());

	std::shared_ptr<PointValidityProvider> provider;
	std::shared_ptr<ValidBuffer> validBuffer;

	auto zBuffer{ m_Buffer->GetZ() };
	const auto dataType{ zBuffer->GetPointType() };

	switch (dataType)
	{
	case OGPS_Int16PointType:
	{
		auto validityInt16{ std::make_shared<Int16ValidBuffer>(zBuffer) };
		provider = validityInt16;
		if (allowInvalidPoints)
		{
			validBuffer = validityInt16;
		}
	} break;
	case OGPS_Int32PointType:
	{
		auto validityInt32{ std::make_shared<Int32ValidBuffer>(zBuffer) };
		provider = validityInt32;
		if (allowInvalidPoints)
		{
			validBuffer = validityInt32;
		}
	} break;
	case OGPS_FloatPointType:
	{
		provider = std::make_shared<FloatInlineValidity>(zBuffer, allowInvalidPoints);
	} break;
	case OGPS_DoublePointType:
	{
		provider = std::make_shared<DoubleInlineValidity>(zBuffer, allowInvalidPoints);
	} break;
	case OGPS_MissingPointType:
	{
		assert(false);
	} break;
	default:
	{
		assert(false);
	} break;
	}

	if (provider)
	{
		m_Buffer->SetValidityProvider(provider, validBuffer);
		return true;
	}

	return false;
}

std::shared_ptr<VectorBuffer> VectorBufferBuilder::GetBuffer() const
{
	return m_Buffer;
}

std::shared_ptr<PointBuffer> VectorBufferBuilder::CreatePointBuffer(OGPS_DataPointType dataType, size_t size, bool& retval) const
{
	std::shared_ptr<PointBuffer> point;

	switch (dataType)
	{
	case OGPS_Int16PointType:
		point = std::make_shared<Int16PointBuffer>();
		retval = true;
		break;
	case OGPS_Int32PointType:
		point = std::make_shared<Int32PointBuffer>();
		retval = true;
		break;
	case OGPS_FloatPointType:
		point = std::make_shared<FloatPointBuffer>();
		retval = true;
		break;
	case OGPS_DoublePointType:
		point = std::make_shared<DoublePointBuffer>();
		retval = true;
		break;
	case OGPS_MissingPointType:
		retval = true;
		break;
	default:
		retval = false;
		assert(false);
		break;
	}

	if (point)
	{
		point->Allocate(size);
	}

	return point;
}
