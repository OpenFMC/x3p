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

#include "point_vector_parser.hxx"

#include "data_point_parser_impl.hxx"
#include "missing_data_point_parser.hxx"

#include "point_vector_reader_context.hxx"
#include "point_vector_writer_context.hxx"

#include <opengps/cxx/point_vector_base.hxx>

#include "stdafx.hxx"

PointVectorParser::~PointVectorParser() = default;

void PointVectorParser::SetX(std::unique_ptr<DataPointParser> value)
{
	assert(value);

	m_X = std::move(value);
}

void PointVectorParser::SetY(std::unique_ptr<DataPointParser> value)
{
	assert(value);

	m_Y = std::move(value);
}

void PointVectorParser::SetZ(std::unique_ptr<DataPointParser> value)
{
	assert(value);

	m_Z = std::move(value);
}

void PointVectorParser::Read(PointVectorReaderContext& context, PointVectorBase& value)
{
	assert(m_X && m_Y && m_Z);

	m_X->Read(context, *value.GetX());
	m_Y->Read(context, *value.GetY());
	m_Z->Read(context, *value.GetZ());
}

void PointVectorParser::Write(PointVectorWriterContext& context, const PointVectorBase& value)
{
	assert(m_X && m_Y && m_Z);

	m_X->Write(context, *value.GetX());
	m_Y->Write(context, *value.GetY());
	m_Z->Write(context, *value.GetZ());
}

std::unique_ptr<DataPointParser> PointVectorParser::CreateDataPointParser(OGPS_DataPointType dataType) const
{
	switch (dataType)
	{
	case OGPS_Int16PointType:
		return std::make_unique<Int16DataPointParser>();
	case OGPS_Int32PointType:
		return std::make_unique<Int32DataPointParser>();
	case OGPS_FloatPointType:
		return std::make_unique<FloatDataPointParser>();
	case OGPS_DoublePointType:
		return std::make_unique<DoubleDataPointParser>();
	case OGPS_MissingPointType:
		return std::make_unique<MissingDataPointParser>();
	default:
		assert(false);
		break;
	}

	return nullptr;
}
