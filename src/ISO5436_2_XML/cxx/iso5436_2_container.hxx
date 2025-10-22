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

/*! @file
 * Concrete implementation of the interface of an X3P container.
 */

#ifndef _OPENGPS_ISO5436_2_CONTAINER_HXX
#define _OPENGPS_ISO5436_2_CONTAINER_HXX

#include <opengps/cxx/exceptions.hxx>
#include <opengps/data_point_type.h>
#include "auto_ptr_types.hxx"
#include "point_vector_proxy_context.hxx"
#include <opengps/cxx/point_iterator.hxx>
#include <opengps/cxx/string.hxx>
#include <opengps/cxx/iso5436_2_xsd.hxx>
#include <zip.h>

namespace OpenGPS
{
	class PointVectorBase;
	class PointVectorParser;
	class PointVectorParserBuilder;
	class VectorBufferBuilder;
	class PointVectorReaderContext;
	class PointVectorWriterContext;
	class VectorBuffer;

	/*! This is the main gate to this software library. It provides all manipulation
	 * methods to handle X3P archive files.
	 */
	class ISO5436_2Container : public std::enable_shared_from_this<ISO5436_2Container>
	{
	public:
		/*!
		 * Creates a new instance.
		 * @param file The path to the X3P file that is serverd by this instance.
		 * The file may not exist yet. In this case ISO5436_2Container::Create
		 * methods must be used to create a new X3P file. Otherwise use
		 * ISO5436_2Container::Create to get an internal handle to an already
		 * existing x3P file container.
		 * @param temp If not empty this specifies an alternativ directory path
		 * to save temporary files. Otherwise the default directory for temporary files
		 * used by the operating system is used. Since the file data archived in the
		 * X3P file needs to be decompressed before using it, this may eventually take
		 * relatively large space from your storage media.
		 */
		ISO5436_2Container(
			const String& file,
			const String& temp);

		/*! Destroys this instance. This closes all open file handles and all
		changes you may have made to an X3P document get lost unless you
		previously saved them by executing ISO5436_2Container::Write. */
		virtual ~ISO5436_2Container();

		/* Implements public ISO5436_2 interface. */

		void Open();

		void Create(
			const Schemas::ISO5436_2::Record1Type& record1,
			const Schemas::ISO5436_2::Record2Type* record2,
			const Schemas::ISO5436_2::MatrixDimensionType& matrixDimension,
			bool useBinaryData = true);

		void Create(
			const Schemas::ISO5436_2::Record1Type& record1,
			const Schemas::ISO5436_2::Record2Type* record2,
			size_t listDimension,
			bool useBinaryData = true);

		PointIteratorAutoPtr CreateNextPointIterator();
		PointIteratorAutoPtr CreatePrevPointIterator();

		void SetMatrixPoint(
			size_t u,
			size_t v,
			size_t w,
			const PointVector* vector);

		void GetMatrixPoint(
			size_t u,
			size_t v,
			size_t w,
			PointVector& vector);

		void SetListPoint(
			size_t index,
			const PointVector& vector);

		void GetListPoint(
			size_t index,
			PointVector& vector);

		void GetMatrixCoord(
			size_t u,
			size_t v,
			size_t w,
			OGPS_Double* x,
			OGPS_Double* y,
			OGPS_Double* z);

		bool IsMatrixCoordValid(
			size_t u,
			size_t v,
			size_t w);

		void GetListCoord(
			size_t index,
			OGPS_Double* x,
			OGPS_Double* y,
			OGPS_Double* z);

		Schemas::ISO5436_2::ISO5436_2Type* GetDocument();

		bool IsMatrix() const;

		void GetMatrixDimensions(
			size_t* size_u,
			size_t* size_v,
			size_t* size_w) const;

		size_t GetListDimension() const;

		void Write(int compressionLevel = Z_DEFAULT_COMPRESSION);

		void Close();

		void AppendVendorSpecific(const String& vendorURI, const String& filePath);
		bool GetVendorSpecific(const String& vendorURI, const String& fileName, const String& targetPath);

	protected:

		/*!
		 * Checks whether points are stored as separate binary files
		 * or directly within the main ISO5436-2 XML document.
		 * @returns Returns true if there exist an external binary
		 * file for point data within the archive, otherwise false is
		 * returned and the point vectors are saved within the
		 * ISO5436-2 XML document also contained within the X3P archive.
		 */
		bool IsBinary() const;

		/*!
		 * Checks whether validity information on point vector data is
		 * provided by an extra binary file. Such a binary file is needed
		 * in connection with non-IEEE754 data types (see the axis definition)
		 * and when invalid measurements are contained within point data only.
		 * @returns Returns true when the existance of such a file needs
		 * to be taken into account, false otherwise.
		 */
		bool HasValidPointsLink() const;

		/*!
		 * Gets the maximum index value possible in X direction.
		 * For vector data in matrix topology this is the value of X direction only.
		 * This is the value of the size of the data point list in the non-matrix case.
		 * @see ISO5436_2Container::IsMatrix.
		 * @return Returns the maximum of an index value in X direction (of matrix topology).
		 * There is a fallback to the maximum amount of point vectors contained in
		 * the point vector list in the non-matrix case.
		 */
		size_t GetMaxU() const;

		/*!
		 * Gets the maximum index value possible in Y direction.
		 * For vector data in matrix topology this is the value of Y direction only.
		 * This is the value of the size of the data point list in the non-matrix case.
		 * @see ISO5436_2Container::IsMatrix.
		 * @return Returns the maximum of an index value in Y direction (of matrix topology).
		 * There is a fallback to the maximum amount of point vectors contained in
		 * the point vector list in the non-matrix case.
		 */
		size_t GetMaxV() const;

		/*!
		 * Gets the maximum index value possible in Z direction.
		 * For vector data in matrix topology this is the value of Z direction only.
		 * This is the value of the size of the data point list in the non-matrix case.
		 * @see ISO5436_2Container::IsMatrix.
		 * @return Returns the maximum of an index value in Z direction (of matrix topology).
		 * There is a fallback to the maximum amount of point vectors contained in
		 * the point vector list in the non-matrix case.
		 */
		size_t GetMaxW() const;

		/*! Gets the file path to the X3P archive the current instance is an interface for. */
		const String& GetFilePath() const;

		/*! Gets the absolute file path to the X3P archive the current instance is an interface for. */
		String GetFullFilePath() const;

		/*! Gets the full path to the directory where temporary files get stored. */
		const String& GetTempDir() const;

		/*! Creates a unique temporary file name. */
		String CreateContainerTempFilePath() const;

		/*! Gets the relative path of the main xml document file within the zip archive. */
		String GetMainArchiveName() const;

		/*! Gets the full path to the temporaryily decompressed main xml document file. */
		String GetMainFileName() const;

		/*! Gets the relative path of the binary point data file within the zip archive. */
		String GetPointDataArchiveName() const;

		/*! Gets the full path to the temporaryily decompressed binary point data file. */
		String GetPointDataFileName();

		/*! Gets the relative path of the binary point validity data file within the zip archive. */
		String GetValidPointsArchiveName() const;

		/*! Gets the full path to the temporaryily decompressed binary point validity data file. */
		String GetValidPointsFileName();

		/*! Gets the relative path of the md5 checksum file within the zip archive. */
		String GetChecksumArchiveName() const;

		/*! Gets the full path to the temporaryily decompressed md5 checksum file. */
		String GetChecksumFileName() const;

		/*!
		 * Decompresses and verifies the current X3P archive.
		 *
		 * @remarks If this throws an exception there may exist incorrect and incomplete data.
		 * Do call ISO5436_2Container::Reset to avoid an inconsistent state.
		 */
		void Decompress();

		/*!
		 * Decompresses a single file within the zip archive.
		 * @param src The name of the file to be decompressed. This is the relative path with
		 * the archive itself set as the root element.
		 * @param dst The absolute(!) target path where uncompressed data gets stored on the media.
		 * @param fileNotFoundAllowed true if it is OK if a file could not be found within the archive.
		 * This method proceeds without throwing an exception then and its return value is false.
		 * @returns Returns false if a file could not be found in the archive (see the discussion above),
		 * true in all other cases.
		 */
		bool Decompress(const String& src, const String& dst, bool fileNotFoundAllowed = false) const;

		/*!
		 * Decompresses the main xml document contained within the X3P archive.
		 * @see ISO5436_2Container::Decompress, ISO5436_2Container::GetMainArchiveName,
		 * ISO5436_2Container::GetMainFileName
		 */
		void DecompressMain() const;

		/*!
		 * Decompresses the md5 checksum file contained within the X3P archive.
		 * @see ISO5436_2Container::Decompress, ISO5436_2Container::GetChecksumArchiveName,
		 * ISO5436_2Container::GetChecksumFileName
		 */
		void DecompressChecksum();

		/*!
		 * Decompresses the binary point data file contained within the X3P archive.
		 * @see ISO5436_2Container::Decompress, ISO5436_2Container::GetPointDataArchiveName,
		 * ISO5436_2Container::GetPointDataFileName
		 */
		void DecompressDataBin();


		/*!
		 * (Over)writes the current X3P archive file with the actual content.
		 */
		void Compress();

		/*!
		 * Creates the internal XML document tree structure.
		 * The Record3 and Record4 structures defined in
		 * the ISO5436-2 XML specification are created automatically.
		 * @param record1 The content of the Record1 XML structure.
		 * @param record2 The content of the Record2 XML structure.
		 * @param matrixDimension The dimension of the matrix structure
		 * or nullptr if a list structure is used.
		 * @param listDimension The size of the data list that stores
		 * the point vectors or 0 if the matrix structure is used instead.
		 * @param useBinaryData Set this to true to store the point
		 * data within an external binary file. If set to false point vectors
		 * will get storead within the ISO5436-2 XML document directly.
		 */
		void CreateDocument(
			const Schemas::ISO5436_2::Record1Type* record1,
			const Schemas::ISO5436_2::Record2Type* record2,
			const Schemas::ISO5436_2::MatrixDimensionType* matrixDimension,
			size_t listDimension,
			bool useBinaryData);

		/*!
		 * Creates an instance of the internal ISO5436-2 XML document tree.
		 * An instance is created from the decompressed main xml document file.
		 */
		void ReadDocument();

		/*!
		 * Assembles a new OpenGPS::PointVectorParser object using the OpenGPS::PointVectorParserBuilder.
		 * @param builder The instance of the builder that is used to create the vector parser.
		 */
		void BuildPointVectorParser(PointVectorParserBuilder& builder) const;

		/*! Gets the data type of point data of the X component of a vector. */
		OGPS_DataPointType GetXaxisDataType() const;

		/*! Gets the data type of point data of the Y component of a vector. */
		OGPS_DataPointType GetYaxisDataType() const;

		/*! Gets the data type of point data of the Z component of a vector. */
		OGPS_DataPointType GetZaxisDataType() const;

		/*!
		 * Gets the data type of point data of an axis.
		 * @param axis The axis description to prove.
		 * @param incremental true if implict point values are allowed for an incremental axis. This holds true for every axis
		 * except the Z axis.
		 * @returns Returns the type of data points of an axis or OGPS_MissingPointType on an incremntal axis definition.
		 */
		OGPS_DataPointType GetAxisDataType(const Schemas::ISO5436_2::AxisDescriptionType& axis, const bool incremental) const;

		/*!
		 * Assembles a new OpenGPS::VectorBuffer object using the OpenGPS::VectorBufferBuilder.
		 * @param builder The instance of the builder that is used to create the vector buffer.
		 * @returns Returns true on success, false otherwise.
		 */
		bool BuildVectorBuffer(VectorBufferBuilder& builder) const;

		/*!
		 * Sets up the internal memory storage of point data.
		 * Creates and allocates the internal vector buffer and fills in point data from either the
		 * ISO5436-2 main xml document or from an external binary file.
		 */
		void CreatePointBuffer();

		/*!
		 * Saves the current state of internal memory storage of point data either to the zip archive
		 * as an external binary point file or directly into the actual tree structure of the internal
		 * document handle of the main ISO5436-2 XML document depending on the current settings.
		 * @param handle The handle to the zip archive where the data is to be stored.
		 */
		void SavePointBuffer(zipFile handle);

		/*!
		 * Removes the point list xml tag and its content from the xml document handle.
		 */
		void ResetXmlPointList();

		/*!
		 * Rests the valid points link xml tag of the xml document handle to its default value.
		 */
		void ResetValidPointsLink();

		/*!
		 * Saves the current state of internal memory storage of point validity data to the zip archive.
		 * @param handle The handle to the zip archive where the data is to be stored.
		 */
		void SaveValidPointsLink(zipFile handle);

		/*!
		 * Saves the main md5 checksum to the zip archive.
		 * @param handle The handle to the zip archive where the data is to be stored.
		 * @param checksum The value of the calculated 128bit md5 checksum.
		 */
		void SaveChecksumFile(zipFile handle, const std::array<unsigned char, 16>& md5);

		/*!
		 * Creates an instance of appropriate access methods to read point data depending on
		 * the current configuration of the already loaded main ISO5436-2 XML document.
		 * @returns An instance to access raw point data for reading or nullptr on failure.
		 * The pointer returned must be released by the caller.
		 */
		std::unique_ptr<PointVectorReaderContext> CreatePointVectorReaderContext();

		/*!
		 * Creates an instance of appropriate access methods to write point data depending on
		 * the current configuration of the main ISO5436-2 XML document.
		 * @param handle The handle to the zip archive is needed by the special implementation
		 * of the context for writing point data to an external binary file.
		 * @returns An instance to write raw point data or nullptr on failure.
		 * The pointer returned must be released by the caller.
		 */
		std::unique_ptr<PointVectorWriterContext> CreatePointVectorWriterContext(zipFile handle) const;

		/*!
		 * Gets the amount of point data that is stored in the archive.
		 */
		size_t GetPointCount() const;

		/*!
		 * Releases any resources allocated to handle the xml document tree or internal storage of vectors in memory.
		 */
		void Reset();

		/*!
		 * Returns true if an ISO5436-2 XML document has been loaded into memory as a tree structure,
		 * or false if the internal document handle does not exist.
		 */
		bool HasDocument() const;

		/*!
		 * Returns true if the internal memory storage area of vector data had been allocated, or false if not.
		 */
		bool HasVectorBuffer() const;

		/*!
		 * Reads the main ISO5436-2 XML document contained in an X3P archive to the internal
		 * document handle as a tree structure.
		 */
		void ReadXmlDocument();

		/*!
		 * Writes the content of the internal document handle to the main XML document present in an X3P archive.
		 * @param handle The handle of the X3P archive.
		 */
		void SaveXmlDocument(zipFile handle);

		/*!
		 * Creates a new instance of a vector proxy context that is used to map point data saved distinctively
		 * as one column for every axis definition to one single row vector. The context object provides the indexing
		 * information necessary for that task.
		 * @returns A new instance of a vector proxy context. The caller ist responsible of releasing this resource.
		 */
		std::shared_ptr<PointVectorProxyContext> CreatePointVectorProxyContext() const;

		/*!
		 * Checks for the current document instance and raises an excpetion if it is not instantiated.
		 */
		void CheckDocumentInstance() const;


	private:
		/*! The path of the X3P archive handles. */
		String m_FilePath;

		/*! The path to a temporary directory unique to the current instance. */
		String m_TempPath;

		/*! The path to the global directory for temporary files. */
		String m_TempBasePath;

		/*! true if an X3P archive is to be created, false if an existing archive has been opened.
		 * The value is undefined if nothing happened so far.
		 */
		bool m_IsCreating{};

		/*! The temporary target path of the uncompressed binary point data file. */
		String m_PointDataFileName;

		/*! The temporary target path of the uncompressed binary point validity data file. */
		String m_ValidPointsFileName;

		/*! The level of compression of the zip archive. */
		int m_CompressionLevel;

		/*! The handle to a tree strucure corresponding to an ISO5436-2 XML document file. */
		std::unique_ptr<Schemas::ISO5436_2::ISO5436_2Type> m_Document;

		/*! The current vector buffer. */
		std::shared_ptr<VectorBuffer> m_VectorBuffer;

		/*! A global point vector proxy used for index based read/writes of point vectors. */
		std::shared_ptr<PointVectorProxyContext> m_ProxyContext;

		/*! A point vector which serves as a temporary buffer. */
		std::shared_ptr<PointVectorBase> m_PointVector;

		/*! Creates a temporary directory in the file system and sets ISO5436_2Container::m_TempPath. */
		void CreateTempDir();

		/*! Returns true if a unique temporary directory has been created or false if not. */
		bool HasTempDir() const;

		/*! Removes the current temporary directory from the file system and erases all of its content. */
		void RemoveTempDir();

		/*! Gets a pointer to the vector buffer or nullptr. */
		std::shared_ptr<VectorBuffer> GetVectorBuffer();

		/*! false, if the md5 checksum could not be verified after reading. */
		bool m_MainChecksum{ true };

		/*! false, if the md5 checksum could not be verified after reading. */
		bool m_DataBinChecksum{ true };

		/*! false, if the md5 checksum could not be verified after reading. */
		bool m_ValidBinChecksum{ true };

		/*! ID of vendorspecific data or empty. @see ISO5436_2Container::m_VendorSpecific. */
		String m_VendorURI;

		typedef std::vector<String> StringList;

		/*! Vendorspecific file names to be added to the container registered with one single vendor id. @see ISO5436_2Container::m_VendorURI */
		StringList m_VendorSpecific;

		/*!
		 * Writes vendorspecific files to the zip container if any.
		 *
		 * @param handle Handle of the target zip archive.
		 * @returns Returns false if not all of the files could be written. true otherwise.
		 */
		bool WriteVendorSpecific(zipFile handle);

		/*!
		 * Sets a valid namespace mapping for writing the ISO5436-2 XML document.
		 * @param map Target for the valid namespace mapping.
		 */
		void ConfigureNamespaceMap(xml_schema::namespace_infomap& map) const;

		/*!
		 * Sets local pathes to XSD files corresponding to the global namespaces of the ISO5436-2 XML specification.
		 * @param props Target for the global to local namespace mapping.
		 * @returns Returns true on success, false otherwise.
		 */
		bool ConfigureNamespaceMap(xml_schema::properties& props) const;

		/*!
		 * Verifies an 128bit md5 checksum.
		 * @param filePath The file which checksum is to be verified.
		 * @param checksum The expected checksum to verify.
		 * @param size The size of the checksum buffer in bytes. This must be equal to 16 always as it is a 128bit md5 sum.
		 * @returns Returns true when the checksum could be verified, false otherwise.
		 */
		bool VerifyChecksum(const String& filePath, const unsigned char* checksum, size_t size) const;

		/*!
		 * Verifies an 128bit md5 checksum.
		 * @param filePath The file which checksum is to be verified.
		 * @param checksum The expected checksum to verify.
		 * @returns Returns true when the checksum could be verified, false otherwise.
		 */
		bool VerifyChecksum(const String& filePath, std::array<unsigned char, 16>& checksum) const;

		/*!
		 * Verifies the checksum of the main document ISO5436-2 XML file.
		 */
		void VerifyMainChecksum();

		/*!
		 * Verifies the checksum of the binary point data file.
		 */
		void VerifyDataBinChecksum();

		/*!
		 * Verifies the checksum of the binary point validity data file.
		 */
		void VerifyValidBinChecksum();

		/*!
		 * Check if all checksums were verified.
		 * If any one of them could not be verified this throws an OpenGPS::Exception of type ::OGPS_ExWarning that may be ignored.
		 */
		void TestChecksums() const;

		/*!
		 * Checks the document for some semantic errors.
		 * If any semantic errors were found, this throws an OpenGPS::Exception of type ::OGPS_ExWarning that may be ignored.
		 */
		void ValidateDocument();

		/*!
		 * Checks whether the current document data is either for profiles or surfaces.
		 * @remarks Throws an exception if the current feature type is undefined.
		 * @returns true, if the document record specifies a profile, false otherwise.
		 */
		bool IsProfile() const;

		/*!
		 * Checks whether the current document data is for point clouds.
		 * @remarks Throws an exception if the current feature type is undefined.
		 * @returns true, if the document record specifies a point cloud, false otherwise.
		 */
		bool IsPointCloud() const;

		/*!
		 * Reads the first md5 checksum from a file that contains md5 checksums of files.
		 * @param fileName The path to the file that contains md5 checksums.
		 * @param checksum Target of the extracted checksum.
		 * @returns Returns true on success, false otherwise.
		 */
		bool ReadMd5FromFile(const String& fileName, std::array<unsigned char, 16>& checksum) const;

		/*!
		 * Extracts the three components of a point vector.
		 *
		 * May throw an OpenGPS::Exception on failure due to type incompatibilities or overflow.
		 * Vector components that result from a missing point are set to IEEE NaN.
		 * This means that incremental axis allways produce a valid result, but absolute axis create a NaN.
		 *
		 * @param vector The vector which components are to be extracted.
		 * @param x Target of the value of the X component of that vector. If this equals nullptr, this component is ignored.
		 * @param y Target of the value of the Y component of that vector. If this equals nullptr, this component is ignored.
		 * @param z Target of the value of the Z component of that vector. If this equals nullptr, this component is ignored.
		 */
		void ConvertPointToCoord(
			const PointVector& vector,
			OGPS_Double* x,
			OGPS_Double* y,
			OGPS_Double* z);

		/*! Returns true if the axis has an incremental axis definition, false otherwise. If true point data of that axis is known implicitly. */
		bool IsIncrementalAxis(Schemas::ISO5436_2::AxisType axis) const;

		/*! Returns true if X has an incremental axis definition, false otherwise. If true point data of that axis is known implicitly. */
		bool IsIncrementalX() const;

		/*! Returns true if Y has an incremental axis definition, false otherwise. If true point data of that axis is known implicitly. */
		bool IsIncrementalY() const;

		/*! Returns true if Z has an incremental axis definition, false otherwise. If true point data of that axis is known implicitly. */
		bool IsIncrementalZ() const;

		/*! Gets the increment of the X axis definition or 1.0 if there is no incremental step. */
		double GetIncrementX() const;

		/*! Gets the increment of the Y axis definition or 1.0 if there is no incremental step. */
		double GetIncrementY() const;

		/*! Gets the offset of the X axis definition or 0.0 if there is no offset at all. */
		double GetOffsetX() const;

		/*! Gets the offset of the Y axis definition or 0.0 if there is no offset at all. */
		double GetOffsetY() const;

		/*! Gets the offset of the Z axis definition or 0.0 if there is no offset at all. */
		double GetOffsetZ() const;

		/*!
		 * Converts any string to its URI conformant compangnon.
		 *
		 * @param src String to convert.
		 * @returns Returns URI conformant string.
		 */
		String ConvertToURI(const String& src) const;

		/*!
		* Implementation of the point iterator interface.
		* A point iterator can be created and initialized
		* by an instance of OpenGPS::ISO5436_2Container only.
		*/
		class PointIteratorImpl : public PointIterator
		{
		public:
			/*!
			* Creates a new instance.
			* @param handle Provides access to point data based on indexes.
			* @param isForward true if iterating in increasing order of
			* indexes but false when starting from the end with decreasing indexes.
			* @param isMatrix true to access the matrix interface methods of
			* the given OpenGPS::ISO5436_2Container instance. false to access
			* the list interface methods instead.
			*/
			PointIteratorImpl(std::shared_ptr<ISO5436_2Container> handle,
				bool isForward,
				bool isMatrix);

			/* Implements the public PointIterator interface. */

			bool HasNext() const override;
			bool HasPrev() const override;

			bool MoveNext() override;
			bool MovePrev() override;

			void ResetNext() override;
			void ResetPrev() override;

			void GetCurrent(PointVector& vector) override;
			void GetCurrentCoord(PointVector& vector) override;

			void SetCurrent(const PointVector* vector) override;

			bool GetPosition(size_t* index) const override;
			bool GetPosition(
				size_t* u,
				size_t* v,
				size_t* w) const override;

		private:
			bool HasNext(const ISO5436_2Container& handle) const;
			bool HasPrev(const ISO5436_2Container& handle) const;

			/*! Instance of the ISO5436-2 X3P to be accessed through this popint iterator. */
			std::weak_ptr<ISO5436_2Container> m_Handle;

			/*! This is the forward- or backward iterator. */
			bool m_IsForward;

			/*! true if in an untouched state. */
			bool m_IsReset{ true };

			/*! true to use matrix indexes and access methods, but false for the simple list interface. */
			bool m_IsMatrix;

			/*! The current index in X direction. Used when iterating both matrices and lists. */
			size_t m_U{};

			/*! The current index in Y direction. Used with matrices only. */
			size_t m_V{};

			/*! The current index in Z direction. Used with matrices only. */
			size_t m_W{};

			/*! The copy-ctor is not implemented. This prevents its usage. */
			PointIteratorImpl(const PointIteratorImpl& src) = delete;
			/*! The assignment-operator is not implemented. This prevents its usage. */
			PointIteratorImpl& operator=(const PointIteratorImpl& src) = delete;
		};
	};
}

#endif
