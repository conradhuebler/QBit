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
#include <QtCore/QDir>

#include <QtCore/QVector>

#include <QtWidgets/QAction>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QLabel>

#include <QtCore/QThreadPool>

#include <QApplication>

#include "libpeakpick/peakpick.h"
#include "src/core/filehandler.h"
#include "src/gui/multispecwidget.h"

#include "qbit.h"




QBit::QBit(): mdiArea(new QMdiArea), m_files(new fileHandler)
{
    
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);
    
    QAction* fileaction = new QAction(this);
    fileaction->setText( "Open File" );
    connect(fileaction, SIGNAL(triggered()), SLOT(LoadFile()) );
    menuBar()->addMenu( "File" )->addAction( fileaction );
    
    QAction* action = new QAction(this);
    action->setText( "Quit" );
    connect(action, SIGNAL(triggered()), SLOT(close()) );
    menuBar()->addMenu( "Exit" )->addAction( action );
    connect(m_files, &fileHandler::SpectrumAdded, this, &QBit::LoadSpectrum);
    connect(m_files, &fileHandler::Finished, this, &QBit::Finished);
}


QBit::~QBit()
{
    delete m_files;
}


void QBit::LoadFile()
{
    QStringList fileName = QFileDialog::getOpenFileNames(this, tr("Open File"),
                                                QDir::homePath(),
                                                tr("Files (*.dat *.txt *.spec fid 1r)"));
    
    if(fileName.size() == 0)
        return;
    
    
    
    
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    m_widget = new MultiSpecWidget(fileName.size(), this);
    m_files->addFiles(fileName);
   
    setCentralWidget(m_widget);     
    QApplication::restoreOverrideCursor();

}

void QBit::LoadSpectrum(int index)
{
    m_widget->addSpectrum(m_files->Spectrum(index));
}

void QBit::Finished()
{
    m_widget->UpdateSeries(1);
    m_widget->ResetZoomLevel();
}




#include "qbit.moc"
