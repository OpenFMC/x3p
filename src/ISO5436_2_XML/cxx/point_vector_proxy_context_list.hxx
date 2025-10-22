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
 * Indexing of point data managed by OpenGPS::VectorBuffer.
 */

#ifndef _OPENGPS_POINT_VECTOR_PROXY_CONTEXT_LIST_HXX
#define _OPENGPS_POINT_VECTOR_PROXY_CONTEXT_LIST_HXX

#include <opengps/cxx/opengps.hxx>
#include "point_vector_proxy_context.hxx"

namespace OpenGPS
{
	/*!
	 * Indexing of point data managed by OpenGPS::VectorBuffer.
	 * The indexes are calculated for point measurements stored in
	 * a simple list structure.
	 */
	class PointVectorProxyContextList : public PointVectorProxyContext
	{
	public:
		/*!
		 * Creates a new instance.
		 * @param maxIndex The maximum amount of indexable measurement data.
		 * In other words: the number of elements contained in the list structure.
		 */
		PointVectorProxyContextList(size_t maxIndex);

		/*!
		 * Sets the current index.
		 * @param index The new arbitrary index.
		 * @returns Returns true on success, false otherwise.
		 */
		void SetIndex(size_t index);

		size_t GetIndex() const override;
		bool CanIncrementIndex() const override;
		bool IncrementIndex() override;
		bool IsMatrix() const override;

	private:
		/*! The current index. */
		size_t m_Index{};

		/*! The maximum index possible. */
		size_t m_MaxIndex;
	};
}

#endif
