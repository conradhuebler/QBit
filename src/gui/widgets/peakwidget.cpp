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

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTableWidget>


#include <QDebug>

#include "peakwidget.h"

PeakWidget::PeakWidget(QWidget *parent) : QWidget(parent), has_manual(false)
{
    m_peak_list = new QTableWidget;

    m_show_peaks = new QPushButton(tr("Show Peaks"));
    m_show_peaks->setCheckable(true);

    m_precision = new QSpinBox;
    m_precision->setMinimum(1);
    m_precision->setMaximum(100);
    m_precision->setValue(9);


    QGridLayout *layout = new QGridLayout;
    layout->addWidget(m_show_peaks, 0,0);
    layout->addWidget(new QLabel(tr("Filter")), 0, 1);
    layout->addWidget(m_precision,0, 2);
    layout->addWidget(m_peak_list, 1, 0, 1, 3);

    setLayout(layout);

    connect(m_show_peaks, &QPushButton::toggled, this, &PeakWidget::ShowPeaks);
    connect(m_precision, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &PeakWidget::PrecisionChanged);
}


void PeakWidget::Update()
{
    m_peak_list->clear();
    m_peak_list->setColumnCount(5);
    m_peak_list->setRowCount( 0 );
    m_peak_list->setHorizontalHeaderLabels(QStringList() << tr("Spectrum") << tr("X") << tr("Y") << tr("Integral") << tr("Integral"));
    BuildList(m_peaks, true);
    if(has_manual)
        BuildList(m_manual, false);
}

void PeakWidget::BuildList(const QVector<std::vector<PeakPick::Peak> > *peaks, bool value)
{
    int rowCount = m_peak_list->rowCount();
    for(int i = 0; i < peaks->size(); ++i)
    {
        for(int j = 0; j < (*peaks)[i].size(); ++j)
        {
            int pos = (*peaks)[i][j].max;
            rowCount++;
            m_peak_list->setRowCount( rowCount );
            QTableWidgetItem *newItem = new QTableWidgetItem((*m_spectra)[i]->Name());
            m_peak_list->setItem(rowCount - 1, 0, newItem);
            if(value)
                newItem = new QTableWidgetItem(QString::number((*m_spectra)[i]->Raw()->X(pos)));
            else
                newItem = new QTableWidgetItem(QString::number((*peaks)[i][j].deconv_x));
            m_peak_list->setItem(rowCount - 1, 1, newItem);
            if(value)
                newItem = new QTableWidgetItem(QString::number((*m_spectra)[i]->Raw()->Y(pos)));
            else
                newItem = new QTableWidgetItem(QString::number((*peaks)[i][j].deconv_y));
            m_peak_list->setItem(rowCount - 1, 2, newItem);
            newItem = new QTableWidgetItem(QString::number((*peaks)[i][j].integ_num));
            m_peak_list->setItem(rowCount - 1, 3, newItem);
            newItem = new QTableWidgetItem(QString::number((*peaks)[i][j].integ_analyt));
            m_peak_list->setItem(rowCount - 1, 4, newItem);
        }
    }
}
