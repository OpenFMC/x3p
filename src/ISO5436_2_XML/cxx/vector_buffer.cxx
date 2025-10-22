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

#include "vector_buffer.hxx"
#include "point_vector_proxy.hxx"
#include "point_buffer.hxx"

#include "stdafx.hxx"

void VectorBuffer::SetX(std::shared_ptr<PointBuffer> value)
{
	assert(!m_X);

	m_X = value;
}

void VectorBuffer::SetY(std::shared_ptr<PointBuffer> value)
{
	assert(!m_Y);

	m_Y = value;
}

void VectorBuffer::SetZ(std::shared_ptr<PointBuffer> value)
{
	assert(!m_Z);

	m_Z = value;
}

void VectorBuffer::SetValidityProvider(std::shared_ptr<PointValidityProvider> value, std::shared_ptr<ValidBuffer> buffer)
{
	assert(!m_ValidityProvider);
	assert(!m_ValidBuffer);

	assert(buffer == nullptr || buffer == value);

	m_ValidityProvider = value;
	m_ValidBuffer = buffer;
}

std::shared_ptr<PointBuffer> VectorBuffer::GetX() const
{
	return m_X;
}

std::shared_ptr<PointBuffer> VectorBuffer::GetY() const
{
	return m_Y;
}

std::shared_ptr<PointBuffer> VectorBuffer::GetZ() const
{
	return m_Z;
}

PointValidityProvider* VectorBuffer::GetValidityProvider() const
{
	return m_ValidityProvider.get();
}

ValidBuffer* VectorBuffer::GetValidityBuffer() const
{
	return m_ValidBuffer.get();
}

bool VectorBuffer::HasValidityBuffer() const
{
	return m_ValidBuffer != nullptr;
}

std::shared_ptr<PointVectorBase> VectorBuffer::CreatePointVectorProxy(std::shared_ptr<PointVectorProxyContext> context)
{
	return std::make_shared<PointVectorProxy>(context, *this);
}
