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
  * Make the internal memory structure of distinct data point buffers
  * accessable as point vector data.
  */

#ifndef _OPENGPS_POINT_VECTOR_PROXY_HXX
#define _OPENGPS_POINT_VECTOR_PROXY_HXX

#include <opengps/cxx/point_vector_base.hxx>
#include <opengps/cxx/data_point.hxx>

namespace OpenGPS
{
	class PointVectorProxyContext;
	class VectorBuffer;
	class PointBuffer;

	/*!
	 * Serves one row of the three distinct buffers of data points managed
	 * by OpenGPS::VectorBuffer and raises the expression of accessing
	 * a single point vector.
	 */
	class PointVectorProxy : public PointVectorBase
	{
	public:
		/*!
		 * Creates a new instance.
		 * @param context Provides information which data row is currently proxied.
		 * This is read-only within the proxy object but can be changed from the outside.
		 * @param buffer Manages the whole set of all point vectors stacked together.
		 */
		PointVectorProxy(
			std::shared_ptr<PointVectorProxyContext> context,
			const VectorBuffer& buffer);

		~PointVectorProxy() override;

		DataPoint* GetX() const override;
		DataPoint* GetY() const override;
		DataPoint* GetZ() const override;

		void Set(const PointVectorBase& value) override;
		void Get(PointVectorBase& value) const override;

	private:
		/*! Provides typesafe access to the point data of the current X component. */
		std::unique_ptr<DataPoint> m_X;

		/*! Provides typesafe access to the point data of the current Y component. */
		std::unique_ptr<DataPoint> m_Y;

		/*! Provides typesafe access to the point data of the current Z component. */
		std::unique_ptr<DataPoint> m_Z;

		/*! The copy-ctor is not implemented. This prevents its usage. */
		PointVectorProxy(const PointVectorProxy& src) = delete;
		/*! The assignment-operator is not implemented. This prevents its usage. */
		PointVectorProxy& operator=(const PointVectorProxy& src) = delete;

		/*!
		 * Proxies one single data point
		 * pointed to by the current context information.
		 */
		class DataPointProxy : public DataPoint
		{
		public:
			/*!
			 * Creates a new instance.
			 * @param context Provides the information which row of point data is currently proxied.
			 * @param buffer The buffer of all point data stacked together.
			 */
			DataPointProxy(
				std::shared_ptr<PointVectorProxyContext> context,
				std::shared_ptr<PointBuffer> buffer);

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

			void Reset() override;

		private:
			template<typename T> inline void GetT(T* value) const;

			template<typename T> inline void SetT(T value);

			/*! Provides information which data row is currently proxied. */
			std::shared_ptr<PointVectorProxyContext> m_Context;

			/*! Provides access to the point data. */
			std::shared_ptr<PointBuffer> m_Buffer;
		};
	};
}

#endif
