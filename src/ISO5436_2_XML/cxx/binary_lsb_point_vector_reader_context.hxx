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
 * Implementation of access methods for reading typed point data from a binary file of point vectors.
 */

#ifndef _OPENGPS_BINARY_LSB_POINT_VECTOR_READER_CONTEXT_HXX
#define _OPENGPS_BINARY_LSB_POINT_VECTOR_READER_CONTEXT_HXX

#include "binary_point_vector_reader_context.hxx"

namespace OpenGPS
{
	class String;

	/*!
	 * Implements OpenGPS::BinaryPointVectorReaderContext for binary files to
	 * be parsed on machines reading in least significant byte order.
	 */
	class BinaryLSBPointVectorReaderContext : public BinaryPointVectorReaderContext
	{
	public:
		using BinaryPointVectorReaderContext::BinaryPointVectorReaderContext;

		void Read(OGPS_Int16& value) override;
		void Read(OGPS_Int32& value) override;
		void Read(OGPS_Float& value) override;
		void Read(OGPS_Double& value) override;

	private:
		template<typename T, size_t TSize> inline void ReadT(T& value);
	};
}

#endif
