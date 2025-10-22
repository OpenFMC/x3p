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

#include <opengps/cxx/iso5436_2.hxx>

#include "iso5436_2_container.hxx"
#include "stdafx.hxx"

 /* Open. */
ISO5436_2::ISO5436_2(
	const String& file,
	const String& temp)
	:m_Instance{ std::make_shared<ISO5436_2Container>(file, temp) }
{
}

ISO5436_2::ISO5436_2(const String& file)
	:m_Instance{ std::make_shared<ISO5436_2Container>(file, _T("")) }
{
}

ISO5436_2::~ISO5436_2()
{
	m_Instance->Close();
}

void ISO5436_2::Open()
{
	m_Instance->Open();
}

void ISO5436_2::Create(
	const Schemas::ISO5436_2::Record1Type& record1,
	const Schemas::ISO5436_2::Record2Type* record2,
	const Schemas::ISO5436_2::MatrixDimensionType& matrixDimension,
	bool useBinaryData)
{
	m_Instance->Create(record1, record2, matrixDimension, useBinaryData);
}

void ISO5436_2::Create(
	const Schemas::ISO5436_2::Record1Type& record1,
	const Schemas::ISO5436_2::Record2Type* record2,
	size_t listDimension,
	bool useBinaryData)
{
	m_Instance->Create(record1, record2, listDimension, useBinaryData);
}

PointIteratorAutoPtr ISO5436_2::CreateNextPointIterator()
{
	return m_Instance->CreateNextPointIterator();
}

PointIteratorAutoPtr ISO5436_2::CreatePrevPointIterator()
{
	return m_Instance->CreatePrevPointIterator();
}

void ISO5436_2::SetMatrixPoint(
	size_t u,
	size_t v,
	size_t w,
	const PointVector* vector)
{
	m_Instance->SetMatrixPoint(u, v, w, vector);
}

void ISO5436_2::GetMatrixPoint(
	size_t u,
	size_t v,
	size_t w,
	PointVector& vector)
{
	m_Instance->GetMatrixPoint(u, v, w, vector);
}

void ISO5436_2::SetListPoint(
	size_t index,
	const PointVector& vector)
{
	m_Instance->SetListPoint(index, vector);
}

void ISO5436_2::GetListPoint(
	size_t index,
	PointVector& vector)
{
	m_Instance->GetListPoint(index, vector);
}

void ISO5436_2::GetMatrixCoord(
	size_t u,
	size_t v,
	size_t w,
	OGPS_Double* x,
	OGPS_Double* y,
	OGPS_Double* z)
{
	m_Instance->GetMatrixCoord(u, v, w, x, y, z);
}

bool ISO5436_2::IsMatrixCoordValid(
	size_t u,
	size_t v,
	size_t w)
{
	return m_Instance->IsMatrixCoordValid(u, v, w);
}

void ISO5436_2::GetListCoord(
	size_t index,
	OGPS_Double* x,
	OGPS_Double* y,
	OGPS_Double* z)
{
	m_Instance->GetListCoord(index, x, y, z);
}

Schemas::ISO5436_2::ISO5436_2Type* ISO5436_2::GetDocument()
{
	return m_Instance->GetDocument();
}

bool ISO5436_2::IsMatrix() const
{
	return m_Instance->IsMatrix();
}

void ISO5436_2::GetMatrixDimensions(
	size_t* size_u,
	size_t* size_v,
	size_t* size_w) const
{
	m_Instance->GetMatrixDimensions(size_u, size_v, size_w);
}

size_t ISO5436_2::GetListDimension() const
{
	return m_Instance->GetListDimension();
}

void ISO5436_2::Write(int compressionLevel)
{
	m_Instance->Write(compressionLevel);
}

void ISO5436_2::Close()
{
	m_Instance->Close();
}

void ISO5436_2::AppendVendorSpecific(const String& vendorURI, const String& filePath)
{
	m_Instance->AppendVendorSpecific(vendorURI, filePath);
}

bool ISO5436_2::GetVendorSpecific(const String& vendorURI, const String& fileName, const String& targetPath)
{
	return m_Instance->GetVendorSpecific(vendorURI, fileName, targetPath);
}
