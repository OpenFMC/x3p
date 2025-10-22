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
 * Interface for reading typed point data from an underlying binary file
 * stream of point vectors.
 */

#ifndef _OPENGPS_BINARY_POINT_VECTOR_READER_CONTEXT_HXX
#define _OPENGPS_BINARY_POINT_VECTOR_READER_CONTEXT_HXX

#include "point_vector_reader_context.hxx"
#include <memory>

namespace OpenGPS
{
	class InputBinaryFileStream;
	class String;

	/*!
	 * Specialized OpenGPS::PointVectorReaderContext for binary streams.
	 */
	class BinaryPointVectorReaderContext : public PointVectorReaderContext
	{
	public:
		void Skip() override;
		bool MoveNext() override;
		bool IsValid() const override;

		/*!
		 * Creates a new instance.
		 * @param filePath Absolute path to the binary file streamed herein.
		 */
		BinaryPointVectorReaderContext(const String& filePath);

		/*! Destroys this instance. */
		~BinaryPointVectorReaderContext() override;

	protected:
		/*!
		 * Asks if the underlying data stream is still valid.
		 * @returns Returns true if no previous access to the underlying
		 * data stream was harmful. Returns false if any damage occured.
		 */
		bool IsGood() const;

		/*!
		 * Returns true if the underlying stream object had successfully
		 * been allocated, false otherwise.
		 */
		bool HasStream() const;

		/*!
		 * Returns the underlying data stream for read access.
		 */
		InputBinaryFileStream* GetStream() const;

		/*!
		 * Closes the internal handle to the binary file and releases its resources.
		 */
		void Close();

		/*! Checks whether the underlying stream is valid. Throws an exception if this is not the case. */
		void CheckStreamAndThrowException();

		/*! Checks whether the underlying stream is valid. Throws an exception if this is not the case. */
		void CheckIsGoodAndThrowException();

	private:
		/*! Pointer to the underlying data stream of binary point vectors. */
		std::unique_ptr<InputBinaryFileStream> m_Stream;
	};
}

#endif
