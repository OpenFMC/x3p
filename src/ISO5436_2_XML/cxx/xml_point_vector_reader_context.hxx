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
 * Implementation of access methods for reading typed point data from a string list of point vectors.
 */

#ifndef _OPENGPS_XML_POINT_VECTOR_READER_CONTEXT_HXX
#define _OPENGPS_XML_POINT_VECTOR_READER_CONTEXT_HXX

#include "point_vector_reader_context.hxx"
#include <opengps/cxx/iso5436_2_xsd.hxx>

namespace OpenGPS
{
	class PointVectorInputStringStream;
	class String;

	/*!
	 * Specialized OpenGPS::PointVectorReaderContext for point vectors stored as list of strings.
	 * Each string in the list represents one point vector. The values of the three coordinates
	 * are seperated either by free space or a semicolon. If a value of a coordinate needs
	 * not to be stored in the string because its corresponding axis has an incremental axis
	 * definition the value is completely omittet, i.e. no semicolon is written either.
	 * If there is an empty string in the list, it is recognised as an invalid 3d point measurement.
	 */
	class XmlPointVectorReaderContext : public PointVectorReaderContext
	{
	public:
		typedef Schemas::ISO5436_2::DataListType::Datum_sequence StringList;

		/*!
		 * Creates a new instance.
		 * @param pointVectorList The list of point vectors to be streamed herein.
		 */
		XmlPointVectorReaderContext(const StringList* pointVectorList);

		/*! Destroys this instance. */
		virtual ~XmlPointVectorReaderContext();

		void Read(OGPS_Int16& value) override;
		void Read(OGPS_Int32& value) override;
		void Read(OGPS_Float& value) override;
		void Read(OGPS_Double& value) override;

		void Skip() override;

		bool MoveNext() override;
		bool IsValid() const override;

	protected:
		/*!
		 * Asks if the underlying data stream is still valid.
		 * @returns Returns true if no previous access to the underlying
		 * data stream was harmful. Returns false if any damage occured.
		 */
		bool IsGood() const;

	private:
		/*!
		 * Feeds the underlying stream object with one single string
		 * out of the inner string list to be parsed as the current vector.
		 * @param buf The string representation of the next point vector to be parsed.
		 */
		void Set(const String& buf);

		/*!
		 * Resets the inner stream object.
		 */
		void Reset();

		template<typename T> inline void ReadT(T& value);

		/*! Checks whether the underlying stream is valid. Throws an exception if this is not the case. */
		void CheckStreamAndThrowException();

		/*! Checks whether the underlying stream is valid. Throws an exception if this is not the case. */
		void CheckIsGoodAndThrowException();

		/*! The inner list of all point vectors to be parsed. */
		const StringList* m_PointVectorList;

		/*! The index of the next point vector to be parsed. */
		size_t m_Next{};

		/*! The inner stream object which streams the current point vector. */
		std::unique_ptr<PointVectorInputStringStream> m_Stream;
	};
}

#endif
