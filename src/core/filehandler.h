/*
 * <QBit loads files.>
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


#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QRunnable>
#include <QtCore/QVector>

#include <libpeakpick/spectrum.h>

#include "nmrspec.h"

class SpectrumLoader : public QRunnable
{
public:
    SpectrumLoader(const QString &file);
    inline ~SpectrumLoader(){ };
    
    virtual void run() override;
    
    QString FullName() const { return m_filename; }
    QString BaseName() const { return m_basename; }
    bool Loaded() const { return m_load; }
    
    
    NMRSpec Spectrum() const { return NMRSpec(m_basename, spectrum, original); }
    
private:
    bool loadAsciiFile();
    bool loadNMRFile();
    bool loadFidFile();
    Vector BinFile2Vector(const QString &filename);
    
    QString m_filename, m_basename, m_path;
    PeakPick::spectrum spectrum, original;
    bool m_load = false;

};


class fileHandler : public QObject
{
    Q_OBJECT
    
public:
    fileHandler();
    ~fileHandler();
    
    int addFile(const QString &filename);
    void addFiles(const QStringList &filenames);
    int addDirectory(const QString &dirname);
    void addDirectories(const QString &dirnames);
    
    NMRSpec * Spectrum(int i) { return m_spectra[i]; }
//     QVector< NMRSpec *> SpectraList() { return m_work_spectra.toVector(); }
    
private:  
    QStringList m_filelist;
    QStringList m_dirlist;
    QVector<NMRSpec *> m_spectra;
    
signals:
    void Finished();
    void SpectrumAdded(int index);
    void FileAdded(int index);
};
