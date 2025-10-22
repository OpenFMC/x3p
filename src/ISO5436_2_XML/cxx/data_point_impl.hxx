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
 * An implementation of a data point to be used for typesafe storage
 * and access of point data.
 */

#ifndef _OPENGPS_DATA_POINT_IMPL_HXX
#define _OPENGPS_DATA_POINT_IMPL_HXX

#include <opengps/cxx/data_point.hxx>

namespace OpenGPS
{
	/*!
	 * A straightforward implementation of OpenGPS::DataPoint. This mainly
	 * supports OpenGPS::PointVector where typesafe storage
	 * of arbitrary point data must be maintained for its vector components.
	 */
	class DataPointImpl : public DataPoint
	{
	public:
		OGPS_DataPointType GetPointType() const override;

		void Get(OGPS_Int16* value) const override;
		void Get(OGPS_Int32* value) const override;
		void Get(OGPS_Float* value) const override;
		void Get(OGPS_Double* value) const override;

		OGPS_Double Get() const override;

		bool IsValid() const override;

		void Set(OGPS_Int16 value) override;
		void Set(OGPS_Int32 value) override;
		void Set(OGPS_Float value) override;
		void Set(OGPS_Double value) override;

		void Set(const DataPoint& src) override;

	protected:
		void Reset() override;

	private:
		/*! This tag defines which value type is currently valid within DataPointImpl::m_Value.
		 * This restricts access to the only safe element of DataPointImpl::m_Value. */
		OGPS_DataPointType m_Type{ OGPS_MissingPointType };

		/*! Typesafe storage for every possible type of point data. */
		union OGPS_DataPointValue
		{
			OGPS_DataPointValue()
				:doubleValue{ 0.0 }
			{
			}

			/*! Stores a value of type ::OGPS_Int16. It is undefined if
			DataPointImpl::m_Type does not equal ::OGPS_Int16PointType. */
			OGPS_Int16 int16Value;

			/*! Stores a value of type ::OGPS_Int32. It is undefined if
			DataPointImpl::m_Type does not equal ::OGPS_Int32PointType. */
			OGPS_Int32 int32Value;

			/*! Stores a value of type ::OGPS_Float. It is undefined if
			DataPointImpl::m_Type does not equal ::OGPS_FloatPointType. */
			OGPS_Float floatValue;

			/*! Stores a value of type ::OGPS_Double. It is undefined if
			DataPointImpl::m_Type does not equal ::OGPS_DoublePointType. */
			OGPS_Double doubleValue;
		};

		/*! The stored value of this data point. @see DataPointImpl::m_Type */
		OGPS_DataPointValue m_Value;
	};
}

#endif
