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

#include "iso5436_2_container.hxx"
#include <opengps/cxx/point_vector.hxx>

#include "stdafx.hxx"

ISO5436_2Container::PointIteratorImpl::PointIteratorImpl(
	std::shared_ptr<ISO5436_2Container> handle,
	bool isForward,
	bool isMatrix)
	:m_Handle{ handle },
	m_IsForward{ isForward },
	m_IsMatrix{ isMatrix }
{
	assert(handle);
}

bool ISO5436_2Container::PointIteratorImpl::HasNext() const
{
	if (auto handle = m_Handle.lock())
	{
		return HasNext(*handle);
	}

	return false;
}

bool ISO5436_2Container::PointIteratorImpl::HasPrev() const
{
	if (auto handle = m_Handle.lock())
	{
		return HasPrev(*handle);
	}

	return false;
}

bool ISO5436_2Container::PointIteratorImpl::MoveNext()
{
	if (auto handle = m_Handle.lock())
	{
		assert(handle->IsMatrix() == m_IsMatrix);

		if (HasNext(*handle))
		{
			if (m_IsReset)
			{
				assert(m_U == 0 && m_V == 0 && m_W == 0);

				m_IsReset = false;

				return true;
			}

			if (m_IsMatrix)
			{
				if (m_U + 1 < handle->GetMaxU())
				{
					++m_U;

					return true;
				}

				if (m_V + 1 < handle->GetMaxV())
				{
					++m_V;
					m_U = 0;

					return true;
				}

				if (m_W + 1 < handle->GetMaxW())
				{
					++m_W;
					m_U = 0;
					m_V = 0;

					return true;
				}
			}
			else
			{
				++m_U;

				return true;
			}
		}
	}

	return false;
}

bool ISO5436_2Container::PointIteratorImpl::MovePrev()
{
	if (auto handle = m_Handle.lock())
	{
		assert(handle->IsMatrix() == m_IsMatrix);

		if (HasPrev(*handle))
		{
			if (m_IsMatrix)
			{
				if (m_IsReset)
				{
					assert(m_W == 0 && m_V == 0 && m_U == 0);
					assert(handle->GetMaxW() > 0 && handle->GetMaxV() > 0 && handle->GetMaxU() > 0);

					m_W = handle->GetMaxW() - 1;
					m_V = handle->GetMaxV() - 1;
					m_U = handle->GetMaxU() - 1;

					m_IsReset = false;

					return true;
				}

				if (m_W > 0)
				{
					--m_W;

					return true;
				}

				if (m_V > 0)
				{
					assert(handle->GetMaxW() > 0);

					m_W = handle->GetMaxW() - 1;
					--m_V;

					return true;
				}

				if (m_U > 0)
				{
					assert(handle->GetMaxW() > 0);
					assert(handle->GetMaxV() > 0);

					m_W = handle->GetMaxW() - 1;
					m_V = handle->GetMaxV() - 1;
					--m_U;

					return true;
				}
			}
			else
			{
				if (m_U > 0)
				{
					--m_U;

					return true;
				}

				if (m_IsReset)
				{
					assert(m_U == 0);
					assert(handle->GetMaxU() > 0);

					m_U = handle->GetMaxU() - 1;
					m_IsReset = false;

					return true;
				}
			}
		}
	}

	return false;
}

void ISO5436_2Container::PointIteratorImpl::ResetNext()
{
	m_U = m_V = m_W = 0;
	m_IsReset = true;
	m_IsForward = true;
}

void ISO5436_2Container::PointIteratorImpl::ResetPrev()
{
	m_U = m_V = m_W = 0;
	m_IsReset = true;
	m_IsForward = false;
}

void ISO5436_2Container::PointIteratorImpl::GetCurrent(PointVector& vector)
{
	if (auto handle = m_Handle.lock())
	{
		assert(handle->IsMatrix() == m_IsMatrix);

		if (m_IsMatrix)
		{
			handle->GetMatrixPoint(m_U, m_V, m_W, vector);
		}
		else
		{
			handle->GetListPoint(m_U, vector);
		}
	}
}

void ISO5436_2Container::PointIteratorImpl::GetCurrentCoord(PointVector& vector)
{
	if (auto handle = m_Handle.lock())
	{
		assert(handle->IsMatrix() == m_IsMatrix);

		OGPS_Double x{}, y{}, z{};

		if (m_IsMatrix)
		{
			handle->GetMatrixCoord(m_U, m_V, m_W, &x, &y, &z);
		}
		else
		{
			handle->GetListCoord(m_U, &x, &y, &z);
		}

		vector.SetXYZ(x, y, z);
	}
}

void ISO5436_2Container::PointIteratorImpl::SetCurrent(const PointVector* vector)
{
	if (auto handle = m_Handle.lock())
	{
		assert(handle->IsMatrix() == m_IsMatrix);

		if (m_IsMatrix)
		{
			handle->SetMatrixPoint(m_U, m_V, m_W, vector);
		}
		else
		{
			// nullptr vector (invalid point) makes no sense in list type
			assert(vector);
			handle->SetListPoint(m_U, *vector);
		}
	}
}

bool ISO5436_2Container::PointIteratorImpl::GetPosition(size_t* index) const
{
	assert(index);

	if (!m_IsMatrix)
	{
		*index = m_U;
		return true;
	}

	return false;
}

bool ISO5436_2Container::PointIteratorImpl::GetPosition(
	size_t* u,
	size_t* v,
	size_t* w) const
{
	if (m_IsMatrix)
	{
		if (u)
		{
			*u = m_U;
		}

		if (v)
		{
			*v = m_V;
		}

		if (w)
		{
			*w = m_W;
		}

		return true;
	}

	return false;
}

bool ISO5436_2Container::PointIteratorImpl::HasNext(const ISO5436_2Container& handle) const
{
	assert(handle.IsMatrix() == m_IsMatrix);

	if (m_IsForward)
	{
		if (m_IsReset)
		{
			assert(m_U == 0 && m_V == 0 && m_W == 0);

			return handle.GetMaxU() > 0;
		}

		if (m_IsMatrix)
		{
			return ((m_W + 1) * (m_V + 1) * (m_U + 1) < handle.GetMaxU() * handle.GetMaxV() * handle.GetMaxW());
		}
		else
		{
			assert(m_V == 0 && m_W == 0);

			return ((m_U + 1) < handle.GetMaxU());
		}
	}

	return false;
}

bool ISO5436_2Container::PointIteratorImpl::HasPrev(const ISO5436_2Container& handle) const
{
	assert(handle.IsMatrix() == m_IsMatrix);

	if (!m_IsForward)
	{
		if (m_IsReset)
		{
			assert(m_U == 0 && m_V == 0 && m_W == 0);

			return handle.GetMaxU() > 0;
		}

		if (m_IsMatrix)
		{
			return ((m_W > 0 && handle.GetMaxW() > 0) || (m_V > 0 && handle.GetMaxV() > 0) || (m_U > 0 && handle.GetMaxU() > 0));
		}
		else
		{
			assert(m_V == 0 && m_W == 0);

			return m_U > 0 && handle.GetMaxU() > 0;
		}
	}

	return false;
}
