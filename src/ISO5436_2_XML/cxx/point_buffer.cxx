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

#include "point_buffer.hxx"
#include "stdafx.hxx"

#include <stdlib.h>
#include <string.h>

#include <opengps/cxx/exceptions.hxx>

void PointBuffer::Set(size_t, OGPS_Int16)
{
   throw Exception(
      OGPS_ExInvalidOperation,
      _EX_T("Could not set point data due to conflicting types."),
      _EX_T("There was an attempt to write point data of type OGPS_Int16, but the axis description indicates a different type of point data. Both type information must match exacly to make this operation valid."),
      _EX_T("OpenGPS::PointBuffer::Set"));
}

void PointBuffer::Set(size_t, OGPS_Int32)
{
   throw Exception(
      OGPS_ExInvalidOperation,
      _EX_T("Could not set point data due to conflicting types."),
      _EX_T("There was an attempt to write point data of type OGPS_Int32, but the axis description indicates a different type of point data. Both type information must match exacly to make this operation valid."),
      _EX_T("OpenGPS::PointBuffer::Set"));
}

void PointBuffer::Set(size_t, OGPS_Float)
{
   throw Exception(
      OGPS_ExInvalidOperation,
      _EX_T("Could not set point data due to conflicting types."),
      _EX_T("There was an attempt to write point data of type OGPS_Float, but the axis description indicates a different type of point data. Both type information must match exacly to make this operation valid."),
      _EX_T("OpenGPS::PointBuffer::Set"));
}

void PointBuffer::Set(size_t, OGPS_Double)
{
   throw Exception(
      OGPS_ExInvalidOperation,
      _EX_T("Could not set point data due to conflicting types."),
      _EX_T("There was an attempt to write point data of type OGPS_Double, but the axis description indicates a different type of point data. Both type information must match exacly to make this operation valid."),
      _EX_T("OpenGPS::PointBuffer::Set"));
}

void PointBuffer::Get(size_t, OGPS_Int16&) const
{
   throw Exception(
      OGPS_ExInvalidOperation,
      _EX_T("Could not read point data due to conflicting types."),
      _EX_T("There was an attempt to read point data of type OGPS_Int16, but the axis description indicates a different type of point data. Both type information must match exacly to make this operation valid."),
      _EX_T("OpenGPS::PointBuffer::Get"));
}

void PointBuffer::Get(size_t, OGPS_Int32&) const
{
   throw Exception(
      OGPS_ExInvalidOperation,
      _EX_T("Could not read point data due to conflicting types."),
      _EX_T("There was an attempt to read point data of type OGPS_Int32, but the axis description indicates a different type of point data. Both type information must match exacly to make this operation valid."),
      _EX_T("OpenGPS::PointBuffer::Get"));
}

void PointBuffer::Get(size_t, OGPS_Float&) const
{
   throw Exception(
      OGPS_ExInvalidOperation,
      _EX_T("Could not read point data due to conflicting types."),
      _EX_T("There was an attempt to read point data of type OGPS_Float, but the axis description indicates a different type of point data. Both type information must match exacly to make this operation valid."),
      _EX_T("OpenGPS::PointBuffer::Get"));
}

void PointBuffer::Get(size_t, OGPS_Double&) const
{
   throw Exception(
      OGPS_ExInvalidOperation,
      _EX_T("Could not read point data due to conflicting types."),
      _EX_T("There was an attempt to read point data of type OGPS_Double, but the axis description indicates a different type of point data. Both type information must match exacly to make this operation valid."),
      _EX_T("OpenGPS::PointBuffer::Get"));
}

size_t PointBuffer::GetSize() const 
{
   return m_Size;
}

void PointBuffer::Allocate(size_t)
{
   throw Exception(
      OGPS_ExNotImplemented,
      _EX_T("This method has not been implemented yet."),
      _EX_T("Implement this method for the current specialisation to be able to store some data herein."),
      _EX_T("OpenGPS::PointBuffer::Allocate"));
}

OGPS_DataPointType PointBuffer::GetPointType() const 
{
   return OGPS_MissingPointType;
}
