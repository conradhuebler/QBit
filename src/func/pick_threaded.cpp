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
    std::vector<PeakPick::Peak> peaks_up, peaks_down;
    if (data) {
        PeakPick::spectrum* sign = new PeakPick::spectrum(m_data);
        sign->InvertSgn();
        peaks_up = PeakPick::PickPeaks(m_data, m_threshold, qPow(2, m_precision - 1));
        peaks_down = PeakPick::PickPeaks(sign, m_threshold, qPow(2, m_precision - 1));
    }
    if(raw)
    {
        for (int i = 0; i < peaks_up.size(); ++i) {
            int pos = PeakPick::FindMaximum(m_raw, peaks_up[i]);
            peaks_up[i].max = pos;
            PeakPick::IntegrateNumerical(m_raw, peaks_up[i]);
        }

        for (int i = 0; i < peaks_down.size(); ++i) {
            int pos = PeakPick::FindMinimum(m_raw, peaks_down[i]);
            peaks_down[i].max = pos;
            PeakPick::IntegrateNumerical(m_raw, peaks_down[i]);
        }
    }
    m_peaks.insert(m_peaks.end(), peaks_up.begin(), peaks_up.end());
    m_peaks.insert(m_peaks.end(), peaks_down.begin(), peaks_down.end());
}
