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

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>


#include <QDebug>

#include "peakwidget.h"

PeakWidget::PeakWidget(QWidget *parent) : QWidget(parent)
{
    m_peak_list = new QTableWidget;

    m_show_peaks = new QPushButton(tr("Show Peaks"));
    m_show_peaks->setCheckable(true);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(m_show_peaks, 0,0);
    layout->addWidget(m_peak_list, 1, 0);

    setLayout(layout);

    connect(m_show_peaks, &QPushButton::toggled, this, &PeakWidget::ShowPeaks);
}


void PeakWidget::Update()
{
    m_peak_list->clear();
    m_peak_list->setColumnCount(3);
    int rowCount = 0;

    m_peak_list->setHorizontalHeaderLabels(QStringList() << tr("Spectrum") << tr("X") << tr("Y"));
    for(int i = 0; i < m_peaks->size(); ++i)
    {
        for(int j = 0; j < (*m_peaks)[i].size(); ++j)
        {
            int pos = (*m_peaks)[i][j].max;
            rowCount++;
            m_peak_list->setRowCount( rowCount );
            QTableWidgetItem *newItem = new QTableWidgetItem((*m_spectra)[i]->Name());
            m_peak_list->setItem(i+j, 0, newItem);
            newItem = new QTableWidgetItem(QString::number((*m_spectra)[i]->Raw()->X(pos)));
            m_peak_list->setItem(i+j, 1, newItem);
            newItem = new QTableWidgetItem(QString::number((*m_spectra)[i]->Raw()->Y(pos)));
            m_peak_list->setItem(i+j, 2, newItem);
        }
    }

}
