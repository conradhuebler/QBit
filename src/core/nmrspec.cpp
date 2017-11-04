/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2017  Conrad Hübler <Conrad.Huebler@gmx.net>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore/QString>

#include <libpeakpick/spectrum.h>


#include "nmrspec.h"

NMRSpec::NMRSpec(const QString &name, const PeakPick::spectrum &data, const PeakPick::spectrum &raw) : m_name(name), m_data(new PeakPick::spectrum(data)), m_raw(new PeakPick::spectrum(raw))
{
    
    
    
    
}



NMRSpec::~NMRSpec()
{
    delete m_raw;
    delete m_data;
}
