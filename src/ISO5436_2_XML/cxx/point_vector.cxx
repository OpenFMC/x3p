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

#include "data_point_impl.hxx"

#include <opengps/cxx/point_vector.hxx>

#include "stdafx.hxx"

PointVector::PointVector()
	:m_X{ std::make_unique<DataPointImpl>() },
	m_Y{ std::make_unique<DataPointImpl>() },
	m_Z{ std::make_unique<DataPointImpl>() }
{

}

PointVector::~PointVector() = default;

DataPoint* PointVector::GetX() const
{
	return m_X.get();
}

DataPoint* PointVector::GetY() const
{
	return m_Y.get();
}

DataPoint* PointVector::GetZ() const
{
	return m_Z.get();
}

void PointVector::GetX(OGPS_Int16* value) const
{
	assert(value);
	m_X->Get(value);
}

void PointVector::GetX(OGPS_Int32* value) const
{
	assert(value);
	m_X->Get(value);
}

void PointVector::GetX(OGPS_Float* value) const
{
	assert(value);
	m_X->Get(value);
}

void PointVector::GetX(OGPS_Double* value) const
{
	assert(value);
	m_X->Get(value);
}

void PointVector::GetY(OGPS_Int16* value) const
{
	assert(value);
	m_Y->Get(value);
}

void PointVector::GetY(OGPS_Int32* value) const
{
	assert(value);
	m_Y->Get(value);
}

void PointVector::GetY(OGPS_Float* value) const
{
	assert(value);
	m_Y->Get(value);
}

void PointVector::GetY(OGPS_Double* value) const
{
	assert(value);
	m_Y->Get(value);
}

void PointVector::GetZ(OGPS_Int16* value) const
{
	assert(value);
	m_Z->Get(value);
}

void PointVector::GetZ(OGPS_Int32* value) const
{
	assert(value);
	m_Z->Get(value);
}

void PointVector::GetZ(OGPS_Float* value) const
{
	assert(value);
	m_Z->Get(value);
}

void PointVector::GetZ(OGPS_Double* value) const
{
	assert(value);
	m_Z->Get(value);
}

void PointVector::SetX(OGPS_Int16 value)
{
	m_X->Set(value);
}

void PointVector::SetX(OGPS_Int32 value)
{
	m_X->Set(value);
}

void PointVector::SetX(OGPS_Float value)
{
	m_X->Set(value);
}

void PointVector::SetX(OGPS_Double value)
{
	m_X->Set(value);
}

void PointVector::SetY(OGPS_Int16 value)
{
	m_Y->Set(value);
}

void PointVector::SetY(OGPS_Int32 value)
{
	m_Y->Set(value);
}

void PointVector::SetY(OGPS_Float value)
{
	m_Y->Set(value);
}

void PointVector::SetY(OGPS_Double value)
{
	m_Y->Set(value);
}

void PointVector::SetZ(OGPS_Int16 value)
{
	m_Z->Set(value);
}

void PointVector::SetZ(OGPS_Int32 value)
{
	m_Z->Set(value);
}

void PointVector::SetZ(OGPS_Float value)
{
	m_Z->Set(value);
}

void PointVector::SetZ(OGPS_Double value)
{
	m_Z->Set(value);
}

void PointVector::GetXYZ(
	OGPS_Double* x,
	OGPS_Double* y,
	OGPS_Double* z) const
{
	if (x != nullptr)
	{
		*x = m_X->Get();
	}

	if (y != nullptr)
	{
		*y = m_Y->Get();
	}

	if (z != nullptr)
	{
		*z = m_Z->Get();
	}
}

void PointVector::SetXYZ(
	OGPS_Double x,
	OGPS_Double y,
	OGPS_Double z) const
{
	m_X->Set(x);

	m_Y->Set(y);

	m_Z->Set(z);
}

bool PointVector::IsValid() const
{
	return m_Z->IsValid();
}

PointVector& PointVector::operator=(const PointVector& src)
{
	Set(src);

	return *this;
}

void PointVector::Set(const PointVectorBase& value)
{
	assert(value.GetX() && value.GetY() && value.GetZ());

	m_X->Set(*value.GetX());
	m_Y->Set(*value.GetY());
	m_Z->Set(*value.GetZ());
}

void PointVector::Get(PointVectorBase& value) const
{
	assert(value.GetX() && value.GetY() && value.GetZ());

	value.GetX()->Set(*m_X);
	value.GetY()->Set(*m_Y);
	value.GetZ()->Set(*m_Z);
}
