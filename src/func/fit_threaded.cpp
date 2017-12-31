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
#include <libpeakpick/glfit.h>

#include "fit_threaded.h"

FitThread::FitThread(const QString &name, int position) : m_name(name), m_position(position), m_fittype(PeakPick::Liberal), result(false), m_threshold(0), runable(false), glfit(false)
{
    setAutoDelete(false);
}

FitThread::FitThread( PeakPick::GLFit *fit, const QString &name, int position) : m_fit(fit), m_name(name), m_position(position), m_fittype(PeakPick::Liberal), result(false), m_threshold(0), runable(false)
{
    setAutoDelete(false);
}

void FitThread::run()
{ 
    if(!runable)
        return;
    result = true;
    if(!glfit)
    {
        m_fit = new PeakPick::GLFit(Data(), m_start, m_end, m_ratio, m_fittype);
        glfit = true;
    }
    m_fit->setGuess(m_guess);
    m_result = Deconvulate(m_fit);
    for(int i = 0; i < m_result->parameter.size()/6; ++i)
    {
        m_peaks[i]->deconv_x = Parameter()(0+i*6);
        m_peaks[i]->deconv_y = PeakPick::Signal(Parameter()(0+i*6), Parameter());
        Vector vector(6);
        vector(0) = Parameter()(0+i*6);
        vector(1) = Parameter()(1+i*6);
        vector(2) = Parameter()(2+i*6);
        vector(3) = Parameter()(3+i*6);
        vector(4) = Parameter()(4+i*6);
        vector(5) = Parameter()(5+i*6);
        m_peaks[i]->integ_analyt = PeakPick::IntegrateGLFunction(vector);
    }
    /*
    if((Data()->Y(m_start) > m_threshold) || (Data()->Y(m_end) > m_threshold))
    {
        m_result->integral = PeakPick::IntegrateGLSignal(m_result->parameter, m_start, m_end);
    }*/
}

FitThread::~FitThread()
{
    if(result)
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
    
void FitThread::reFit()
{
    m_result = Deconvulate(m_fit);
    //emit FitFinished();
}

QString FitThread::toHtml() const
{
    QString result;
    result += "<table>";
    result += "<tr> <th>Name</th> <th>Peak Position</th> <th>a</th> <th>c</th> <th>&gamma;</th> <th>% Gauss: %Lorentzian</th> <th>scale</th> </tr>";
    Vector parameter;

    parameter = Parameter();
    for(int i = 0; i < m_fit->Functions(); ++i)
        result += "<tr><td>" + Name() + "</td><td>" + QString::number(parameter(0+i*6)) + "</td><td>" + QString::number(parameter(1+i*6)) + "</td><td>" + QString::number(parameter(2+i*6)) + "</td><td>" + QString::number(parameter(3+i*6)) + "</td><td>" + QString::number(parameter(4+i*6)) + "</td><td>" + QString::number(parameter(5+i*6)) + "</td></tr>";
    result += "</table>";
    result += "Sum of Errors = " + QString::number(SumError()) + "... Sum of Squares = " + QString::number(SumSquared()) + "\n";
    return result;
}
