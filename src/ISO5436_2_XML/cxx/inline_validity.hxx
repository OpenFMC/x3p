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
 * Communicate validity of point vectors through special IEEE754 values.
 */

#ifndef _OPENGPS_INLINE_VALIDITY_HXX
#define _OPENGPS_INLINE_VALIDITY_HXX

#include <opengps/cxx/opengps.hxx>
#include <opengps/cxx/exceptions.hxx>
#include "point_validity_provider.hxx"
#include "point_buffer.hxx"
#include <limits>

namespace OpenGPS
{
	/*!
	 * Implements OpenGPS::PointValidityProvider as a lookup
	 * of a special IEEE754 value.
	 *
	 * If the value of the point data stored for the Z component
	 * of a point vector equals the special IEEE754 value
	 * of not-a-number, then the point measurement is identified as invalid.
	 */
	template<typename T, OGPS_DataPointType TPoint> class InlineValidityT : public PointValidityProvider
	{
	public:
		/*!
		 * Creates a new instance.
		 * @param value The point buffer of the Z axis.
		 * @param allowInvalidPoints Whether the point buffer is allowed to contain
		 * invalid points.
		 */
		InlineValidityT(std::shared_ptr<PointBuffer> value, bool allowInvalidPoints);

		void SetValid(size_t index, bool value) override;
		bool IsValid(size_t index) const override;

	private:
		/*! Defines whether the point buffer is allowed to contain invalid points. */
		const bool m_AllowInvalidPoints;

		/*! Not implemented. */
		InlineValidityT(const InlineValidityT& src) = delete;
		InlineValidityT& operator=(const InlineValidityT& src) = delete;
	};

	template<typename T, OGPS_DataPointType TPoint>
	inline InlineValidityT<T, TPoint>::InlineValidityT(std::shared_ptr<PointBuffer> value, bool allowInvalidPoints)
		:PointValidityProvider(value),
		m_AllowInvalidPoints(allowInvalidPoints)
	{
		assert(value && value->GetPointType() == TPoint);
	}

	template<typename T, OGPS_DataPointType TPoint>
	inline void InlineValidityT<T, TPoint>::SetValid(size_t index, bool value)
	{
		static_assert(std::numeric_limits<T>::has_quiet_NaN, "Type for InlineValidityT has no NaN");

		if (!value)
		{
			if (!m_AllowInvalidPoints)
			{
				throw Exception(
					OGPS_ExInvalidOperation,
					_EX_T("This point data set may not contain invalid points."),
					_EX_T("An attempt was made to set an invalid point, although this has been explicitly forbidden. E.g. point clouds of record1 feature type PCL are not allowed to contain invalid points."),
					_EX_T("OpenGPS::InlineValidityT::SetValid"));
			}

			GetPointBuffer()->Set(index, std::numeric_limits<T>::quiet_NaN());
		}
	}

	template<typename T, OGPS_DataPointType TPoint>
	inline bool InlineValidityT<T, TPoint>::IsValid(size_t index) const
	{
		static_assert(std::numeric_limits<T>::has_quiet_NaN, "Type for InlineValidityT has no NaN");

		T value{};
		GetPointBuffer()->Get(index, value);
		// Comparing a NaN to itself is allways false
		const bool isValid = (value == value);
		if (!isValid && !m_AllowInvalidPoints)
		{
			throw Exception(
				OGPS_ExWarning,
				_EX_T("This point data set may not contain invalid points."),
				_EX_T("Although this has been explicitly forbidden the current point data contains invalid points. This may be ignored, but e.g. for point clouds of record1 feature type PCL it makes no sense to contain invalid points. These points should not be within the cloud, afterall."),
				_EX_T("OpenGPS::InlineValidityT::IsValid"));
		}
		return isValid;
	}

	typedef InlineValidityT<OGPS_Float, OGPS_FloatPointType> FloatInlineValidity;
	typedef InlineValidityT<OGPS_Double, OGPS_DoublePointType> DoubleInlineValidity;
}

#endif
