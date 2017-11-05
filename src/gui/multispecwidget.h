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

#include <QtWidgets/QWidget>

#include <QtCharts/QtCharts>
#include <QtCore/QRunnable>
#include <QtCore/QPointer>

#include <libpeakpick/spectrum.h>
#include <libpeakpick/analyse.h>

#include "src/core/nmrspec.h"

class QPushButton;

class ChartView;
class SelectGuess;

class UpdateThread : public QRunnable
{
public:
    inline UpdateThread() { setAutoDelete(false); }
    
    inline void run() override
    {
        int crude = 0;
        int tight = 0;
            
        m_series->clear();

        int count = 0;
        for(int i = 0; i < m_spectrum->size(); i += m_tick)  
        {
            if(m_spectrum->X(i) < m_xmin || m_spectrum->X(i) > m_xmax)
                continue;

            if(m_spectrum->Y(i)*m_scaling > m_spectrum->StdDev() || crude == 48 || tight == 24 || i == 0)
            {
                if(qAbs(m_xmax -m_xmin) < 2)
                    m_series->append(QPointF(m_spectrum->X(i), (m_raw->Y(i)*m_scaling) + m_number));
                else
                    m_series->append(QPointF(m_spectrum->X(i), (m_spectrum->Y(i)*m_scaling) + m_number));
                
                if(crude == 48)
                    crude = 0;
                if(tight == 24)
                    tight = 0;
            }
            count++;
            tight++;
            crude++;
        }
    }
    inline void setNumber(int number) { m_number = number; }
    inline void setSpectrum(const PeakPick::spectrum *spectrum, const PeakPick::spectrum *raw) { m_spectrum = spectrum; m_raw = raw; }
    inline void setSeries(QPointer<QtCharts::QLineSeries> series) { m_series = series; }
    inline void setScaling(double scaling) { m_scaling = scaling; }
    inline void setTick(int tick) { m_tick = tick;}
    inline void setRange(double xmin, double xmax) { m_xmin = xmin; m_xmax = xmax; }
    
private:
    QPointer<QtCharts::QLineSeries> m_series;
    const PeakPick::spectrum *m_spectrum, *m_raw;
    int m_tick = 12;
    double m_scaling = 1, m_xmin, m_xmax;
    int m_number;
};


class FitThread : public QRunnable
{
public:
    inline FitThread() { setAutoDelete(false); }
    
    virtual void run() override;
    PeakPick::spectrum *spectrum;
    PeakPick::Peak peak;
    Vector parameter, guess; 
    int functions;

};

class MultiSpecWidget : public QWidget
{
    Q_OBJECT
    
public:
    MultiSpecWidget(int files, QWidget *parent);
    ~MultiSpecWidget();
    
    
public slots:
    void addSpectrum(NMRSpec *spectrum);
    void UpdateSeries(int tick);
    void ResetZoomLevel(); 
    
private:
    QPushButton *m_normalise, *m_denoise, *m_reload, *m_pickpeaks, *m_fit_single, *m_deconvulate, *m_take;
    QDoubleSpinBox *m_start, *m_max, *m_end;
    QSpinBox *m_functions;
    ChartView *m_chartview;
    QtCharts::QChart *m_chart;
    QVector<QPointer<QtCharts::QLineSeries > >m_raw_spec, m_spectrum, m_peaks, m_fit; 
    QStringList m_filenames;
    QVector<PeakPick::Peak > m_peak_list;
    QVector<NMRSpec *> m_spectra;
    QVector<PeakPick::Peak> m_maxpeak;
    QVector<double > m_threshold;
    QVector< UpdateThread * > m_data_threads; //, m_raw_threads;
    QVector< FitThread *> m_fit_threads;
    std::vector<PeakPick::Peak> peaks;
    QPointer<QtCharts::QLineSeries > m_chloroform;
    int m_files, m_scale_jobs;
    double m_scale, m_xmin, m_xmax;
    bool m_first_zoom;
    SelectGuess *m_select;
    
private slots:
    void Scale(double factor);
    void PickPeaks();
    void Deconvulate();
    void PrepareFit();
    void FitSingle();
    void MaxChanged();
    void UpdateRange();
    void scaleUp();
    void scaleDown();
    void AddRect(const QPointF &point1, const QPointF &point2);
    void MinChanged(double val);
    void MaxChanged(double val);
    
signals:
    void PeakSelected(const QPointF &point);
};



