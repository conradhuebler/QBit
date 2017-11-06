/*
 * <fileHandler loads files.>
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


#include <vector>
#include <fstream>
#include <string>
#include <bitset>

#include <QtCore/QCollator>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDebug>
#include <QtCore/QThreadPool>

#include <libpeakpick/spectrum.h>
#include <libpeakpick/analyse.h>

#include "filehandler.h"


static void findRecursion(const QString &path, QStringList *result)
{
    QDir currentDir(path);
    QStringList pattern = QStringList() << "1r";
    const QString prefix = path + QLatin1Char('/');
    foreach (const QString &match, currentDir.entryList(pattern, QDir::Files | QDir::NoSymLinks))
        result->append(prefix + match);
    foreach (const QString &dir, currentDir.entryList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot))
        findRecursion(prefix + dir, result);
}

SpectrumLoader::SpectrumLoader(const QString &str) : m_filename(str)
{
    setAutoDelete(false);
    QFileInfo info(m_filename);
    m_basename = info.baseName();
    m_path = info.path();
}


bool SpectrumLoader::loadAsciiFile()
{
    Vector y;
    
    QFile file(m_filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << file.errorString();
        return false; 
    }
    
    QStringList filecontent = QString(file.readAll()).split("\n");
    double min = 0;
    double max = 0;
    int i = 1;
    int number = 0;
    std::vector<double> entries;
    for(const QString &str : filecontent)
    {
        if(str.isEmpty() || str.isNull())
            continue;
        if(str.contains("LEFT"))
        {
            QStringList items = str.split(" ");
            min = items[3].toDouble();
            max = items[6].toDouble();
        }
        if(str.contains("SIZE"))
        {
            QStringList items = str.split(" ");
            number = items[3].toInt();
        }
        if(!str.contains("#"))
        {
            i++;
            entries.push_back(str.toDouble());
        }
    }
    
    y = Vector::Map(&entries[0], number); 
    original = PeakPick::spectrum(y,-1*min,max); 
    return true;
}

bool SpectrumLoader::loadNMRFile()
{

    Vector y = BinFile2Vector(m_filename);;
    if(y.size() == 0)
    {
        qDebug() << m_filename << " is empty?!";
        return false;
    }
    const QString high_field = "##$ABSF1=";
    const QString low_field = "##$ABSF2=";
    
    QFile peakrng( m_path + QDir::separator() + "procs");
    
    if(!peakrng.open(QIODevice::ReadOnly))
    {
        qDebug() << peakrng.errorString();
        return false; 
    }
    double min = 0;
    double max = 0;
    QString range = peakrng.readAll();
    QStringList lines = range.split("\n");
    QString prev;
    for(QString &str : lines)
    {
        if(str.contains(high_field))
            min = str.remove((high_field)).toDouble();
        if(str.contains(low_field))
            max =str.remove(low_field).toDouble();
    }
    
    original = PeakPick::spectrum(y,-1*min,-1*max); 
    
    QStringList path_list  = m_path.split(QDir::separator());
    
    m_basename = path_list[path_list.size() - 4];
    return true;
    
}

bool SpectrumLoader::loadFidFile()
{
    Vector y = BinFile2Vector(m_filename);
    if(y.size() == 0)
        return false;
    original = PeakPick::spectrum(y,0,y.size()); 
    
    QStringList path_list  = m_path.split(QDir::separator());
    
    m_basename = path_list[path_list.size() - 2];
    return true;
}

Vector SpectrumLoader::BinFile2Vector(const QString& filename)
{
    int  intNum   = 0;
    std::vector<double> entries;
    Vector y;
    std::ifstream file_i (filename.toStdString(), std::ios::binary);
    if(file_i.is_open())
    {
        int number = 0;
        while(true)
        {
            file_i.read(reinterpret_cast<char *>(&intNum),sizeof(intNum));   
            
            if(file_i.eof()) {
                break;
            }
            entries.push_back(intNum);
            number++;
        }
        y = Vector::Map(&entries[0], number);
        file_i.close ();
    }
    return y;
}


void SpectrumLoader::run()
{
    if(m_filename.contains("txt"))
        m_load = loadAsciiFile();
    else if(m_filename.contains("1r"))
        m_load = loadNMRFile();
    else if(m_filename.contains("fid"))
        m_load = loadFidFile();
    
    if(m_load)
    {
        spectrum = PeakPick::spectrum(original);
        PeakPick::Normalise(&original,0,2);
        PeakPick::Normalise(&spectrum, 0, 2);
        PeakPick::SmoothFunction(&spectrum, 12);
    }
}


fileHandler::fileHandler()
{
    
    
    
}


fileHandler::~fileHandler()
{
    qDeleteAll(m_spectra);
}


int fileHandler::addFile(const QString& filename)
{
    SpectrumLoader * loader = new SpectrumLoader(filename);;
    loader->run();
    if(!loader->Loaded())
        return -1;
    m_spectra.append( new NMRSpec(loader->Spectrum()) );
    emit SpectrumAdded(m_spectra.size() - 1);
    delete loader;
    emit Finished();
    return m_spectra.size() - 1;
}

void fileHandler::addFiles(const QStringList& filenames)
{
    QVector<SpectrumLoader *> loader;
    for(const QString &str : filenames)
    {
        SpectrumLoader *load = new SpectrumLoader(str);
        loader << load;
        QThreadPool::globalInstance()->start(load);  
    }
    
    QThreadPool::globalInstance()->waitForDone();
    for(int i = 0; i < loader.size(); ++i)
    {
        if(!loader[i]->Loaded())
            continue;
        m_spectra.append( new NMRSpec(loader[i]->Spectrum()) );
        emit SpectrumAdded(m_spectra.size() - 1);
        
    }
    qDeleteAll(loader);
    emit Finished();
}


int fileHandler::addDirectory(const QString& dirname)
{
    QStringList files;
    findRecursion(dirname, &files);
        
    
    
        QCollator collator;
        collator.setNumericMode(true);
        std::sort(
            files.begin(),
                  files.end(),
                  [&collator](const QString &key1, const QString &key2)
                  {
                      return collator.compare(key1, key2) < 0;
                  });
    
    QVector<SpectrumLoader *> loader;

    for(const QString &str : files)
    {
        SpectrumLoader * load = new SpectrumLoader(str);;
        loader << load;
        QThreadPool::globalInstance()->start(load);  
    }    
    
    QThreadPool::globalInstance()->waitForDone();
    int count = 0;
    for(int i = 0; i < loader.size(); ++i)
    {
        if(!loader[i]->Loaded())
            continue;
        count++;
        m_spectra.append( new NMRSpec(loader[i]->Spectrum()) );
        emit FileAdded(m_spectra.size() - 1);
        
    }
    qDeleteAll(loader);
    return count;
}

void fileHandler::addDirectories(const QString& dirnames)
{
    
}
