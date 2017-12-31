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

#include <QtCore/QPair>
#include <QtCore/QPointer>

#include <QtWidgets/QPushButton>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>

#include <libpeakpick/glfit.h>

#include "src/func/fit_threaded.h"

#include "glfitwidget.h"


GLSingleFit::GLSingleFit(int index, const Vector &vector, QWidget *parent) : m_index(index), m_parameter(vector)
{

}


GLFitWidget::GLFitWidget(QPointer<FitThread> fitthread, QWidget *parent) :m_fitthread(fitthread), QWidget(parent)
{
    m_glfit = m_fitthread->GLFit();
    QVBoxLayout *list = new QVBoxLayout;
    for(int i = 0; i < m_glfit->Functions(); ++i)
    {
        GLSingleFit *fit = new GLSingleFit(i, m_glfit->Function(i));
        m_list.append(fit);
        list->addWidget(fit);
    }

    m_fits_widget = new QWidget;
    m_fits_widget->setLayout(list);

    QVBoxLayout *main = new QVBoxLayout;
    m_overview = new QWidget;
    main->addWidget(m_overview);
    main->addWidget(m_fits_widget);
    setLayout(main);
    QGridLayout *layout = new QGridLayout;

    m_header = new QLabel();
    layout->addWidget(m_header, 0, 0);
    m_add = new QPushButton(tr("+"));
    m_add->setMaximumSize(30,30);
    m_add->setFlat(true);
    layout->addWidget(m_add, 0, 1);

    m_param = new QPushButton(tr("info"));
    m_param->setFlat(true);
    m_param->setMaximumSize(30,30);
    layout->addWidget(m_param, 0, 2);
    connect(m_add, &QPushButton::clicked, this, &GLFitWidget::AddFunction);
    connect(m_param, &QPushButton::clicked, this, &GLFitWidget::ShowParameter);

    m_overview->setLayout(layout);
    setUiOverView();
}

void GLFitWidget::setUiOverView()
{

    QString text = QString::number(m_glfit->Functions()) + " functions.\n";
    for(int i = 0; i < m_glfit->Functions(); ++i)
    {
        text += QString::number(m_glfit->X_0(i)) + " ";
        if(i%3 == 0 && i)
            text += "\n";
    }
    m_header->setText(text);

}

void GLFitWidget::AddFunction()
{
    double zahl = 0;
    for(int i = 0; i < m_glfit->Functions(); ++i)
        zahl += m_glfit->X_0(i);
    zahl /= double(m_glfit->Functions());

    m_glfit->createLock();
    m_glfit->addFunction(m_glfit->StdGuess(zahl), m_glfit->StdLock(1));

    m_fitthread->reFit();
    m_glfit->releaseLock();
    m_fitthread->reFit();
    m_glfit->Print();
    emit m_fitthread->FitFinished();
    setUiOverView();
}


void GLFitWidget::ShowParameter()
{
    QMessageBox::information(this, tr("About this fitted peaks"), m_fitthread->toHtml());
}

#include "glfitwidget.moc"
