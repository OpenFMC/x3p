/***************************************************************************
 *   Copyright by Georg Wiora (NanoFocus AG) 2008                          *
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

#include "X3PUtilities.h"

#include <iomanip>
#include <opengps/cxx/info.hxx>
#ifdef _WIN32
#include <tchar.h>
#endif
#include <ctime>
#include <locale>
#include <codecvt>

using namespace std;
using namespace OpenGPS::Schemas::ISO5436_2;

mxArray* ConvertWtoMStr(const std::wstring& inp)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return mxCreateString(converter.to_bytes(inp).c_str());
}

std::wstring ConvertMtoWStr(const mxArray* inp)
{
	if (auto str = mxArrayToUTF8String(inp))
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.from_bytes(str);
	}
	return std::wstring();
}

mxArray* GetPointInfoStructure(OGPS_ISO5436_2Handle handle)
{
	mxArray* res{};

	const auto document{ ogps_GetDocument(handle) };
	if (ogps_HasError())
	{
		mexErrMsgIdAndTxt("openGPS:X3P:XMLDocument", "Could not access XML document in current x3p file!");
		return nullptr;
	}

	const auto& r1{ document->Record1() };

	const size_t nelem{ 8 };
	const char* fieldnames[nelem] = { "Revision","FeatureType","IsMatrix","IsList",
								   "isXIncremental","isYIncremental","isZIncremental","DLL_Version" };

	res = mxCreateStructMatrix(1, 1, nelem, &(fieldnames[0]));
	mxSetField(res, 0, "Revision", ConvertWtoMStr(r1.Revision()));
	mxSetField(res, 0, "FeatureType", ConvertWtoMStr(r1.FeatureType()));
	mxSetField(res, 0, "IsMatrix", mxCreateLogicalScalar(ogps_IsMatrix(handle)));
	mxSetField(res, 0, "IsList", mxCreateLogicalScalar(ogps_IsMatrix(handle) ? false : true));
	mxSetField(res, 0, "isXIncremental", mxCreateLogicalScalar(
		r1.Axes().CX().AxisType() == Record1Type::Axes_type::CX_type::AxisType_type::I
		? true : false));
	mxSetField(res, 0, "isYIncremental", mxCreateLogicalScalar(
		r1.Axes().CY().AxisType() == Record1Type::Axes_type::CY_type::AxisType_type::I
		? true : false));
	mxSetField(res, 0, "isZIncremental", mxCreateLogicalScalar(
		r1.Axes().CZ().AxisType() == Record1Type::Axes_type::CZ_type::AxisType_type::I
		? true : false));
	mxSetField(res, 0, "DLL_Version", ConvertWtoMStr(GetX3P_Dll_ID()));

	return res;
}

std::wstring GetX3P_Dll_ID()
{
	OpenGPS::String Name;
	OpenGPS::Info::GetName(&Name);

	OpenGPS::String Version;
	OpenGPS::Info::GetVersion(&Version);

	std::wstring msg{ Name };
	msg.append(_T(" V"));
	msg.append(Version);

	return msg;
}

/*!
  @brief Helper function to return the current time properly formated.

  @return A string containing the time stamp from now.

  @note There is only a windows implementation yet. In other cases
  return a dummy. That is enough for testing purposes.
*/
#ifdef _WIN32
OpenGPS::String TimeStamp()
{
	time_t ltime;
	struct tm lt;
	// Time zone offset
	long tzoff;
	// Set timezone
	_tzset();
	// Get time zone offset
	_get_timezone(&tzoff);
	// Offset ours and minutes
	int tzoff_h, tzoff_m;
	tzoff_h = -(int)floor(((double)tzoff) / 3600.);
	tzoff_m = -(int)floor(((double)tzoff) / 60. + 0.5) - tzoff_h * 60;

	// Get current time
	time(&ltime);
	// get local time
	localtime_s(&lt, &ltime);

	// Correct tz offset by dst
	if (lt.tm_isdst > 0)
		tzoff_h++;

	// Absolute offset for printing
	int tzoff_habs = abs(tzoff_h);
	OGPS_Character tzoffsign = tzoff_h < 0 ? _T('-') : _T('+');

	// Create a string of pattern "2007-04-30T13:58:02.6+02:00"
	wostringstream sout;
	sout << std::setfill(_T('0')) << setw(4) << (lt.tm_year + 1900) << _T('-') << setw(2) << lt.tm_mon << _T('-') << setw(2) << lt.tm_mday
		<< _T('T') << setw(2) << lt.tm_hour << _T(':') << setw(2) << lt.tm_min << _T(':') << setw(2) << lt.tm_sec << _T(".000000")
		<< tzoffsign << setw(2) << tzoff_habs << _T(':') << setw(2) << tzoff_m << ends;

	return sout.str();
}
#else
// There is only a windows implementation yet.
//In other cases return a dummy. That is enough for testing purposes.
OpenGPS::String TimeStamp()
{
	return _T("2000-01-01T00:00:00.000000+00:00");
}

#endif
