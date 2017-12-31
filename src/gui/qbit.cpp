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

#include <QtCore/QCollator>
#include <QtCore/QDir>
#include <QtCore/QVector>

#include <QtWidgets/QAction>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QApplication>

#include "libpeakpick/peakpick.h"
#include <libpeakpick/glfit.h>

#include "src/core/filehandler.h"
#include "src/func/fit_threaded.h"

#include "src/gui/widgets/multispecwidget.h"
#include "src/gui/widgets/peakwidget.h"
#include "src/gui/widgets/fileswidget.h"
#include "src/gui/widgets/glfitlist.h"

#include "qbit.h"

QBit::QBit():  m_files(new fileHandler), m_spec_widget(new MultiSpecWidget(this))
{    
    m_open = new QAction(this);
    m_open->setText( "Open File" );
    connect(m_open, &QAction::triggered, this, static_cast<void(QBit::*)()>(&QBit::LoadFile ));
    
    m_openDir = new QAction(this);
    m_openDir->setText( "Open Dir" );
    connect(m_openDir, &QAction::triggered, this, &QBit::LoadDir );
    
    m_load_sev= new QAction(this);
    m_load_sev->setText( "Open Selected" );
    connect(m_load_sev, &QAction::triggered, this, &QBit::LoadSeveral );
    
    m_quit = new QAction(this);
    m_quit->setText( "Quit" );
    connect(m_quit, &QAction::triggered, this, &QMainWindow::close );
   
    m_file = new QToolBar;
    m_file->addAction(m_open);
    m_file->addAction(m_openDir);
    
    m_manipulate = new QToolBar;
    m_manipulate->addAction(m_load_sev);
    
    m_system = new QToolBar;
    m_system->addAction(m_quit);
    
    addToolBar(m_file);
    addToolBar(m_manipulate);
    addToolBar(m_system);

    m_files_widget = new FilesWidget;
    //m_files_widget->setMaximumWidth(200);

    m_peak_widget = new PeakWidget;

    m_glfitlist_widget = new GLFitList;
    QScrollArea *fit_scroll = new QScrollArea;
    fit_scroll->setWidget(m_glfitlist_widget);
    fit_scroll->setWidgetResizable(true);
    fit_scroll->setAlignment(Qt::AlignTop);

    m_files_dock = new QDockWidget(tr("Files"));
    m_files_dock->setObjectName(tr("files"));
    m_files_dock->setWidget(m_files_widget);

    m_peaks_dock = new QDockWidget(tr("Peak List"));
    m_peaks_dock->setObjectName(tr("peaks"));
    m_peaks_dock->setWidget(m_peak_widget);

    m_glfitlist_dock = new QDockWidget(tr("Fit List"));
    m_glfitlist_dock->setObjectName("glfitlist");
    m_glfitlist_dock->setWidget(fit_scroll);

    addDockWidget(Qt::LeftDockWidgetArea, m_files_dock);
    addDockWidget(Qt::RightDockWidgetArea, m_peaks_dock);
    addDockWidget(Qt::RightDockWidgetArea, m_glfitlist_dock);

    setCentralWidget(m_spec_widget);

    connect(m_files, &fileHandler::SpectrumAdded, this, &QBit::LoadSpectrum);
    connect(m_files, &fileHandler::Finished, this, &QBit::Finished);
    connect(m_files, &fileHandler::FileAdded, this, &QBit::addFile);
    connect(m_files_widget, &FilesWidget::LoadItem, this, &QBit::LoadItem);
    connect(m_peak_widget, &PeakWidget::ShowPeaks, m_spec_widget, &MultiSpecWidget::ShowPickedPeaks);
    connect(m_spec_widget, &MultiSpecWidget::PeakPicked, this, &QBit::PeakPicked);
    connect(m_spec_widget, &MultiSpecWidget::DeconvulationFinished, this, &QBit::PeakPicked);
    connect(m_spec_widget, &MultiSpecWidget::DeconvulationFinished, m_glfitlist_widget, &GLFitList::UpdateList);
    connect(m_peak_widget, &PeakWidget::PrecisionChanged, m_spec_widget, &MultiSpecWidget::PickPeaks);
}


QBit::~QBit()
{
    delete m_files;
}


void QBit::LoadFile(const QString &file)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    m_spec_widget->clear();
    m_files->addFile(file);
    QApplication::restoreOverrideCursor();
}


void QBit::LoadFiles(QStringList fileName)
{
    if(fileName.size() == 0)
        return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    m_spec_widget->clear();

    QCollator collator;
            collator.setNumericMode(true);
            std::sort(
                fileName.begin(),
                      fileName.end(),
                      [&collator](const QString &key1, const QString &key2)
                      {
                          return collator.compare(key1, key2) < 0;
                      });

    m_files->addFiles(fileName);

    QApplication::restoreOverrideCursor();
}

void QBit::LoadFile()
{
    QStringList fileName = QFileDialog::getOpenFileNames(this, tr("Open File"),
                                                QDir::homePath(),
                                                tr("Files (*.dat *.txt *.spec fid 1r *.dpt *SPA)"));
    LoadFiles(fileName);
}

void QBit::LoadDir()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open File"),
                                                QDir::homePath());
    
    if(m_files->addDirectory(dir) > 0)
        LoadItem(m_files_widget->item(0));
}

void QBit::LoadSpectrum(int index)
{
    addFile(index);
    m_current_index = index;
    m_spec_widget->addSpectrum(m_files->Spectrum(index));
}

void QBit::Finished()
{
    m_spec_widget->UpdateSeries(6);
    m_spec_widget->ResetZoomLevel();
    m_peak_widget->setSpectraList(m_spec_widget->SpectraList() );
    m_glfitlist_widget->setFitList(m_spec_widget->GLFitList() );
    m_spec_widget->PickPeaks(9);
}

void QBit::addFile(int index)
{
    if(index == -1)
        return;
    QString name = m_files->Spectrum(index)->Name();
    QString path = m_files->Spectrum(index)->Path();
    
    QListWidgetItem *item = new QListWidgetItem(name);
    item->setData(Qt::UserRole, index);
    item->setData(Qt::UserRole + 1, path);
    
    m_files_widget->addItem(item);
}

void QBit::LoadItem(const QListWidgetItem * item)
{
    QRectF zoom = m_spec_widget->getZoom();
    int index = item->data(Qt::UserRole).toInt();
    if(index == m_current_index)
        return;
    m_spec_widget->clear();
    m_spec_widget->addSpectrum(m_files->Spectrum(index));
    Finished();
    m_current_index = index;
    m_spec_widget->setZoom(zoom);
}

void QBit::LoadSeveral()
{
    QRectF zoom = m_spec_widget->getZoom();
    m_spec_widget->clear();
    for(const QListWidgetItem *item : m_files_widget->selectedItems())
    {
        int index = item->data(Qt::UserRole).toInt();
        qDebug() << m_spec_widget->addSpectrum(m_files->Spectrum(index));
    }
    Finished();
    m_spec_widget->setZoom(zoom);
    m_files_widget->clearSelection();
}

void QBit::PeakPicked()
{
    m_peak_widget->setPeaks(m_spec_widget->PeakList());
    m_peak_widget->setManualPeaks(m_spec_widget->ManualPeakList());
}

#include "qbit.moc"
