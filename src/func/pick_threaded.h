/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2017 - 2022 Conrad Hübler <Conrad.Huebler@gmx.net>
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

#include <QtCore/QRunnable>

class PickThread : public QRunnable
{
public:
    PickThread();
    ~PickThread();
    virtual void run() override;
    
    const PeakPick::spectrum *Data() const { return m_data; }
    void setData(const PeakPick::spectrum *spectrum) { m_data = spectrum; data = true; }
    void setRaw(const PeakPick::spectrum *spectrum) { m_raw = spectrum; raw = true;}
    void setRange(double start, double end) { m_start = start; m_end = end; }
    
    inline void setPrecision(int precision) { m_precision = precision; }
    inline void setThreshold(double threshold) { m_threshold = threshold; }
    inline std::vector<PeakPick::Peak> getPeaks() const { return m_peaks; }
    inline int Start() const { return m_start; }
    inline int End() const { return m_end; }
    
    
private:
    QString m_name;
    const PeakPick::spectrum *m_raw;
    const PeakPick::spectrum *m_data;
    int m_start, m_end, m_precision;
    std::vector<PeakPick::Peak> m_peaks;
    double m_threshold;
    bool data, raw;
};
