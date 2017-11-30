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
#include <QtCharts/QChartGlobal>
#include <QtWidgets/QGraphicsItem>
#include <QtGui/QFont>

class QGraphicsSceneMouseEvent;
//class QtCharts::QChart;

class PeakCallOut : public QGraphicsItem
{
public:

    PeakCallOut(QtCharts::QChart *parent);

       void setText(const QString &text, const QPointF &point);
       void setAnchor(QPointF point);
       void updateGeometry();

       QRectF boundingRect() const;
       void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);

   protected:
       void mousePressEvent(QGraphicsSceneMouseEvent *event);
       void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

   private:
       QString m_text;
       QRectF m_textRect;
       QRectF m_rect;
       QPointF m_anchor, m_text_position;
       QFont m_font;
       QtCharts::QChart *m_chart;
};

