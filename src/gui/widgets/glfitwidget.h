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
 *
 */

#pragma once

#include <Eigen/Dense>

#include <QtCore/QPointer>

#include <QtWidgets/QWidget>

#include <libpeakpick/glfit.h>

#include "src/func/fit_threaded.h"


typedef Eigen::VectorXd Vector;

class QLabel;
class QPushButton;

class GLSingleFit : public QWidget
{
    Q_OBJECT
public:
    GLSingleFit(int index, const Vector &vector, QWidget *parent = 0);


private:
    Vector m_parameter;
    QPushButton *m_remove;
    int m_index;
};


class GLFitWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GLFitWidget( QPointer<FitThread> fitthread, QWidget *parent = 0);
    ~GLFitWidget();
    QPointer<FitThread > getFitThread() const { return m_fitthread; }
    
signals:
    void removeItem();

public slots:

private:
    PeakPick::GLFit * m_glfit;
    QPointer<FitThread > m_fitthread;
    QVector<QPointer<GLSingleFit > > m_list;
    QWidget *m_fits_widget, *m_overview;
    QPushButton *m_add, *m_param, *m_rm;
    void setUiOverView();
    QLabel *m_header;

private slots:
    void AddFunction();
    void ShowParameter();
};

