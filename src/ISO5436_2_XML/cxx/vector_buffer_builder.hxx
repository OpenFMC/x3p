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
 * Methods to assemble a OpenGPS::VectorBuffer instance.
 */

#ifndef _OPENGPS_VECTOR_BUFFER_BUILDER_HXX
#define _OPENGPS_VECTOR_BUFFER_BUILDER_HXX

#include <opengps/cxx/opengps.hxx>
#include <opengps/data_point_type.h>
#include <memory>

namespace OpenGPS
{
	class PointBuffer;
	class VectorBuffer;

	/*!
	 * Creates an object which is able to assemble a OpenGPS::VectorBuffer instance.
	 */
	class VectorBufferBuilder
	{
	public:
		/*!
		 * Creates the initial OpenGPS::VectorBuffer to be assembled.
		 * @remarks This must preceed all other steps of the building process.
		 */
		bool BuildBuffer();

		/*!
		 * Connects the appropriate OpenGPS::PointBuffer connected with the X axis description.
		 * @param dataType The type of point data connected to the X axis. A value of
		 * ::OGPS_MissingPointType describes an incremental (non-explicit) axis for which
		 * no point data needs to be stored.
		 * @param size The amount of points stored for the X component of all available vectors.
		 */
		bool BuildX(OGPS_DataPointType dataType, size_t size);

		/*!
		 * Connects the appropriate OpenGPS::PointBuffer connected with the Y axis description.
		 * @param dataType The type of point data connected to the Y axis. A value of
		 * ::OGPS_MissingPointType describes an incremental (non-explicit) axis for which
		 * no point data needs to be stored.
		 * @param size The amount of points stored for the Y component of all available vectors.
		 */
		bool BuildY(OGPS_DataPointType dataType, size_t size);

		/*!
		 * Connects the appropriate OpenGPS::PointBuffer connected with the Z axis description.
		 * @param dataType The type of point data connected to the X axis. Since the Z componet
		 * is forced to be saved explicitly regardless whether the axis is defined as incremental
		 * or not, a value of ::OGPS_MissingPointType is invalid here.
		 * @param size The amount of points stored for the Z component of all available vectors.
		 */
		bool BuildZ(OGPS_DataPointType dataType, size_t size);

		/*!
		 * Connects the appropriate OpenGPS::PointValidityProvider.
		 * @param allowInvalidPoints Set this to true, if invalid points are allowed to be contained
		 * within point vector data, otherwise set this to false. E.g. it makes no sense for a
		 * point cloud to have invalid points (these points should not be within the cloud, afterall).
		 * @remarks This is the last step of the building process.
		 */
		bool BuildValidityProvider(bool allowInvalidPoints);

		/*! Gets the object built. This object instance remains managed by the builder, though. */
		std::shared_ptr<VectorBuffer> GetBuffer() const;

	private:
		/*!
		 * Creates the appropriate OpenGPS::PointBuffer instance depending on the type of point data to be handled.
		 * @param dataType The type of point data that must be handled by the OpenGPS::PointBuffer instance to be created.
		 * @param size The amount of point data to be handled.
		 * @param retval Gets true on success, false otherwise.
		 * @returns Returns an instance of a buffer object or nullptr if the type of point data to be handled equals ::OGPS_MissingPointType.
		 */
		std::shared_ptr<PointBuffer> CreatePointBuffer(OGPS_DataPointType dataType, size_t size, bool& retval) const;

		/*! The vector buffer object to be assembled. */
		std::shared_ptr<VectorBuffer> m_Buffer;
	};
}

#endif
