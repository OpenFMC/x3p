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
  * Implementation of access methods for writing typed point data to a string list of point vectors.
  */

#ifndef _OPENGPS_XML_POINT_VECTOR_WRITER_CONTEXT_HXX
#define _OPENGPS_XML_POINT_VECTOR_WRITER_CONTEXT_HXX

#include "point_vector_writer_context.hxx"
#include <opengps/cxx/iso5436_2_xsd.hxx>

namespace OpenGPS
{
	class PointVectorOutputStringStream;
	class String;

	/*!
	 * Specialized OpenGPS::PointVectorWriterContext for point vectors stored as list of strings.
	 * Each string in the list represents one point vector. The values of the three coordinates
	 * are seperated either by free space or a semicolon. If a value of a coordinate needs
	 * not to be stored in the string because its corresponding axis has an incremental axis
	 * definition the value is completely omittet, i.e. no semicolon is written either.
	 */
	class XmlPointVectorWriterContext : public PointVectorWriterContext
	{
	public:
		typedef Schemas::ISO5436_2::DataListType::Datum_sequence StringList;

		/*!
		 * Creates a new instance.
		 * @param pointVectorList The list of point vectors to be streamed herein.
		 */
		XmlPointVectorWriterContext(StringList* pointVectorList);

		void Write(OGPS_Int16 value) override;
		void Write(OGPS_Int32 value) override;
		void Write(OGPS_Float value) override;
		void Write(OGPS_Double value) override;

		void Skip() override;

		void MoveNext() override;

	protected:
		/*!
		 * Asks if the underlying data stream is still valid.
		 * @returns Returns true if no previous access to the underlying
		 * data stream was harmful. Returns false if any damage occured.
		 */
		bool IsGood() const override;

		/*!
		 * Appends the separator of two data point values.
		 */
		void AppendSeparator();

	private:
		/*!
		 * Gets the content of the inner stream buffer. After all three
		 * vector components have been written this euqals the string
		 * representation of a single point vector.
		 *
		 * @param value Returns the content of the stream buffer.
		 */
		void Get(String& value) const;

		/*!
		 * Resets/empties the inner stream buffer.
		 */
		void Reset();

		template<typename T> inline void WriteT(T value);

		/*! Checks whether the underlying stream is valid. Throws an exception if this is not the case. */
		void CheckStreamAndThrowException();

		/*! Checks whether the underlying stream is valid. Throws an exception if this is not the case. */
		void CheckIsGoodAndThrowException();

		/*! The inner stream buffer which holds the current compilation of
		 * a point vector to be added to the string list. */
		std::unique_ptr<PointVectorOutputStringStream> m_Stream;

		/*! true if a separator needs to be added on the next call to
		 * XmlPointVectorWriterContext::Write, false otherwise. */
		bool m_NeedsSeparator{};

		/*! The inner string list of point vectors written so far. */
		StringList* m_PointVectorList;
	};
}

#endif
