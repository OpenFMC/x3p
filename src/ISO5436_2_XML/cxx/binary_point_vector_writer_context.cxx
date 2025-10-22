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

#include "binary_point_vector_writer_context.hxx"
#include <opengps/cxx/exceptions.hxx>
#include "stdafx.hxx"

BinaryPointVectorWriterContext::BinaryPointVectorWriterContext(zipFile handle)
	:m_Buffer{ std::make_unique<ZipStreamBuffer>(handle, true) },
	m_Stream{ std::make_unique<ZipOutputStream>(*m_Buffer) }
{
}

BinaryPointVectorWriterContext::~BinaryPointVectorWriterContext()
{
	Close();
}

std::ostream* BinaryPointVectorWriterContext::GetStream()
{
	assert(m_Stream && m_Buffer);
	return m_Stream.get();
}

void BinaryPointVectorWriterContext::Close()
{
	m_Stream.reset();
	m_Buffer.reset();
}

bool BinaryPointVectorWriterContext::HasStream() const
{
	if (m_Stream)
	{
		assert(m_Buffer);
		return true;
	}

	return false;
}

bool BinaryPointVectorWriterContext::IsGood() const
{
	assert(m_Stream && m_Buffer);

	const auto state{ m_Stream->rdstate() };
	return (state == std::ios_base::goodbit || state == std::ios_base::eofbit);
}

void BinaryPointVectorWriterContext::CheckStreamAndThrowException()
{
	if (!HasStream())
	{
		throw Exception(
			OGPS_ExInvalidOperation,
			_EX_T("No binary file stream available."),
			_EX_T("The operation on the binary file stream failed, because the stream has been closed already."),
			_EX_T("OpenGPS::BinaryPointVectorWriterContext"));
	}
}

void BinaryPointVectorWriterContext::CheckIsGoodAndThrowException()
{
	if (!IsGood())
	{
		throw Exception(
			OGPS_ExInvalidOperation,
			_EX_T("The underlying binary stream object became invalid."),
			_EX_T("A read/write error occured."), \
			_EX_T("OpenGPS::BinaryPointVectorWriterContext"));
	}
}

void BinaryPointVectorWriterContext::Skip()
{
	CheckStreamAndThrowException();
	CheckIsGoodAndThrowException();
}

void BinaryPointVectorWriterContext::MoveNext()
{
}

void BinaryPointVectorWriterContext::GetMd5(std::array<unsigned char, 16>& md5)
{
	assert(HasStream());
	m_Buffer->GetMd5(md5);
}
