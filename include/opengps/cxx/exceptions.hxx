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
 * Exception types thrown within this software library.
 */


#ifndef _OPENGPS_CXX_EXCPTIONS_HXX
#define _OPENGPS_CXX_EXCPTIONS_HXX

#include <stdexcept>
#include <opengps/cxx/opengps.hxx>
#include <opengps/messages.h>
#include <opengps/cxx/string.hxx>

namespace OpenGPS
{
	/*!
	 * Describes a general exception.
	 */
	class _OPENGPS_EXPORT Exception : public std::runtime_error
	{
	public:
		/*!
		 * Creates a new instance.
		 *
		 * @param id Type of the exception object.
		 * @param text Brief description of the exception.
		 * @param details Detailed description of the exception. May be set to nullptr.
		 * @param method The name of the method wherein the failure condition occured. May be set to nullptr.
		 */
		Exception(OGPS_ExceptionId id, const OGPS_ExceptionChar* text, const OGPS_ExceptionChar* details, const OGPS_ExceptionChar* method);
		/*!
		 * Creates a new instance.
		 *
		 * @param rhs The instance to copy from.
		 */
		Exception(const Exception& rhs);

		Exception& operator=(const Exception& rhs);

		/*! Gets an identifier for the current type of excpetion. */
		OGPS_ExceptionId id() const;

		/*! Gets a detailed description possible with hints to its avoidance. */
		const String& details() const;

		/*! Gets the name of the method where the exception occured or nullptr. */
		const String& method() const;

	private:
		/*! Describes the type of failure. */
		OGPS_ExceptionId m_Id;

		/*! Detailed description of the failure. */
		String m_Details;

		/*! Name of the method wherein the failure occured. */
		String m_Method;
	};
}

#endif
