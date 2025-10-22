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
 * Interface for writing typed point data to an underlying binary of point vectors.
 */

#ifndef _OPENGPS_BINARY_POINT_VECTOR_WRITER_CONTEXT_HXX
#define _OPENGPS_BINARY_POINT_VECTOR_WRITER_CONTEXT_HXX

#include "point_vector_writer_context.hxx"
#include "zip_stream_buffer.hxx"
#include <memory>

namespace OpenGPS
{
	/*!
	 * Implements OpenGPS::PointVectorWriterContext for writing to
	 * compressed binary streams of point vectors. Usually
	 * this points to a file descriptor within a zip archive.
	 */
	class BinaryPointVectorWriterContext : public PointVectorWriterContext
	{
	public:
		/*!
		 * Creates a new instance.
		 * @param handle The zip-stream where binary data is written to.
		 */
		BinaryPointVectorWriterContext(zipFile handle);

		/*! Destroys this instance. */
		~BinaryPointVectorWriterContext() override;

		/*!
		 * Closes the internal handle to the binary stream and frees its resources.
		 */
		void Close();

		void Skip() override;
		void MoveNext() override;

		/*!
		 * Gets the md5 checksum of all bytes written.
		 * When called this resets the currently computed md5 sum. Future
		 * calls to this method will ignore older md5 data.
		 */
		void GetMd5(std::array<unsigned char, 16>& md5);

	protected:
		/*!
		 * Gets the internal binary stream.
		 * @returns Returns the target binary stream or nullptr.
		 */
		std::ostream* GetStream();

		/*!
		 * Asks whether there is a target stream available.
		 * @returns Returns true if there is an operable target
		 * stream for point data, false otherwise.
		 * @see BinaryPointVectorWriterContext::IsGood
		 */
		bool HasStream() const;

		/*!
		 * Asks if the underlying data stream is still valid.
		 * @returns Returns true if no previous access to the underlying
		 * data stream was harmful. Returns false if any damage occured.
		 */
		bool IsGood() const override;

		/*! Checks whether the underlying stream is valid. Throws an exception if this is not the case. */
		void CheckStreamAndThrowException();

		/*! Checks whether the underlying stream is valid. Throws an exception if this is not the case. */
		void CheckIsGoodAndThrowException();
	private:
		/*! The target buffer where compressed binary data gets stored. */
		std::unique_ptr<ZipStreamBuffer> m_Buffer;

		/*! The stream buffer which makes BinaryPointVectorWriterContext::m_Buffer
		 * accessible through the more abstract std::ostream interface. */
		std::unique_ptr<ZipOutputStream> m_Stream;
	};
}

#endif
