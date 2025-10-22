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

#include "point_vector_proxy.hxx"
#include "point_vector_proxy_context.hxx"

#include "vector_buffer.hxx"
#include "point_buffer.hxx"

#include "stdafx.hxx"

PointVectorProxy::PointVectorProxy(std::shared_ptr<PointVectorProxyContext> context, const VectorBuffer& buffer)
	:m_X{ std::make_unique<DataPointProxy>(context, buffer.GetX()) },
	m_Y{ std::make_unique<DataPointProxy>(context, buffer.GetY()) },
	m_Z{ std::make_unique<DataPointProxy>(context, buffer.GetZ()) }
{
	assert(context);
}

PointVectorProxy::~PointVectorProxy() = default;

DataPoint* PointVectorProxy::GetX() const
{
	return m_X.get();
}

DataPoint* PointVectorProxy::GetY() const
{
	return m_Y.get();
}

DataPoint* PointVectorProxy::GetZ() const
{
	return m_Z.get();
}

void PointVectorProxy::Set(const PointVectorBase& value)
{
	if (m_X->IsValid())
	{
		m_X->Set(*value.GetX());
	}

	if (m_Y->IsValid())
	{
		m_Y->Set(*value.GetY());
	}

	m_Z->Set(*value.GetZ());
}

void PointVectorProxy::Get(PointVectorBase& value) const
{
	auto x = value.GetX();
	auto y = value.GetY();
	auto z = value.GetZ();

	assert(x && y && z);

	x->Set(*m_X);
	y->Set(*m_Y);
	z->Set(*m_Z);
}
