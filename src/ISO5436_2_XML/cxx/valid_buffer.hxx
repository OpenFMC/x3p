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
 * Communicate validity of point vectors through external binary file.
 */

#ifndef _OPENGPS_VALID_BUFFER_HXX
#define _OPENGPS_VALID_BUFFER_HXX

#include <opengps/cxx/opengps.hxx>
#include "point_validity_provider.hxx"
#include <iostream>

namespace OpenGPS
{
	/*!
	 * Implements the OpenGPS::PointValidityProvider as an external binary file.
	 *
	 * All bit values of the binary file correspond to the location of the point
	 * vector of the same index. If the file bit is on (set to one) the point
	 * vector is valid, if the bit at the given index is off, then the point
	 * vector at the corresponding location has invalid data.
	 */
	class ValidBuffer : public PointValidityProvider
	{
	public:
		/*! Destroys this instance. */
		~ValidBuffer() override;

		/*! Returns wheter the bit buffer has already been allocated. */
		bool IsAllocated() const;

		/*!
		 * Maps the bit buffer from a binary stream.
		 * @param stream The bit array gets copied from here.
		 */
		void Read(std::basic_istream<char>& stream);

		/*!
		 * Maps the bit buffer to a binary stream.
		 * @param stream The internal bit array gets written to the given stream.
		 */
		void Write(std::ostream& stream);

		/*!
		 * Checks whether the current instance serves any point data that is marked as invalid.
		 * @returns false if none of the referenced point data is marked as invalid here, otherwise true.
		 */
		bool HasInvalidMarks() const;

		void SetValid(size_t index, bool value) override;
		bool IsValid(size_t index) const override;

	protected:
		/*!
		 * Creates a new instance.
		 * @param value The point buffer of the Z axis.
		 */
		ValidBuffer(std::shared_ptr<PointBuffer> value);

		/*! Allocates the internal bit array. Initially all point vectors are assumed to be valid. */
		void Allocate();

		/*! Frees allocated resources. */
		void Reset();

	private:
		/*!
		 * Allocates the internal bit array. Initially all point vectors are assumed to be valid.
		 * @param rawSize Amount of memory to be allocated in bytes.
		 */
		void AllocateRaw(size_t rawSize);

		/*! Pointer to the internal bit array. */
		std::unique_ptr<unsigned char[]> m_ValidityBuffer;

		/*! Size of the bit array in bytes. */
		size_t m_RawSize{};
	};

	/*!
	 * Implementation of OpenGPS::ValidBuffer for Z axis of ::OGPS_Int16 data type.
	 */
	class Int16ValidBuffer : public ValidBuffer
	{
	public:
		/*!
		 * Creates a new instance.
		 * @param value The point buffer of the Z axis.
		 */
		Int16ValidBuffer(std::shared_ptr<PointBuffer> value);

		void SetValid(size_t index, bool value) override;
	};

	/*!
	 * Implementation of OpenGPS::ValidBuffer for Z axis of ::OGPS_Int32 data type.
	 */
	class Int32ValidBuffer : public ValidBuffer
	{
	public:
		/*!
		 * Creates a new instance.
		 * @param value The point buffer of the Z axis.
		 */
		Int32ValidBuffer(std::shared_ptr<PointBuffer> value);

		void SetValid(size_t index, bool value) override;
	};
}

#endif
