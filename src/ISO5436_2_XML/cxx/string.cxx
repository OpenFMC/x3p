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

#include <opengps/cxx/string.hxx>

#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <string.h>

#include "stdafx.hxx"

String::String()
	:BaseType()
{
}

String::String(const BaseType& s)
	:BaseType{ s }
{
}

String::String(const OGPS_Character* s)
	:BaseType{ s }
{
}

String::String(const String& s)
	:BaseType{ s }
{
}

String& String::operator=(const String& rhs)
{
	BaseType::operator=(rhs);
	return *this;
}

String::~String() = default;

const char* String::ToChar()
{
#ifdef _UNICODE
	const auto len{ length() };
	auto chars = std::make_unique<char[]>(len + 1);
	wcstombs(chars.get(), c_str(), len);
	chars[len] = 0;
	m_Chars = std::move(chars);
	return m_Chars.get();
#else
	return c_str();
#endif
}

void String::FromChar(const char* s)
{
	if (s)
	{
#ifdef _UNICODE
		FromChar(s, strlen(s));
#else
		*this = s;
#endif
	}
	else
	{
		clear();
	}
}

void String::FromChar(const char* const s, const size_t length)
{
	if (s && length > 0)
	{
#ifdef _UNICODE
		auto chars = std::make_unique<wchar_t[]>(length);
		mbstowcs(chars.get(), s, length);
		this->assign(chars.get(), length);
#else
		this->assign(s, length);
#endif
	}
	else
	{
		clear();
	}
}

size_t String::CopyTo(OGPS_Character* const target, const size_t size) const
{
	const auto len{ length() };

	if (size < len + 1)
	{
		return (len + 1);
	}

	assert(target);

	if (len > 0)
	{
#ifdef _UNICODE
		wcsncpy(target, c_str(), size);
#else
		strncpy(target, c_str(), size);
#endif
	}
	else
	{
		target[len] = 0;
	}

	return len;
}

bool String::ConvertToMd5(std::array<unsigned char, 16>& md5) const
{
	if (size() != 32)
	{
		return false;
	}

	String str(*this);

	for (size_t n = 30; n > 0; n -= 2)
	{
		str.insert(n, 1, _T(' '));
	}

#ifdef _UNICODE
	std::wistringstream buffer(str);
#else
	std::istringstream buffer(str);
#endif

	std::array<int, 16> md5n{};
	for (size_t k = 0; k < md5n.size(); ++k)
	{
		buffer >> std::hex >> md5n[k];

		assert(md5n[k] >= 0 && md5n[k] < 256);

		md5[k] = static_cast<unsigned char>(md5n[k]);
	}

	return buffer.eof() && !buffer.fail();
}

bool String::ConvertFromMd5(const std::array<unsigned char, 16>& md5)
{
#ifdef _UNICODE
	std::wostringstream buffer;
#else
	std::ostringstream buffer;
#endif

	buffer.fill('0');

	for (size_t k = 0; k < md5.size(); ++k)
	{
		buffer << std::hex << std::setw(2) << md5[k];
	}

	if (buffer.good())
	{
		assign(buffer.str());
		return true;
	}

	return false;
}

String& String::ReplaceAll(const String& old_str, const String& new_str)
{
	size_t pos{};
	do
	{
		pos = find(old_str, pos);
		if (pos != String::npos)
		{
			replace(pos, old_str.length(), new_str);
			pos += new_str.length();
		}
	} while (pos != String::npos);
	return *this;
}
