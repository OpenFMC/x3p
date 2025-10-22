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

#include "xml_point_vector_reader_context.hxx"
#include "point_vector_iostream.hxx"

#include "stdafx.hxx"

#include <opengps/cxx/string.hxx>
#include <opengps/cxx/exceptions.hxx>

XmlPointVectorReaderContext::XmlPointVectorReaderContext(const StringList* pointVectorList)
	:m_PointVectorList{ pointVectorList }
{
	assert(pointVectorList);
}

XmlPointVectorReaderContext::~XmlPointVectorReaderContext()
{
	Reset();
}

void XmlPointVectorReaderContext::Set(const String& buf)
{
	assert(m_Stream);

	m_Stream->clear();
	m_Stream->str(buf);
}

void XmlPointVectorReaderContext::Reset()
{
	m_Stream.reset();

	m_Next = 0;
}

template<typename T>
inline void XmlPointVectorReaderContext::ReadT(T& value)
{
	CheckStreamAndThrowException();
	*m_Stream >> value;
	CheckIsGoodAndThrowException();
}

void XmlPointVectorReaderContext::Read(OGPS_Int16& value)
{
	ReadT<OGPS_Int16>(value);
}

void XmlPointVectorReaderContext::Read(OGPS_Int32& value)
{
	ReadT<OGPS_Int32>(value);
}

void XmlPointVectorReaderContext::Read(OGPS_Float& value)
{
	ReadT<OGPS_Float>(value);
}

void XmlPointVectorReaderContext::Read(OGPS_Double& value)
{
	ReadT<OGPS_Double>(value);
}

void XmlPointVectorReaderContext::Skip()
{
	CheckStreamAndThrowException();
	CheckIsGoodAndThrowException();
}

bool XmlPointVectorReaderContext::IsGood() const
{
	assert(m_Stream);

	return (m_Stream->rdstate() == std::ios_base::goodbit || m_Stream->rdstate() == std::ios_base::eofbit);
}

bool XmlPointVectorReaderContext::MoveNext()
{
	assert(m_PointVectorList);

	if (m_Next > 0)
	{
		if (m_Next < m_PointVectorList->size())
		{
			Set(m_PointVectorList->at(m_Next++));
			return true;
		}

		Reset();
		return false;
	}

	if (m_PointVectorList->size() > 0)
	{
		assert(!m_Stream);

		m_Stream = std::make_unique<PointVectorInputStringStream>(m_PointVectorList->at(m_Next++));

		return true;
	}

	return false;
}

bool XmlPointVectorReaderContext::IsValid() const
{
	return (m_Stream && m_Stream->str().length() > 0);
}

void XmlPointVectorReaderContext::CheckStreamAndThrowException()
{
	if (!m_Stream)
	{
		throw Exception(
			OGPS_ExInvalidOperation,
			_EX_T("No stream object available."),
			_EX_T("The operation on the stream object failed, because the stream has been released already."),
			_EX_T("OpenGPS::XmlPointVectorReaderContext"));
	}
}

void XmlPointVectorReaderContext::CheckIsGoodAndThrowException()
{
	if (!IsGood())
	{
		throw Exception(
			OGPS_ExInvalidOperation,
			_EX_T("The underlying stream object became invalid."),
			_EX_T("A read/write error occured."),
			_EX_T("OpenGPS::XmlPointVectorReaderContext"));
	}
}
