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

#include "fileswidget.h"

FilesWidget::FilesWidget(QWidget *parent) : QWidget(parent)
{
    QGridLayout *layout = new QGridLayout;

    m_files_widget = new QListWidget;
    m_files_widget->setMaximumWidth(200);
    m_files_widget->setSelectionMode(QAbstractItemView::MultiSelection);

    layout->addWidget(m_files_widget, 0, 0);

    setLayout(layout);

    connect(m_files_widget, &QListWidget::itemClicked, this, &FilesWidget::LoadItem);

}
