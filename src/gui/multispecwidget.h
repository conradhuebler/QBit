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
        series->clear(); 
        for(int i = 0; i < spectrum->size(); i += tick)  
            series->append( spectrum->X(i), spectrum->Y(i) + number);
        
    }
    
    QPointer<QtCharts::QLineSeries> series;
    const PeakPick::spectrum *spectrum;
    int tick = 5;
    int number;
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
//     void AddSpectrum(const PeakPick::spectrum &spectrum, const PeakPick::spectrum &original, const QString &name = "raw data");
    void addSpectrum(NMRSpec *spectrum);
    void UpdateSeries(int tick);
    void ResetZoomLevel(); 
    
private:
    QPushButton *m_normalise, *m_denoise, *m_reload, *m_pickpeaks, *m_fit_single, *m_deconvulate, *m_take;
    QDoubleSpinBox *m_start, *m_max, *m_end;
    QSpinBox *m_functions;
    ChartView *m_chartview;
    QtCharts::QChart *m_chart;
    QVector<QPointer<QtCharts::QLineSeries > >m_spectrum, m_peaks, m_fit; //, *m_std, *m_sthdthresh;
    QStringList m_filenames;
    QVector<PeakPick::Peak > m_peak_list;
    QVector<NMRSpec *> m_spectra;
//     QVector<PeakPick::spectrum> m_original;
//     QVector<PeakPick::spectrum *> m_work;
    QVector<PeakPick::Peak> m_maxpeak;
    QVector<double > m_threshold;
    QVector< UpdateThread * > m_threads;
    QVector< FitThread *> m_fit_threads;
    std::vector<PeakPick::Peak> peaks;
    QPointer<QtCharts::QLineSeries > m_chloroform;
    int m_files, m_scale_jobs;
    double m_scale;
    bool m_first_zoom;
    SelectGuess *m_select;
    
private slots:
    void Scale(double factor);
    void PickPeaks();
    void DeNoise();
    void Reload();
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



