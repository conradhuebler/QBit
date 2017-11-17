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

#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QPushButton>

#include "fitparameter.h"

FitParameter::FitParameter(const QString &result, QWidget *parent) : m_result(result), QDialog(parent)
{
    setUi();
    setModal(false);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    resize(800,600);
}

FitParameter::~FitParameter()
{
}


void FitParameter::setUi()
{
    QGridLayout *layout = new QGridLayout;
    m_parameter = new QTextEdit;
    m_parameter->setText(m_result);
    layout->addWidget(m_parameter);
    setLayout(layout);
}
