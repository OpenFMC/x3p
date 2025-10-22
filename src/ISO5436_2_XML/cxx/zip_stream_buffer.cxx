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

#include <string.h>
#include "zip_stream_buffer.hxx"
#include "stdafx.hxx"

#include <opengps/cxx/string.hxx>

ZipStreamBuffer::ZipStreamBuffer(zipFile handle, bool enable_md5)
	:m_Handle{ handle }
{
	if (enable_md5)
	{
		m_Md5Context = std::make_unique<md5_context>();
		md5_starts(m_Md5Context.get());
	}
}

std::streamsize ZipStreamBuffer::xsputn(const char_type* s, std::streamsize count)
{
	if (m_Md5Context)
	{
		md5_update(m_Md5Context.get(), reinterpret_cast<const unsigned char*>(s), static_cast<int>(count));
	}

	if (zipWriteInFileInZip(m_Handle, s, static_cast<unsigned int>(count)) == ZIP_OK)
	{
		return count;
	}

	return 0;
}

bool ZipStreamBuffer::GetMd5(std::array<unsigned char, 16>& md5)
{
	if (m_Md5Context)
	{
		md5_finish(m_Md5Context.get(), md5.data());
		md5_starts(m_Md5Context.get());

		return true;
	}

	return false;
}

ZipOutputStream::ZipOutputStream(ZipStreamBuffer& buffer)
	:std::ostream(&buffer)
{
}

std::ostream& ZipOutputStream::write(const char* s)
{
	if (s)
	{
		const auto size{ strlen(s) };
		return std::ostream::write(s, size);
	}

	return *this;
}
