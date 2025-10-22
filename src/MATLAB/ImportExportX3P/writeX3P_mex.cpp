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

 // Mex file to write X3P files to matlab

#include "X3PUtilities.h"
#include <mex.h>
#include <matrix.h>

#include <cmath>
#include <limits>
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
#include <sstream>
#include <xsd/cxx/tree/stream-extraction.hxx>
#ifdef _WIN32
#include <tchar.h>
#endif

template<class DateTime_t>
#ifdef _UNICODE
DateTime_t* parseDateTime(const std::wstring& str)
#else
DateTime_t* parseDateTime(const std::string& str)
#endif
{
	int year;
	unsigned short month;
	unsigned short day;
	unsigned short hours;
	unsigned short minutes;
	double seconds;
	short zone_hours(0);
	short zone_minutes(0);
#ifdef _UNICODE
	wchar_t ch1, ch2, ch3, chz(0);
	std::wistringstream ss(str);
#else
	char ch1, ch2, ch3, chz(0);
	istringstream ss(str);
#endif

	ss >> year >> ch1 >> month >> ch2 >> day >> ch3;
	if (ch1 != '-' || ch2 != '-' || ch3 != 'T') return nullptr;
	ss >> hours >> ch1 >> minutes >> ch2 >> seconds >> chz;
	if (ch1 != ':' || ch2 != ':') return nullptr;
	if (chz == '-' || chz == '+') {
		ss >> zone_hours >> ch1 >> zone_minutes;
		if (ch1 != ':') return nullptr;
		if (chz == '+')
			return new DateTime_t(year, month, day, hours, minutes, seconds, zone_hours, zone_minutes);
		else
			return new DateTime_t(year, month, day, hours, minutes, seconds, -zone_hours, -zone_minutes);
	}
	else {
		return new DateTime_t(year, month, day, hours, minutes, seconds);
	}
}

using namespace std;
using namespace OpenGPS::Schemas::ISO5436_2;

// Check for incremental data in axis vector for arbitrary numeric type
// Array must have at least two dimensions
// Returns 0 for non incremental axis
template <class T> static double AxisIncrement(const mxArray* x, double& offset)
{
	double curr{}, increment{};
	const T* ptr = (const T*)mxGetPr(x);
	mxAssert(mxGetNumberOfElements(x) > 1,
		"AxisIncrement Assertion: input array must have at least 2 dimensions!");
	// Number of elements in first two dimensions
	const auto dims{ mxGetDimensions(x) };
	size_t numel = dims[0] * dims[1];
	mxAssert(numel > 1, "AxisIncrement Assertion: Number of elements in first two dimensions must be >= 2");

	// estimate increment from first element
	offset = *ptr;
	increment = (*(ptr + 1)) - (*ptr);
	while (numel-- > 0)
	{
		// Get current element
		curr = (*(ptr + 1)) - (*ptr);
		// compare current increment to first one
		if (fabs(increment - curr) > std::numeric_limits<T>::epsilon())
			// This difference is too big, this is not an incremental axis
			return 0;
	}

	// This is an incremental axis. Return the increment
	return increment;
}

// Check for incremental x or y axis from x or y data vector
// Returns the increment or 0 for non incremental axis
static bool AxisIsIncremental(const mxArray* x, double& increment, double& offset)
{
	auto ndims{ mxGetNumberOfDimensions(x) };
	const auto dims{ mxGetDimensions(x) };

	// Preset increment
	increment = 0;
	offset = 0;

	// Flag for coord vector
	bool isVector{};

	// Check number of dimensions if only 1 or [1,v] or [u,1] it is a vector
	if (ndims > 2)
		// This is not incremental
		return false;
	// Check for [u,1,w] or [1,v,w]
	if (ndims == 2)
	{
		if (((dims[0] == 1) && (dims[1] > 1))
			|| ((dims[1] == 1) && (dims[0] > 1)))
			// This is a 1d coordinate vector and can be checked to be incremental
			isVector = true;
		else
			return false;
	}
	else if (ndims == 1)
		// 1d vector
		isVector = true;
	else
		// Scalar or more dimensions
		return false;

	switch (mxGetClassID(x))
	{
	case mxDOUBLE_CLASS:
		increment = AxisIncrement<OGPS_Double>(x, offset);
		break;
	case mxSINGLE_CLASS:
		increment = AxisIncrement<OGPS_Float>(x, offset);
		break;
	case mxINT16_CLASS:
		increment = AxisIncrement<OGPS_Int16>(x, offset);
		break;
	case mxINT32_CLASS:
		increment = AxisIncrement<OGPS_Int32>(x, offset);
		break;
	default:
	{
		ostringstream msg;
		// TODO: Error messages are not correctly printed yet!
		msg << "Error: Input arrays must be Double, Single, Int16 or Int32 type!"
			<< endl << ends;
		mexErrMsgIdAndTxt("openGPS:writeX3P:AxisType", msg.str().c_str());
		return false;
	}
	}

	return isVector;
}

// Check meta structure for completeness
static bool IsMetaComplete(const mxArray* meta)
{
	const size_t nelem{ 8 };
	const char* fieldnames[nelem] = { "Date",/* Optional: "Creator", */ "Instrument_Manufacturer",
		"Instrument_Model","Instrument_Serial","Instrument_Version",
		"CalibrationDate","ProbingSystem_Type","ProbingSystem_Identification"
		/*, Optional: "Comment" */ };

	// check all fields for existence
	for (size_t i = 0; i < nelem; ++i)
	{
		if (nullptr == mxGetField(meta, 0, fieldnames[i]))
		{
			ostringstream msg;
			msg << "Error: Meta data record is incomplete, field \""
				<< string(fieldnames[i]) << "\" is missing!" << endl << ends;
			mexErrMsgIdAndTxt("openGPS:writeX3P:MetaData", msg.str().c_str());
			return false;
		}
	}
	// All elements exist
	return true;
}

// Get type of probing system from string
static Record2Type::ProbingSystem_type::Type_type GetProbingSystemTypeEnum(const mxArray* inp)
{
	// Input array must be a string
	// with one value of "Contacting", "NonContacting", "Software"
	wstring winp{ ConvertMtoWStr(mxGetField(inp, 0, "ProbingSystem_Type")) };

	if (winp == _T("Contacting"))
		return Record2Type::ProbingSystem_type::Type_type::Contacting;
	else if (winp == _T("NonContacting"))
		return Record2Type::ProbingSystem_type::Type_type::NonContacting;
	else if (winp == _T("Software"))
		return Record2Type::ProbingSystem_type::Type_type::Software;

	// Illegal value
	ostringstream msg;
	msg << "Error: Meta data field \"ProbingSystem_type\" has an illegal value\""
		<< winp << "\". Possible values are one of \"Contacting\", \"NonContacting\" or \"Software\"." << endl << ends;
	mexErrMsgIdAndTxt("openGPS:writeX3P:ProbingSystem_type", msg.str().c_str());
	// This statement is never executed!
	return Record2Type::ProbingSystem_type::Type_type::Software;
}

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	wstring SyntaxHelp{
		_T("Call Syntax:\n")
		_T("  pinfo = writeX3P(FileName,FeatureType,x,y,z,meta[,...])\n")
		_T("    FileName - name of file to write\n")
		_T("    FeatureType - one of 'PRF' for a line profile, 'SUR' for surface or 'PCL' for a point cloud.\n")
		_T("    x     - 1,2 or 3d array of x coordinates in meter\n")
		_T("    y     - 1,2 or 3d array of y coordinates in meter\n")
		_T("    z     - 1,2 or 3d array of z coordinates in meter\n")
		_T("    meta  - Meta data structure of the file with the following elements:\n")
		_T("     .Date    - Data set creation date of the form '2007-04-30T13:58:02.6+02:00'.\n")
		_T("                Set to empty string to use current time.\n")
		_T("     .Creator - optional name of the creator or empty array.\n")
		_T("     .Instrument_Manufacturer - String with name of the manufacturer\n")
		_T("     .Instrument_Model - String with instrument model or software name\n")
		_T("     .Instrument_Serial - String with serial number of instrument or 'not available'.\n")
		_T("     .Instrument_Version - Hardware and software version string of Instrument\n")
		_T("     .CalibrationDate - Date of last calibration of the form '2007-04-30T13:58:02.6+02:00'\n")
		_T("     .ProbingSystem_Type - one of  'Software','Contacting' or 'NonContacting'\n")
		_T("     .ProbingSystem_Identification - String identifying lens, probe tip, etc.\n")
		_T("     .Comment    - 'A user comment specific to this dataset'\n")
		_T("    pinfo - Info about data organisation\n")
		_T("  Additional key-value-pairs can be specified after the last argument:\n")
		_T("    'rotation' - a 3x3 real matrix R containing a rotation matrix that is applied\n")
		_T("                 to each 3D-data point P on readback. R1 = R*P+T\n")
		_T("    'translation'- a 3 element vector T containing a translation vector for the data\n")
		_T("                 that is applied to each 3D-data point P on readback: R1 = R*P+T\n")
		_T("    'VendorSpecific' - a path of a file to be packet into x3p file as a vendor\n")
		_T("                 specific extension. The filename portion of the file is used as\n")
		_T("                 the URI for this extension is '") OGPS_VEXT_URI _T("'\n")
		_T("<a href=\"http://www.opengps.eu/\"www.opengps.eu</a>\n\n")
		OGPS_LICENSETEXT };
	SyntaxHelp.append(GetX3P_Dll_ID());

	// check for proper number of arguments
	if ((nrhs < 6) || (nrhs > 12))
	{
		ostringstream msg;
		msg << "writeX3P was called with " << nrhs << " input arguments!" << endl
			<< SyntaxHelp << ends;
		mexErrMsgIdAndTxt("openGPS:writeX3P:nrhs", msg.str().c_str());
	}
	// Check number of output arguments
	if (nlhs > 1)
	{
		ostringstream msg;
		msg << "writeX3P was called with " << nlhs << " output arguments!" << endl
			<< SyntaxHelp << ends;
		mexErrMsgIdAndTxt("openGPS:writeX3P:nlhs", msg.str().c_str());
	}
	// Flag for structure output argument available
	bool hasPointInfo{};
	if (nlhs >= 1)
		hasPointInfo = true;

	// Filename
	const mxArray* inFileName{ prhs[0] };
	// feature type string
	const mxArray* inFeatureType{ prhs[1] };
	// Get pointers to input data
	const mxArray* inMatrixX{ prhs[2] };  // input matrix with x-Values
	const mxArray* inMatrixY{ prhs[3] };  // input matrix with y-Values
	const mxArray* inMatrixZ{ prhs[4] };  // input matrix with z-Values
	const mxArray* meta{ prhs[5] };       // Meta data structure
	const mxArray** inKeywords{};         // Keyword-value pairs
	const mxArray* inRotation{};          // Rotation matrix
	const mxArray* inTranslation{};       // Translation vector
	const mxArray* inVendorSpecific{};    // Argument string for vendor specific extension

	// vendor specific extension filename
	std::wstring VendorFileNameL;

	// Flags for keyword existence
	bool bHasRotation{};
	bool bHasTranslation{};
	bool bHasExtension{};

	const size_t cKeywordOffset{ 6 };
	const size_t inNKeywords{ nrhs - cKeywordOffset };

	// Parse keyword arguments
	if (inNKeywords > 0)
	{
		// Get pointer to start of keyword value pairs
		inKeywords = &prhs[cKeywordOffset];
		// Parse keys
		for (size_t i = 0; i < inNKeywords; i++)
		{
			// Convert string to C
			wstring key{ ConvertMtoWStr(inKeywords[i]) };
			// convert keyword name to lower case
			transform(key.begin(), key.end(), key.begin(), ::tolower);

			// check rotation matrix
			if (key == _T("rotation"))
			{
				// check for available argument
				if (inNKeywords <= i + 1)
				{
					// Throw error message
					mexErrMsgIdAndTxt("openGPS:writeX3P:missingArgument", "'rotation' keyword not followd by an argument");
				}
				// Fetch next argument and increment index
				inRotation = inKeywords[++i];
				// check for argument type
				if (!mxIsNumeric(inRotation) || !mxIsDouble(inRotation) || mxGetNumberOfElements(inRotation) != 9)
				{
					mexErrMsgIdAndTxt("openGPS:writeX3P:notMatrix", "rotation matrix must have 9 double precision elements");
				}
				// Everything is fine
				bHasRotation = true;
			}
			// check rotation matrix
			else if (key == _T("translation"))
			{
				// check for available argument
				if (inNKeywords <= i + 1)
				{
					// Throw error message
					mexErrMsgIdAndTxt("openGPS:writeX3P:missingArgument", "'translation' keyword not followd by an argument");
				}
				// Fetch next argument and increment index
				inTranslation = inKeywords[++i];
				// check for argument type
				if (!mxIsNumeric(inTranslation) || !mxIsDouble(inTranslation) || mxGetNumberOfElements(inTranslation) != 3)
				{
					mexErrMsgIdAndTxt("openGPS:writeX3P:notVector", "translation vector must have 3 double precision elements");
				}
				// Everything is fine
				bHasTranslation = true;
			}
			else if (key == _T("vendorspecific"))
			{
				// check for available argument
				if (inNKeywords <= i + 1)
				{
					// Throw error message
					mexErrMsgIdAndTxt("openGPS:writeX3P:missingArgument", "'VendorSpecific' keyword not followd by an argument");
				}
				// Fetch next argument and increment index
				inVendorSpecific = inKeywords[++i];
				// Check argument type
				if (!mxIsChar(inVendorSpecific) ||
					mxGetNumberOfElements(inVendorSpecific) < 1) {
					mexErrMsgIdAndTxt("openGPS:writeX3P:notString", "'VendorSpecific' argument must be a string");
				}
				// get the filename
				VendorFileNameL = std::wstring{ ConvertMtoWStr(inVendorSpecific) };
				bHasExtension = true;
			}
			else
			{
				// Unkwon argument
				ostringstream msg;
				msg << "Warning: unknown keyword argument on position " << cKeywordOffset + i << "!" << endl << ends;
				mexWarnMsgIdAndTxt("openGPS:writeX3P:IllegalArgument", msg.str().c_str());
			}
		}

	}

	if (!mxIsChar(inFileName) || mxGetNumberOfElements(inFileName) < 1)
	{
		mexErrMsgIdAndTxt("openGPS:writeX3P:notString", "FileName argument must be a string");
	}
	std::wstring FileNameL{ ConvertMtoWStr(inFileName) };

	if (!mxIsChar(inFeatureType) || mxGetNumberOfElements(inFeatureType) < 1)
	{
		mexErrMsgIdAndTxt("openGPS:writeX3P:notString", "FeatureType argument must be a string");
	}

	if (!mxIsNumeric(inMatrixX) || mxGetNumberOfElements(inMatrixX) < 1)
	{
		mexErrMsgIdAndTxt("openGPS:writeX3P:notNumeric", "X input argument must be numeric");
	}
	if (!mxIsNumeric(inMatrixY) || mxGetNumberOfElements(inMatrixY) < 1)
	{
		mexErrMsgIdAndTxt("openGPS:writeX3P:notNumeric", "Y input argument must be numeric");
	}
	if (!mxIsNumeric(inMatrixZ) || mxGetNumberOfElements(inMatrixZ) < 1)
	{
		mexErrMsgIdAndTxt("openGPS:writeX3P:notNumeric", "Z input argument must be numeric");
	}

	if (!mxIsStruct(meta) || mxGetNumberOfElements(meta) < 1)
	{
		mexErrMsgIdAndTxt("openGPS:writeX3P:notNumeric", "Meta data input argument must be a meta data structure");
	}

	auto ftype{ ConvertMtoWStr(inFeatureType) };
	typedef enum { FT_undefined, FT_profile, FT_surface, FT_pointcloud } FeatureType_T;
	FeatureType_T ft{ FT_undefined };

	if (ftype == OGPS_FEATURE_TYPE_PROFILE_NAME)
		ft = FT_profile;
	else if (ftype == OGPS_FEATURE_TYPE_SURFACE_NAME)
		ft = FT_surface;
	else if (ftype == OGPS_FEATURE_TYPE_POINTCLOUD_NAME)
		ft = FT_pointcloud;
	else
	{
		ostringstream msg;
		msg << "Error: FeatureType argument is \"" << ftype
			<< "\" must be one of 'SUR', 'PRF', or 'PCL'!"
			<< endl << ends;
		mexErrMsgIdAndTxt("openGPS:writeX3P:FeatureType", msg.str().c_str());
		return;
	}

	auto ndimx{ mxGetNumberOfDimensions(inMatrixX) };
	auto ndimy{ mxGetNumberOfDimensions(inMatrixY) };
	auto ndimz{ mxGetNumberOfDimensions(inMatrixZ) };

	// Has to be one to 3 dimensions
	if ((ndimx < 1) || (ndimx > 3) || (ndimy < 1) || (ndimy > 3) || (ndimz < 1) || (ndimz > 3))
	{
		ostringstream msg;
		msg << "Error: Input arrays must have 1 to 3 dimensions but have [ndimx,ndimy,ndimz]=["
			<< ndimx << ", " << ndimy << ", " << ndimz << "]!" << endl << ends;
		mexErrMsgIdAndTxt("openGPS:writeX3P:InputDimensions", msg.str().c_str());
		return;
	}

	if (false == IsMetaComplete(meta))
		return;

	// Get dimensions of z-data matrix
	// Other dimensions are a surface
	const auto dim{ mxGetDimensions(inMatrixZ) };

	// x or y are incremental if dimension of x or y are [n,1] or [1,n]
	// and contained values are close to beeing incremental
	double xIncrement{};
	double yIncrement{};
	double xOffset{};
	double yOffset{};
	// Flags for incremental axis
	bool xIncremental{};
	bool yIncremental{};
	// Switch for featuretype, check data dimensions and incremental axis
	switch (ft)
	{
		// Profile Feature type
	case FT_profile:
		// A profile must have dimensions N,1,M or 1,N,M
		// Both cases are handled the same way
		if ((dim[0] != 1) && (dim[1] != 1))
		{
			ostringstream msg;
			msg << "Error: For a line profile the input array Z must have dimensions [u,1,w] but has ["
				<< dim[0] << ", " << dim[1] << ", w]!" << endl << ends;
			mexErrMsgIdAndTxt("openGPS:writeX3P:NotProfileData", msg.str().c_str());
			return;
		}

		// A profile must have more than one point n>1 and m>1
		if ((dim[0] * dim[1]) < 2)
		{
			ostringstream msg;
			msg << "Error: A profile must have at least two points. One of dimensions u and v must be > 1. [u,v,w] is ["
				<< dim[0] << ", " << dim[1] << ", w]!" << endl << ends;
			mexErrMsgIdAndTxt("openGPS:writeX3P:NotProfileData", msg.str().c_str());
			return;
		}

		// Check the used axis for incremental type. The other axis is allways incremental.
		if (1 == dim[0])
			xIncremental = true;
		else if (AxisIsIncremental(inMatrixX, xIncrement, xOffset))
			xIncremental = true;
		if (1 == dim[1])
			yIncremental = true;
		else if (AxisIsIncremental(inMatrixY, yIncrement, yOffset))
			yIncremental = true;
		break;
		// Surface Feature type
	case FT_surface:
		// A surface must have n>1 and m>1
		if ((dim[0] < 2) || (dim[1] < 2))
		{
			ostringstream msg;
			msg << "Error: For a surface the input array Z must have dimensions [u,v,w] with u>=2 and v>=2 but has ["
				<< dim[0] << ", " << dim[1] << ", w]!" << endl << ends;
			mexErrMsgIdAndTxt("openGPS:writeX3P:NotSurfaceData", msg.str().c_str());
			return;
		}

		// Check for incremental axis
		if (AxisIsIncremental(inMatrixX, xIncrement, xOffset))
			xIncremental = true;
		if (AxisIsIncremental(inMatrixY, yIncrement, yOffset))
			yIncremental = true;
		break;
		// Point cloud Feature type
	case FT_pointcloud:
		// Point cloud allows any dimension but no incremental axis
		xIncremental = false;
		yIncremental = false;
		break;
	}

	Record1Type::Revision_type revision{ OGPS_ISO5436_2000_REVISION_NAME };
	Record1Type::FeatureType_type featureType{ ftype };
	std::unique_ptr<Record1Type::Axes_type::CX_type::AxisType_type> xaxisType;

	if (xIncremental)
		xaxisType = std::make_unique<Record1Type::Axes_type::CX_type::AxisType_type>(Record1Type::Axes_type::CX_type::AxisType_type::I);
	else
		xaxisType = std::make_unique<Record1Type::Axes_type::CX_type::AxisType_type>(Record1Type::Axes_type::CX_type::AxisType_type::A);

	// TODO: Data type is allways double
	Record1Type::Axes_type::CX_type::DataType_type xdataType{ Record1Type::Axes_type::CX_type::DataType_type::D };
	Record1Type::Axes_type::CX_type xaxis{ *xaxisType };
	xaxis.DataType(xdataType);
	if (xIncrement == 0)
		xaxis.Increment(1.0);
	else
		xaxis.Increment(xIncrement);
	// TODO: Offset should be set to -average of data min/max
	if (bHasTranslation)
		// Set translation to value specified in keyword argument
		xaxis.Offset(mxGetPr(inTranslation)[0]);
	else
		if (xIncremental)
			xaxis.Offset(xOffset);
		else
			xaxis.Offset(0.0);

	xaxisType.reset();


	std::unique_ptr<Record1Type::Axes_type::CY_type::AxisType_type> yaxisType;

	if (yIncremental)
		yaxisType = std::make_unique<Record1Type::Axes_type::CY_type::AxisType_type>(Record1Type::Axes_type::CY_type::AxisType_type::I);
	else
		yaxisType = std::make_unique<Record1Type::Axes_type::CY_type::AxisType_type>(Record1Type::Axes_type::CY_type::AxisType_type::A);

	// TODO: Data type is allways double
	Record1Type::Axes_type::CY_type::DataType_type ydataType{ Record1Type::Axes_type::CY_type::DataType_type::D };
	Record1Type::Axes_type::CY_type yaxis{ *yaxisType };
	yaxis.DataType(ydataType);
	if (yIncrement == 0)
		yaxis.Increment(1.0);
	else
		yaxis.Increment(yIncrement);

	// TODO: Offset should be set to -average of data min/max
	if (bHasTranslation)
		// Set translation to value specified in keyword argument
		yaxis.Offset(mxGetPr(inTranslation)[1]);
	else
		if (yIncremental)
			yaxis.Offset(yOffset);
		else
			yaxis.Offset(0.0);

	yaxisType.reset();

	// Setup z-axis, allways double absolute
	Record1Type::Axes_type::CZ_type::AxisType_type zaxisType{ Record1Type::Axes_type::CZ_type::AxisType_type::A };
	Record1Type::Axes_type::CZ_type::DataType_type zdataType{ Record1Type::Axes_type::CZ_type::DataType_type::D };
	Record1Type::Axes_type::CZ_type zaxis{ zaxisType };
	zaxis.DataType(zdataType);
	zaxis.Increment(1.0);
	// TODO: Offset should be set to average of data min/max
	if (bHasTranslation)
		// Set translation to value specified in keyword argument
		zaxis.Offset(mxGetPr(inTranslation)[2]);
	else
		zaxis.Offset(0.0);

	Record1Type::Axes_type axis{ xaxis, yaxis, zaxis };

	if (bHasRotation)
	{
		// Get pointer to rotation matrix
		auto rotMat{ mxGetPr(inRotation) };
		AxesType::Rotation_type rot(
			rotMat[0], rotMat[3], rotMat[6],
			rotMat[1], rotMat[4], rotMat[7],
			rotMat[2], rotMat[5], rotMat[8]);
		axis.Rotation(rot);
	}

	// Create Record1
	Record1Type record1{ revision, featureType, axis };

	// Create RECORD2 with information from meta data structure
	// Data set creation date
	std::unique_ptr<Record2Type::Date_type> date;
	// Check for a date with correct length given in meta data field
	// TODO: We should at least do some format cecking
	auto dlen = mxGetNumberOfElements(mxGetField(meta, 0, "Date"));
	// If length is less than 2 we assume it has been left blank intentionally
	if (dlen > 1)
	{
		date.reset(parseDateTime<Record2Type::Date_type>(ConvertMtoWStr(mxGetField(meta, 0, "Date"))));
	}
	if (date == nullptr)
	{
		if (dlen > 1)
		{
			// Issue a warning
			mexWarnMsgIdAndTxt("openGPS:writeX3P:CreationDate",
				"The creation date must be formatted\n"
				"similar to '2007-04-30T13:58:02.600000+02:00'. Using the current date as\n"
				"data set creation date\n");
		}
		// Use current date as creation date
		date.reset(parseDateTime<Record2Type::Date_type>(TimeStamp()));
	}

	// Instrument manufacturer
	Record2Type::Instrument_type::Manufacturer_type
		manufacturer(ConvertMtoWStr(mxGetField(meta, 0, "Instrument_Manufacturer")));

	// Instrument model
	Record2Type::Instrument_type::Model_type
		model(ConvertMtoWStr(mxGetField(meta, 0, "Instrument_Model")));

	// Instrument serial number set to default if not specified
	std::unique_ptr<Record2Type::Instrument_type::Serial_type> serial;

	if (mxGetNumberOfElements(mxGetField(meta, 0, "Instrument_Serial")) == 0)
	{
		serial = std::make_unique<Record2Type::Instrument_type::Serial_type>(_T("not available"));
	}
	else
	{
		serial = std::make_unique<Record2Type::Instrument_type::Serial_type>(
			ConvertMtoWStr(mxGetField(meta, 0, "Instrument_Serial")));
	}
	// Instrument hard and software version
	Record2Type::Instrument_type::Version_type
		version{ ConvertMtoWStr(mxGetField(meta, 0, "Instrument_Version")) };
	// Assemble instrument information
	Record2Type::Instrument_type instrument{ manufacturer, model, *serial, version };
	serial.reset();

	// Calibration date
	std::unique_ptr<Record2Type::CalibrationDate_type> calibrationDate;
	calibrationDate.reset(parseDateTime<Record2Type::CalibrationDate_type>(ConvertMtoWStr(mxGetField(meta, 0, "CalibrationDate"))));

	// Type of probing system: Contacting, non Contacting, Software 
	Record2Type::ProbingSystem_type::Type_type type{ GetProbingSystemTypeEnum(meta) };

	// Type of probing system
	Record2Type::ProbingSystem_type::Identification_type id{ ConvertMtoWStr(mxGetField(meta, 0, "ProbingSystem_Identification")) };
	// Create the probing system type
	Record2Type::ProbingSystem_type probingSystem{ type, id };

	// Create Record2 from collected data
	Record2Type record2{ *date, instrument, *calibrationDate, probingSystem };

	// Delete date records
	date.reset();
	calibrationDate.reset();

	// Look out for comment field
	auto cfield{ mxGetField(meta, 0, "Comment") };
	if (cfield != nullptr)
	{
		if (mxGetNumberOfElements(cfield) > 0)
		{
			record2.Comment(Record2Type::Comment_type(ConvertMtoWStr(cfield)));
		}
		else
			cfield = nullptr;
	}

	if (cfield == nullptr)
	{
		// Issu warning
		mexWarnMsgIdAndTxt("openGPS:writeX3P:Comment",
			"Data should contain a meaningful comment.");
	}

	// Check for creator field
	auto crfield{ mxGetField(meta, 0, "Creator") };
	if (crfield != nullptr)
	{
		// Check for elements in comment string
		if (mxGetNumberOfElements(crfield) > 0)
		{
			// Add comment to record 2
			record2.Creator(Record2Type::Creator_type(ConvertMtoWStr(crfield)));
		}
	}

	// Create MATRIX
	size_t mdims[3] = {};
	mdims[0] = dim[0];
	if (ndimz > 1)
		mdims[1] = dim[1];
	else
		mdims[1] = 1;
	if (ndimz > 2)
		mdims[2] = dim[2];
	else
		mdims[2] = 1;

	bool useBinary{};
	// For more than 5000 Elements use binary encoding
	if (mxGetNumberOfElements(inMatrixZ) >= 5000)
		useBinary = true;

	OGPS_ISO5436_2Handle handle{};

	// Create ISO5436_2 container
	if (ft == FT_pointcloud)
	{
		// This is a list data type
		handle =
			ogps_CreateListISO5436_2(FileNameL.c_str(), nullptr, record1, &record2, mdims[0] * mdims[1] * mdims[2], useBinary);
	}
	else
	{
		// all other are matrix data types
		// Define dimensions
		MatrixDimensionType dimMatrix{ mdims[0], mdims[1], mdims[2] };
		// Create the container
		handle =
			ogps_CreateMatrixISO5436_2(FileNameL.c_str(), nullptr, record1, &record2, dimMatrix, useBinary);
	}

	// Add data points
	auto vector{ ogps_CreatePointVector() };

	// Matrix indices
	size_t u{};
	size_t v{};
	size_t w{};

	// Data pointer to z,y,x
	mxDouble* pdblZ{}, * pdblY{}, * pdblX{};
	mxSingle* pfltZ{}, * pfltY{}, * pfltX{};
	mxUint16* pshrtZ{}, * pshrtY{}, * pshrtX{};
	mxUint32* plngZ{}, * plngY{}, * plngX{};
	// Data type
	unsigned int dtype{};
	if (mxIsDouble(inMatrixZ))
		dtype = 1;
	else if (mxIsSingle(inMatrixZ))
		dtype = 2;
	else if (mxIsUint16(inMatrixZ))
		dtype = 3;
	else if (mxIsUint32(inMatrixZ))
		dtype = 4;

	// Get data pointer
	switch (dtype)
	{
	case 1:
		// Get pointer to z data
		pdblZ = mxGetDoubles(inMatrixZ);
		// Get pointer to xy data if not incremental
		if (!xIncremental)
			pdblX = mxGetDoubles(inMatrixX);
		if (!yIncremental)
			pdblY = mxGetDoubles(inMatrixY);
		break;
	case 2:
		// Get pointer to z data
		pfltZ = mxGetSingles(inMatrixZ);
		// Get pointer to xy data if not incremental
		if (!xIncremental)
			pfltX = mxGetSingles(inMatrixX);
		if (!yIncremental)
			pfltY = mxGetSingles(inMatrixY);
		break;
	case 3:
		// Get pointer to z data
		pshrtZ = mxGetUint16s(inMatrixZ);
		// Get pointer to xy data if not incremental
		if (!xIncremental)
			pshrtX = mxGetUint16s(inMatrixX);
		if (!yIncremental)
			pshrtY = mxGetUint16s(inMatrixY);
		break;
	case 4:
		// Get pointer to z data
		plngZ = mxGetUint32s(inMatrixZ);
		// Get pointer to xy data if not incremental
		if (!xIncremental)
			plngX = mxGetUint32s(inMatrixX);
		if (!yIncremental)
			plngY = mxGetUint32s(inMatrixY);
		break;
	}

	size_t index{};
	for (w = 0; w < mdims[2]; ++w)
	{
		for (v = 0; v < mdims[1]; ++v)
		{
			for (u = 0; u < mdims[0]; ++u)
			{
				bool isValid{ true };
				// Set z-coordinate
				switch (dtype)
				{
				case 1:
					isValid = !mxIsNaN(*pdblZ)
						&& (xIncremental ? true : !mxIsNaN(*pdblX))
						&& (yIncremental ? true : !mxIsNaN(*pdblY));
					if (isValid)
					{
						ogps_SetDoubleZ(vector, *(pdblZ++));
						if (!xIncremental)
							ogps_SetDoubleX(vector, *(pdblX++));
						if (!yIncremental)
							ogps_SetDoubleY(vector, *(pdblY++));
					}
					else
					{
						++pdblZ;
						if (!xIncremental)
							++pdblX;
						if (!yIncremental)
							++pdblY;
					}
					break;
				case 2:
					isValid = !mxIsNaN(*pfltZ)
						&& (xIncremental ? true : !mxIsNaN(*pfltX))
						&& (yIncremental ? true : !mxIsNaN(*pfltY));
					if (isValid)
					{
						ogps_SetFloatZ(vector, *(pfltZ++));
						if (!xIncremental)
							ogps_SetFloatX(vector, *(pfltX++));
						if (!yIncremental)
							ogps_SetFloatY(vector, *(pfltY++));
					}
					else
					{
						++pfltZ;
						if (!xIncremental)
							++pfltX;
						if (!yIncremental)
							++pfltY;
					}
					break;
				case 3:
					// BUG: This is not fully implemented yet!
					// Set z-value
					ogps_SetInt16Z(vector, *(pshrtZ++));
					break;
				case 4:
					// BUG: This is not fully implemented yet!
					// Set z-value
					ogps_SetInt32Z(vector, *(plngZ++));
					break;
				}

				// 2. if the z axis is of absolute type and the
				// other two are incremental, we simply set up just z
				// values and leave x an y values untouched (missing).

				if (isValid)
				{
					// 3. Write into document
					// Check for feature type: PCL have list, other have matrix
					if (ft == FT_pointcloud)
						// Unsorted point list
						ogps_SetListPoint(handle, index++, vector);
					else
						// Matrix organized point list
						ogps_SetMatrixPoint(handle, u, v, w, vector);
				}
				else
				{
					// PCL does not have invalid points
					if (ft != FT_pointcloud)
						// Set data point to invalid
						ogps_SetMatrixPoint(handle, u, v, w, nullptr);
				}
			}
		}
	}
	ogps_FreePointVector(&vector);

	if (bHasExtension)
	{
		ogps_AppendVendorSpecific(handle, OGPS_VEXT_URI, VendorFileNameL.c_str());
	}

	ogps_GetDocument(handle);
	if (ogps_HasError())
	{
		mexErrMsgIdAndTxt("openGPS:openX3P:XMLDocument", "Could not access XML document!");
	}

	if (hasPointInfo)
		plhs[0] = GetPointInfoStructure(handle);

	ogps_WriteISO5436_2(handle);
	ogps_CloseISO5436_2(&handle);

	if (ogps_HasError())
	{
		ostringstream msg;
		msg << "Error writing X3P-file name \"" << FileNameL << "\"!" << endl
			<< ogps_GetErrorMessage() << endl
			<< ogps_GetErrorDescription() << endl
			<< ends;
		mexErrMsgIdAndTxt("openGPS:writeX3P:FileOpen", msg.str().c_str());
		return;
	}
}
