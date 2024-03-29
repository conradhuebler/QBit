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

#include <QtWidgets/QWidget>

#include <QtCharts/QtCharts>

#include <QtCore/QPair>
#include <QtCore/QRunnable>
#include <QtCore/QPointer>

#include <libpeakpick/spectrum.h>
#include <libpeakpick/analyse.h>
#include <libpeakpick/glfit.h>

#include "src/core/nmrspec.h"

class QPushButton;
class QDoubleSpinBox;
class QCheckBox;
class QThreadPool;
class ChartView;
class SelectGuess;

class FitThread;
class PickThread;
class GLFit;
class PeakPosCallOut;
class UpdateThread : public QRunnable
{
public:
    inline UpdateThread() { setAutoDelete(false); }
    
    inline void run() override
    {
        m_series->clear();

        m_tick = 50;
        if (qAbs(m_xmax - m_xmin) < 8 || m_spectrum->size() < 15000)
            TightAdd();
        else
            LooseAdd();

        //        TightAdd();
    }
    
    
    
    inline void setNumber(int number) { m_number = number; }
    inline void setSpectrum(const PeakPick::spectrum *spectrum, const PeakPick::spectrum *raw) { m_spectrum = spectrum; m_raw = raw; }
    inline void setSeries(QPointer<QLineSeries> series) { m_series = series; }
    inline void setScaling(double scaling) { m_scaling = scaling; }
    inline void setTick(int tick) { m_tick = tick;}
    inline void setRange(double xmin, double xmax) { m_xmin = xmin; m_xmax = xmax; }
    
private:
    inline void LooseAdd()
    {
        int crude = 0;
        int tight = 0;
        int count = 0;
        
         for(int i = 0; i < m_spectrum->size(); i += m_tick)  
        {
            if(m_spectrum->X(i) < m_xmin || m_spectrum->X(i) > m_xmax)
                continue;

            if (m_spectrum->Y(i) * m_scaling * 50 > m_spectrum->StdDev() || crude == 36 || tight == 18 || i == 0) {
                if(qAbs(m_xmax -m_xmin) < 2)
                    m_series->append(QPointF(m_spectrum->X(i), (m_raw->Y(i)*m_scaling) + m_number));
                else
                    m_series->append(QPointF(m_spectrum->X(i), (m_spectrum->Y(i)*m_scaling) + m_number));
                if (crude == 36)
                    crude = 0;
                if (tight == 18)
                    tight = 0;
            }
            count++;
            tight++;
            crude++;
        }
    }
    inline void TightAdd()
    {
        int stepsize = 5;
        for (int i = 0; i < m_spectrum->size(); i += stepsize) {
            if(m_spectrum->X(i) < m_xmin || m_spectrum->X(i) > m_xmax)
                continue;

            m_series->append(QPointF(m_spectrum->X(i), (m_raw->Y(i)*m_scaling) + m_number));
            // qDebug() << QPointF(m_spectrum->X(i), (m_raw->Y(i)*m_scaling) + m_number);
        }
    }
    QPointer<QLineSeries> m_series;
    const PeakPick::spectrum *m_spectrum, *m_raw;
    int m_tick = 12;
    double m_scaling = 1, m_xmin, m_xmax;
    int m_number;
};


class MultiSpecWidget : public QWidget
{
    Q_OBJECT
    
public:
    MultiSpecWidget(QWidget *parent);
    ~MultiSpecWidget();
    void clear();
    
    QVector<std::vector<PeakPick::Peak> > * PeakList() { return &m_peaks_list; }
    QVector<std::vector<PeakPick::Peak> > * ManualPeakList() { return &m_manual_peaks; }
    QVector<NMRSpec *>  * SpectraList()  { return &m_spectra; }
    QRectF getZoom() const;
    void setZoom(const QRectF &rect);
    QVector< QPointer<FitThread >  > *GLFitList() { return &m_glfits; }

public slots:
    int addSpectrum(NMRSpec *spectrum);
    void UpdateSeries(int tick);
    void ResetZoomLevel(); 
    void ShowPickedPeaks(bool show = true);
    void PickPeaks(int precision);

private:
    QPushButton  *m_pickpeaks, *m_fit_single, *m_deconvulate, *m_deconvulate_single;
    QDoubleSpinBox *m_ratio; 
    QCheckBox *m_conservative;
    ChartView *m_chartview;
    QChart* m_chart;
    QPointer<QThreadPool> m_threads;
    QStringList m_filenames;

    QVector<double > m_threshold;


    QVector<NMRSpec *> m_spectra;
    QVector<std::vector<PeakPick::Peak> > m_peaks_list, m_manual_peaks;
    QVector<PickThread * > m_pick_threads;
    QVector< QPointer<FitThread > > m_fit_threads;
    QVector< QPointer<FitThread >  > m_glfits;
    QVector<QPointer<QLineSeries>> m_spectrum, m_peaks, m_fit;
    QVector< UpdateThread * > m_data_threads;

    QVector<PeakPosCallOut*> m_peak_anno;

    int m_files, m_scale_jobs;
    double m_scale, m_xmin, m_xmax, m_thresh_factor;
    bool m_first_zoom;
    SelectGuess *m_select;
    Vector AnalyseFitThreads(const QVector<QPointer< FitThread > > &threads);

private slots:
    void Scale(double factor);
    void Deconvulate();
    void SingleDeconvulate();
    void PrepareFit();
    void FitSingle();
    void scaleUp();
    void scaleDown();
    void AddRect(const QPointF &point1, const QPointF &point2);
    void MinChanged(double val);
    void MaxChanged(double val);
    void UpdatePeaks(double factor);
    void PlotFit();

signals:
    void PeakSelected(const QPointF &point);
    void PeakPicked();
    void DeconvulationFinished();
    void Message(const QString &message, const QString &indentifier, int prior);
};



