/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2016  Conrad Hübler <Conrad.Huebler@gmx.net>
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
 * 
 */

#ifndef CHARTVIEW_H
#define CHARTVIEW_H

// #include "src/ui/dialogs/chartconfig.h"
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>
#include <QtCharts/QAreaSeries>
#include <QtCore/QPointer>
class QPushButton;
class QChart;

struct ChartConfig;
struct PgfPlotConfig
{
    QString colordefinition;
    QString plots;
    QStringList table;
};

namespace ToolSet{
    inline qreal scale(qreal value, qreal &pow)
    {
        if(qAbs(value) < 1 && value)
        {
            while(qAbs(value) < 1)
            {
                pow /= 10;
                value *= 10;
            }
        }
        else if(qAbs(value) > 10)
        {
            while(qAbs(value) > 10)
            {
                pow *= 10;
                value /= 10;
            }
        }
        return value;
    }
    
    inline qreal scale(qreal value)
    {
        qreal pot;
        return scale(value, pot);
    }
    
    inline qreal ceil(qreal value)
    {
        double pot = 1;
        value = scale(value, pot);
        int integer = int(value) + 1;    
        if(value < 0)
            integer -= 1;
        return qreal(integer)*pot;
    }
    
    inline qreal floor(qreal value)
    {
        double pot = 1;
        value = scale(value, pot);
        
        int integer = int(value);
        if(value < 0)
            integer -= 1;
        return qreal(integer)*pot;
    }
}

class BoxItem : public QGraphicsItem
{
public:
    BoxItem(const QPointF &point) : m_point(point) {  }
    QRectF boundingRect() const
    {
        qreal penWidth = 1;
        return QRectF(-10 - penWidth / 2, -10 - penWidth / 2,
                      20 + penWidth, 20 + penWidth);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget)
    {
        painter->drawRoundedRect(m_point.x(), m_point.y(), 20, 20, 5, 5);
    }
    
    QPointF m_point;
};


class ChartViewPrivate : public QtCharts::QChartView
{
  Q_OBJECT
public:
    inline ChartViewPrivate(QWidget *parent = Q_NULLPTR) : QtCharts::QChartView(parent), m_double_clicked(false)
    {
        setRubberBand(QChartView::HorizontalRubberBand);
        addVerticalSeries();
    }
    inline ChartViewPrivate(QtCharts::QChart *chart, QWidget *parent = Q_NULLPTR) : QtCharts::QChartView(parent) , m_double_clicked(false)
    {
        setChart(chart); setAcceptDrops(true); 
        setRenderHint(QPainter::Antialiasing, true);
        setRubberBand(QChartView::HorizontalRubberBand);
        addVerticalSeries();
    }
    inline ~ChartViewPrivate(){ };
    
    
public slots:
    inline void UpdateVerticalLine(double x)
    {
       m_vertical_series->replace(0, QPointF(x, m_min));
       m_vertical_series->replace(1, QPointF(x, m_max));
       m_vertical_series->setName(QString::number(x));
    }
    
    inline void UpdateView(double min, double max)
    {
            m_min = min;
            m_max = max;
    }
    
    void UpdateSelectionChart(const QPointF &point);
    
protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent * event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mouseDoubleClickEvent( QMouseEvent * event ) override;
    
private:
    inline void addVerticalSeries()
    {
        m_vertical_series = new QtCharts::QLineSeries;
        QPen pen = m_vertical_series->pen();
        pen.setWidth(1);
        pen.setColor(Qt::gray);
        m_vertical_series->setPen(pen);    
        QPointF start =QPointF(0, -1);
        QPointF end = QPointF(0, 10);
        
        m_vertical_series->append(start);
        m_vertical_series->append(end);
        chart()->addSeries(m_vertical_series);
        
        m_upper = new QtCharts::QLineSeries;
        m_upper->append(0,0);
        m_upper->append(0,0);
        m_lower = new QtCharts::QLineSeries;
        m_lower->append(0,0);
        m_lower->append(0,0);
        
        m_area = new QtCharts::QAreaSeries(m_upper, m_lower);
        

        chart()->addSeries(m_area);
        m_area->hide();
    }
    
    void handleMouseMoved(const QPointF &point);
    
    QtCharts::QLineSeries *m_vertical_series, *m_upper, *m_lower;
    QtCharts::QAreaSeries *m_area;
    QPointF rect_start;
    double m_min, m_max;
    bool m_double_clicked;
private slots:

    
signals:
    void ZoomChanged();
    void scaleUp();
    void scaleDown();
    void AddRect(const QPointF &point1, const QPointF &point2);
    void PointDoubleClicked(const QPointF &point);
};


class ChartView : public QWidget
{
    Q_OBJECT
public:
    ChartView(QtCharts::QChart *chart, bool latex_supported = false);
    ChartView();
    inline ~ChartView() { }
    void addSeries( QtCharts::QAbstractSeries* series, bool legend = false );
    qreal YMax() const { return m_ymax; }
    inline qreal XMax() const 
    {
        QPointer<QtCharts::QValueAxis> x_axis = qobject_cast<QtCharts::QValueAxis *>( m_chart->axisX());
        if(x_axis)
            return x_axis->max();
        else
            return 0;
    }
    inline qreal XMin() const 
    {
        QPointer<QtCharts::QValueAxis> x_axis = qobject_cast<QtCharts::QValueAxis *>( m_chart->axisX());
        if(x_axis)
            return x_axis->min();
        else
            return 0;
    }
    inline void setYMax(qreal ymax) 
    {
        QPointer<QtCharts::QValueAxis> y_axis = qobject_cast<QtCharts::QValueAxis *>( m_chart->axisY());
        if(y_axis)
        {
            y_axis->setMin(0);
            y_axis->setTickCount(0);
            y_axis->setMax(ymax);
        }
    }
    
    inline void removeSeries(QtCharts::QAbstractSeries *series) { m_chart->removeSeries(series); }
    inline QList<QtCharts::QAbstractSeries *> series() const { return m_chart->series(); }
    
public slots:
    void formatAxis();
    
    void setXAxis(const QString &str) { m_x_axis = str; emit AxisChanged(); }
    void setYAxis(const QString &str) { m_y_axis = str; emit AxisChanged(); };
        
    inline void UpdateView(double min, double max)
    {
            m_chart_private->UpdateView(min, max);
    }
    
private:
    ChartViewPrivate *m_chart_private;
    QPointer< QtCharts::QChart > m_chart;
    QPushButton *m_config;
    void setUi();
    bool has_legend, connected;
    QString m_x_axis, m_y_axis;
    ChartConfig getChartConfig() const;
    PgfPlotConfig getScatterTable() const;
    PgfPlotConfig getLineTable() const;
    QString Color2RGB(const QColor &color) const;
    void WriteTable(const QString &str) const;
//     ChartConfigDialog m_chartconfigdialog;
    bool m_pending, m_lock_scaling, m_latex_supported;   
    qreal m_ymax;

private slots:
//     void PlotSettings();
//     void PrintPlot();
//     void ExportLatex();
//     void ExportGnuplot();
    void ExportPNG();
//     void setChartConfig(const ChartConfig &chartconfig);
    void forceformatAxis();
    
signals:
    void AxisChanged();
    void ZoomChanged();
    void scaleUp();
    void scaleDown();
    void AddRect(const QPointF &point1, const QPointF &point2);
    void PointDoubleClicked(const QPointF &point);
};

#endif // CHARTVIEW_H
