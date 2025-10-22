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

#include "xml_point_vector_writer_context.hxx"
#include "point_vector_iostream.hxx"

#include "stdafx.hxx"

#include <opengps/cxx/string.hxx>
#include <opengps/cxx/exceptions.hxx>

XmlPointVectorWriterContext::XmlPointVectorWriterContext(StringList* pointVectorList)
	:m_Stream{ std::make_unique<PointVectorOutputStringStream>() },
	m_PointVectorList{ pointVectorList }
{
	assert(pointVectorList);

	m_Stream->setf(std::ios::scientific);
	m_Stream->precision(15);
}

void XmlPointVectorWriterContext::Get(String& value) const
{
	if (m_Stream)
	{
		value = m_Stream->str();
	}
	else
	{
		value.clear();
	}
}

void XmlPointVectorWriterContext::Reset()
{
	if (m_Stream)
	{
		m_Stream->str(_T(""));
	}

	m_NeedsSeparator = false;
}

template<typename T>
inline void XmlPointVectorWriterContext::WriteT(T value)
{
	CheckStreamAndThrowException();

	AppendSeparator();
	*m_Stream << value;

	CheckIsGoodAndThrowException();
}

void XmlPointVectorWriterContext::Write(OGPS_Int16 value)
{
	WriteT<OGPS_Int16>(value);
}

void XmlPointVectorWriterContext::Write(OGPS_Int32 value)
{
	WriteT<OGPS_Int32>(value);
}

void XmlPointVectorWriterContext::Write(OGPS_Float value)
{
	WriteT<OGPS_Float>(value);
}

void XmlPointVectorWriterContext::Write(OGPS_Double value)
{
	WriteT<OGPS_Double>(value);
}

void XmlPointVectorWriterContext::Skip()
{
	CheckStreamAndThrowException();
	CheckIsGoodAndThrowException();
}

bool XmlPointVectorWriterContext::IsGood() const
{
	assert(m_Stream);

	return (m_Stream->rdstate() == std::ios_base::goodbit || m_Stream->rdstate() == std::ios_base::eofbit);
}

void XmlPointVectorWriterContext::AppendSeparator()
{
	assert(m_Stream);

	if (m_NeedsSeparator)
	{
		m_Stream->write(_T(";"), 1);
	}

	m_NeedsSeparator = true;
}

void XmlPointVectorWriterContext::MoveNext()
{
	assert(m_Stream && m_PointVectorList);

	String vector;
	Get(vector);
	Schemas::ISO5436_2::DataListType::Datum_type datum(vector);
	m_PointVectorList->push_back(datum);

	Reset();
}

void XmlPointVectorWriterContext::CheckStreamAndThrowException()
{
	if (!m_Stream)
	{
		throw Exception(
			OGPS_ExInvalidOperation,
			_EX_T("No stream object available."),
			_EX_T("The operation on the stream object failed, because the stream has been released already."),
			_EX_T("OpenGPS::XmlPointVectorWriterContext"));
	}
}

void XmlPointVectorWriterContext::CheckIsGoodAndThrowException()
{
	if (!IsGood())
	{
		throw Exception(
			OGPS_ExInvalidOperation,
			_EX_T("The underlying stream object became invalid."),
			_EX_T("A read/write error occured."),
			_EX_T("OpenGPS::XmlPointVectorWriterContext"));
	}
}
