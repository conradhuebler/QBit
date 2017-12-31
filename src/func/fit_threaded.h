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

#include <QtCore/QObject>
#include <QtCore/QRunnable>

#include <libpeakpick/deconvulate.h>
#include <libpeakpick/glfit.h>


class FitThread : public QObject , public QRunnable
{
    Q_OBJECT
public:
    FitThread(const QString &name, int position);
    FitThread( PeakPick::GLFit *fit, const QString &name, int position);

    ~FitThread();
    virtual void run() override;
    
    void reFit();

    void setGLFit(PeakPick::GLFit *fit) { m_fit = fit; }
    inline const PeakPick::spectrum *Data() const { return m_spectrum; }
    inline void setPeaks(std::vector<PeakPick::Peak * >peaks) { m_peaks = peaks; runable = true; }
    void setData(const PeakPick::spectrum *spectrum) { m_spectrum = spectrum; }
    QString Name() const { return m_name; }
    int Position() const { return m_position; }
    
    void setGLRatio( double ratio) { m_ratio = ratio; }
    void setGuess( const Vector &guess) { m_guess = guess; }
    void setThreshold(double threshold) { m_threshold = threshold; }
    Vector Guess() const { return m_guess; }
    Vector Parameter() const;
    double SumError() const;
    double SumSquared() const;
    void setRange(double start, double end) { m_start = start; m_end = end; }
    
    int Start() const { return m_start; }
    int End() const { return m_end; }
    
    void setFitType(int fittype) { m_fittype = fittype; }
    PeakPick::GLFit * GLFit() const { return m_fit; }

private:
    QString m_name;
    double m_ratio, m_threshold;
    const PeakPick::spectrum *m_spectrum;
    PeakPick::GLFit *m_fit;
    std::vector<PeakPick::Peak *> m_peaks;
    Vector m_parameter, m_guess; 
    int m_start, m_end, m_position, m_fittype;
    PeakPick::FitResult *m_result;
    bool result, runable, glfit;

signals:
    void FitFinished();
};
