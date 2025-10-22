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
 *   openGPS and the openGPS logo is a registered trademark of             *
 *   Physikalisch Technische Bundesanstalt (PTB)                           *
 *   http://www.ptb.de/                                                    *
 *                                                                         *
 *   More information about openGPS can be found at                        *
 *   http://www.opengps.eu/                                                *
 ***************************************************************************/

 /*! @file
  *  Example programm demonstrating some of the features of the X3P-data format.
  */

#include <opengps/cxx/opengps.hxx>

#include <opengps/iso5436_2.h>
#include <opengps/cxx/iso5436_2.hxx>
#include <opengps/cxx/iso5436_2_handle.hxx>
#include <opengps/cxx/iso5436_2_xsd.hxx>
#include <opengps/cxx/point_iterator.hxx>
#include <opengps/cxx/point_vector.hxx>
#include <opengps/cxx/data_point.hxx>
#include <opengps/cxx/string.hxx>
#include <opengps/cxx/info.hxx>
#include <opengps/cxx/iso5436_2_xsd_utils.hxx>

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ios>
#include <ostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <cmath>

#ifdef _WIN32
#include <tchar.h>
#endif

using namespace std;
using namespace OpenGPS::Schemas::ISO5436_2;

/*!
  @brief Prints list or matrix dimension of a given x3p handle to wcout.

  @param handle;

  @return true on success, false if handle is neither a list nor a matrix.
*/
static bool PrintDimensions(const OGPS_ISO5436_2Handle handle)
{
	assert(handle);

	const auto ismatrix{ ogps_IsMatrix(handle) };
	if (ogps_HasError())
	{
		cerr << "Error checking matrix type!" << endl;
		return false;
	}

	if (ismatrix)
	{
		size_t sx{}, sy{}, sz{};
		ogps_GetMatrixDimensions(handle, &sx, &sy, &sz);
		wcout << "Matrix dimensions are (x,y,z): " << sx << ", " << sy << ", " << sz << endl;
		return true;
	}
	else
	{
		const auto sx{ ogps_GetListDimension(handle) };
		wcout << "This is a list with " << sx << " elements." << endl;
		return true;
	}
}


/*!
  @brief Helper function to return the current time properly formated.

  @return A string containing the time stamp from now.

  @note There is only a windows implementation yet. In other cases
  return a dummy. That is enough for testing purposes.
*/
#ifdef _WIN32
static OpenGPS::String TimeStamp()
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
#ifdef _UNICODE
	wostringstream sout;
#else
	ostringstream sout;
#endif
	sout << std::setfill(_T('0')) << setw(4) << (lt.tm_year + 1900) << _T('-') << setw(2) << lt.tm_mon << _T('-') << setw(2) << lt.tm_mday
		<< _T('T') << lt.tm_hour << _T(':') << lt.tm_min << _T(':') << setw(2) << lt.tm_sec << _T(".0")
		<< tzoffsign << setw(2) << tzoff_habs << _T(':') << setw(2) << tzoff_m << ends;

	return sout.str();
}
#else
static OpenGPS::String TimeStamp()
{
	//    time_t ltime;
	//    struct tm lt;
		// Time zone offset
	long tzoff;

	// Set timezone
	tzset();
	// Get time zone offset
	time_t tm1, tm2;
	struct tm* lt, * t2;
	tm1 = time(nullptr);
	t2 = gmtime(&tm1);
	tm2 = mktime(t2);
	lt = localtime(&tm1);
	tzoff = (long)(tm1 - tm2);
	//    get_timezone(&tzoff);
		// Offset ours and minutes
	int tzoff_h, tzoff_m;
	tzoff_h = -(int)floor(((double)tzoff) / 3600.);
	tzoff_m = -(int)floor(((double)tzoff) / 60. + 0.5) - tzoff_h * 60;

	// Get current time
//    time( &ltime );
//    // get local time
//    localtime_s(&lt,&ltime);
//
//    // Correct tz offset by dst
//    if (lt.tm_isdst > 0)
//      tzoff_h++;

	// Absolute offset for printing
	int tzoff_habs = abs(tzoff_h);
	OGPS_Character tzoffsign = tzoff_h < 0 ? _T('-') : _T('+');

	// Create a string of pattern "2007-04-30T13:58:02.6+02:00"
	wostringstream sout;
	sout << std::setfill(_T('0')) << setw(4) << (lt->tm_year + 1900) << _T('-') << setw(2) << lt->tm_mon << _T('-') << setw(2) << lt->tm_mday
		<< _T('T') << lt->tm_hour << _T(':') << lt->tm_min << _T(':') << setw(2) << lt->tm_sec << _T(".0")
		<< tzoffsign << setw(2) << tzoff_habs << _T(':') << setw(2) << tzoff_m << ends;

	return sout.str();
}

#endif


/*!
  @brief Prints all meta data available in the current document.

  @param Pointer to xml document tree

  @return true on success, false if no meta data available.
*/
static bool PrintMetaData(const ISO5436_2Type* document)
{
	assert(document);

	// Get a reference to optional record 2
	const auto& r2opt{ document->Record2() };

	// Check for presence of meta data
	if (r2opt.present() == false)
	{
		wcout << "Warning: This document does not contain a meta data record (Record2)!" << endl;
		return false;
	}

	wcout << std::endl << "Meta data in record2:" << std::endl;

	const auto& r2{ r2opt.get() };

	// Data set creation date
	wcout << "Data set creation date: " << r2.Date() << endl;

	// Check for data set creator
	if (r2.Creator().present())
	{
		if (r2.Creator().get().length() < 1)
			wcout << "Warning: Data set creator name is present, but empty!" << endl;
		else
			wcout << "Data set has been created by \"" << r2.Creator().get() << '"' << std::endl;
	}
	else
		wcout << "Data set was created anonymously." << endl;

	// Measuring instrument
	wcout << "Measuring Instrument Description: " << endl
		<< "  Manufacturer: \"" << r2.Instrument().Manufacturer() << '"' << endl
		<< "         Model: \"" << r2.Instrument().Model() << '"' << endl
		<< " Serial Number: \"" << r2.Instrument().Serial() << '"' << endl
		<< "       Version: \"" << r2.Instrument().Version() << '"' << endl;

	// Calibration
	wcout << "Instrument was calibrated: \"" << r2.CalibrationDate() << endl;

	// Probing system type
	wcout << "Probing system type: \"" << r2.ProbingSystem().Type() << '"' << endl
		<< "Probing system identification: \"" << r2.ProbingSystem().Identification() << '"' << endl;

	// Extract and print information
	if (r2.Comment().present())
	{
		if (r2.Comment().get().length() < 1)
			wcout << "Warning: File comment field is present, but empty!" << endl;
		else
			wcout << "File Comment: \"" << r2.Comment().get() << '"' << endl;
	}
	else
		wcout << "Warning: File contains no Comment field!" << endl;

	return true;
}

/*!
  @brief Prints all meta data available in the current document.

  @param handle to current X3P document

  @return true on success, false if no meta data available.
*/
static bool PrintMetaData(const OGPS_ISO5436_2Handle handle)
{
	assert(handle);

	auto document{ ogps_GetDocument(handle) };
	assert(document);

	if (ogps_HasError())
	{
		cerr << "Error accessing XML document while printing meta data!" << endl;
		return false;
	}

	return PrintMetaData(document);
}

/*!
   * @brief Creates and writes a simple ISO5436-2 XML X3P file
   * with two incremental and one absolute axis (the z-axis).
   *
   * @param fileName Full path to the ISO5436-2 XML X3P to write.
   */
static void simpleExample(const OpenGPS::String& fileName)
{
	// Simple example where we have two incremental and one absolute axis (the z-axis).

	// Create RECORD1
	Record1Type::Revision_type revision{ OGPS_ISO5436_2000_REVISION_NAME };
	Record1Type::FeatureType_type featureType{ OGPS_FEATURE_TYPE_SURFACE_NAME };

	Record1Type::Axes_type::CX_type::AxisType_type xaxisType{ Record1Type::Axes_type::CX_type::AxisType_type::I }; // incremental
	Record1Type::Axes_type::CX_type::DataType_type xdataType{ Record1Type::Axes_type::CX_type::DataType_type::D }; // double
	Record1Type::Axes_type::CX_type xaxis{ xaxisType };
	xaxis.DataType(xdataType);
	xaxis.Increment(1.60123456789123456789E-0002);
	xaxis.Offset(1.0123456789123456789e-1);

	Record1Type::Axes_type::CY_type::AxisType_type yaxisType{ Record1Type::Axes_type::CY_type::AxisType_type::I }; // incremental
	Record1Type::Axes_type::CY_type::DataType_type ydataType{ Record1Type::Axes_type::CY_type::DataType_type::D }; // double
	Record1Type::Axes_type::CY_type yaxis{ yaxisType };
	yaxis.DataType(ydataType);
	yaxis.Increment(1.60123456789123456789E-0002);
	yaxis.Offset(0.0);

	Record1Type::Axes_type::CZ_type::AxisType_type zaxisType(Record1Type::Axes_type::CZ_type::AxisType_type::A); // absolute
	Record1Type::Axes_type::CZ_type::DataType_type zdataType(Record1Type::Axes_type::CZ_type::DataType_type::D); // double
	Record1Type::Axes_type::CZ_type zaxis{ zaxisType };
	zaxis.DataType(zdataType);
	// Default value for absolute axis
	zaxis.Increment(1);
	zaxis.Offset(-1e-1);

	Record1Type::Axes_type axis{ xaxis, yaxis, zaxis };

	Record1Type record1{ revision, featureType, axis };

	// Create RECORD2
	Record2Type::Date_type date{ TimeStamp(), 0 };

	Record2Type::Instrument_type::Manufacturer_type manufacturer{ _T("NanoFocus AG") };
	Record2Type::Instrument_type::Model_type model{ _T("ISO5436_2_XML_Demo Software") };
	Record2Type::Instrument_type::Serial_type serial{ _T("not available") };
	Record2Type::Instrument_type::Version_type version{ _OPENGPS_VERSIONSTRING };
	Record2Type::Instrument_type instrument{ manufacturer, model, serial, version };

	Record2Type::CalibrationDate_type calibrationDate{ _T("2007-04-30T13:58:02.6+02:00"), 0 };

	Record2Type::ProbingSystem_type::Type_type type{ Record2Type::ProbingSystem_type::Type_type::Software };
	Record2Type::ProbingSystem_type::Identification_type id{ _T("Random number generator") };
	Record2Type::ProbingSystem_type probingSystem{ type, id };

	Record2Type::Comment_type comment{ _T("This is a user comment specific to this dataset.") };

	Record2Type record2{ date, instrument, calibrationDate, probingSystem };
	record2.Comment(comment);

	// Create ISO5436_2 container
	MatrixDimensionType matrix{ 2, 2, 2 };
	auto handle{ ogps_CreateMatrixISO5436_2(fileName.c_str(), nullptr, record1, &record2, matrix, false) };

	// Add data points
	//1. Create point vector buffer for three points. 
	auto vector{ ogps_CreatePointVector() };

	// 2. Since the z axis is of absolute type and the
	// other two are incremental, we simply set up just z
	// values and leave x an y values untouched (missing). 
	ogps_SetDoubleZ(vector, 4.86219120804151E-0001);

	// 3. Write into document
	ogps_SetMatrixPoint(handle, 0, 0, 0, vector);

	// Loop all data points we want to add
	ogps_SetDoubleZ(vector, 3E-0001);
	ogps_SetMatrixPoint(handle, 1, 0, 0, vector);
	ogps_SetDoubleZ(vector, 2E-0001);
	ogps_SetMatrixPoint(handle, 0, 1, 0, vector);
	ogps_SetDoubleZ(vector, 1E-0001);
	ogps_SetMatrixPoint(handle, 1, 1, 0, vector);

	// 2b/3b. We have missing point data, here.
	ogps_SetMatrixPoint(handle, 0, 0, 1, nullptr);

	// The above will show up in xml like this:
	//
	// <DataList>
	// <Datum>4.86219120804151E-0001</Datum>
	//
	// <!-- missing data points are represented by an empty tag-->
	// <Datum/>
	//
	// <Datum>8.23683772970184E-0001</Datum>
	// </DataList>
	//
	ogps_SetDoubleZ(vector, 5E-0001);
	ogps_SetMatrixPoint(handle, 1, 0, 1, vector);
	ogps_SetDoubleZ(vector, 6E-0001);
	ogps_SetMatrixPoint(handle, 0, 1, 1, vector);
	ogps_SetDoubleZ(vector, 7E-0001);
	ogps_SetMatrixPoint(handle, 1, 1, 1, vector);

	ogps_FreePointVector(&vector);

	// Create vendorspecific data.
	OpenGPS::String vname{ fileName.substr(0, fileName.find_last_of(_T("/\\")) + 1) };
	vname.append(_T("vendor.tmp"));
	std::wofstream vendor_dat(vname.ToChar(), std::ios::trunc);
	vendor_dat << _T("Vendorspecific data.") << std::endl;
	vendor_dat.close();

	ogps_AppendVendorSpecific(handle, _T("http://www.example.com/format/version1"), vname.c_str());

	PrintDimensions(handle);

	ogps_WriteISO5436_2(handle);
	ogps_CloseISO5436_2(&handle);
}

static void mediumComplexExample(const OpenGPS::String& fileName)
{
	// More complex example where we have three absolute axis.

	// Create RECORD1
	Record1Type::Revision_type revision{ OGPS_ISO5436_2000_REVISION_NAME };
	Record1Type::FeatureType_type featureType{ OGPS_FEATURE_TYPE_SURFACE_NAME };

	Record1Type::Axes_type::CX_type::AxisType_type xaxisType{ Record1Type::Axes_type::CX_type::AxisType_type::A }; // absolute
	Record1Type::Axes_type::CX_type::DataType_type xdataType{ Record1Type::Axes_type::CX_type::DataType_type::L }; // int32
	Record1Type::Axes_type::CX_type xaxis{ xaxisType };
	xaxis.DataType(xdataType);
	xaxis.Increment(10e-6); // 10 micro metre
	xaxis.Offset(1000.0e-6); // 1 milli metre

	Record1Type::Axes_type::CY_type::AxisType_type yaxisType{ Record1Type::Axes_type::CY_type::AxisType_type::A }; // absolute
	Record1Type::Axes_type::CY_type::DataType_type ydataType{ Record1Type::Axes_type::CY_type::DataType_type::F }; // float
	Record1Type::Axes_type::CX_type yaxis{ yaxisType };
	yaxis.DataType(ydataType);
	yaxis.Increment(1); // set to 1 for float and double axis
	yaxis.Offset(-1000.0e-6); // -1 milli metre

	Record1Type::Axes_type::CZ_type::AxisType_type zaxisType{ Record1Type::Axes_type::CZ_type::AxisType_type::A }; // absolute
	Record1Type::Axes_type::CZ_type::DataType_type zdataType{ Record1Type::Axes_type::CZ_type::DataType_type::D }; // double
	Record1Type::Axes_type::CX_type zaxis{ zaxisType };
	zaxis.DataType(zdataType);
	zaxis.Increment(1); // set to 1 for float and double axis
	zaxis.Offset(1000.0e-6); // 1 milli metre

	Record1Type::Axes_type axis{ xaxis, yaxis, zaxis };

	Record1Type record1{ revision, featureType, axis };

	// Create RECORD2
	Record2Type::Date_type date{ TimeStamp(), 0 };

	Record2Type::Instrument_type::Manufacturer_type manufacturer{ _T("NanoFocus AG") };
	Record2Type::Instrument_type::Model_type model{ _T("ISO5436_2_XML_Demo Software") };
	Record2Type::Instrument_type::Serial_type serial{ _T("not available") };
	Record2Type::Instrument_type::Version_type version{ _OPENGPS_VERSIONSTRING };
	Record2Type::Instrument_type instrument{ manufacturer, model, serial, version };

	Record2Type::CalibrationDate_type calibrationDate{ _T("2007-04-30T13:58:02.6+02:00"), 0 };

	Record2Type::ProbingSystem_type::Type_type type{ Record2Type::ProbingSystem_type::Type_type::Software };
	Record2Type::ProbingSystem_type::Identification_type id{ _T("Random number generator") };
	Record2Type::ProbingSystem_type probingSystem{ type, id };

	Record2Type::Comment_type comment{ _T("This is a user comment specific to this dataset.") };

	Record2Type record2{ date, instrument, calibrationDate, probingSystem };
	record2.Comment(comment);

	// Create ISO5436_2 container
	MatrixDimensionType matrix{ 4, 2, 2 };
	auto handle{ ogps_CreateMatrixISO5436_2(fileName.c_str(), nullptr, record1, &record2, matrix, true) };

	PrintDimensions(handle);

	// Add data points
	auto vector{ ogps_CreatePointVector() };

	// There are three absolute axis, so we have to
	// provide point values for all three axis. Though, we
	// do care about their data type. 

	auto iterator{ ogps_CreateNextPointIterator(handle) };

	// Create new points (current point set should be empty - of course)
	// Loop all 16 data points we want to add...
	OGPS_Int32 cnt{};
	while (ogps_MoveNextPoint(iterator))
	{
		// Int not short! Although this wouldn't mess up
		// if we had forgotten "L": both data types are compatible,
		// and we would have accepted both (short and int) in
		// this special case here.
		ogps_SetInt32X(vector, 4L * cnt);
		// automatic type conversion occurs for double //
		ogps_SetFloatY(vector, 2.5F * cnt);
		// Z has data type double //
		ogps_SetDoubleZ(vector, 4.8 * cnt);

		ogps_SetCurrentPoint(iterator, vector);

		++cnt;

		if (ogps_HasError())
		{
			break;
		}
	}

	ogps_FreePointIterator(&iterator);

	// The above will show up in xml like this:
	//
	// <DataList>
	// <Datum>4;2.5;4.8</Datum>
	// </DataList>
	

	ogps_FreePointVector(&vector);

	PrintDimensions(handle);

	ogps_WriteISO5436_2(handle);
	ogps_CloseISO5436_2(&handle);
}

static void mediumComplexExampleWInvalid(const OpenGPS::String& fileName)
{
	// More complex example with integer encoded z-axis and invalid points

	// Create RECORD1
	Record1Type::Revision_type revision{ OGPS_ISO5436_2000_REVISION_NAME };
	Record1Type::FeatureType_type featureType{ OGPS_FEATURE_TYPE_SURFACE_NAME };

	Record1Type::Axes_type::CX_type::AxisType_type xaxisType{ Record1Type::Axes_type::CX_type::AxisType_type::A }; // absolute
	Record1Type::Axes_type::CX_type::DataType_type xdataType{ Record1Type::Axes_type::CX_type::DataType_type::L }; // int32
	Record1Type::Axes_type::CX_type xaxis{ xaxisType };
	xaxis.DataType(xdataType);
	xaxis.Increment(10e-6); // 10 micrometres
	xaxis.Offset(1000.0e-6); // 1 millimetre

	Record1Type::Axes_type::CY_type::AxisType_type yaxisType{ Record1Type::Axes_type::CY_type::AxisType_type::A }; // absolute
	Record1Type::Axes_type::CY_type::DataType_type ydataType{ Record1Type::Axes_type::CY_type::DataType_type::F }; // float
	Record1Type::Axes_type::CX_type yaxis{ yaxisType };
	yaxis.DataType(ydataType);
	yaxis.Increment(1); // set to 1 for float and double axis
	yaxis.Offset(-1000.0e-6); // -1 milli metre

	Record1Type::Axes_type::CZ_type::AxisType_type zaxisType(Record1Type::Axes_type::CZ_type::AxisType_type::A); // absolute
	Record1Type::Axes_type::CZ_type::DataType_type zdataType(Record1Type::Axes_type::CZ_type::DataType_type::I); // 16 bit integer
	Record1Type::Axes_type::CX_type zaxis{ zaxisType };
	zaxis.DataType(zdataType);
	zaxis.Increment(1e-6); // set to 1 for float and double axis
	zaxis.Offset(1000.0e-6); // 1 milli metre

	Record1Type::Axes_type axis{ xaxis, yaxis, zaxis };

	Record1Type record1{ revision, featureType, axis };

	// Create RECORD2
	Record2Type::Date_type date{ TimeStamp(), 0 };

	Record2Type::Instrument_type::Manufacturer_type manufacturer{ _T("NanoFocus AG") };
	Record2Type::Instrument_type::Model_type model{ _T("ISO5436_2_XML_Demo Software") };
	Record2Type::Instrument_type::Serial_type serial{ _T("not available") };
	Record2Type::Instrument_type::Version_type version{ _OPENGPS_VERSIONSTRING };
	Record2Type::Instrument_type instrument{ manufacturer, model, serial, version };

	Record2Type::CalibrationDate_type calibrationDate{ _T("2007-04-30T13:58:02.6+02:00"), 0 };

	Record2Type::ProbingSystem_type::Type_type type{ Record2Type::ProbingSystem_type::Type_type::Software };
	Record2Type::ProbingSystem_type::Identification_type id{ _T("Random number generator") };
	Record2Type::ProbingSystem_type probingSystem{ type, id };

	Record2Type::Comment_type comment{ _T("This is a user comment specific to this dataset.") };

	Record2Type record2{ date, instrument, calibrationDate, probingSystem };
	record2.Comment(comment);

	MatrixDimensionType matrix{ 4, 2, 2 };
	auto handle{ ogps_CreateMatrixISO5436_2(fileName.c_str(), nullptr, record1, &record2, matrix, true) };

	PrintDimensions(handle);

	// Add data points
	auto vector{ ogps_CreatePointVector() };

	// 2. There are three absolute axis, so we have to
	// provide point values for all three axis. Though, we
	// do care about their data type.

	auto iterator{ ogps_CreateNextPointIterator(handle) };

	// Create new points (current point set should be empty - of course)
	// Loop all 16 data points we want to add...
	OGPS_Int32 cnt{};
	while (ogps_MoveNextPoint(iterator))
	{
		// Int not short! Although this wouldn't mess up
		// if we had forgotten "L": both data types are compatible,
		// and we would have accepted both (short and int) in
		// this special case here. 
		ogps_SetInt32X(vector, 4L * cnt);
		// automatic type conversion occurs for double
		ogps_SetFloatY(vector, 2.5F * (cnt % 4));

		// Z has data type 16 bit int
		ogps_SetInt16Z(vector, static_cast<OGPS_Int16>(floor(4.8 * cnt)));

		// 3. Write into document
		// Generate a missing point
		if (cnt != 8)
			ogps_SetCurrentPoint(iterator, vector);
		else
			ogps_SetCurrentPoint(iterator, nullptr);

		++cnt;

		if (ogps_HasError())
		{
			break;
		}
	}

	ogps_FreePointIterator(&iterator);

	// The above will show up in xml like this:
	//
	// <DataList>
	// <Datum>4;2.5;4.8</Datum>
	// </DataList>
	
	ogps_FreePointVector(&vector);

	PrintDimensions(handle);

	ogps_WriteISO5436_2(handle);
	ogps_CloseISO5436_2(&handle);
}

static void readonlyExample(const OpenGPS::String& fileName)
{
	// We want to read in some file and read its point data.
	std::wcout << endl << endl << "readonlyExample(\"" << fileName.c_str() << "\")" << endl;

	auto handle{ ogps_OpenISO5436_2(fileName.c_str(), nullptr) };

	if (ogps_HasError())
	{
		std::cerr << "Error opening file \"" << fileName << "\"" << endl;
		return;
	}

	if (!handle)
		return;

	PrintMetaData(handle);
	PrintDimensions(handle);

	auto vector{ ogps_CreatePointVector() };

	auto iterator{ ogps_CreateNextPointIterator(handle) };

	while (ogps_MoveNextPoint(iterator))
	{
		ogps_GetCurrentPoint(iterator, vector);

		if (ogps_HasError())
		{
			break;
		}

		if (ogps_IsValidPoint(vector))
		{
			OGPS_Double x{}, y{}, z{};
			ogps_GetXYZ(vector, &x, &y, &z);
			std::wcout << ("X: ") << x << ("; Y: ") << y << ("; Z: ") << z << std::endl;
		}
		else
		{
			std::wcout << ("Invalid point") << std::endl;
		}

	}

	std::wcout << std::endl;

	assert(!ogps_HasNextPoint(iterator));

	ogps_FreePointIterator(&iterator);
	ogps_FreePointVector(&vector);

	ogps_CloseISO5436_2(&handle);
}

static void readonlyExampleMatrix(const OpenGPS::String& fileName)
{
	// Same as above, but here matrix format is assumed to provide indices within the output.
	std::wcout << endl << endl << "readonlyExampleMatrix(\"" << fileName.c_str() << "\")" << endl;

	auto handle{ ogps_OpenISO5436_2(fileName.c_str(), nullptr) };

	if (ogps_HasError())
	{
		std::cerr << "Error opening file \"" << fileName << "\"" << endl;
		return;
	}

	if (!handle)
		return;

	PrintMetaData(handle);
	PrintDimensions(handle);

	// Assume matrix format.

	auto vector{ ogps_CreatePointVector() };

	auto iterator{ ogps_CreateNextPointIterator(handle) };

	while (ogps_MoveNextPoint(iterator))
	{
		size_t u{}, v{}, w{};
		ogps_GetMatrixPosition(iterator, &u, &v, &w);

		ogps_GetCurrentPoint(iterator, vector);

		if (ogps_HasError())
		{
			break;
		}

		if (ogps_IsValidPoint(vector))
		{
			OGPS_Double x{}, y{}, z{};
			ogps_GetXYZ(vector, &x, &y, &z);
			std::wcout << ("U: ") << u << ("; V: ") << v << ("; W: ") << w << ("X: ") << x << ("; Y: ") << y << ("; Z: ") << z << std::endl;
		}
		else
		{
			std::wcout << ("U: ") << u << ("; V: ") << v << ("; W: ") << w << (": Invalid point") << std::endl;
		}

	}

	std::wcout << std::endl;

	assert(!ogps_HasNextPoint(iterator));

	ogps_FreePointIterator(&iterator);
	ogps_FreePointVector(&vector);

	ogps_CloseISO5436_2(&handle);
}

static void readonlyExample2(const OpenGPS::String& fileName)
{
	// We want to read in some file and read its point data. 
	std::wcout << endl << endl << "readonlyExample2(\"" << fileName.c_str() << "\")" << endl;

	// This time reading real typed data points.
	// With type information obtained from xml document.

	auto handle{ ogps_OpenISO5436_2(fileName.c_str(), nullptr) };

	if (ogps_HasError())
	{
		std::cerr << "Error opening file \"" << fileName << "\"" << endl;
		return;
	}

	if (!handle)
		return;

#ifdef _DEBUG
	auto document{ ogps_GetDocument(handle) };

	// Z axis data type must be present (even if it is an incremental axis).
	assert(document->Record1().Axes().CZ().DataType().present());
#endif

	PrintMetaData(handle);
	PrintDimensions(handle);

	auto vector{ ogps_CreatePointVector() };

	auto iterator{ ogps_CreateNextPointIterator(handle) };

	while (ogps_MoveNextPoint(iterator))
	{
		ogps_GetCurrentPoint(iterator, vector);

		if (ogps_HasError())
		{
			break;
		}

		if (ogps_IsValidPoint(vector))
		{
			// Get transformed point at current position
			ogps_GetCurrentCoord(iterator, vector);
			std::wcout << "X; Y; Z = ";

			switch (ogps_GetPointTypeX(vector))
			{
			case OGPS_Int16PointType: {
				const auto xs{ ogps_GetInt16X(vector) };
				std::wcout << xs;
			} break;
			case OGPS_Int32PointType: {
				const auto xi{ ogps_GetInt32X(vector) };
				std::wcout << xi;
			} break;
			case OGPS_FloatPointType: {
				const auto xf{ ogps_GetFloatX(vector) };
				std::wcout << xf;
			} break;
			case OGPS_DoublePointType: {
				const auto xd{ ogps_GetDoubleX(vector) };
				std::wcout << xd;
			} break;
			case OGPS_MissingPointType:
				break;
			}

			std::wcout << "; ";

			switch (ogps_GetPointTypeY(vector))
			{
			case OGPS_Int16PointType: {
				const auto ys{ ogps_GetInt16Y(vector) };
				std::wcout << ys;
			} break;
			case OGPS_Int32PointType: {
				const auto yi{ ogps_GetInt32Y(vector) };
				std::wcout << yi;
			} break;
			case OGPS_FloatPointType: {
				const auto yf{ ogps_GetFloatY(vector) };
				std::wcout << yf;
			} break;
			case OGPS_DoublePointType: {
				const auto yd{ ogps_GetDoubleY(vector) };
				std::wcout << yd;
			} break;
			case OGPS_MissingPointType:
				break;
			}

			std::wcout << "; ";

			switch (ogps_GetPointTypeZ(vector))
			{
			case OGPS_Int16PointType: {
				const auto zs = ogps_GetInt16Z(vector);
				std::wcout << zs;
			} break;
			case OGPS_Int32PointType: {
				const auto zi{ ogps_GetInt32Z(vector) };
				std::wcout << zi;
			} break;
			case OGPS_FloatPointType: {
				const auto zf{ ogps_GetFloatZ(vector) };
				std::wcout << zf;
			} break;
			case OGPS_DoublePointType: {
				const auto zd{ ogps_GetDoubleZ(vector) };
				std::wcout << zd;
			} break;
			case OGPS_MissingPointType:
				break;
			}

			std::wcout << std::endl;
		}
		else
		{
			std::wcout << "X; Y; Z = MISSING" << std::endl;
		}
	}

	std::wcout << std::endl;

	assert(!ogps_HasNextPoint(iterator));

	ogps_FreePointIterator(&iterator);
	ogps_FreePointVector(&vector);

	ogps_CloseISO5436_2(&handle);
}

static void readonlyExample3(const OpenGPS::String& fileName)
{
	// We want to read in some file and read its point data. 
	std::wcout << endl << endl << "readonlyExample3(\"" << fileName.c_str() << "\")" << endl;
	// Same as above but with C++ Interface.

	OpenGPS::ISO5436_2 iso5436_2(fileName);
	if (ogps_HasError())
	{
		std::cerr << "Error opening file \"" << fileName << "\"" << endl;
		return;
	}

	try
	{
		iso5436_2.Open();
	}
	catch (OpenGPS::Exception& e)
	{
		auto err{ e.details() };
		std::cerr << "Error opening file \"" << fileName << "\"" << endl
			<< err << endl;
		return;
	}

	auto document{ iso5436_2.GetDocument() };

	if (document)
	{
		PrintMetaData(document);

		auto iterator{ iso5436_2.CreateNextPointIterator() };

		OpenGPS::PointVector vector;
		while (iterator->MoveNext())
		{
			iterator->GetCurrent(vector);

			std::wcout << "X; Y; Z = ";

			if (vector.IsValid())
			{
				switch (vector.GetX()->GetPointType()) {
				case OGPS_Int16PointType: {
					OGPS_Int16 xs{};
					vector.GetX()->Get(&xs);
					std::wcout << xs;
				} break;
				case OGPS_Int32PointType: {
					OGPS_Int32 xi{};
					vector.GetX()->Get(&xi);
					std::wcout << xi;
				} break;
				case OGPS_FloatPointType: {
					OGPS_Float xf{};
					vector.GetX()->Get(&xf);
					std::wcout << xf;
				} break;
				case OGPS_DoublePointType: {
					OGPS_Double xd{};
					vector.GetX()->Get(&xd);
					std::wcout << xd;
				} break;
				default: {
					// incremental axis without data type
				} break;
				}

				std::wcout << "; ";

				switch (vector.GetY()->GetPointType()) {
				case OGPS_Int16PointType: {
					OGPS_Int16 ys{};
					vector.GetY()->Get(&ys);
					std::wcout << ys;
				} break;
				case OGPS_Int32PointType: {
					OGPS_Int32 yi{};
					vector.GetY()->Get(&yi);
					std::wcout << yi;
				} break;
				case OGPS_FloatPointType: {
					OGPS_Float yf{};
					vector.GetY()->Get(&yf);
					std::wcout << yf;
				} break;
				case OGPS_DoublePointType: {
					OGPS_Double yd{};
					vector.GetY()->Get(&yd);
					std::wcout << yd;
				} break;
				default: {
					// incremental axis without data type
				} break;
				}

				std::wcout << "; ";

				switch (vector.GetZ()->GetPointType()) {
				case OGPS_Int16PointType: {
					OGPS_Int16 zs{};
					vector.GetZ()->Get(&zs);
					std::wcout << zs;
				} break;
				case OGPS_Int32PointType: {
					OGPS_Int32 zi{};
					vector.GetZ()->Get(&zi);
					std::wcout << zi;
				} break;
				case OGPS_FloatPointType: {
					OGPS_Float zf{};
					vector.GetZ()->Get(&zf);
					std::wcout << zf;
				} break;
				case OGPS_DoublePointType: {
					OGPS_Double zd{};
					vector.GetZ()->Get(&zd);
					std::wcout << zd;
				} break;
				default: {
					// missing - should not happen. Z axis must be present.
				} break;
				}

				std::wcout << std::endl;
			}
			else
			{
				std::wcout << "X; Y; Z = MISSING" << std::endl;
			}
		}

		std::wcout << std::endl;

		assert(!iterator->HasNext());
	}

	iso5436_2.Close();
}

static void readonlyExample4(const OpenGPS::String& fileName)
{
	// We want to read in some file and read its point data.
	std::wcout << endl << endl << "readonlyExample4(\"" << fileName.c_str() << "\")" << endl;

	OpenGPS::ISO5436_2 iso5436_2(fileName);
	if (ogps_HasError())
	{
		std::cerr << "Error opening file \"" << fileName << "\"" << endl;
		return;
	}

	try
	{
		iso5436_2.Open();
	}
	catch (OpenGPS::Exception& e)
	{
		auto err{ e.details() };
		std::cerr << "Error opening file \"" << fileName << "\"" << endl
			<< err << endl;
		return;
	}

	if (!ogps_HasError())
	{
		auto document{ iso5436_2.GetDocument() };
		if (document)
		{
			PrintMetaData(document);

			// Is data list? / Is matrix?
			if (document->Record3().ListDimension().present())
			{
				assert(document->Record3().ListDimension().present());
	
				// safe cast, since values greater than 32bit are not supported by the ISO5436-2 XML specification.
				const auto maxIndex{ static_cast<size_t>(document->Record3().ListDimension().get()) };
				for (size_t index = 0; index < maxIndex; ++index)
				{
					OpenGPS::PointVector vector;
					iso5436_2.GetListPoint(index, vector);

					if (ogps_HasError())
					{
						break;
					}

					iso5436_2.SetListPoint(index, vector);

					if (ogps_HasError())
					{
						break;
					}

					if (vector.IsValid())
					{
						OGPS_Double x{}, y{}, z{};
						vector.GetXYZ(&x, &y, &z);

						std::wcout << ("N: ") << index << ("X: ") << x << ("; Y: ") << y << ("; Z: ") << z << std::endl;
					}
				}
			}
		}
		iso5436_2.Close();
	}
}

// Performance test using Int16 z-Data. Simple writing an unsorted points cloud. So all axis have to be absolute.
static void performanceInt16(const OpenGPS::String& fileName, size_t dimension, bool binary)
{
	std::wcout << endl << endl << "performanceInt16(\"" << fileName.c_str() << "\")" << endl;

	const auto start{ clock() };

	// Create RECORD1
	Record1Type::Revision_type revision(OGPS_ISO5436_2000_REVISION_NAME);
	// A list has to be stored as type point cloud
	Record1Type::FeatureType_type featureType(OGPS_FEATURE_TYPE_POINTCLOUD_NAME);

	Record1Type::Axes_type::CX_type::AxisType_type xaxisType{ Record1Type::Axes_type::CX_type::AxisType_type::A }; // absolute
	Record1Type::Axes_type::CX_type::DataType_type xdataType{ Record1Type::Axes_type::CX_type::DataType_type::L }; // int32
	Record1Type::Axes_type::CX_type xaxis{ xaxisType };
	xaxis.DataType(xdataType);
	// Increment and offset of x and y are needed to scale the integer values to metric coordinates
	// The x axis is scaled in units of 10 micrometer
	xaxis.Increment(10E-6);
	xaxis.Offset(0.0);

	Record1Type::Axes_type::CY_type::AxisType_type yaxisType{ Record1Type::Axes_type::CY_type::AxisType_type::A }; // absolute
	Record1Type::Axes_type::CY_type::DataType_type ydataType{ Record1Type::Axes_type::CY_type::DataType_type::I }; // int16
	Record1Type::Axes_type::CY_type yaxis{ yaxisType };
	yaxis.DataType(ydataType);
	// The y axis is scaled in units of 10 micrometer
	yaxis.Increment(10E-6);
	yaxis.Offset(0.0);

	Record1Type::Axes_type::CZ_type::AxisType_type zaxisType{ Record1Type::Axes_type::CZ_type::AxisType_type::A }; // absolute
	Record1Type::Axes_type::CZ_type::DataType_type zdataType{ Record1Type::Axes_type::CZ_type::DataType_type::I }; // int16
	Record1Type::Axes_type::CZ_type zaxis{ zaxisType };
	zaxis.DataType(zdataType);
	zaxis.Increment(1e-6);
	zaxis.Offset(-32768e-6);
	Record1Type::Axes_type axis{ xaxis, yaxis, zaxis };

	Record1Type record1{ revision, featureType, axis };

	// Create RECORD2
	Record2Type::Date_type date{ TimeStamp(), 0 };

	Record2Type::Instrument_type::Manufacturer_type manufacturer{ _T("NanoFocus AG") };
	Record2Type::Instrument_type::Model_type model{ _T("ISO5436_2_XML_Demo Software") };
	Record2Type::Instrument_type::Serial_type serial{ _T("not available") };
	Record2Type::Instrument_type::Version_type version{ _OPENGPS_VERSIONSTRING };
	Record2Type::Instrument_type instrument{ manufacturer, model, serial, version };

	Record2Type::CalibrationDate_type calibrationDate{ _T("2007-04-30T13:58:02.6+02:00"), 0 };

	Record2Type::ProbingSystem_type::Type_type type{ Record2Type::ProbingSystem_type::Type_type::Software };
	Record2Type::ProbingSystem_type::Identification_type id{ _T("Random number generator") };
	Record2Type::ProbingSystem_type probingSystem(type, id);

	Record2Type::Comment_type comment{ _T("This file is a list of points written as performance test in int16 precision.") };

	Record2Type record2{ date, instrument, calibrationDate, probingSystem };
	record2.Comment(comment);

	// Create ISO5436_2 container
	auto handle{ ogps_CreateListISO5436_2(fileName.c_str(), nullptr, record1, &record2, dimension, binary) };

	// Add data points
	auto vector{ ogps_CreatePointVector() };

	srand(static_cast<unsigned int>(time(0)));

	for (size_t n = 0; n < dimension; ++n)
	{
		const auto randomz{ static_cast<OGPS_Int16>((rand() - (RAND_MAX / 2)) % std::numeric_limits<OGPS_Int16>::max()) };
		const auto randomy{ static_cast<OGPS_Int16>((rand() - (RAND_MAX / 2)) % std::numeric_limits<OGPS_Int16>::max()) };
		ogps_SetInt32X(vector, static_cast<OGPS_Int32>(n - (dimension / 2)));
		ogps_SetInt16Y(vector, randomy);
		ogps_SetInt16Z(vector, randomz);
		ogps_SetListPoint(handle, n, vector);
	}

	ogps_FreePointVector(&vector);

	PrintMetaData(handle);
	PrintDimensions(handle);

	ogps_WriteISO5436_2(handle);
	ogps_CloseISO5436_2(&handle);

	const auto stop{ clock() };

	std::wcout << std::endl << "Writing an X3P list file containing " << dimension
		<< " points in int16 " << (binary ? "binary" : "xml")
		<< " format took " << ((static_cast<double>(stop - start)) / CLOCKS_PER_SEC)
		<< " seconds." << std::endl;
}

static void performanceDouble(OpenGPS::String fileName, size_t dimension, bool binary)
{
	std::wcout << endl << endl << "performanceDouble(\"" << fileName.c_str() << "\")" << endl;

	const auto start{ clock() };

	// Simple example where we have two incremental and one absolute axis (the z-axis)

	// Create RECORD1
	Record1Type::Revision_type revision{ OGPS_ISO5436_2000_REVISION_NAME };
	Record1Type::FeatureType_type featureType(OGPS_FEATURE_TYPE_PROFILE_NAME);

	Record1Type::Axes_type::CX_type::AxisType_type xaxisType{ Record1Type::Axes_type::CX_type::AxisType_type::I }; // incremental
	Record1Type::Axes_type::CX_type::DataType_type xdataType{ Record1Type::Axes_type::CX_type::DataType_type::I }; // int16
	Record1Type::Axes_type::CX_type xaxis{ xaxisType };
	xaxis.DataType(xdataType);
	// A profile is still 3D. so increment and offset of x and y have a meaning, but both are incremented from the same index!
	// This example is a profile along x-axis
	xaxis.Increment(10E-6);
	// Test very small numbers
	xaxis.Offset(1.12345678912345678912345e-19);

	Record1Type::Axes_type::CY_type::AxisType_type yaxisType{ Record1Type::Axes_type::CY_type::AxisType_type::I }; // incremental
	Record1Type::Axes_type::CY_type::DataType_type ydataType{ Record1Type::Axes_type::CY_type::DataType_type::I }; // int16
	Record1Type::Axes_type::CY_type yaxis{ yaxisType };
	yaxis.DataType(ydataType);
	// Increment defaults to 1.
	yaxis.Increment(1);
	yaxis.Offset(0.0);

	Record1Type::Axes_type::CZ_type::AxisType_type zaxisType{ Record1Type::Axes_type::CZ_type::AxisType_type::A }; // absolute
	Record1Type::Axes_type::CZ_type::DataType_type zdataType{ Record1Type::Axes_type::CZ_type::DataType_type::D }; // double
	Record1Type::Axes_type::CZ_type zaxis(zaxisType);
	zaxis.DataType(zdataType);

	Record1Type::Axes_type axis{ xaxis, yaxis, zaxis };

	Record1Type record1{ revision, featureType, axis };

	// Create RECORD2
	Record2Type::Date_type date{ TimeStamp(), 0 };

	Record2Type::Instrument_type::Manufacturer_type manufacturer{ _T("NanoFocus AG") };
	Record2Type::Instrument_type::Model_type model{ _T("ISO5436_2_XML_Demo Software") };
	Record2Type::Instrument_type::Serial_type serial{ _T("not available") };
	Record2Type::Instrument_type::Version_type version{ _OPENGPS_VERSIONSTRING };
	Record2Type::Instrument_type instrument(manufacturer, model, serial, version);

	Record2Type::CalibrationDate_type calibrationDate{ _T("2007-04-30T13:58:02.6+02:00"), 0 };

	Record2Type::ProbingSystem_type::Type_type type{ Record2Type::ProbingSystem_type::Type_type::Software };
	Record2Type::ProbingSystem_type::Identification_type id{ _T("Random number generator") };
	Record2Type::ProbingSystem_type probingSystem(type, id);

	Record2Type::Comment_type comment{ _T("This file is a line profile written as performance test in double precision.") };

	Record2Type record2{ date, instrument, calibrationDate, probingSystem };
	record2.Comment(comment);

	// Set dimensions
	OpenGPS::Schemas::ISO5436_2::MatrixDimensionType mdim{ dimension, 1, 1 };

	// Create ISO5436_2 container
	auto handle{ ogps_CreateMatrixISO5436_2(fileName.c_str(), nullptr, record1, &record2, mdim, binary) };

	// Add data points

	auto vector{ ogps_CreatePointVector() };

	srand(static_cast<unsigned int>(time(0)));

	for (size_t n = 0; n < dimension; ++n)
	{
		const auto random{ rand() * (1.0 / static_cast<double>(RAND_MAX)) };
		ogps_SetDoubleZ(vector, random);

		// Write into document
		// Profiles have to be saved as a matrix of size n,1,m
		// So it would be wrong to use list format here
		// ogps_SetListPoint(handle, n, vector);
		ogps_SetMatrixPoint(handle, n, 0, 0, vector);
	}

	ogps_FreePointVector(&vector);

	PrintMetaData(handle);
	PrintDimensions(handle);

	ogps_WriteISO5436_2(handle);
	ogps_CloseISO5436_2(&handle);

	const auto stop{ clock() };

	std::wcout << std::endl << "Writing an X3P file containing " << dimension
		<< " points in double " << (binary ? "binary" : "xml")
		<< " format took " << ((static_cast<double>(stop - start)) / CLOCKS_PER_SEC)
		<< " seconds." << std::endl << std::endl;
}

// Converts a given X3P file either to binary or text format (if dstFormatIsBinary parameter equals false).
static void convertFormat(const OpenGPS::String& srcFileName, const OpenGPS::String& dstFileName, bool dstFormatIsBinary)
{
	std::wcout << endl << endl << "convertFormat(\"" << srcFileName.c_str() << "\") to \"" << dstFileName.c_str() << "\"" << endl;
	// We want to read in some file regardless whether it was written
	// in text or binary format and then copy its contents into
	// a newly created file to ensure binary format of X3P.
	// The original file will be overwritten! 

	auto src_handle{ ogps_OpenISO5436_2(srcFileName.c_str(), nullptr) };

	if (!src_handle)
		return;

	PrintMetaData(src_handle);
	PrintDimensions(src_handle);

	auto document{ ogps_GetDocument(src_handle) };

	// Check whether it is matrix or list format and allocate the new file handle accordingly.
	OGPS_ISO5436_2Handle dst_handle{};
	if (ogps_IsMatrix(src_handle) && !ogps_HasError())
	{
		dst_handle = ogps_CreateMatrixISO5436_2(dstFileName.c_str(), nullptr, document->Record1(), document->Record2().present() ? &*document->Record2() : nullptr, *document->Record3().MatrixDimension(), dstFormatIsBinary);
	}
	else
	{
		dst_handle = ogps_CreateListISO5436_2(dstFileName.c_str(), nullptr, document->Record1(), document->Record2().present() ? &*document->Record2() : nullptr, static_cast<size_t>(*document->Record3().ListDimension()), dstFormatIsBinary);
	}

	if (dst_handle)
	{
		auto vector{ ogps_CreatePointVector() };
		auto src{ ogps_CreateNextPointIterator(src_handle) };
		auto dst{ ogps_CreateNextPointIterator(dst_handle) };

		while (ogps_MoveNextPoint(src) && ogps_MoveNextPoint(dst))
		{
			ogps_GetCurrentPoint(src, vector);

			if (!ogps_HasError())
			{
				ogps_SetCurrentPoint(dst, (!ogps_IsValidPoint(vector) || ogps_HasError()) ? nullptr : vector);
			}

			if (ogps_HasError())
			{
				break;
			}
		}

		assert(!ogps_HasNextPoint(src));
		assert(!ogps_HasNextPoint(dst));

		ogps_FreePointIterator(&src);
		ogps_FreePointIterator(&dst);
		ogps_FreePointVector(&vector);
	}

	ogps_CloseISO5436_2(&src_handle);

	if (dst_handle)
	{
		ogps_WriteISO5436_2(dst_handle);
		ogps_CloseISO5436_2(&dst_handle);
	}
}

#if defined _WIN32 && defined _UNICODE
int _cdecl _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char* argv[])
#endif
{
	// Number of points to generate for performance counter
	const size_t performanceCounter{ 1000 };
	if (argc != 2)
	{
		std::wcout << "Usage: ISO5436_2_XML_demo <full path to sample files>/" << std::endl << std::endl <<
			"Please specify the full path to the directory where the *.x3p sample files reside. The path should also contain the terminating directory separator. Ensure that you have write access to that path." << std::endl << std::endl <<
			"This simple demo program parses the sample files and prints its contents onto the console. Do not change the names of the sample files, since these are hard coded herein. The purpose of the demo is to get you familiar with the openGPS(R) API." << std::endl;
		return 1;
	}

	// Print version and copyright information
	OpenGPS::String LibName;
	OpenGPS::String LibVersion;
	OpenGPS::String LibAbout;

	// get Info strings from Library
	OpenGPS::Info::GetName(&LibName);
	OpenGPS::Info::GetVersion(&LibVersion);
	OpenGPS::Info::GetAbout(&LibAbout);

	std::wcout << "This programm uses the Library \"" << LibName.c_str()
		<< "\" Version " << LibVersion.c_str() << endl
		<< LibAbout.c_str() << endl;
	OpenGPS::Info::PrintVersion();
	OpenGPS::Info::PrintCopyright();
	OpenGPS::Info::PrintLicense();

#ifdef _UNICODE
	#ifdef _WIN32
		OpenGPS::String path = argv[1];
	#else
		OpenGPS::String wpath;
		wpath.FromChar(argv[1]);
		OpenGPS::String path = wpath;
	#endif
#else
	OpenGPS::String path = argv[1];
#endif
	OpenGPS::String tmp;

	tmp = path; tmp += _T("ISO5436-sample1.x3p");
	readonlyExample(tmp);

	tmp = path; tmp += _T("ISO5436-sample4_bin.x3p");
	readonlyExample2(tmp);

	tmp = path; tmp += _T("ISO5436-sample3.x3p");
	readonlyExample2(tmp);

	tmp = path; tmp += _T("ISO5436-sample3.x3p");
	readonlyExample3(tmp);

	tmp = path; tmp += _T("ISO5436-sample2.x3p");
	readonlyExample4(tmp);

	tmp = path; tmp += _T("simple.x3p");
	simpleExample(tmp);

	tmp = path; tmp += _T("medium.x3p");
	mediumComplexExample(tmp);

	tmp = path; tmp += _T("medium_invalid_points.x3p");
	mediumComplexExampleWInvalid(tmp);

	std::wcout << std::endl << "Starting performance tests..." << std::endl;

	tmp = path; tmp += _T("performance_int16_bin.x3p");
	performanceInt16(tmp, performanceCounter, true);

	tmp = path; tmp += _T("performance_int16.x3p");
	performanceInt16(tmp, performanceCounter, false);

	tmp = path; tmp += _T("performance_double_bin.x3p");
	performanceDouble(tmp, performanceCounter, true);

	tmp = path; tmp += _T("performance_double.x3p");
	performanceDouble(tmp, performanceCounter, false);

	return 0;
}
