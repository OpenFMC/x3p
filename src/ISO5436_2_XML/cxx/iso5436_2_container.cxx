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

#include "iso5436_2_container.hxx"

#include <opengps/cxx/point_vector.hxx>
#include <opengps/cxx/data_point.hxx>

#include "point_vector_parser_builder.hxx"
#include "point_vector_parser.hxx"

#include "xml_point_vector_reader_context.hxx"
#include "xml_point_vector_writer_context.hxx"

#include "binary_lsb_point_vector_reader_context.hxx"
#include "binary_msb_point_vector_reader_context.hxx"

#include "binary_lsb_point_vector_writer_context.hxx"
#include "binary_msb_point_vector_writer_context.hxx"

#include "vector_buffer_builder.hxx"
#include "vector_buffer.hxx"

#include "point_vector_proxy_context_matrix.hxx"
#include "point_vector_proxy_context_list.hxx"

#include "environment.hxx"

#include "point_vector_iostream.hxx"

#include "zip_stream_buffer.hxx"

#include <limits>
#include <iostream>
#include <fstream>
#include <memory>
#include <iomanip>
#include <sstream>
#include <cmath>

/* zlib/minizip header files */
#include <unzip.h>
#include <zip.h>

/* md5 */
#include "../xyssl/md5.h"

#include "stdafx.hxx"

#define _OPENGPS_ZIP_CHUNK_MAX (256*1024)
#define _OPENGPS_FILE_URI_PREF _T("file:///")
#define _OPENGPS_WHITESPACE _T(" ")
#define _OPENGPS_URI_WHITESPACE _T("%20")

/*!
 * Converts a value into a smaller type.
 * Throws an exception on overflow, so this conversion is safe.
 * @param value The value to be converted.
 * @returns Returns the same value, but as a cast to a smaller data type.
 */
template<typename T>
static inline size_t ConvertToSizeT(T value)
{
	if (static_cast<T>(std::numeric_limits<size_t>::max()) < value)
	{
		throw Exception(OGPS_ExOverflow,
			_EX_T("An integer overflow occured."),
			_EX_T("The amount of point data stored as a list of points or at least one of the dimensions of the topology matrix did exceed the maximum value that could be handled by an unsigned long data type. To avoid this problem either reduce the amount of point data stored or the dimensions of the matrix topology to be able to fit the upper bound of an unsigned long data type."),
			_EX_T("ConvertSize"));
	}

	return static_cast<size_t>(value);
}

/*!
 * Converts a value into a smaller type.
 * Throws an exception on overflow, so this conversion is safe.
 * @param value The value to be converted.
 * @returns Returns the same value, but casted to a shorter data type.
 */
template<typename T>
static inline OGPS_Int32 ConvertToInt32(T value)
{
	if (static_cast<T>(std::numeric_limits<OGPS_Int32>::max()) < value)
	{
		throw Exception(OGPS_ExOverflow,
			_EX_T("An integer overflow occured."),
			_EX_T("Point data of implicit axes is derived from the current point index. Since point indexes are of unsigned type, very large values can not be converted into a signed integer. But within a point vector the components are of signed integer and therefore the current index value could not be saved in a point vector. In other words: reduce the amount of point data stored to be able to fit the upper bound of a signed integer data type."),
			_EX_T("ConvertToInt32"));
	}

	return static_cast<OGPS_Int32>(value);
}

/*!
 * Multiplication of two values and conversion of the result to a shorter data type.
 * Throws an exception on overflow, so this conversion is safe.
 * @param value1 The first value of the operands.
 * @param value2 The second value of the operands.
 * @returns Returns the result of their multiplication, but casted to a shorter data type.
 */
static inline size_t SafeMultipilcation(unsigned long long value1, unsigned long long value2)
{
	if (value1 > (std::numeric_limits<size_t>::max() / value2))
	{
		throw Exception(OGPS_ExOverflow,
			_EX_T("An integer overflow occured due to a multiplication operation."),
			_EX_T("Two values of the same data type were multiplied with each other, but the result is too large to fit the same data type. Your point data is layed out by a matrix topology. Then every data point of a single axis may be indexed successfully of their own, but a single index might not be sufficient to be able of indexing all three axes components at once. Since this is a required property of the current implementation, the amount of point data needs to be reduced, so that indexing of all axes components with a single index becomes possible again."),
			_EX_T("SafeMultipilcation"));
	}

	return static_cast<size_t>(value1 * value2);
}

ISO5436_2Container::ISO5436_2Container(
	const String& file,
	const String& temp)
	:m_FilePath { file },
	m_TempPath{ temp },
	m_CompressionLevel { Z_DEFAULT_COMPRESSION }
{
}

ISO5436_2Container::~ISO5436_2Container() = default;

void ISO5436_2Container::Open()
{
	if (HasDocument())
	{
		throw Exception(
			OGPS_ExInvalidOperation,
			_EX_T("The document already exists."),
			_EX_T("Close the existing document before you open another."),
			_EX_T("OpenGPS::ISO5436_2Container::Open"));
	}

	try
	{
		CreateTempDir();

		try
		{
			Decompress();
			CreatePointBuffer();
		}
		catch (...)
		{
			Reset();
			RemoveTempDir();
			throw;
		}

		RemoveTempDir();
	}
	catch (...)
	{
		Reset();
		throw;
	}

	m_IsCreating = false;

	ValidateDocument();
	TestChecksums();
}

void ISO5436_2Container::Decompress()
{
	assert(HasTempDir());

	DecompressMain();
	ReadDocument();
	DecompressChecksum();
	DecompressDataBin();
}

bool ISO5436_2Container::VerifyChecksum(const String& filePath, const unsigned char* checksum, size_t size) const
{
	assert(filePath.size() > 0);

	if (!checksum || size != 16)
	{
		return false;
	}

	String filePathBuffer(filePath);
	std::array<unsigned char, 16> md5{};

	if (!md5_file(filePathBuffer.ToChar(), md5.data()))
	{
		for (size_t n = 0; n < md5.size(); ++n)
		{
			if (checksum[n] != md5[n])
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

bool ISO5436_2Container::VerifyChecksum(const String& filePath, std::array<unsigned char, 16>& checksum) const
{
	return VerifyChecksum(filePath, checksum.data(), checksum.size());
}

void ISO5436_2Container::VerifyMainChecksum()
{
	assert(HasDocument());

	const auto file{ GetMainFileName() };
	const auto chksFileName{ GetChecksumFileName() };

	std::array<unsigned char, 16> checksum{};
	if (ReadMd5FromFile(chksFileName, checksum))
	{
		m_MainChecksum = VerifyChecksum(file, checksum);
		return;
	}

	m_MainChecksum = false;
}

void ISO5436_2Container::VerifyDataBinChecksum()
{
	assert(HasDocument() && IsBinary());

	const auto file{ GetPointDataFileName() };

	if (m_Document->Record3().DataLink().present())
	{
		const auto& md5{ m_Document->Record3().DataLink()->MD5ChecksumPointData() };
		m_DataBinChecksum = VerifyChecksum(file, reinterpret_cast<const unsigned char*>(md5.data()), md5.size());
		return;
	}

	m_DataBinChecksum = false;
}

void ISO5436_2Container::VerifyValidBinChecksum()
{
	assert(HasDocument() && IsBinary() && HasValidPointsLink());

	const auto file{ GetValidPointsFileName() };

	if (m_Document->Record3().DataLink().present())
	{
		const auto& md5{ m_Document->Record3().DataLink()->MD5ChecksumValidPoints() };
		if (md5.present())
		{
			m_ValidBinChecksum = VerifyChecksum(file, reinterpret_cast<const unsigned char*>(md5->data()), md5->size());
			return;
		}
	}

	m_ValidBinChecksum = false;
}

bool ISO5436_2Container::ReadMd5FromFile(const String& fileName, std::array<unsigned char, 16>& checksum) const
{
#ifdef _UNICODE
#if !_WIN32
	String tempFileName(fileName);
	std::wifstream file(tempFileName.ToChar());
#else
	std::wifstream file(fileName.c_str());
#endif
#else
	std::ifstream file(fileName.c_str());
#endif

	String md5;
	file >> md5;
	file.close();

	return md5.ConvertToMd5(checksum);
}

void ISO5436_2Container::Create(
	const Schemas::ISO5436_2::Record1Type& record1,
	const Schemas::ISO5436_2::Record2Type* record2,
	const Schemas::ISO5436_2::MatrixDimensionType& matrixDimension,
	bool useBinaryData)
{
	if (HasDocument())
	{
		throw Exception(
			OGPS_ExInvalidOperation,
			_EX_T("The document already exists."),
			_EX_T("Close the existing document before you create another."),
			_EX_T("ISO5436_2Container::Create"));
	}

	CreateDocument(&record1, record2, &matrixDimension, 0, useBinaryData);
}

void ISO5436_2Container::Create(
	const Schemas::ISO5436_2::Record1Type& record1,
	const Schemas::ISO5436_2::Record2Type* record2,
	size_t listDimension,
	bool useBinaryData)
{
	if (HasDocument())
	{
		throw Exception(
			OGPS_ExInvalidOperation,
			_EX_T("The document already exists."),
			_EX_T("Close the existing document before you create another."),
			_EX_T("ISO5436_2Container::Create"));
	}

	CreateDocument(&record1, record2, nullptr, listDimension, useBinaryData);
}

PointIteratorAutoPtr ISO5436_2Container::CreateNextPointIterator()
{
	CheckDocumentInstance();
	return std::make_unique<PointIteratorImpl>(shared_from_this(), true, IsMatrix());
}

PointIteratorAutoPtr ISO5436_2Container::CreatePrevPointIterator()
{
	CheckDocumentInstance();
	return std::make_unique<PointIteratorImpl>(shared_from_this(), false, IsMatrix());
}

void ISO5436_2Container::SetMatrixPoint(
	size_t u,
	size_t v,
	size_t w,
	const PointVector* vector)
{
	CheckDocumentInstance();

	assert(IsMatrix());
	assert(m_PointVector);
	assert(m_ProxyContext);
#ifdef _DEBUG
	OGPS_Int32 value_x{ -1 }, value_y{ -1 };
	if (vector != nullptr)
	{
		if (vector->GetX()->GetPointType() == OGPS_Int32PointType)
		{
			vector->GetX()->Get(&value_x);
		}
		if (vector->GetY()->GetPointType() == OGPS_Int32PointType)
		{
			vector->GetY()->Get(&value_y);
		}
	}

	assert(!vector || (IsIncrementalX() && vector->GetX()->GetPointType() == OGPS_MissingPointType || (value_x >= 0 && u == static_cast<size_t>(value_x))) || (!IsIncrementalX() && vector->GetX()->GetPointType() != OGPS_MissingPointType));
	assert(!vector || (IsIncrementalY() && vector->GetY()->GetPointType() == OGPS_MissingPointType || (value_y >= 0 && v == static_cast<size_t>(value_y))) || (!IsIncrementalY() && vector->GetY()->GetPointType() != OGPS_MissingPointType));
	assert(!vector || vector->GetZ()->GetPointType() != OGPS_MissingPointType);
#endif

	if (!m_ProxyContext->IsMatrix())
	{
		throw Exception(
			OGPS_ExInvalidOperation,
			_EX_T("Attempt to add a data point in matrix format when a point list is supported only."),
			_EX_T("The current instance of the document does not support the matrix topology."),
			_EX_T("OpenGPS::ISO5436_2Container::SetMatrixPoint"));
	}

	std::dynamic_pointer_cast<PointVectorProxyContextMatrix>(m_ProxyContext)->SetIndex(u, v, w);

	if (vector)
	{
		m_PointVector->Set(*vector);
	}

	GetVectorBuffer()->GetValidityProvider()->SetValid(m_ProxyContext->GetIndex(), vector != nullptr);
}

void ISO5436_2Container::GetMatrixPoint(
	size_t u,
	size_t v,
	size_t w,
	PointVector& vector)
{
	CheckDocumentInstance();

	assert(IsMatrix());
	assert(m_PointVector);
	assert(m_ProxyContext);

	if (!m_ProxyContext->IsMatrix())
	{
		throw Exception(
			OGPS_ExInvalidOperation,
			_EX_T("Attempt to read a data point in matrix format when a point list is supported only."),
			_EX_T("The current instance of the document does not support the matrix topology."),
			_EX_T("OpenGPS::ISO5436_2Container::GetMatrixPoint"));
	}

	std::dynamic_pointer_cast<PointVectorProxyContextMatrix>(m_ProxyContext)->SetIndex(u, v, w);

	if (GetVectorBuffer()->GetValidityProvider()->IsValid(m_ProxyContext->GetIndex()))
	{
		m_PointVector->Get(vector);
	}
	else
	{
		vector.GetX()->Reset();
		vector.GetY()->Reset();
		vector.GetZ()->Reset();
	}

	if (IsIncrementalX())
	{
		assert(vector.GetX()->GetPointType() == OGPS_MissingPointType);

		vector.SetX(ConvertToInt32(u));
	}

	if (IsIncrementalY())
	{
		assert(vector.GetY()->GetPointType() == OGPS_MissingPointType);

		vector.SetY(ConvertToInt32(v));
	}
}

void ISO5436_2Container::SetListPoint(
	size_t index,
	const PointVector& vector)
{
	CheckDocumentInstance();

	assert(!IsMatrix());
	assert(m_PointVector);
	assert(m_ProxyContext);

	assert((IsIncrementalX() && vector.GetX()->GetPointType() == OGPS_MissingPointType) || (!IsIncrementalX() && vector.GetX()->GetPointType() != OGPS_MissingPointType) || (IsIncrementalX() && vector.GetX()->GetPointType() == OGPS_Int32PointType));
	assert((IsIncrementalY() && vector.GetY()->GetPointType() == OGPS_MissingPointType) || (!IsIncrementalY() && vector.GetY()->GetPointType() != OGPS_MissingPointType) || (IsIncrementalY() && vector.GetY()->GetPointType() == OGPS_Int32PointType));
	assert(vector.GetZ()->GetPointType() != OGPS_MissingPointType);

	if (m_ProxyContext->IsMatrix())
	{
		throw Exception(
			OGPS_ExInvalidOperation,
			_EX_T("Attempt to add a data point to a point list when matrix topology is supported only."),
			_EX_T("Data points of the current instance of the document must be accessed as a point list only."),
			_EX_T("OpenGPS::ISO5436_2Container::SetListPoint"));
	}

	std::dynamic_pointer_cast<PointVectorProxyContextList>(m_ProxyContext)->SetIndex(index);

	m_PointVector->Set(vector);
}

void ISO5436_2Container::GetListPoint(
	size_t index,
	PointVector& vector)
{
	CheckDocumentInstance();

	assert(!IsMatrix());
	assert(m_PointVector);
	assert(m_ProxyContext);

	if (m_ProxyContext->IsMatrix())
	{
		throw Exception(
			OGPS_ExInvalidOperation,
			_EX_T("Attempt to read a data point from a point list when matrix topology is supported only."),
			_EX_T("Data points of the current instance of the document must be accessed as a point list only."),
			_EX_T("OpenGPS::ISO5436_2Container::GetListPoint"));
	}

	std::dynamic_pointer_cast<PointVectorProxyContextList>(m_ProxyContext)->SetIndex(index);

	m_PointVector->Get(vector);

	if (IsIncrementalX())
	{
		assert(vector.GetX()->GetPointType() == OGPS_MissingPointType);

		vector.SetX(ConvertToInt32(index));
	}

	if (IsIncrementalY())
	{
		assert(vector.GetY()->GetPointType() == OGPS_MissingPointType);

		vector.SetY(ConvertToInt32(index));
	}
}

void ISO5436_2Container::GetMatrixCoord(
	size_t u,
	size_t v,
	size_t w,
	OGPS_Double* x,
	OGPS_Double* y,
	OGPS_Double* z)
{
	assert(HasDocument());
	assert(IsMatrix());

	PointVector vector;
	GetMatrixPoint(u, v, w, vector);
	ConvertPointToCoord(vector, x, y, z);
}

bool ISO5436_2Container::IsMatrixCoordValid(
	size_t u,
	size_t v,
	size_t w)
{
	CheckDocumentInstance();

	assert(IsMatrix());

	if (!m_ProxyContext->IsMatrix())
	{
		throw Exception(
			OGPS_ExInvalidOperation,
			_EX_T("Attempt to verify if a data point in matrix topology does exist when a point list is supported only."),
			_EX_T("Data points within point lists are always valid. It makes sense to verify when using matrix topology only."),
			_EX_T("OpenGPS::ISO5436_2Container::IsMatrixCoordValid"));
	}

	std::dynamic_pointer_cast<PointVectorProxyContextMatrix>(m_ProxyContext)->SetIndex(u, v, w);

	return GetVectorBuffer()->GetValidityProvider()->IsValid(m_ProxyContext->GetIndex());
}

void ISO5436_2Container::GetListCoord(
	size_t index,
	OGPS_Double* x,
	OGPS_Double* y,
	OGPS_Double* z)
{
	assert(HasDocument());
	assert(!IsMatrix());

	PointVector vector;
	GetListPoint(index, vector);
	ConvertPointToCoord(vector, x, y, z);
}

void ISO5436_2Container::ConvertPointToCoord(
	const PointVector& vector,
	OGPS_Double* x,
	OGPS_Double* y,
	OGPS_Double* z)
{
	OGPS_Double* myx = vector.GetX()->IsValid() ? x : nullptr;
	OGPS_Double* myy = vector.GetY()->IsValid() ? y : nullptr;
	OGPS_Double* myz = vector.GetZ()->IsValid() ? z : nullptr;

	vector.GetXYZ(myx, myy, myz);

	if (myx)
	{
		*myx *= GetIncrementX();
		*myx += GetOffsetX();
	}
	else
	{
		if (x)
		{
			*x = std::numeric_limits<OGPS_Double>::quiet_NaN();
		}
	}

	if (myy)
	{
		*myy *= GetIncrementY();
		*myy += GetOffsetY();
	}
	else
	{
		if (y)
		{
			*y = std::numeric_limits<OGPS_Double>::quiet_NaN();
		}
	}

	if (myz)
	{
		*myz += GetOffsetZ();
	}
	else
	{
		if (z)
		{
			*z = std::numeric_limits<OGPS_Double>::quiet_NaN();
		}
	}
}

Schemas::ISO5436_2::ISO5436_2Type* ISO5436_2Container::GetDocument()
{
	return m_Document.get();
}

void ISO5436_2Container::Write(int compressionLevel)
{
	assert(compressionLevel >= Z_DEFAULT_COMPRESSION && compressionLevel <= Z_BEST_COMPRESSION);

	CheckDocumentInstance();

	m_CompressionLevel = compressionLevel;

	Compress();

	ValidateDocument();
}

void ISO5436_2Container::Close()
{
	Reset();
	RemoveTempDir();
}

bool ISO5436_2Container::IsMatrix() const
{
	CheckDocumentInstance();

	// is matrix present?
	if (m_Document->Record3().MatrixDimension().present())
	{
		// ensure there's either matrix or list
		assert(!m_Document->Record3().ListDimension().present());

		return true;
	}

	// the point list should be present instead
	if (!m_Document->Record3().ListDimension().present())
	{
		throw Exception(
			OGPS_ExInvalidOperation,
			_EX_T("It could not be decided whether the X3P instance is of either matrix or list type."),
			_EX_T("Point vectors can be either seen as topologically ordered matrix space or unordered sets. Make your decision within the XML document before using this method."),
			_EX_T("ISO5436_2Container::IsMatrix"));
	}

	return false;
}

bool ISO5436_2Container::IsProfile() const
{
	assert(HasDocument());

	const auto& featureType{ m_Document->Record1().FeatureType() };
	if (featureType == OGPS_FEATURE_TYPE_PROFILE_NAME)
	{
		return true;
	}
	else if (featureType == OGPS_FEATURE_TYPE_SURFACE_NAME)
	{
		return false;
	}
	else if (featureType == OGPS_FEATURE_TYPE_POINTCLOUD_NAME)
	{
		return false;
	}

	throw Exception(
		OGPS_ExOverflow,
		_EX_T("The feature type sepcified is unknown."),
		_EX_T("Only profiles, surface, and point cloud feature types are valid."),
		_EX_T("ISO5436_2Container::IsProfile"));
}

bool ISO5436_2Container::IsPointCloud() const
{
	assert(HasDocument());

	const auto& featureType{ m_Document->Record1().FeatureType() };
	if (featureType == OGPS_FEATURE_TYPE_POINTCLOUD_NAME)
	{
		return true;
	}
	else if (featureType == OGPS_FEATURE_TYPE_SURFACE_NAME)
	{
		return false;
	}
	else if (featureType == OGPS_FEATURE_TYPE_PROFILE_NAME)
	{
		return false;
	}

	throw Exception(
		OGPS_ExOverflow,
		_EX_T("The feature type sepcified is unknown."),
		_EX_T("Only profiles, surface, and point cloud feature types are valid."),
		_EX_T("ISO5436_2Container::IsPointCloud"));
}

void ISO5436_2Container::GetMatrixDimensions(
	size_t* size_u,
	size_t* size_v,
	size_t* size_w) const
{
	CheckDocumentInstance();

	if (!IsMatrix())
	{
		throw Exception(
			OGPS_ExInvalidOperation,
			_EX_T("It is assumed that point data is stored in matrix topology but actually it is not."),
			_EX_T("The dimensions of the matrix wherein point data is stored cannot be obtained because this document stores point data in a list structure."),
			_EX_T("ISO5436_2Container::GetMatrixDimensions"));
	}

	// Extract dimensions
	if (size_u)
	{
		*size_u = ConvertToSizeT(m_Document->Record3().MatrixDimension()->SizeX());
	}
	if (size_v)
	{
		*size_v = ConvertToSizeT(m_Document->Record3().MatrixDimension()->SizeY());
	}
	if (size_w)
	{
		*size_w = ConvertToSizeT(m_Document->Record3().MatrixDimension()->SizeZ());
	}
}

size_t ISO5436_2Container::GetListDimension() const
{
	CheckDocumentInstance();

	if (IsMatrix())
	{
		throw Exception(
			OGPS_ExInvalidOperation,
			_EX_T("It is assumed that point data is stored in list structure but actually it is not."),
			_EX_T("The size of the list structure wherin point data is stored cannot be obtained because this document stores point data in a matrix topology."),
			_EX_T("ISO5436_2Container::GetListDimension"));
	}

	// Extract dimensions
	return ConvertToSizeT(m_Document->Record3().ListDimension().get());
}

String ISO5436_2Container::CreateContainerTempFilePath() const
{
	auto env = Environment::GetInstance();

	return env->ConcatPathes(GetTempDir(), env->GetUniqueName());
}

String ISO5436_2Container::GetFullFilePath() const
{
	// TODO
	return m_FilePath;
}

const String& ISO5436_2Container::GetFilePath() const
{
	return m_FilePath;
}

const String& ISO5436_2Container::GetTempDir() const
{
	assert(m_TempPath.length() > 0);

	return m_TempPath;
}

String ISO5436_2Container::GetMainArchiveName() const
{
	return _OPENGPS_XSD_ISO5436_MAIN_PATH;
}

String ISO5436_2Container::GetMainFileName() const
{
	return Environment::GetInstance()->ConcatPathes(GetTempDir(), GetMainArchiveName());
}

String ISO5436_2Container::GetPointDataArchiveName() const
{
	assert(HasDocument() && IsBinary());

	return m_Document->Record3().DataLink()->PointDataLink();
}

String ISO5436_2Container::GetPointDataFileName()
{
	if (m_PointDataFileName.empty())
	{
		auto env = Environment::GetInstance();
		m_PointDataFileName = env->ConcatPathes(GetTempDir(), env->GetUniqueName());
	}

	return m_PointDataFileName;
}

String ISO5436_2Container::GetValidPointsArchiveName() const
{
	assert(HasDocument() && IsBinary() && HasValidPointsLink());

	return m_Document->Record3().DataLink()->ValidPointsLink().get();
}

String ISO5436_2Container::GetValidPointsFileName()
{
	if (m_ValidPointsFileName.empty())
	{
		auto env = Environment::GetInstance();
		m_ValidPointsFileName = env->ConcatPathes(GetTempDir(), env->GetUniqueName());
	}

	return m_ValidPointsFileName;
}

String ISO5436_2Container::GetChecksumArchiveName() const
{
	assert(HasDocument());

	return m_Document->Record4().ChecksumFile();
}

String ISO5436_2Container::GetChecksumFileName() const
{
	return Environment::GetInstance()->ConcatPathes(GetTempDir(), GetChecksumArchiveName());
}

void ISO5436_2Container::DecompressMain() const
{
	const auto src{ GetMainArchiveName() };
	const auto dst{ GetMainFileName() };

	_VERIFY(Decompress(src, dst), true);
}

void ISO5436_2Container::DecompressChecksum()
{
	assert(HasDocument());

	const auto src{ GetChecksumArchiveName() };
	const auto dst{ GetChecksumFileName() };

	_VERIFY(Decompress(src, dst), true);
	VerifyMainChecksum();
}

void ISO5436_2Container::DecompressDataBin()
{
	if (IsBinary())
	{
		_VERIFY(Decompress(GetPointDataArchiveName(), GetPointDataFileName()), true);
		VerifyDataBinChecksum();

		if (HasValidPointsLink())
		{
			_VERIFY(Decompress(GetValidPointsArchiveName(), GetValidPointsFileName()), true);
			VerifyValidBinChecksum();
		}
	}
}

bool ISO5436_2Container::Decompress(const String& src, const String& dst, const bool fileNotFoundAllowed) const
{
	auto filePath{ GetFullFilePath() };
	auto handle{ unzOpen(filePath.ToChar()) };

	if (!handle)
	{
		/* Todo: Wiora: Should provide information about filename */
		throw Exception(
			OGPS_ExGeneral,
			_EX_T("The X3P archive to be decompressed could not be opened."),
			_EX_T("Verify whether the file exists and if you have sufficient access privilegs."),
			_EX_T("OpenGPS::ISO5436_2Container::Decompress"));
	}

	bool fileNotFound{};
	bool fileNotOpened{};
	bool targetNotWritten{};
	bool success{};

	try
	{
		// Locate the document/file to be decompressed in the archive.
		String srcbuf(src);
		if (unzLocateFile(handle, srcbuf.ToChar(), 2 /* case insensitive search */) == UNZ_OK)
		{
			// Open the current file for reading.
			if (unzOpenCurrentFile(handle) == UNZ_OK)
			{
				// Need information about file size
				unz_file_info fileInfo;
				if (unzGetCurrentFileInfo(handle, &fileInfo, nullptr, 0, nullptr, 0, nullptr, 0) == UNZ_OK)
				{
					// Open binary target stream for uncompressed data
					OutputBinaryFileStream binaryTarget(dst);

					if (!binaryTarget.fail())
					{
						uLong written{ 0 };
						const auto length{ fileInfo.uncompressed_size };

						// Don't uncompress this file as a whole, but in loops
						// of a predefined maximum chunk size. Otherwise we
						// might get out of memory...
						while (written < length)
						{
							auto size{ static_cast<int>(std::min(length - written, static_cast<uLong>(_OPENGPS_ZIP_CHUNK_MAX))) };

							assert(size > 0);

							auto buffer = std::make_unique<char[]>(size);
							auto bytesCopied{ unzReadCurrentFile(handle, buffer.get(), size) };

							if (bytesCopied == size)
							{
								binaryTarget.write(buffer.get(), bytesCopied);
							}

							if (bytesCopied != size || binaryTarget.fail())
							{
								written = 0;
								break;
							}

							written += size;
						}

						success = (written == length);
					}
					else
					{
						targetNotWritten = true;
					}

					binaryTarget.close();

					targetNotWritten = targetNotWritten || binaryTarget.fail();
				}
				else
				{
					fileNotOpened = true;
				}
			}
			else
			{
				fileNotOpened = true;
			}
		}
		else
		{
			fileNotFound = true;
		}
	}
	catch (...)
	{
		_VERIFY(unzClose(handle), UNZ_OK);
		throw;
	}

	_VERIFY(unzClose(handle), UNZ_OK);

	if (fileNotFound && !fileNotFoundAllowed)
	{
		if (fileNotFound)
		{
			throw Exception(
				OGPS_ExGeneral,
				_EX_T("The X3P container document does not contain the supposed resource."),
				_EX_T("For a X3P archive to be valid there must exist an instance of the ISO5436-2 XML specification in its root named main.xml. Also all additional resources given in main.xml must be contained herein."),
				_EX_T("OpenGPS::ISO5436_2Container::Decompress"));
		}

		if (fileNotOpened)
		{
			throw Exception(
				OGPS_ExGeneral,
				_EX_T("A resource successfully located within the X3P archive could not be opened."),
				_EX_T("Please check whether the X3P archive is corrupted using a zip file utility of your choice, then repair the archive and try again."),
				_EX_T("OpenGPS::ISO5436_2Container::Decompress"));
		}

		if (targetNotWritten)
		{
			throw Exception(
				OGPS_ExGeneral,
				_EX_T("A resource contained in an X3P archive could not be extracted to its target file."),
				_EX_T("Please check your access permissions on the temporary directory."),
				_EX_T("OpenGPS::ISO5436_2Container::Decompress"));
		}

		if (!success)
		{
			throw Exception(
				OGPS_ExGeneral,
				_EX_T("A resource contained in an X3P archive could not be extracted to its target file."),
				_EX_T("Please check if you have enough space left on your filesystem."),
				_EX_T("OpenGPS::ISO5436_2Container::Decompress"));
		}
	}

	return success;
}

void ISO5436_2Container::Compress()
{
	bool noHandleCreated{};
	bool vendorfilesAdded{ true };
	bool success{};

	String systemErrorMessage;

	CreateTempDir();

	auto targetZip{ CreateContainerTempFilePath() };
	auto handle{ zipOpen(targetZip.ToChar(), APPEND_STATUS_CREATE) };

	try
	{
		if (handle)
		{
			try
			{
				SavePointBuffer(handle);
				vendorfilesAdded = WriteVendorSpecific(handle);
				SaveValidPointsLink(handle);
				SaveXmlDocument(handle);

				success = true;
			}
			catch (...)
			{
				_VERIFY(zipClose(handle, nullptr), ZIP_OK);
				throw;
			}
			_VERIFY(zipClose(handle, nullptr), ZIP_OK);
		}
		else
		{
			noHandleCreated = true;
		}

		if (success)
		{
			if (!Environment::GetInstance()->RenameFile(targetZip, GetFullFilePath()))
			{
				systemErrorMessage = Environment::GetInstance()->GetLastErrorMessage();
			}
		}
	}
	catch (...)
	{
		RemoveTempDir();
		throw;
	}

	RemoveTempDir();

	if (noHandleCreated)
	{
		throw Exception(
			OGPS_ExGeneral,
			_EX_T("The X3P archive could not be written."),
			_EX_T("Zlib library could not create a handle to the X3P target file. Check your permissions on the temporary directory."),
			_EX_T("OpenGPS::ISO5436_2Container::Compress"));
	}

	if (!systemErrorMessage.empty())
	{
		throw Exception(
			OGPS_ExGeneral,
			_EX_T("The X3P archive could not be copied from the temporary directory to its target filename."),
			systemErrorMessage.ToChar(),
			_EX_T("OpenGPS::ISO5436_2Container::Compress"));
	}

	if (!vendorfilesAdded)
	{
		throw Exception(
			OGPS_ExWarning,
			_EX_T("At least some of the vendorspecific files supplied could not be added to the X3P archive."),
			_EX_T("Please verify that the files containing additional data still exist on the filesystem at this time. Also check for enuogh file system space left and permissions."),
			_EX_T("OpenGPS::ISO5436_2Container::Compress"));
	}
}

void ISO5436_2Container::CreateDocument(
	const Schemas::ISO5436_2::Record1Type* record1,
	const Schemas::ISO5436_2::Record2Type* record2,
	const Schemas::ISO5436_2::MatrixDimensionType* matrixDimension,
	size_t listDimension,
	bool useBinaryData)
{
	assert(!HasDocument());
	assert(record1);
	assert((matrixDimension != nullptr && listDimension == 0) || (matrixDimension == nullptr && listDimension > 0));

	try
	{
		Schemas::ISO5436_2::Record3Type record3;
		Schemas::ISO5436_2::Record4Type record4(_OPENGPS_XSD_ISO5436_MAIN_CHECKSUM_PATH);

		m_Document = std::make_unique<Schemas::ISO5436_2::ISO5436_2Type>(*record1, record3, record4);

		if (record2)
		{
			m_Document->Record2(*record2);
		}

		if (matrixDimension)
		{
			m_Document->Record3().MatrixDimension(*matrixDimension);
		}
		else
		{
			m_Document->Record3().ListDimension(listDimension);
		}

		if (useBinaryData)
		{
			Schemas::ISO5436_2::DataLinkType dataLink((Schemas::ISO5436_2::DataLinkType::PointDataLink_type)(_OPENGPS_XSD_ISO5436_DATALINK_PATH), (Schemas::ISO5436_2::DataLinkType::MD5ChecksumPointData_type)NULL); // compiler error guess using the constructor with DataLinkType and two default 0 is ok here
			m_Document->Record3().DataLink(dataLink);
		}
		else
		{
			Schemas::ISO5436_2::DataListType dataList;
			m_Document->Record3().DataList(dataList);
		}

		// Build and setup internal point buffer
		VectorBufferBuilder v_builder;
		if (BuildVectorBuffer(v_builder))
		{
			m_VectorBuffer = v_builder.GetBuffer();
		}
		m_ProxyContext = CreatePointVectorProxyContext();
		m_PointVector = GetVectorBuffer()->CreatePointVectorProxy(m_ProxyContext);
	}
	catch (...)
	{
		Reset();
		throw;
	}

	m_IsCreating = true;
}

void ISO5436_2Container::ReadDocument()
{
	assert(!HasDocument());

	ReadXmlDocument();

	assert(HasDocument());
}

void ISO5436_2Container::ReadXmlDocument()
{
	xml_schema::properties props;
	if (!ConfigureNamespaceMap(props))
	{
		throw Exception(
			OGPS_ExInvalidOperation,
			_EX_T("The location of your local iso5436_2.xsd specification could not be extracted."),
			_EX_T("Verify that the environment variable OPENGPS_LOCATION is set and correctly points to the root of your local openGPS(R) installation."),
			_EX_T("OpenGPS::ISO5436_2Container::ReadXmlDocument"));
	}

	String xmlFilePath = GetMainFileName();

	try
	{
		m_Document = Schemas::ISO5436_2::ISO5436_2(xmlFilePath, 0, props);
	}
	catch (const xml_schema::exception& e)
	{
#ifdef _UNICODE
		std::wostringstream dump;
#else
		std::ostringstream dump;
#endif

		dump << e;

		String m(dump.str());

		throw Exception(
			OGPS_ExGeneral,
			e.what(),
			m.ToChar(),
			nullptr);
	}
}

void ISO5436_2Container::CreatePointBuffer()
{
	assert(!HasVectorBuffer());
	assert(HasDocument());

	// Build and setup internal point buffer
	VectorBufferBuilder v_builder;
	if (BuildVectorBuffer(v_builder))
	{
		m_VectorBuffer = v_builder.GetBuffer();
	}

	auto vectorBuffer{ GetVectorBuffer() };

	// read valid points file
	if (HasValidPointsLink() && vectorBuffer->HasValidityBuffer())
	{
		InputBinaryFileStream vstream(GetValidPointsFileName());
		vectorBuffer->GetValidityBuffer()->Read(vstream);
	}

	// Create point parser for this document
	PointVectorParserBuilder p_builder;
	BuildPointVectorParser(p_builder);

	auto parser{ p_builder.GetParser() };
	auto context{ CreatePointVectorReaderContext() };
	auto proxy_context{ CreatePointVectorProxyContext() };

	assert(context);
	assert(proxy_context);

	auto vector{ vectorBuffer->CreatePointVectorProxy(proxy_context) };

	while (context->MoveNext())
	{
		if (context->IsValid())
		{
			parser->Read(*context, *vector);
		}
		else
		{
			// For integer types the point buffer should have already been read from a file (see above).
			// Otherwise no such buffer is needed, because floating point types have special values set for beeing "invalid".
			assert(!vectorBuffer->HasValidityBuffer() || vectorBuffer->GetValidityBuffer()->IsAllocated());
			vectorBuffer->GetValidityProvider()->SetValid(proxy_context->GetIndex(), false);
		}

		proxy_context->IncrementIndex();
	}

	// When the point buffer has been created,
	// we can savely drop the original xml content
	ResetXmlPointList();

	// initialize global vector proxy
	m_ProxyContext = CreatePointVectorProxyContext();
	m_PointVector = vectorBuffer->CreatePointVectorProxy(m_ProxyContext);
}

void ISO5436_2Container::ResetXmlPointList()
{
	assert(HasDocument());

	if (m_Document->Record3().DataList().present())
	{
		m_Document->Record3().DataList()->Datum().clear();
	}
}

void ISO5436_2Container::ResetValidPointsLink()
{
	assert(HasDocument());

	if (!IsBinary() || !GetVectorBuffer()->HasValidityBuffer() || !GetVectorBuffer()->GetValidityBuffer()->IsAllocated() || !GetVectorBuffer()->GetValidityBuffer()->HasInvalidMarks())
	{
		// no, we do not need an external validity file
		if (m_Document->Record3().DataLink().present() && m_Document->Record3().DataLink()->ValidPointsLink().present())
		{
			m_Document->Record3().DataLink()->ValidPointsLink().reset();
		}
	}
	else
	{
		// yes, we do need an external validity file, so set default pathes if nothing has been set up yet
		if (!m_Document->Record3().DataLink()->ValidPointsLink().present())
		{
			m_Document->Record3().DataLink()->ValidPointsLink(_OPENGPS_XSD_ISO5436_VALIDPOINTSLINK_PATH);
		}
	}

	// reset any checksum data
	if (m_Document->Record3().DataLink().present() && m_Document->Record3().DataLink()->MD5ChecksumValidPoints().present())
	{
		m_Document->Record3().DataLink()->MD5ChecksumValidPoints().reset();
	}
}

void ISO5436_2Container::SaveChecksumFile(zipFile handle, const std::array<unsigned char, 16>& checksum)
{
	assert(handle);

	bool retval{};

	String section(GetChecksumArchiveName());
	if (zipOpenNewFileInZip(handle,
		section.ToChar(),
		nullptr,
		nullptr,
		0,
		nullptr,
		0,
		nullptr,
		Z_DEFLATED,
		m_CompressionLevel) == ZIP_OK)
	{
		ZipStreamBuffer buffer(handle, false);
		ZipOutputStream zipOut(buffer);

		if (!zipOut.fail())
		{
			String md5out;
			if (md5out.ConvertFromMd5(checksum))
			{
				String mainArchiveName(GetMainArchiveName());

				zipOut.write(md5out.ToChar());
				zipOut.write(" *");
				zipOut.write(mainArchiveName.ToChar());
				zipOut.write("\n");

				retval = true;
			}
		}

		if (zipCloseFileInZip(handle) == ZIP_OK)
		{
			retval = retval && true;
		}
	}

	if (!retval)
	{
		throw Exception(
			OGPS_ExGeneral,
			_EX_T("The main checksum file could not be written to the X3P file container."),
			_EX_T("Check whether there is enough space left on your filesystem and for appropriate permissions."),
			_EX_T("OpenGPS::ISO5436_2Container::SaveChecksumFile"));
	}
}

void ISO5436_2Container::SaveXmlDocument(zipFile handle)
{
	assert(handle);

	xml_schema::namespace_infomap map;
	ConfigureNamespaceMap(map);

	// Creates new file in the zip container.
	String mainDocument(GetMainArchiveName());
	if (zipOpenNewFileInZip(handle,
		mainDocument.ToChar(),
		nullptr,
		nullptr,
		0,
		nullptr,
		0,
		nullptr,
		Z_DEFLATED,
		m_CompressionLevel) != ZIP_OK)
	{
		throw Exception(
			OGPS_ExGeneral,
			_EX_T("The main ISO5436-2 XML document could not be written to the X3P file container."),
			_EX_T("Zlib could not open the appropriate handle. Check your permissions and that there is enough space left on your filesystem."),
			_EX_T("OpenGPS::ISO5436_2Container::SaveXmlDocument"));
	}

	ZipStreamBuffer buffer(handle, true);
	ZipOutputStream zipOut(buffer);

	if (!zipOut.fail())
	{
		try
		{
			xercesc::XMLPlatformUtils::Initialize();
			try
			{
				Schemas::ISO5436_2::ISO5436_2(zipOut, *m_Document, map, _T("UTF-8"), xml_schema::flags::dont_initialize);
				xercesc::XMLPlatformUtils::Terminate();
			}
			catch (...)
			{
				xercesc::XMLPlatformUtils::Terminate();
				throw;
			}
		}
		catch (const xml_schema::exception& e)
		{
			_VERIFY(zipCloseFileInZip(handle), ZIP_OK);

#ifdef _UNICODE
			std::wostringstream dump;
#else
			std::ostringstream dump;
#endif

			dump << e;

			String m(dump.str());

			throw Exception(
				OGPS_ExGeneral,
				e.what(),
				m.ToChar(),
				nullptr);
		}
		catch (...)
		{
			_VERIFY(zipCloseFileInZip(handle), ZIP_OK);
			throw;
		}
		_VERIFY(zipCloseFileInZip(handle), ZIP_OK);
	}

	std::array<unsigned char, 16> md5{};
	_VERIFY(buffer.GetMd5(md5), true);
	SaveChecksumFile(handle, md5);
}

void ISO5436_2Container::ConfigureNamespaceMap(xml_schema::namespace_infomap& map) const
{
	map[_T("p")].name = _OPENGPS_XSD_ISO5436_NAMESPACE;
	map[_T("p")].schema = _OPENGPS_XSD_ISO5436_LOCATION;
}

String ISO5436_2Container::ConvertToURI(const String& src) const
{
	auto uri{ src };
	uri.ReplaceAll(_OPENGPS_WHITESPACE, _OPENGPS_URI_WHITESPACE);
	// TODO: there are surely other conformance issues.
	return uri;
}

bool ISO5436_2Container::ConfigureNamespaceMap(xml_schema::properties& props) const
{
	String xsdPathBuf;
	if (Environment::GetInstance()->GetVariable(_OPENGPS_ENV_OPENGPS_LOCATION, xsdPathBuf))
	{
		xsdPathBuf = Environment::GetInstance()->ConcatPathes(xsdPathBuf, _OPENGPS_ISO5436_LOCATION);

		String xsdPath;
		Environment::GetInstance()->GetPathName(xsdPathBuf, xsdPath);
		if (xsdPath.size() > 0)
		{
			xsdPath = ConvertToURI(_OPENGPS_FILE_URI_PREF + xsdPath);
			props.schema_location(_OPENGPS_XSD_ISO5436_NAMESPACE, xsdPath);
			return true;
		}
	}

	return false;
}

void ISO5436_2Container::SaveValidPointsLink(zipFile handle)
{
	if (HasValidPointsLink() || (HasVectorBuffer() && GetVectorBuffer()->HasValidityBuffer() && GetVectorBuffer()->GetValidityBuffer()->IsAllocated() && GetVectorBuffer()->GetValidityBuffer()->HasInvalidMarks()))
	{
		assert(IsBinary());

		// Creates new file in the zip container.
		String section(GetValidPointsArchiveName());
		if (zipOpenNewFileInZip(handle,
			section.ToChar(),
			nullptr,
			nullptr,
			0,
			nullptr,
			0,
			nullptr,
			Z_DEFLATED,
			m_CompressionLevel) != ZIP_OK)
		{
			throw Exception(
				OGPS_ExGeneral,
				_EX_T("The binary point validity file could not be written to the X3P archive."),
				_EX_T("Zlib could not open the appropriate handle. Check your permissions and that there is enough space left on your filesystem."),
				_EX_T("OpenGPS::ISO5436_2Container::SaveValidPointsLink"));
		}

		auto vectorBuffer{ GetVectorBuffer() };

		ZipStreamBuffer vbuffer(handle, true);
		ZipOutputStream vstream(vbuffer);

		try
		{
			if (!vstream.fail())
			{
				assert(vectorBuffer->HasValidityBuffer());
				vectorBuffer->GetValidityBuffer()->Write(vstream);
			}
		}
		catch (...)
		{
			_VERIFY(zipCloseFileInZip(handle), ZIP_OK);
			throw;
		}
		_VERIFY(zipCloseFileInZip(handle), ZIP_OK);

		// Add Checksum for valid.bin
		std::array<unsigned char, 16> md5{};
		_VERIFY(vbuffer.GetMd5(md5), true);

		const Schemas::ISO5436_2::DataLinkType::MD5ChecksumValidPoints_type checksum(md5.data(), md5.size());
		m_Document->Record3().DataLink()->MD5ChecksumValidPoints(checksum);
	}
}

void ISO5436_2Container::SavePointBuffer(zipFile handle)
{
	assert(handle);

	const auto isBinary{ IsBinary() };

	if (isBinary)
	{
		// Creates new file in the zip container.
		String section(GetPointDataArchiveName());
		if (zipOpenNewFileInZip(handle,
			section.ToChar(),
			nullptr,
			nullptr,
			0,
			nullptr,
			0,
			nullptr,
			Z_DEFLATED,
			m_CompressionLevel) != ZIP_OK)
		{
			throw Exception(
				OGPS_ExGeneral,
				_EX_T("Could not write binary point data to the X3P archive."),
				_EX_T("Zlib could not open the target handle. Check for filesystem permissions and enough space left."),
				_EX_T("OpenGPS::ISO5436_2Container::SavePointBuffer"));
		}
	}

	try
	{
		assert(HasDocument() && HasVectorBuffer());

		// Before we start: reset changes made to the
		// point list xml tag. Points will be replaced
		// with those vlaues in the current vector buffer.
		ResetXmlPointList();
		ResetValidPointsLink();

		// Create point parser for this document
		PointVectorParserBuilder p_builder;
		BuildPointVectorParser(p_builder);

		auto parser{ p_builder.GetParser() };
		auto context{ CreatePointVectorWriterContext(handle) };

		assert(context);

		auto vectorBuffer{ GetVectorBuffer() };

		auto proxy_context{ CreatePointVectorProxyContext() };

		assert(proxy_context);

		auto vector{ vectorBuffer->CreatePointVectorProxy(proxy_context) };

		if (proxy_context->CanIncrementIndex())
		{
			do
			{
				if (isBinary || vectorBuffer->GetValidityProvider()->IsValid(proxy_context->GetIndex()))
				{
					parser->Write(*context, *vector);
				}
				context->MoveNext();
			} while (proxy_context->IncrementIndex());
		}

		if (isBinary)
		{
			std::array<unsigned char, 16> md5{};
			dynamic_cast<BinaryPointVectorWriterContext*>(context.get())->GetMd5(md5);
			const Schemas::ISO5436_2::DataLinkType::MD5ChecksumPointData_type checksum(md5.data(), md5.size());
			m_Document->Record3().DataLink()->MD5ChecksumPointData(checksum);
		}
	}
	catch (...)
	{
		if (isBinary)
		{
			_VERIFY(zipCloseFileInZip(handle), ZIP_OK);
		}
		throw;
	}

	if (isBinary)
	{
		_VERIFY(zipCloseFileInZip(handle), ZIP_OK);
	}
}

void ISO5436_2Container::BuildPointVectorParser(PointVectorParserBuilder& builder) const
{
	builder.BuildParser();
	builder.BuildX(GetXaxisDataType());
	builder.BuildY(GetYaxisDataType());
	builder.BuildZ(GetZaxisDataType());
}

bool ISO5436_2Container::BuildVectorBuffer(VectorBufferBuilder& builder) const
{
	assert(HasDocument());

	const auto size{ GetPointCount() };

	const auto allowInvalidPoints{ !IsPointCloud() };

	return (builder.BuildBuffer() &&
		builder.BuildX(GetXaxisDataType(), size) &&
		builder.BuildY(GetYaxisDataType(), size) &&
		builder.BuildZ(GetZaxisDataType(), size) &&
		builder.BuildValidityProvider(allowInvalidPoints));
}

std::unique_ptr<PointVectorReaderContext> ISO5436_2Container::CreatePointVectorReaderContext()
{
	// instantiate binary reader context
	if (IsBinary())
	{
		// path to the source binary file
		const auto binaryFilePath = GetPointDataFileName();
		if (binaryFilePath.length() > 0)
		{
			// find out if we are on lsb or msb
			// hardware and create appropriate context
			if (Environment::IsLittleEndian())
			{
				return std::make_unique<BinaryLSBPointVectorReaderContext>(binaryFilePath);
			}

			return std::make_unique<BinaryMSBPointVectorReaderContext>(binaryFilePath);
		}

		return nullptr;
	}

	// instantiate xml string reader context...
	// find list tag in xml document and check whether it is present
	const auto& dataList{ m_Document->Record3().DataList() };
	if (dataList.present())
	{
		const auto& datum{ dataList->Datum() };
		return std::make_unique<XmlPointVectorReaderContext>(&datum);
	}

	return nullptr;
}

std::unique_ptr<PointVectorWriterContext> ISO5436_2Container::CreatePointVectorWriterContext(zipFile handle) const
{
	assert(handle);

	// instantiate binary writer context
	if (IsBinary())
	{
		// find out if we are on lsb or msb
		// hardware and create appropriate context
		if (Environment::IsLittleEndian())
		{
			return std::make_unique<BinaryLSBPointVectorWriterContext>(handle);
		}

		return std::make_unique<BinaryMSBPointVectorWriterContext>(handle);
	}

	// instantiate xml string reader context...
	// find list tag in xml document and check whether it is present
	auto& dataList{ m_Document->Record3().DataList() };
	if (dataList.present())
	{
		auto& datum{ dataList->Datum() };

		// should be empty for now (and later filled by the writer)
		if (datum.size() == 0)
		{
			return std::make_unique<XmlPointVectorWriterContext>(&datum);
		}
	}

	return nullptr;
}

OGPS_DataPointType ISO5436_2Container::GetXaxisDataType() const
{
	assert(HasDocument());

	return GetAxisDataType(m_Document->Record1().Axes().CX(), true);
}

OGPS_DataPointType ISO5436_2Container::GetYaxisDataType() const
{
	assert(HasDocument());

	return GetAxisDataType(m_Document->Record1().Axes().CY(), true);
}

OGPS_DataPointType ISO5436_2Container::GetZaxisDataType() const
{
	assert(HasDocument());

	return GetAxisDataType(m_Document->Record1().Axes().CZ(), false);
}

OGPS_DataPointType ISO5436_2Container::GetAxisDataType(const Schemas::ISO5436_2::AxisDescriptionType& axis, const bool incremental) const
{
	if (axis.DataType().present())
	{
		if (!incremental || axis.AxisType() != Schemas::ISO5436_2::AxisType::I)
		{
			switch (axis.DataType().get())
			{
			case Schemas::ISO5436_2::DataType::I:
				return OGPS_Int16PointType;
			case Schemas::ISO5436_2::DataType::L:
				return OGPS_Int32PointType;
			case Schemas::ISO5436_2::DataType::F:
				return OGPS_FloatPointType;
			case Schemas::ISO5436_2::DataType::D:
				return OGPS_DoublePointType;
			default:
				assert(false);
				break;
			}
		}
	}

	return OGPS_MissingPointType;
}

size_t ISO5436_2Container::GetPointCount() const
{
	assert(HasDocument());

	// calculate point count of matrix type
	if (IsMatrix())
	{
		const auto xSize{ m_Document->Record3().MatrixDimension()->SizeX() };
		const auto ySize{ m_Document->Record3().MatrixDimension()->SizeY() };
		const auto zSize{ m_Document->Record3().MatrixDimension()->SizeZ() };

		assert(xSize > 0 && ySize > 0 && zSize > 0);

		return SafeMultipilcation(SafeMultipilcation(xSize, ySize), zSize);
	}

	// calculate point count of list type
	return ConvertToSizeT(m_Document->Record3().ListDimension().get());
}

size_t ISO5436_2Container::GetMaxU() const
{
	// Check because this is used from outside by the point iterator.
	CheckDocumentInstance();

	if (IsMatrix())
	{
		return ConvertToSizeT(m_Document->Record3().MatrixDimension()->SizeX());
	}

	return ConvertToSizeT(m_Document->Record3().ListDimension().get());
}

size_t ISO5436_2Container::GetMaxV() const
{
	// Check because this is used from outside by the point iterator.
	CheckDocumentInstance();

	if (IsMatrix())
	{
		return ConvertToSizeT(m_Document->Record3().MatrixDimension()->SizeY());
	}

	return ConvertToSizeT(m_Document->Record3().ListDimension().get());
}

size_t ISO5436_2Container::GetMaxW() const
{
	// Check because this is used from outside by the point iterator.
	CheckDocumentInstance();

	if (IsMatrix())
	{
		return ConvertToSizeT(m_Document->Record3().MatrixDimension()->SizeZ());
	}

	return ConvertToSizeT(m_Document->Record3().ListDimension().get());
}

bool ISO5436_2Container::IsBinary() const
{
	assert(HasDocument());

	const auto hasDataLink{ m_Document->Record3().DataLink().present() };

	// ensure data ist available in either binary or text format
	assert((hasDataLink && !m_Document->Record3().DataList().present()) || (!hasDataLink && m_Document->Record3().DataList().present()));

	return hasDataLink;
}

bool ISO5436_2Container::HasValidPointsLink() const
{
	assert(HasDocument());

	return (IsBinary() && m_Document->Record3().DataLink()->ValidPointsLink().present());
}

void ISO5436_2Container::Reset()
{
	m_MainChecksum = true;
	m_DataBinChecksum = true;
	m_ValidBinChecksum = true;
	m_Document.reset();
	m_VectorBuffer.reset();
	m_PointVector.reset();
	m_ProxyContext.reset();
	m_PointDataFileName.clear();
	m_ValidPointsFileName.clear();
	m_VendorURI.clear();
	m_VendorSpecific.clear();
}

bool ISO5436_2Container::HasDocument() const
{
	return m_Document != nullptr;
}

bool ISO5436_2Container::HasVectorBuffer() const
{
	return m_VectorBuffer != nullptr;
}

void ISO5436_2Container::CreateTempDir()
{
	assert(!HasTempDir());

	const auto env{ Environment::GetInstance() };
	const auto uniqueDirectory{ env->GetUniqueName() };

	String temp;
	bool created{};
	if (m_TempBasePath.length() > 0 && env->PathExists(m_TempBasePath))
	{
		temp = env->ConcatPathes(m_TempBasePath, uniqueDirectory);

		// TODO: try new random
		if (!env->PathExists(temp))
		{
			created = env->CreateDir(temp);
		}
	}

	if (!created)
	{
		const auto sysTemp{ env->GetTempDir() };
		if (env->PathExists(sysTemp))
		{
			temp = env->ConcatPathes(sysTemp, uniqueDirectory);

			// TODO: try new random
			if (!env->PathExists(temp))
			{
				created = env->CreateDir(temp);
			}
		}
	}

	if (created)
	{
		m_TempPath = temp;
	}
	else
	{
		m_TempPath.clear();

		throw Exception(
			OGPS_ExGeneral,
			_EX_T("Temporary directory could not be created."),
			_EX_T("Check for filesystem permissions."),
			_EX_T("OpenGPS::ISO5436_2Container::CreateTempDir"));
	}
}

bool ISO5436_2Container::HasTempDir() const
{
	return m_TempPath.length() > 0;
}

void ISO5436_2Container::RemoveTempDir()
{
	m_PointDataFileName.clear();
	m_ValidPointsFileName.clear();

	if (HasTempDir())
	{
		if (Environment::GetInstance()->RemoveDir(m_TempPath))
		{
			m_TempPath.clear();
		}
	}
}

std::shared_ptr<VectorBuffer> ISO5436_2Container::GetVectorBuffer()
{
	assert(m_VectorBuffer);
	return m_VectorBuffer;
}

bool ISO5436_2Container::IsIncrementalAxis(Schemas::ISO5436_2::AxisType axis) const
{
	return (axis == Schemas::ISO5436_2::Record1Type::Axes_type::CX_type::AxisType_type::I);
}

bool ISO5436_2Container::IsIncrementalX() const
{
	assert(HasDocument());

	const auto& cx{ m_Document->Record1().Axes().CX() };
	return IsIncrementalAxis(cx.AxisType());
}

bool ISO5436_2Container::IsIncrementalY() const
{
	assert(HasDocument());

	const auto& cy{ m_Document->Record1().Axes().CY() };
	return IsIncrementalAxis(cy.AxisType());
}

bool ISO5436_2Container::IsIncrementalZ() const
{
	assert(HasDocument());

	const auto& cz{ m_Document->Record1().Axes().CZ() };
	return IsIncrementalAxis(cz.AxisType());
}

double ISO5436_2Container::GetIncrementX() const
{
	assert(HasDocument());

	const auto& cx{ m_Document->Record1().Axes().CX() };
	if (cx.Increment().present())
	{
		return cx.Increment().get();
	}

	return 1.0;
}

double ISO5436_2Container::GetIncrementY() const
{
	assert(HasDocument());

	const auto& cy{ m_Document->Record1().Axes().CY() };
	if (cy.Increment().present())
	{
		return cy.Increment().get();
	}

	return 1.0;
}

double ISO5436_2Container::GetOffsetX() const
{
	assert(HasDocument());

	const auto& cx{ m_Document->Record1().Axes().CX() };
	if (cx.Offset().present())
	{
		return cx.Offset().get();
	}

	return 0.0;
}

double ISO5436_2Container::GetOffsetY() const
{
	assert(HasDocument());

	const auto& cy{ m_Document->Record1().Axes().CY() };
	if (cy.Offset().present())
	{
		return cy.Offset().get();
	}

	return 0.0;
}

double ISO5436_2Container::GetOffsetZ() const
{
	assert(HasDocument());

	const auto& cz{ m_Document->Record1().Axes().CZ() };
	if (cz.Offset().present())
	{
		return cz.Offset().get();
	}

	return (0.0);
}

std::shared_ptr<PointVectorProxyContext> ISO5436_2Container::CreatePointVectorProxyContext() const
{
	assert(HasDocument());

	if (IsMatrix())
	{
		const auto& mtype{ m_Document->Record3().MatrixDimension().get() };

		SafeMultipilcation(SafeMultipilcation(mtype.SizeX(), mtype.SizeY()), mtype.SizeZ());

		const auto sx{ ConvertToSizeT(mtype.SizeX()) };
		const auto sy{ ConvertToSizeT(mtype.SizeY()) };
		const auto sz{ ConvertToSizeT(mtype.SizeZ()) };

		return std::make_shared<PointVectorProxyContextMatrix>(sx, sy, sz);
	}

	assert(m_Document->Record3().ListDimension().present());

	return std::make_shared<PointVectorProxyContextList>(ConvertToSizeT(m_Document->Record3().ListDimension().get()));
}

void ISO5436_2Container::ValidateDocument()
{
	if (HasDocument())
	{
		if (IsIncrementalZ())
		{
			throw Exception(
				OGPS_ExWarning,
				_EX_T("The z axis is defined as incremental."),
				_EX_T("Since the z-coordinate is not derived from topology, therefore the z axis must always be with absolute values."),
				_EX_T("OpenGPS::ISO5436_2Container::ValidateDocument"));
		}

		if (IsMatrix())
		{
			if (IsProfile())
			{
				size_t size_u{}, size_v{}, size_w{};
				GetMatrixDimensions(&size_u, &size_v, &size_w);

				if (size_v != 1)
				{
					throw Exception(
						OGPS_ExWarning,
						_EX_T("The feature type is profile, but matrix dimensions do not fit."),
						_EX_T("PRF type files must always be encoded as matrix of size N,1,M with N the number of points and M the number of layers in z direction."),
						_EX_T("OpenGPS::ISO5436_2Container::ValidateDocument"));
				}
			}

			if (IsPointCloud())
			{
				throw Exception(
					OGPS_ExWarning,
					_EX_T("The feature type is a point cloud, but it is stored in matrix format."),
					_EX_T("It makes no sense to store point data of the unordered PCL feature type in (ordered) matrix format."),
					_EX_T("OpenGPS::ISO5436_2Container::ValidateDocument"));
			}
		}
		else
		{
			if (IsIncrementalX() || IsIncrementalY())
			{
				throw Exception(
					OGPS_ExInvalidOperation,
					_EX_T("At least one of the x/y axes definitions is incremental, although point data is stored within an unordered list."),
					_EX_T("List type files do not represent a topology, so there is no relation between list position and coordinate. Thus the x and y and z axis must not be of implicit (incremental) type."),
					_EX_T("OpenGPS::ISO5436_2Container::ValidateDocument"));
			}

			if (IsProfile())
			{
				throw Exception(
					OGPS_ExInvalidOperation,
					_EX_T("The feature type is profile, but point data is stored in an unordered list."),
					_EX_T("PRF type files must always be encoded as matrix of size N,1,M with N the number of points and M the number of layers in z direction."),
					_EX_T("OpenGPS::ISO5436_2Container::ValidateDocument"));
			}

			// Must be of type PCL (Point cloud)
			if (!IsPointCloud())
			{
				throw Exception(
					OGPS_ExInvalidOperation,
					_EX_T("The data is stored as an unordered point list, but the feature type is not a PointCloud (PCL)."),
					_EX_T("Unordered point clouds must be stored as a PCL type files."),
					_EX_T("OpenGPS::ISO5436_2Container::ValidateDocument"));
			}
		}

		// Check for Null-increment, X axis
		if (IsIncrementalX())
		{
			const auto incrementX{ GetIncrementX() };
			if (_OPENGPS_DBL_EQN(incrementX))
			{
				throw Exception(
					OGPS_ExOverflow,
					_EX_T("The increment of the x axis equals to null."),
					_EX_T("The x axis is defined as incremental, but an increment of null is invalid here."),
					_EX_T("OpenGPS::ISO5436_2Container::ValidateDocument"));
			}
		}

		// Check for Null-increment, Y axis
		if (IsIncrementalY())
		{
			const auto incrementY{ GetIncrementY() };
			if (_OPENGPS_DBL_EQN(incrementY))
			{
				throw Exception(
					OGPS_ExOverflow,
					_EX_T("The increment of the y axis equals to null."),
					_EX_T("The y axis is defined as incremental, but an increment of null is invalid here."),
					_EX_T("OpenGPS::ISO5436_2Container::ValidateDocument"));
			}
		}
	}
}

void ISO5436_2Container::TestChecksums() const
{
	if (!m_MainChecksum)
	{
		throw Exception(
			OGPS_ExWarning,
			_EX_T("The checksum of the main.xml document contained in an X3P archive could not be verified."),
			_EX_T("Although some data had been extracted there is no guarantee of their integrity."),
			_EX_T("OpenGPS::ISO5436_2Container::TestChecksums"));
	}

	if (!m_DataBinChecksum)
	{
		throw Exception(
			OGPS_ExWarning,
			_EX_T("The checksum of binary point data contained in an X3P archive could not be verified."),
			_EX_T("Although some data had been extracted there is no guarantee of their integrity."),
			_EX_T("OpenGPS::ISO5436_2Container::TestChecksums"));
	}

	if (!m_ValidBinChecksum)
	{
		throw Exception(
			OGPS_ExWarning,
			_EX_T("The checksum of binary point validity data contained in an X3P archive could not be verified."),
			_EX_T("Although some data had been extracted there is no guarantee of their integrity."),
			_EX_T("OpenGPS::ISO5436_2Container::TestChecksums"));
	}
}

void ISO5436_2Container::AppendVendorSpecific(const String& vendorURI, const String& filePath)
{
	CheckDocumentInstance();

	assert(vendorURI.size() > 0 && filePath.size() > 0);
	assert(m_VendorURI.empty() || m_VendorSpecific.size() > 0);

	if (!m_VendorURI.empty() && m_VendorURI != vendorURI)
	{
		throw Exception(
			OGPS_ExInvalidOperation,
			_EX_T("The vendor URI specified is inconsistent."),
			_EX_T("The current vendor URI does not equal the vendor URI given by a previous call."),
			_EX_T("OpenGPS::ISO5436_2Container::AppendVendorSpecific"));
	}

	if (m_VendorURI.empty())
	{
		m_VendorURI = vendorURI;
	}

	m_VendorSpecific.push_back(filePath);
}

bool ISO5436_2Container::GetVendorSpecific(const String& vendorURI, const String& fileName, const String& targetPath)
{
	CheckDocumentInstance();

	if (m_IsCreating)
	{
		throw Exception(
			OGPS_ExInvalidOperation,
			_EX_T("This method call is invalid in the current context."),
			_EX_T("Vendorspecific data can be extracted only if an X3P archive is opened. But currently such an archive is to be created."),
			_EX_T("OpenGPS::ISO5436_2Container::GetVendorSpecific"));
	}

	if (vendorURI.empty())
	{
		throw Exception(
			OGPS_ExInvalidArgument,
			_EX_T("The argument vendorURI is empty."),
			_EX_T("Supply an URI that is unique worldwide to identify your data format. Example: http://www.example-inc.com/myformat"),
			_EX_T("OpenGPS::ISO5436_2Container::GetVendorSpecific"));
	}

	if (m_Document->VendorSpecificID().present())
	{
		if (m_Document->VendorSpecificID().get() == vendorURI)
		{
			return Decompress(fileName, targetPath, true);
		}
		else
		{
			std::ostringstream msg;
			msg << _EX_T("File vendor URI is \"") << m_Document->VendorSpecificID().get() << _EX_T("\"") << std::endl
				<< _EX_T("Argument vendor URI is \"") << vendorURI << _EX_T("\"") << std::endl << std::ends;
			std::string msg1(msg.str());

			throw Exception(
				OGPS_ExWarning,
				_EX_T("The argument vendorURI is not equal to the file vendor uri."),
				msg1.c_str(),
				_EX_T("OpenGPS::ISO5436_2Container::GetVendorSpecific"));
		}
	}
	else
	{
		throw Exception(
			OGPS_ExWarning,
			_EX_T("No vendor specific extension in file detected."),
			_EX_T("The file does not contain a vendorURI."),
			_EX_T("OpenGPS::ISO5436_2Container::GetVendorSpecific"));
	}
}

bool ISO5436_2Container::WriteVendorSpecific(zipFile handle)
{
	bool success = true;

	if (m_VendorURI.size() > 0 && m_VendorSpecific.size() > 0)
	{
		m_Document->VendorSpecificID(m_VendorURI);

		for (size_t n = 0; n < m_VendorSpecific.size(); ++n)
		{
			// Creates new file in the zip container.
			String vendor = m_VendorSpecific[n];
			String avname = Environment::GetInstance()->GetFileName(vendor);
			if (zipOpenNewFileInZip(handle,
				avname.ToChar(),
				nullptr,
				nullptr,
				0,
				nullptr,
				0,
				nullptr,
				Z_DEFLATED,
				m_CompressionLevel) != ZIP_OK)
			{
				// zip file could not be created
				success = false;
			}
			else
			{
				try
				{
					ZipStreamBuffer vbuffer(handle, false);
					ZipOutputStream vstream(vbuffer);

					std::ifstream src(vendor.ToChar(), std::ios::in | std::ios::binary | std::ios::ate);

					if (!src.is_open())
					{
						success = false;
					}
					else
					{
						// get length of file:
						std::streampos length = src.tellg();
						src.seekg(0, std::ios::beg);

						if (length < 0)
						{
							throw Exception(
								OGPS_ExGeneral,
								_EX_T("Could not write additional vendor specific data to the X3P archive."),
								_EX_T("The size of the source file could not be determined."),
								_EX_T("OpenGPS::ISO5436_2Container::WriteVendorSpecific"));
						}

						if (length > 0)
						{
							do
							{
								const size_t chunk = std::min(static_cast<size_t>(_OPENGPS_ZIP_CHUNK_MAX), static_cast<size_t>(length));
								voidp buffer = (voidp)malloc(chunk);

								if (buffer)
								{
									try
									{
										src.read((char*)buffer, chunk);

										if (src.fail())
										{
											if (!src.eof())
											{
												throw Exception(
													OGPS_ExGeneral,
													_EX_T("Could not write additional vendor specific data to the X3P archive."),
													_EX_T("The vendorspecific file to be added could not be read completely."),
													_EX_T("OpenGPS::ISO5436_2Container::WriteVendorSpecific"));
											}
										}

										if (zipWriteInFileInZip(handle, (voidp)buffer, static_cast<unsigned int>(chunk)) != ZIP_OK)
										{
											throw Exception(
												OGPS_ExInvalidOperation,
												_EX_T("Could not write additional vendor specific data to the X3P archive."),
												_EX_T("Zlib could write all external data. Please check that there is enough space left on the device."),
												_EX_T("OpenGPS::ISO5436_2Container::WriteVendorSpecific"));
										}
										_OPENGPS_FREE(buffer);
									}
									catch (...)
									{
										_OPENGPS_FREE(buffer);
										throw;
									}
								}
								else
								{
									throw Exception(
										OGPS_ExGeneral,
										_EX_T("Could not allocate memory via malloc."),
										_EX_T("Verify that there is enough free virtual memory installed."),
										_EX_T("OpenGPS::ISO5436_2Container::WriteVendorSpecific"));
								}

								length -= chunk;
							} while (!src.eof() && length > 0);
						}
					}
				}
				catch (...)
				{
					_VERIFY(zipCloseFileInZip(handle), ZIP_OK);
					throw;
				}

				_VERIFY(zipCloseFileInZip(handle), ZIP_OK);
			}
		}
	}
	else
	{
		if (m_Document->VendorSpecificID().present())
		{
			m_Document->VendorSpecificID().reset();
		}
	}

	return success;
}

void ISO5436_2Container::CheckDocumentInstance() const
{
	if (!m_Document)
	{
		throw Exception(
			OGPS_ExInvalidOperation,
			_EX_T("There is no instance of an XML document."),
			_EX_T("Please either load an existing X3P file using the Open method or initialize a new document with the Create method before you operate on the document. From within the C interface check with ogps_GetError whether the document is loaded or created successfuly."),
			_EX_T("ISO5436_2Container::CheckDocument"));
	}
}
