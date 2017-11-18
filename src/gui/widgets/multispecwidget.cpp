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

#include <libpeakpick/spectrum.h>
#include <libpeakpick/analyse.h>
#include <libpeakpick/mathhelper.h>


#include <QtCore/QThreadPool>
#include <QtWidgets/QGridLayout>

#include <QtCharts/QAreaSeries>
#include <QtCharts/QtCharts>

#include "src/func/fit_threaded.h"

#include "chartview.h"
#include "src/gui/dialogs/selectguess.h"
#include "src/gui/dialogs/fitparameter.h"
#include "multispecwidget.h"


MultiSpecWidget::MultiSpecWidget(QWidget *parent ) : QWidget(parent), m_files(0), m_scale(2), m_first_zoom(false), m_scale_jobs(0)
{
    m_chart = new QtCharts::QChart;
    m_chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);
    
    m_chartview = new ChartView(m_chart, true);
    
    QGridLayout *layout = new QGridLayout;
    
    QHBoxLayout *toolbar = new QHBoxLayout;

    m_pickpeaks = new QPushButton(tr("Pick Peaks"));
    connect(m_pickpeaks, SIGNAL(clicked()), this, SLOT(PickPeaks()));
    toolbar->addWidget(m_pickpeaks);
    
    m_precision = new QSpinBox;
    m_precision->setMinimum(1);
    m_precision->setMaximum(6);
    toolbar->addWidget(new QLabel(tr("Precision")));
    toolbar->addWidget(m_precision);
    
    m_fit_single = new QPushButton(tr("Fit Single Peak"));
    connect(m_fit_single, SIGNAL(clicked()), this, SLOT(PrepareFit()));
    toolbar->addWidget(m_fit_single);
    
    m_deconvulate = new QPushButton(tr("Deconvulate"));
    connect(m_deconvulate, SIGNAL(clicked()), this, SLOT(Deconvulate()));
    toolbar->addWidget(m_deconvulate);

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
}

MultiSpecWidget::~MultiSpecWidget()
{
    clear();
}

void MultiSpecWidget::addSpectrum(NMRSpec *spectrum)
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
        
    FitThread *fit = new FitThread(spectrum->Name(), m_spectra.size() - 1);
    fit->setData( m_spectra[m_spectra.size() - 1]->Raw() );
    m_fit_threads << fit;
    m_files++;
}

void MultiSpecWidget::clear()
{
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
        QThreadPool::globalInstance()->start(m_data_threads[j]);
        m_threshold << m_spectra[j]->Data()->Threshold();
    }
    QThreadPool::globalInstance()->waitForDone();
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

void MultiSpecWidget::PickPeaks()
{
    m_maxpeak.clear();
    for(int i = 0; i < m_spectra.size(); ++i)
    {
        peaks = PeakPick::PickPeaks(m_spectra[i]->Data(), m_threshold[i], qPow(10, m_precision->value()-1));
        double inten = 0;
        for(QPointer<QtCharts::QLineSeries> serie : m_peaks)
        {
            if(serie)
                m_chart->removeSeries(serie);
        }
        for(const PeakPick::Peak &peak : peaks)
        {
            QtCharts::QLineSeries *series = new QtCharts::QLineSeries;
            
            series->append(m_spectra[i]->Data()->X(peak.start), 0);
            series->append(m_spectra[i]->Data()->X(peak.max), m_spectra[i]->Data()->Y(peak.max));
            series->append(m_spectra[i]->Data()->X(peak.end), 0);
            series->setName(QString::number(m_spectra[i]->Data()->X(peak.max)));
            if(inten < qAbs(m_spectra[i]->Data()->X(peak.end) - m_spectra[i]->Data()->X(peak.start) )*m_spectra[i]->Data()->Y(peak.max))
            {
                inten = qAbs(m_spectra[i]->Data()->X(peak.end) - m_spectra[i]->Data()->X(peak.start) )*m_spectra[i]->Data()->Y(peak.max);
                m_maxpeak << peak;
                m_chloroform = series;
            }
            if(qAbs(m_spectra[i]->Data()->X(peak.end) - m_spectra[i]->Data()->X(peak.start) )*m_spectra[i]->Data()->Y(peak.max) > 10e-5)
            {
                m_chartview->addSeries(series, m_spectra[i]->Data()->Y(peak.max) > 0.5*m_spectra[i]->Data()->StdDev());
                m_peaks << series;
                m_peak_list.append( peak );
            }
        }
    }
}


void MultiSpecWidget::Deconvulate()
{
    QVector<FitThread *> threads;
    qDeleteAll(m_fit);
    m_fit.clear();
    double min = m_chartview->XMin();
    double max = m_chartview->XMax();
    int start = 0, end = 0;
    
    
    for(int i = 0; i < m_spectra.size(); ++i)
    {
        
        FitThread *thread = new FitThread(m_spectra[i]->Name(), i);
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
        
        
        for(int nr = 0; nr < m_peak_list.size(); ++nr)
        {
            const PeakPick::Peak peak = m_peak_list[nr];
            if(peak.max > start && peak.max < end && !(m_spectra[i]->Data()->Y(peak.max) < 0.5*m_spectra[i]->Data()->StdDev()))
            {
                guess_vector.push_back( m_spectra[i]->Data()->X(peak.max ));    
                std::cout << peak.max << " " << m_spectra[i]->Data()->X(peak.max) << std::endl;
            }
        }
        
        Vector guess = Vector::Map(&guess_vector[0], guess_vector.size());
        
        thread->setGuess( guess );
        thread->setRange(start, end);
        thread->setData( m_spectra[i]->Raw() );
        thread->setGLRatio(m_ratio->value());
        QThreadPool::globalInstance()->start(thread);
        threads << thread;
        
    }
    
    QThreadPool::globalInstance()->waitForDone();
    AnalyseFitThreads(threads);
    qDeleteAll(threads);
}

void MultiSpecWidget::AnalyseFitThreads(const QVector<FitThread *> &threads)
{
    QString result, last_row;
    result = "Name\t\tPosition\ta\tc\tgamma\tscale\n";
    for(int work = 0; work < threads.size(); ++work)
    {
        
        Vector parameter = threads[work]->Parameter();
        int guess = threads[work]->Guess().size();
        int start = threads[work]->Start();
        int end = threads[work]->End();
        for(int i = 0; i < guess; ++i)
            result += threads[work]->Name() + "\t" + QString::number(parameter(0+i*5)) + "\t" + QString::number(parameter(1+i*5)) + "\t" + QString::number(parameter(2+i*5)) + "\t" + QString::number(parameter(3+i*5)) + "\t" + QString::number(parameter(4+i*5)) + "\n";
        for(int i = 0; i <  guess; ++i)
            last_row += QString::number(parameter(0+i*5)) + " ";
        last_row += "\n";
        
        result += "Sum of Errors = " + QString::number(threads[work]->SumError()) + "... Sum of Squares = " + QString::number(threads[work]->SumSquared()) + "\n";
        
        QtCharts::QLineSeries *series = new QtCharts::QLineSeries;
        
        for(int i = start; i <= end; ++i)
        {
            double x = threads[work]->Data()->X(i);
            double y = PeakPick::Signal(x, parameter, guess)*m_scale;
            if(y > threads[work]->Data()->StdDev()/2.0)
                series->append(x, y + threads[work]->Position() );
        }
        series->setName(QString::number(parameter(0)));
        m_chartview->addSeries(series);
        
        m_fit << series;
    }
    
    result += "Gaussian function defined as: 1/(a*sqrt(2*pi))*exp(-pow((x-x_0),2)/(2*pow(c,2)))\n";
    result += "Lorentzian function defined as: 1/pi*(0.5*gamma)/(pow(x-x_0,2)+pow(0.5*gamma,2))\n";
    FitParameter *fit = new FitParameter(result, this);
    fit->show();
    std::cout << result.toStdString() << std::endl;
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
    QString value_input = m_select->PeakList();
    std::vector<double> entries;
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
        m_fit_threads[work]->setGuess( guess );
        m_fit_threads[work]->setRange( start, end );
        m_fit_threads[work]->setGLRatio(m_ratio->value());
        QThreadPool::globalInstance()->start(m_fit_threads[work]);
    }
    QThreadPool::globalInstance()->waitForDone();
    AnalyseFitThreads(m_fit_threads);
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

    PeakPick::Peak  peak;
    int start = 0, end = 0, work;
    double diff_min = 10, diff_max = 10;
        bool inrange = false;    
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
    
    qDebug() << work << m_spectra.size();
    Vector guess(1);
    guess(0) = (x_max+x_min)/2.0;
    FitThread *thread = new FitThread("name", work);
    
    thread->setData(m_spectra[work]->Raw());
    thread->setGuess( guess );
    thread->setRange( start, end );
    thread->setGLRatio(m_ratio->value());
    thread->run();
    
        AnalyseFitThreads( QVector<FitThread *>() << thread );
    
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
