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

#include <QtWidgets/QLayout>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>

#include "logwidget.h"

LogWidget::LogWidget(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout;

    setLayout(layout);
    m_tabs = new QTabWidget;
    layout->addWidget(m_tabs);
    addMessage("", "overview", 0);
}


void LogWidget::addMessage(const QString &message, const QString &identifier, int prior)
{
    Q_UNUSED(prior)
    if(!m_names.contains(identifier))
    {
        QTextEdit *text = new QTextEdit;
        int index = m_tabs->addTab(text, identifier);
        m_names[identifier] = index;
        if(prior > 0)
            text->append(message);
    }else
    {
        QTextEdit *text = qobject_cast<QTextEdit *>(m_tabs->widget(m_names[identifier]));
        text->append(message);
    }

}
