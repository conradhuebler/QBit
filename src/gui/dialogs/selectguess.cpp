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

#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

#include <QDebug>

#include "selectguess.h"

SelectGuess::SelectGuess()
{
    setUi();
    setModal(false);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
}

SelectGuess::~SelectGuess()
{
}


void SelectGuess::setUi()
{
    QGridLayout *layout = new QGridLayout;
    m_start = new QDoubleSpinBox;
    m_end = new QDoubleSpinBox;
    
    connect(m_start, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &SelectGuess::MinChanged);
    connect(m_end, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &SelectGuess::MaxChanged);
    
    m_maxima = new QLineEdit;
    m_maxima->setClearButtonEnabled(true);
/*
    layout->addWidget(new QLabel(tr("Start Region")), 0, 0);
    layout->addWidget(m_start, 0, 1);
    
    layout->addWidget(new QLabel(tr("End Region")), 0, 2);
    layout->addWidget(m_end, 0, 3);
*/
    layout->addWidget(new QLabel(tr("Peak Guess:")), 1, 0);
    layout->addWidget(m_maxima, 1, 1, 1, 2);
    
    m_fit = new QPushButton(tr("Fit"));
    connect(m_fit, &QPushButton::clicked, this, &SelectGuess::Fit);
    layout->addWidget(m_fit, 2, 3);
    
    setLayout(layout);
}

void SelectGuess::addMaxima(const QPointF &point)
{
    if(!isHidden())
        m_maxima->setText(m_maxima->text() + " " + QString::number(point.x()));
}

QString SelectGuess::PeakList() const
{
    return m_maxima->text();
}

void SelectGuess::clear()
{
    m_maxima->clear();
}
