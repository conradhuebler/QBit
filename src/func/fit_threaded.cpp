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

#include <libpeakpick/deconvulate.h>
#include <libpeakpick/spectrum.h>

#include "fit_threaded.h"

FitThread::FitThread(const QString &name, int position) : m_name(name), m_position(position), m_fittype(PeakPick::Liberal)
{
    setAutoDelete(false);
}

void FitThread::run()
{ 
    m_result = PeakPick::LiberalDeconvulate(Data(), m_start, m_end, m_ratio, m_guess, m_fittype);
}

FitThread::~FitThread()
{
    delete m_result;
}

Vector FitThread::Parameter() const
{
    return m_result->parameter;
}

double FitThread::SumError() const
{
    return m_result->sum_error;
}

double FitThread::SumSquared() const
{
    return m_result->sum_squared;
}
    
