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

#pragma once

#include <QtCore/QString>

#include <libpeakpick/spectrum.h>

class NMRSpec 
{
    
public:
    NMRSpec(const QString &name, const PeakPick::spectrum &raw, const PeakPick::spectrum &data, bool nmr);
    ~NMRSpec();
    
    PeakPick::spectrum * Raw() { return m_raw; }
    PeakPick::spectrum * Data() { return m_data; }
    PeakPick::spectrum * Fid() {return m_fid; }
    
    QString Name() const { return m_name; }
    QString Path() const { return m_path; }
    void setPath(const QString &path) { m_path = path; }
    bool isNMR() const { return m_nmr; }
private:
    QString m_name, m_path;
    PeakPick::spectrum *m_raw, *m_data, *m_fid;
    bool m_nmr = true;
};
