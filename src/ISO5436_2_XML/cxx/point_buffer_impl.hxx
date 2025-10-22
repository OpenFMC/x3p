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
  * Allocate static memory to store point data.
  */

#ifndef _OPENGPS_POINT_BUFFER_IMPL_HXX
#define _OPENGPS_POINT_BUFFER_IMPL_HXX

#include "point_buffer.hxx"

namespace OpenGPS
{
	/*!
	 * Manages static memory and typesafe access.
	 * Allocates an internal memory buffer to store point data of type ::OGPS_Int16.
	 */
	template<typename TValue, OGPS_DataPointType TType> class PointBufferT : public PointBuffer
	{
	public:
		void Allocate(size_t size) override
		{
			assert(!m_Buffer);

			m_Buffer = AllocateT<TValue>(size);
		}

		void Set(size_t index, TValue value) override
		{
			assert(index < GetSize() && m_Buffer);

			m_Buffer[index] = value;
		}

		void Get(size_t index, TValue& value) const override
		{
			assert(index < GetSize() && m_Buffer);

			value = m_Buffer[index];
		}

		OGPS_DataPointType GetPointType() const override
		{
			return TType;
		}

	private:
		/*! Pointer to internal memory. */
		std::unique_ptr<TValue[]> m_Buffer;
	};

	typedef PointBufferT<OGPS_Int16, OGPS_Int16PointType> Int16PointBuffer;
	typedef PointBufferT<OGPS_Int32, OGPS_Int32PointType> Int32PointBuffer;
	typedef PointBufferT<OGPS_Float, OGPS_FloatPointType> FloatPointBuffer;
	typedef PointBufferT<OGPS_Double, OGPS_DoublePointType> DoublePointBuffer;
}

#endif
