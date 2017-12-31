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

#pragma once

#include <QtCore/QPointer>
#include <QtCore/QVector>

#include <QtWidgets/QMainWindow>

#include "src/core/filehandler.h"

class QHBoxLayout;
class QListWidget;
class QWidget;
class QTabWidget;

class MultiSpecWidget;
class FilesWidget;
class PeakWidget;
class GLFitList;
class LogWidget;

class QListWidgetItem;

class QBit : public QMainWindow
{
    Q_OBJECT
public:
    QBit();
    ~QBit();
    
    void LoadFile(const QString &file);
    void LoadFiles(QStringList files);
    
    
    
private:
    QPointer<fileHandler > m_files;
    QPointer<MultiSpecWidget > m_spec_widget;
    QPointer<PeakWidget > m_peak_widget;
    QPointer<FilesWidget> m_files_widget;
    QPointer<GLFitList> m_glfitlist_widget;
    QPointer<LogWidget> m_logwidget;
    QPointer<QWidget > m_mainwidget;
    QDockWidget *m_files_dock, *m_peaks_dock, *m_glfitlist_dock, *m_logdock;

    QHBoxLayout *m_layout;
    int m_current_index; 
    QAction *m_open, *m_openDir, *m_load_sev, *m_quit;
    QToolBar *m_file, *m_manipulate, *m_system;
    
private slots:
    void LoadFile();
    void LoadDir();
    void LoadSpectrum(int index);
    void Finished();
    void addFile(int index);
    void LoadItem(const QListWidgetItem * item);
    void LoadSeveral();
    void PeakPicked();
};
