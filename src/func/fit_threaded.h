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

#include <QtCore/QRunnable>

struct FitResult;

class FitThread : public QRunnable
{
public:
    FitThread(const QString &name, int position); // : m_name(name), m_position(position), m_fittype(PeakPick::Liberal) { setAutoDelete(false); }
    ~FitThread();
    virtual void run() override;
    
    const PeakPick::spectrum *Data() const { return m_spectrum; }
    void setData(const PeakPick::spectrum *spectrum) { m_spectrum = spectrum; }
    QString Name() const { return m_name; }
    int Position() const { return m_position; }
    
    void setGLRatio( double ratio) { m_ratio = ratio; }
    void setGuess( const Vector &guess) { m_guess = guess; }
    Vector Guess() const { return m_guess; }
    Vector Parameter() const; // { return m_result->parameter; }
    double SumError() const; // { return m_result->sum_error; }
    double SumSquared() const; // { return m_result->sum_squared; }
    void setRange(double start, double end) { m_start = start; m_end = end; }
    
    int Start() const { return m_start; }
    int End() const { return m_end; }
    
    void setFitType(int fittype) { m_fittype = fittype; }
    
private:
    QString m_name;
    double m_ratio;
    const PeakPick::spectrum *m_spectrum;
    Vector m_parameter, m_guess; 
    int m_start, m_end, m_position, m_fittype;
    FitResult *m_result;
    bool result;
};
