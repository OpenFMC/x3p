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
 * The environment of Microsoft Windows operating systems.
 */

#ifndef _OPENGPS_WIN32_ENVIRONMENT_HXX
#define _OPENGPS_WIN32_ENVIRONMENT_HXX

#ifdef _WIN32

#include "environment.hxx"

namespace OpenGPS
{
   /*!
    * Implements access methods to various parts of the environment
    * on Microsoft Windows operating systems.
    */
   class Win32Environment : public Environment
   {
   public:
      /*! Creates a new instance. */
      Win32Environment();

      /*! Destroys this instance. */
      ~Win32Environment() override;

      OGPS_Character GetDirectorySeparator() const override;
      OGPS_Character GetAltDirectorySeparator() const override;
      bool GetPathName(const String& path, String& clean_path) const override;
      String GetFileName(const String& path) const override;
      String ConcatPathes(const String& path1, const String& path2) const override;
      bool PathExists(const String& file) const override;
      bool RemoveFile(const String& file) const override;
      String GetUniqueName() const override;
      bool CreateDir(const String& path) const override;
      bool RemoveDir(const String& path) const override;
      String GetTempDir() const override;
      bool RenameFile(const String& src, const String& dst) const override;
      bool GetVariable(const String& varName, String& value) const override;
      String GetLastErrorMessage() const override;

   private:
      /*! Resets the last system error API code. */
      void ResetLastErrorCode() const;

      /*! Tracks whether the sofware randomization API still needs to be initialized before its usage. */
      static bool m_InitRandom;
   };
}

#endif

#endif
