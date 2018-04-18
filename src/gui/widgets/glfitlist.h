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
 *
 */

#pragma once

#include <QtCore/QPair>
#include <QtCore/QPointer>

#include <QtWidgets/QWidget>

#include <libpeakpick/libpeakpick/deconvulate.h>

#include "src/func/fit_threaded.h"

class QVBoxLayout;

class GLFitWidget;

class GLFitList : public QWidget
{
    Q_OBJECT
public:
    explicit GLFitList(QWidget *parent = 0);
    void setFitList(QVector< QPointer<FitThread >  > * glfit_list) { m_glfit_list = glfit_list; UpdateList(); }

signals:

public slots:
    void UpdateList();

private:
    QVector< QPointer<FitThread >  > * m_glfit_list;
    QVector<QPointer<GLFitWidget> > m_widget_list;
    QVBoxLayout *main_layout;
    
private slots:
    void removeItem();

};

