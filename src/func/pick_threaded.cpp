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

#include <QtCore/QThread>
#include <QtCore/qmath.h>

#include <libpeakpick/analyse.h>
#include <libpeakpick/spectrum.h>

#include "pick_threaded.h"

PickThread::PickThread() : m_precision(3)
{
    data = false;
    raw = false;
    setAutoDelete(false);
}


PickThread::~PickThread()
{

}

void PickThread::run()
{
    if(data)
        m_peaks = PeakPick::PickPeaks(m_data, m_threshold, qPow(2, m_precision-1));
    if(raw)
    {
        for(int i = 0; i < m_peaks.size(); ++i)
        {
            int pos = PeakPick::FindMaximum(m_raw, m_peaks[i]);
            m_peaks[i].max = pos;
            PeakPick::IntegrateNumerical(m_raw, m_peaks[i]);
        }
    }
}
