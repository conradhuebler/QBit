/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2017 - 2019 Conrad Hübler <Conrad.Huebler@gmx.net>
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

#include <libpeakpick/spectrum.h>
#include <libpeakpick/analyse.h>
#include <libpeakpick/mathhelper.h>
#include <libpeakpick/glfit.h>

#include <QtCore/QPair>
#include <QtCore/QThreadPool>
#include <QtWidgets/QGridLayout>

#include <QtCharts/QAreaSeries>
#include <QtCharts/QtCharts>

#include "src/func/fit_threaded.h"
#include "src/func/pick_threaded.h"

#include "chartview.h"
#include "src/gui/dialogs/selectguess.h"
#include "src/gui/dialogs/fitparameter.h"
#include "peakcallout.h"

#include "multispecwidget.h"


MultiSpecWidget::MultiSpecWidget(QWidget *parent ) : QWidget(parent), m_files(0), m_scale(2), m_first_zoom(false), m_scale_jobs(0), m_threads(new QThreadPool()), m_thresh_factor(10)
{
    m_chart = new QtCharts::QChart;
    m_chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);
    
    m_chartview = new ChartView(m_chart, true);
    
    QGridLayout *layout = new QGridLayout;
    
    QHBoxLayout *toolbar = new QHBoxLayout;

    m_fit_single = new QPushButton(tr("Fit Single Peak"));
    connect(m_fit_single, SIGNAL(clicked()), this, SLOT(PrepareFit()));
    toolbar->addWidget(m_fit_single);
    
    m_conservative = new QCheckBox(tr("Conservative Fit"));
    m_conservative->setTristate(true);
    m_conservative->setCheckState(Qt::Checked);
    toolbar->addWidget(m_conservative);

    m_deconvulate = new QPushButton(tr("Deconvulate"));
    connect(m_deconvulate, SIGNAL(clicked()), this, SLOT(Deconvulate()));
    toolbar->addWidget(m_deconvulate);

    m_deconvulate_single = new QPushButton(tr("Deconvulate"));
    connect(m_deconvulate_single, SIGNAL(clicked()), this, SLOT(SingleDeconvulate()));
    toolbar->addWidget(m_deconvulate_single);

    m_ratio = new QDoubleSpinBox;
    m_ratio->setMinimum(0);
    m_ratio->setMaximum(1);
    m_ratio->setDecimals(3);
    m_ratio->setValue(0.9);
    m_ratio->setSingleStep(0.05);
    
    toolbar->addWidget(new QLabel(tr("Lorentzian")));
    toolbar->addWidget(m_ratio);
    
    layout->addLayout(toolbar, 0, 0);
    
    toolbar = new QHBoxLayout;
    toolbar->addWidget(new QLabel(tr("Parameter")));
    
    layout->addLayout(toolbar, 1, 0);
    layout->addWidget(m_chartview, 2, 0);
    
    setLayout(layout);
    
    m_select = new SelectGuess;
    
    connect(m_chartview, SIGNAL(ZoomChanged()), this, SLOT(ResetZoomLevel()));
    connect(m_chartview, SIGNAL(scaleDown()), this, SLOT(scaleDown()));
    connect(m_chartview, SIGNAL(scaleUp()), this, SLOT(scaleUp()));
    connect(m_chartview, SIGNAL(AddRect(const QPointF &, const QPointF &)), this, SLOT(AddRect(const QPointF &, const QPointF &)));
    connect(m_select, &SelectGuess::MinChanged, this, &MultiSpecWidget::MinChanged);
    connect(m_select, &SelectGuess::MaxChanged, this, static_cast<void(MultiSpecWidget::*)(double)>(&MultiSpecWidget::MaxChanged));
    connect(m_select, &SelectGuess::Fit, this, &MultiSpecWidget::FitSingle);
    connect(m_chartview, &ChartView::PointDoubleClicked, m_select, &SelectGuess::addMaxima);

    m_threads->setMaxThreadCount(4);
}

MultiSpecWidget::~MultiSpecWidget()
{
    clear();
}

int MultiSpecWidget::addSpectrum(NMRSpec *spectrum)
{
    m_spectra << spectrum;
    
    m_xmin = qMin(m_xmin, spectrum->Data()->XMin());
    m_xmax = qMax(m_xmax, spectrum->Data()->XMax());
    
    QPointer<QtCharts::QLineSeries > spec = new QtCharts::QLineSeries;
    spec->setName(spectrum->Name());
    spec->setUseOpenGL(true);
    
    m_spectrum << spec;

    m_chartview->addSeries(spec, true);
    m_chartview->setXAxis("chemical shift [ppm]");
    m_chartview->setYAxis("Intensity");
    
    UpdateThread *thread = new UpdateThread;
    thread->setSpectrum( m_spectra[m_spectra.size() - 1]->Data() , m_spectra[m_spectra.size() - 1]->Raw());
    thread->setNumber( m_spectrum.size() - 1 );
    thread->setSeries(  spec );
    m_data_threads << thread;
    
    m_chartview->addSeries(spec, true);
    m_chartview->setXAxis("chemical shift [ppm]");
    m_chartview->setYAxis("Intensity");
        
    QPointer<FitThread >fit = new FitThread(spectrum->Name(), m_spectra.size() - 1);
    fit->setData( m_spectra[m_spectra.size() - 1]->Raw() );
    m_fit_threads << fit;

    PickThread *pick = new PickThread();
    pick->setData( m_spectra[m_spectra.size() - 1]->Data() );
    pick->setRaw( m_spectra[m_spectra.size() - 1]->Raw() );
    m_pick_threads << pick;

    m_files++;
    return m_files;
}

void MultiSpecWidget::clear()
{
    qDeleteAll(m_pick_threads);
    m_pick_threads.clear();

    qDeleteAll(m_glfits);
    m_glfits.clear();

    qDeleteAll(m_data_threads);
    m_data_threads.clear();
    qDeleteAll(m_fit_threads);
    m_fit_threads.clear();
    qDeleteAll(m_peaks);
    m_peaks.clear();
    qDeleteAll(m_fit);
    m_fit.clear();
    qDeleteAll(m_spectrum);
    m_spectrum.clear();
    m_spectra.clear();
    m_files = 0;
    m_xmin = 0;
    m_xmax = 0;
}

QRectF MultiSpecWidget::getZoom() const
{
    return QRectF(QPointF(m_chartview->XMin(), m_chartview->YMax()), QPointF(m_chartview->XMax(), m_chartview->YMin()));
}

void MultiSpecWidget::setZoom(const QRectF &rect)
{
    m_chartview->setX(rect.topLeft().x(), rect.bottomRight().x());
    // m_chartview->setY(rect.bottomRight().y(), rect.topLeft().y());
    m_chartview->setYMax(rect.topLeft().y());
    Scale(1);
}

void MultiSpecWidget::ResetZoomLevel()
{
    m_chartview->formatAxis();
    m_chartview->setYMax(m_spectrum.size() + 2);
    m_chartview->setX(m_xmin, m_xmax);
    UpdateSeries(6);
}

void MultiSpecWidget::UpdateSeries(int tick)
{
    m_threshold.clear();
    for(int j = 0; j < m_spectrum.size(); ++j)
    {
        m_data_threads[j]->setScaling(m_scale);
        m_data_threads[j]->setTick( tick );
        m_data_threads[j]->setRange(m_chartview->XMin(), m_chartview->XMax());
        m_threads->start(m_data_threads[j]);
        m_threshold << m_spectra[j]->Data()->Threshold();
    }
    m_threads->waitForDone();
    if(m_first_zoom)
        m_chartview->setYMax(m_spectrum.size() + 2);
    else
        m_first_zoom = true;
    
    m_chartview->UpdateView(-1, m_spectrum.size() + 2);
}

void MultiSpecWidget::Scale(double factor)
{
    m_scale *= factor;
    UpdatePeaks(factor);
    m_scale_jobs--;
    
    if(m_scale_jobs)
        UpdateSeries(m_spectra.size()*6);
    else
        UpdateSeries(6);
    
}

void MultiSpecWidget::ShowPickedPeaks(bool show)
{
    QRectF rect = getZoom();
    for(int i = 0; i < m_peaks.size(); ++i)
    {
        m_peaks[i]->setVisible(show);
        m_peaks[i]->setVisible(show);
    }
    setZoom(rect);
}


void MultiSpecWidget::PickPeaks(int precision)
{
    QRectF rect = getZoom();
    m_peaks_list.clear();
    qDeleteAll(m_peaks);
    qDeleteAll(m_peak_anno);
    m_peak_anno.clear();
    m_peaks.clear();
    int start = 0, end = 0;
    for(int i = 0; i < m_spectra.size(); ++i)
    {
            double diff_min = 10, diff_max = 10;
            for(int j = 0; j < m_spectra[i]->Data()->size(); ++j)
            {
                double Xi = m_spectra[i]->Data()->X(j);

                double t_diff_min = qAbs(Xi-m_chartview->XMin());
                double t_diff_max = qAbs(Xi-m_chartview->XMax());

                if( t_diff_min < diff_min)
                    start = j;

                if(t_diff_max < diff_max)
                    end = j;

                diff_min = t_diff_min;
                diff_max = t_diff_max;
            }
        m_pick_threads[i]->setPrecision(precision-1);
        m_pick_threads[i]->setThreshold(m_threshold[i]/m_thresh_factor);
        m_pick_threads[i]->setRange(start, end);
        m_threads->start(m_pick_threads[i]);
    }
    quint64 start_time = QDateTime::currentMSecsSinceEpoch();

    while(m_threads->activeThreadCount())
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    qDebug() << "Took " << (QDateTime::currentMSecsSinceEpoch() - start_time) << " msecs for picking!";
    for(int i = 0; i < m_pick_threads.size(); ++i)
    {
        std::vector<PeakPick::Peak> peaks = m_pick_threads[i]->getPeaks();

        double inten = 0;

        for(const PeakPick::Peak &peak : peaks)
        {
            /*
            QtCharts::QLineSeries *series = new QtCharts::QLineSeries;
            
            series->append(m_spectra[i]->Data()->X(peak.start), 0);
            series->append(m_spectra[i]->Data()->X(peak.max), m_spectra[i]->Data()->Y(peak.max) + i);
            series->append(m_spectra[i]->Data()->X(peak.end), 0);
            {
                m_chartview->addSeries(series, false);
                m_peaks << series;
                series->setVisible(false);
            }
            */

            QPointF point(m_spectra[i]->Raw()->X((peak.max)), i+1); //(m_spectra[i]->Raw()->Y(peak.max)*m_scale*1.1));
            PeakCallOut *annotation = new PeakCallOut(m_chart);
                    annotation->setText(QString("%1").arg(m_spectra[i]->Raw()->X(peak.max)), point);
                    annotation->setAnchor(QPointF(m_spectra[i]->Raw()->X(peak.max), i+1)); //(m_spectra[i]->Raw()->Y(peak.max)*m_scale*1.1)));
                    annotation->setZValue(11);
                    annotation->updateGeometry();
                    annotation->show();
            m_peak_anno.append( annotation);

        }
        m_peaks_list << peaks;
    }
    emit PeakPicked();
    setZoom(rect);
}


void MultiSpecWidget::Deconvulate()
{
    QVector<QPointer<FitThread > > threads;
    qDeleteAll(m_fit);
    m_fit.clear();
    double min = m_chartview->XMin();
    double max = m_chartview->XMax();
    int start = 0, end = 0;
    
    QVector<int> index_peak;
    
    for(int i = 0; i < m_spectra.size(); ++i)
    {
        
        QPointer<FitThread >thread = new FitThread(m_spectra[i]->Name(), i);
        std::vector<double > guess_vector;
        
        double diff_min = 10, diff_max = 10;
        for(int j = 0; j < m_spectra[i]->Data()->size(); ++j)
        { 
            double Xi = m_spectra[i]->Data()->X(j);
            
            double t_diff_min = qAbs(Xi-min);
            double t_diff_max = qAbs(Xi-max);
            
            if( t_diff_min < diff_min)
                start = j;
            
            if(t_diff_max < diff_max)
                end = j;
            
            diff_min = t_diff_min;
            diff_max = t_diff_max;
        }
        
        std::vector<PeakPick::Peak *> peaks;
        for(int nr = 0; nr < m_peaks_list[i].size(); ++nr)
        {
            const PeakPick::Peak peak = m_peaks_list[i][nr];
            if(peak.max > start && peak.max < end)
            {
                guess_vector.push_back( m_spectra[i]->Data()->X(peak.max ));    
                index_peak << nr;
                peaks.push_back(&m_peaks_list[i][nr]);
            }
        }
        
        Vector guess = Vector::Map(&guess_vector[0], guess_vector.size());
        
        thread->setGuess( guess );
        thread->setRange(start, end);
        thread->setData( m_spectra[i]->Raw() );
        thread->setGLRatio(m_ratio->value());
        thread->setThreshold(m_threshold[i]/m_thresh_factor);
        thread->setPeaks(peaks);
        if(m_conservative->checkState() == Qt::Checked)
            thread->setFitType(3);
        else if(m_conservative->checkState() == Qt::PartiallyChecked)
            thread->setFitType(2);
        else
            thread->setFitType(1);
        m_threads->start(thread);
        threads << thread;
        
    }
    
   // while(QThreadPool::globalInstance()->th)
    while(m_threads->activeThreadCount())
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    m_threads->waitForDone();
    Vector parameter = AnalyseFitThreads(threads);
    //for(int i = 0; i < index_peak; ++i)
    //    m_peak_list[index_peak[i]] = parameter(0+i*6);
    //qDeleteAll(threads);
    emit DeconvulationFinished();
}


void MultiSpecWidget::SingleDeconvulate()
{
    QVector<QPointer< FitThread > > threads;
    qDeleteAll(m_fit);
    m_fit.clear();
    double min = m_chartview->XMin();
    double max = m_chartview->XMax();
    int start = 0, end = 0;

    QVector<int> index_peak;

    for(int i = 0; i < m_spectra.size(); ++i)
    {
        double diff_min = 10, diff_max = 10;
        for(int j = 0; j < m_spectra[i]->Data()->size(); ++j)
        {
            double Xi = m_spectra[i]->Data()->X(j);

            double t_diff_min = qAbs(Xi-min);
            double t_diff_max = qAbs(Xi-max);

            if( t_diff_min < diff_min)
                start = j;

            if(t_diff_max < diff_max)
                end = j;

            diff_min = t_diff_min;
            diff_max = t_diff_max;
        }
        for(int nr = 0; nr < m_peaks_list[i].size(); ++nr)
        {
            const PeakPick::Peak peak = m_peaks_list[i][nr];
            if(peak.max > start && peak.max < end)
            {
                QPointer<FitThread > thread = new FitThread(m_spectra[i]->Name(), i);
                std::vector<double > guess_vector;
                std::vector<PeakPick::Peak *> peaks;
                peaks.push_back(&m_peaks_list[i][nr]);
                guess_vector.push_back( m_spectra[i]->Data()->X(peak.max ));
                index_peak << nr;

                Vector guess = Vector::Map(&guess_vector[0], guess_vector.size());
                thread->setGuess( guess );

                int diff = (peak.end-peak.start)/2;
                std::cout << peak.start << " " << peak.max << " " << peak.end << " " << diff << " " << peak.start << " " << peak.end+diff << std::endl;

                thread->setRange(peak.start-diff, peak.end+diff);
                thread->setData( m_spectra[i]->Raw() );
                thread->setGLRatio(m_ratio->value());
                thread->setThreshold(m_threshold[i]/m_thresh_factor);
                thread->setPeaks(peaks);
                if(m_conservative->checkState() == Qt::Checked)
                    thread->setFitType(3);
                else if(m_conservative->checkState() == Qt::PartiallyChecked)
                    thread->setFitType(2);
                else
                    thread->setFitType(1);
                m_threads->start(thread);
                threads << thread;
            }
        }
    }
    while(m_threads->activeThreadCount())
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    //m_threads->waitForDone();
    Vector parameter = AnalyseFitThreads(threads);
    //for(int i = 0; i < index_peak; ++i)
    //    m_peak_list[index_peak[i]] = parameter(0+i*6);
    //qDeleteAll(threads);
    emit DeconvulationFinished();
}


Vector MultiSpecWidget::AnalyseFitThreads(const QVector<QPointer< FitThread > > &threads)
{
    QRectF rect = getZoom();
    QString result, last_row;
    Vector parameter;
    for(int work = 0; work < threads.size(); ++work)
    {
        QString peaklist;
        parameter = threads[work]->Parameter();
        int guess = threads[work]->GLFit()->Functions();
        int start = threads[work]->Start();
        int end = threads[work]->End();
        for(int i = 0; i <  guess; ++i)
            peaklist += QString::number(parameter(0+i*6)) + " ";
        last_row += peaklist + "\n";
        result += threads[work]->toHtml() + "<br \>";
        
        QtCharts::QLineSeries *series = new QtCharts::QLineSeries;
        
        for(int i = start; i <= end; ++i)
        {
            double x = threads[work]->Data()->X(i);
            double y = PeakPick::Signal(x, parameter)*m_scale;
            if(qAbs(y - threads[work]->Data()->StdDev()/4.0) > 1E-2)
                series->append(x, y + threads[work]->Position() );
        }
        series->setName(QString::number(parameter(0)));
        m_chartview->addSeries(series);
        
        m_fit << series;

        for(int i = 0; i < parameter.size()/6; ++i)
        {
            series = new QtCharts::QLineSeries;
            for(int j = start; j <= end; ++j)
            {
                double x = threads[work]->Data()->X(j);
                double y = PeakPick::SignalSingle(x, parameter, i)*m_scale;
                if(qAbs(y - threads[work]->Data()->StdDev()/4.0) > 1E-2)
                    series->append(x, y + threads[work]->Position() );
            }
            series->setName(QString::number(parameter(0)));
            m_chartview->addSeries(series);

            QPen pen = series->pen();
            pen.setStyle(Qt::DotLine);
            pen.setWidth(1);
            series->setPen(pen);

            m_fit << series;
        }
        m_glfits.append( threads[work] );
        connect(threads[work], &FitThread::FitFinished, this, &MultiSpecWidget::PlotFit);
        emit Message(threads[work]->toHtml(), threads[work]->Name(), 1);
        emit Message(peaklist, threads[work]->Name(), 1);
    }

    result += "<p>Gaussian function defined as: 1/(a*sqrt(2*pi))*exp(-pow((x-x_0),2)/(2*pow(c,2)))</p>";
    result += "<p>Lorentzian function defined as: 1/pi*(0.5*gamma)/(pow(x-x_0,2)+pow(0.5*gamma,2))</p>";
    result += last_row;

    emit Message(result, "overview", 1);

    // FitParameter *fit = new FitParameter(result, this);
    // fit->show();
    std::cout << result.toStdString() << std::endl;
    setZoom(rect);
    return parameter;
}


void MultiSpecWidget::PlotFit()
{
    qDeleteAll(m_fit);
    m_fit.clear();

    FitThread *thread = qobject_cast<FitThread *>(QObject::sender());
    Vector parameter = thread->Parameter();
    int guess = thread->GLFit()->Functions();
    int start = thread->Start();
    int end = thread->End();
    QString result, peaklist;

    for(int i = 0; i <  guess; ++i)
        peaklist += QString::number(parameter(0+i*6)) + " ";

    QtCharts::QLineSeries *series = new QtCharts::QLineSeries;

    for(int i = start; i <= end; ++i)
    {
        double x = thread->Data()->X(i);
        double y = PeakPick::Signal(x, parameter)*m_scale;
        if(qAbs(y - thread->Data()->StdDev()/4.0) > 1E-2)
            series->append(x, y + thread->Position() );
    }
    series->setName(QString::number(parameter(0)));
    m_chartview->addSeries(series);

    m_fit << series;

    for(int i = 0; i < parameter.size()/6; ++i)
    {
        series = new QtCharts::QLineSeries;
        for(int j = start; j <= end; ++j)
        {
            double x = thread->Data()->X(j);
            double y = PeakPick::SignalSingle(x, parameter, i)*m_scale;
            if(qAbs(y - thread->Data()->StdDev()/4.0) > 1E-2)
                series->append(x, y + thread->Position() );
        }
        series->setName(QString::number(parameter(0)));
        m_chartview->addSeries(series);

        QPen pen = series->pen();
        pen.setStyle(Qt::DotLine);
        pen.setWidth(1);
        series->setPen(pen);

        m_fit << series;
    }
    emit Message(thread->toHtml(), thread->Name(), 1);
    emit Message(peaklist, thread->Name(), 1);
}


void MultiSpecWidget::PrepareFit()
{
    m_select->clear();
    m_select->show();
}


void MultiSpecWidget::FitSingle()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    m_select->hide();
    qDeleteAll(m_fit);
    m_fit.clear();
    m_manual_peaks.clear();
    QString value_input = m_select->PeakList();
    std::vector<double> entries;
    std::vector<PeakPick::Peak *> peaks;
    if (!value_input.isEmpty())
    {
        QStringList input = value_input.split(" ");
        for(const QString &str : qAsConst(input))
        {
            if(str.isNull() || str.isEmpty()) continue;
                entries.push_back(str.toDouble());
        }
        
    }
    
    Vector guess = Vector::Map(&entries[0], entries.size());
    int start = 0, end = 0;
    for(int work = 0; work < m_spectra.size(); ++work)
    {
        double diff_min = 10, diff_max = 10;
        for(int i = 0; i < m_spectra[work]->Data()->size(); ++i)
        { 
            double Xi = m_spectra[work]->Data()->X(i);
            
            double t_diff_min = qAbs(Xi-m_chartview->XMin());
            double t_diff_max = qAbs(Xi-m_chartview->XMax());
            
            if( t_diff_min < diff_min)
                start = i;
            
            if(t_diff_max < diff_max)
                end = i;
            
            diff_min = t_diff_min;
            diff_max = t_diff_max;
        }
        std::vector<PeakPick::Peak> list;
        for(int nr = 0; nr < guess.size(); ++nr)
        {
            PeakPick::Peak peak;
            peak.max = guess(nr);
            peak.start = start;
            peak.end = end;
            list.push_back(peak);
        }
        m_manual_peaks << list;
        for(int nr = 0; nr < guess.size(); ++nr)
            peaks.push_back(&m_manual_peaks[work][nr]);

        m_fit_threads[work]->setGuess( guess );
        m_fit_threads[work]->setRange( start, end );
        m_fit_threads[work]->setGLRatio(m_ratio->value());
        m_fit_threads[work]->setThreshold(m_threshold[work]/m_thresh_factor);
        m_fit_threads[work]->setPeaks(peaks);
        if(m_conservative->checkState() == Qt::Checked)
            m_fit_threads[work]->setFitType(3);
        else if(m_conservative->checkState() == Qt::PartiallyChecked)
            m_fit_threads[work]->setFitType(2);
        else
            m_fit_threads[work]->setFitType(1);
        m_threads->start(m_fit_threads[work]);
    }
    m_threads->waitForDone();
    AnalyseFitThreads(m_fit_threads);
    emit DeconvulationFinished();
    QApplication::restoreOverrideCursor();
}


void MultiSpecWidget::scaleUp()
{
    m_scale_jobs++;
    Scale(1.1);
    
}

void MultiSpecWidget::scaleDown()
{
    m_scale_jobs++;
    Scale(0.9);
}

void MultiSpecWidget::AddRect(const QPointF &point1, const QPointF &point2)
{    
    
    double x_min = point1.x();
    double y_min = qMin(point1.y(), point2.y());
    double x_max = point2.x();
    double y_max = qMax(point1.y(), point2.y());

    std::vector<PeakPick::Peak *> peaks;
    int start = 0, end = 0, work;
    double diff_min = 10, diff_max = 10;
    bool inrange = false;
    qDebug() << m_spectra.size();
    for(work = 0; work < m_spectra.size() || !inrange; ++work)
    {

        for(int i = 0; i < m_spectra[work]->Data()->size(); ++i)
        { 
            double Xi = m_spectra[work]->Data()->X(i);
            if(Xi < x_min || Xi > x_max)
                continue;
            double Yi = m_spectra[work]->Raw()->Y(i);
            
//             std::cout << Xi << ": " << y_min << " " << Yi*m_scale+work << " " <<  y_max << std::endl;
            if(Yi*m_scale+work > y_min && Yi*m_scale+work < y_max)
            {
                inrange = true;
//                 std::cout << "got it ....." << std::endl;
            }
            double t_diff_min = qAbs(Xi-m_chartview->XMin());
            double t_diff_max = qAbs(Xi-m_chartview->XMax());
            
            if( t_diff_min < diff_min)
                start = i;
            
            if(t_diff_max < diff_max)
                end = i;
            
            diff_min = t_diff_min;
            diff_max = t_diff_max;
        }
        
        if(inrange)
            break;
    }
    
    std::vector<double > peak_guess;

        for(int j = 0; j < m_peaks_list[work].size(); ++j)
        {
            if(m_peaks_list[work][j].max < x_max && m_peaks_list[work][j].max > x_min)
            {
                peak_guess.push_back(m_peaks_list[work][j].max);
                peaks.push_back(&m_peaks_list[work][j]);
            }
        }


    qDebug() << work << m_spectra.size();
    Vector guess;
    if(peaks.size() == 0)
    {
        guess = Vector(1);
        guess(0) = (x_max+x_min)/2.0;
        PeakPick::Peak peak;
        peak.max = (x_max+x_min)/2.0;
        peak.start = start;
        peak.end = end;
        std::vector<PeakPick::Peak > list;
        list.push_back(peak);
        m_manual_peaks << list;
        peaks.push_back(&m_manual_peaks[work][m_manual_peaks.size() - 1]);
    }else
    {
        guess = Vector::Map(&peak_guess[0], peak_guess.size());
    }
    QPointer<FitThread > thread = new FitThread("name", work);
    
    thread->setData(m_spectra[work]->Raw());
    thread->setGuess( guess );
    thread->setRange( start, end );
    thread->setGLRatio(m_ratio->value());
    thread->setPeaks(peaks);
    thread->run();

    AnalyseFitThreads(QVector<QPointer<FitThread>>() << thread);

    delete thread;
    /* 
              qDebug() << m_spectra[0]->PosOfPoint(min) << m_spectra[0]->PosOfPoint(max); 
              qDebug() << min << m_spectra[0]->X(m_spectra[0]->PosOfPoint(min)) << max << m_spectra[0]->X(m_spectra[0]->PosOfPoint(max)); 
              
              QtCharts::QLineSeries *m_upper = new QtCharts::QLineSeries;
              m_upper->append(point1);
              m_upper->append(point2.x(),point1.y());
              QtCharts::QLineSeries *m_lower = new QtCharts::QLineSeries;
              m_lower->append(point1.x(),point2.y());
              m_lower->append(point2);
              QtCharts::QAreaSeries *m_area = new QtCharts::QAreaSeries(m_upper, m_lower);
              
              QPen pen(Qt::darkGray);
              pen.setWidth(1);
              m_area->setPen(pen);
      
              QLinearGradient gradient(QPointF(0, 0), QPointF(0, 1));
              gradient.setColorAt(0.0, Qt::lightGray);
              gradient.setColorAt(1.0, Qt::gray);
              gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
              m_area->setBrush(gradient);
          
              m_chartview->addSeries(m_area);
     */
}

void MultiSpecWidget::MinChanged(double val)
{
    
    
}

void MultiSpecWidget::MaxChanged(double val)
{
    
    
}

void MultiSpecWidget::UpdatePeaks(double factor)
{
    for(QPointer<QtCharts::QLineSeries > series : m_peaks)
    {
        QVector<QPointF> points = series->pointsVector();
        for(int i = 0; i < series->count(); ++i)
        {
            series->replace(i, points[i].x(),points[i].y()*factor );
        }
    }
    
    for(QPointer<QtCharts::QLineSeries > series : m_fit)
    {
        QVector<QPointF> points = series->pointsVector();
        for(int i = 0; i < series->count(); ++i)
        {
            series->replace(i, points[i].x(),points[i].y()*factor );
        }
    }
    
}

#include "multispecwidget.moc"
