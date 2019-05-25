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

#include <QtCore/QPointer>

#include <QtWidgets/QLayout>

#include <libpeakpick/glfit.h>

#include "src/gui/widgets/glfitwidget.h"
#include "glfitlist.h"

GLFitList::GLFitList(QWidget *parent) : QWidget(parent)
{
     main_layout = new QVBoxLayout;

     setLayout(main_layout);
}


void GLFitList::UpdateList()
{
    qDeleteAll(m_widget_list);
    m_widget_list.clear();

    for(int i = 0; i < (*m_glfit_list).size(); ++i)
    {
        QPointer<GLFitWidget > widget = new GLFitWidget((*m_glfit_list)[i]);
        main_layout->addWidget(widget);
        m_widget_list.append(widget);
    }
}

void GLFitList::removeItem()
{
 /*   QPointer<GLFitWidget > widget = qobject_cast<GLFitWidget *>(QObject::sender());
    int index = (*m_glfit_list).indexOf(widget->getFitThread());
    m_widget_list.remove(m_widget_list.indexOf(widget));
    m_glfit_list->remove(index);
    main_layout->removeWidget(widget);*/
//     delete widget;
}


#include "glfitlist.moc"
