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

#include <QtCharts/QChart>
#include <QtGui/QFontMetrics>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsSceneMouseEvent>

#include "peakposcallout.h"

PeakPosCallOut::PeakPosCallOut(QChart* chart)
    : QGraphicsItem(chart)
    , m_chart(chart)
{
}

QRectF PeakPosCallOut::boundingRect() const
{
    QPointF anchor = mapFromParent(m_chart->mapToPosition(m_anchor));
    QRectF rect;
    rect.setLeft(qMin(m_rect.left(), anchor.x()));
    rect.setRight(qMax(m_rect.right(), anchor.x()));
    rect.setTop(qMin(m_rect.top(), anchor.y()));
    rect.setBottom(qMax(m_rect.bottom(), anchor.y()));
    return rect;
}

void PeakPosCallOut::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    QPainterPath path;

    QPointF anchor = mapFromParent(m_chart->mapToPosition(m_text_position));
    QPointF peak = mapFromParent(m_chart->mapToPosition(m_anchor));
    //if (!m_rect.contains(anchor)) {
    path.moveTo(anchor);
    path.lineTo(anchor);
    path.lineTo(peak);
    path = path.simplified();
    //}

    painter->drawPath(path);
    painter->drawText(anchor, m_text);
}

void PeakPosCallOut::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    event->setAccepted(true);
}

void PeakPosCallOut::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        setPos(mapToParent(event->pos() - event->buttonDownPos(Qt::LeftButton)));
        event->setAccepted(true);
    } else {
        event->setAccepted(false);
    }
}

void PeakPosCallOut::setText(const QString& text, const QPointF& point)
{
    m_text_position = point;
    m_text = text;
    QFontMetrics metrics(m_font);
    m_textRect = metrics.boundingRect(QRect(0, 0, 150, 150), Qt::AlignLeft, m_text);
    prepareGeometryChange();
    m_rect = m_textRect.adjusted(0, 0, 0, 0);
    setRotation(-90);
}

void PeakPosCallOut::setAnchor(QPointF point)
{
    m_anchor = point;
}

void PeakPosCallOut::updateGeometry()
{
    prepareGeometryChange();
    setPos(m_chart->mapToPosition(m_anchor) + QPoint(10, -50));
}
