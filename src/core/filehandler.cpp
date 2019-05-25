/*
 * <fileHandler loads files.>
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

#include <kiss_fft.h>

#include <complex>
#include <vector>
#include <fstream>
#include <string>
#include <bitset>

#include <QtCore/QCollator>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QLocale>
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
    bool nmr = false;
    std::vector<double> entries;

    if (!filecontent.size())
        return false;

    if (filecontent.first().contains("SPECMAN_ASCII(ACD)")) {
        bool content = false;
        for (const QString& str : filecontent) {
            if (!content) {

                if (str.contains("PointsCount")) {
                    QStringList items = str.split(" ");
                    number = items[2].toInt();
                }
                if (str.contains("FirstX")) {
                    QStringList items = str.split(" ");
                    min = items[2].toDouble();
                }
                if (str.contains("LastX")) {
                    QStringList items = str.split(" ");
                    max = items[2].toDouble() * -1;
                    content = true;
                }
            } else {
                QStringList items = str.split("\t");
                if (items.size() == 2) {
                    entries.push_back(items[1].toDouble());
                }
            }
        }
        number = entries.size();
    } else {

        for (const QString& str : filecontent) {
            if (str.isEmpty() || str.isNull())
                continue;
            if (str.contains("LEFT")) {
                QStringList items = str.split(" ");
                min = items[3].toDouble();
                max = items[6].toDouble();
                nmr = true;
            }
            if (str.contains("SIZE")) {
                QStringList items = str.split(" ");
                number = items[3].toInt();
            }
            if (!str.contains("#")) {
                i++;
                if (nmr)
                    entries.push_back(str.toDouble());
                else {
                    QStringList list = QString(str).split("\t");
                    if (list.size() < 2)
                        continue;
                    QLocale local;
                    entries.push_back(local.toDouble(list[1]));
                    max = local.toDouble(list[0]);
                    if (i == 1)
                        min = local.toDouble(list[0]);
                }
            }
        }
    }
    if (number == 0) {
      max = i;
      number = i;
    }
    if (!entries.size())
        return false;

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
    
    QStringList path_list  = m_path.split("/");
    
    m_basename = path_list[path_list.size() - 4];
    return true;
    
}

bool SpectrumLoader::loadJEOLFile()
{
    int  intNum   = 0;
    std::cout << "starting with jeol file " << std::endl;
    std::vector<double> entries;
    Vector y;
    std::ifstream file_i (m_filename.toStdString(), std::ios::binary);
    if(file_i.is_open())
    {
        int number = 0;
        while(true)
        {
            char *_char = new char[sizeof(intNum)];
            file_i.read(_char,sizeof(intNum));   
            std::cout << std::string(_char) << std::endl;
            
            
            if(file_i.eof()) {
                break;
            }
//             entries.push_back(intNum);
//             number++;
        }
//         y = Vector::Map(&entries[0], number);
        file_i.close ();
    }
    
    
//     original = PeakPick::spectrum(y,-15,0); 
    
    return true;
    
}

bool SpectrumLoader::loadFidFile()
{
    Vector y = BinFile2Vector(m_filename);
    if(y.size() == 0)
        return false;
    
    const int nfft=y.size();
    const int fill = 2;

    kiss_fft_cfg fwd1 = kiss_fft_alloc(nfft*fill,0,NULL,NULL);
    kiss_fft_cfg fwd2 = kiss_fft_alloc(nfft*fill,0,NULL,NULL);
    // kiss_fft_cfg inv = kiss_fft_alloc(nfft/2,1,NULL,NULL);

    std::vector<std::complex<float>> x1(nfft*fill, 0.0);
    std::vector<std::complex<float>> x2(nfft*fill, 0.0);

    std::vector<std::complex<float>> fx1(nfft*fill, 0.0);
    std::vector<std::complex<float>> fx2(nfft*fill, 0.0);

    double size = y.size();
    for(int i = 0; i < y.size()/2; i = i + 1)
    {

        x1[i] = y(i);
        // x1[y.size()/2 + i - 1] = 0;
        // x1[y.size() + i - 1] = 0;

        x2[i] = y(y.size()/2  + i - 1);
        // x2[y.size()/2 + i - 1] = 0;
        // x2[y.size() + i - 1] = 0;
    }
    kiss_fft(fwd1,(kiss_fft_cpx*)&x1[0],(kiss_fft_cpx*)&fx1[0]);
    kiss_fft(fwd2,(kiss_fft_cpx*)&x2[0],(kiss_fft_cpx*)&fx2[0]);
    std::cout << " fourier transform finished ... " << std::endl;
    std::vector<double > raw_spec;

    //for (int k=0;k<nfft/2 - 2;k += 2)
    //for (int k=nfft*fill/2.0;k < nfft*fill ;++k)
    for(int k = 0; k < nfft*fill/2; ++k)
    {
         // float re = fx1[k].real();
         float re = fx1[k].real();
         raw_spec.push_back( re );
    }
    
    kiss_fft_free(fwd1);
    kiss_fft_free(fwd2);
    // kiss_fft_free(inv);
    
    
    /*
     * All of this is ugly, only time will tell, how to do this physically correct
     */

    Vector spec = Vector::Map(&raw_spec[0], nfft*fill/2);
    const QString SW_h = "##$SW_h=";
    const QString TD = "##$TD=";
    const QString O1 = "##$O1=";

    QFile peakrng( m_path + QDir::separator() + "acqus");
    double sw_h = 15.0;
    double td = -0;
    double o1 = 0;
    if(peakrng.open(QIODevice::ReadOnly))
    {
        QString range = peakrng.readAll();
        QStringList lines = range.split("\n");
        for(QString &str : lines)
        {
            if(str.contains(SW_h))
                sw_h = str.remove((SW_h)).toDouble();
            if(str.contains(TD))
                td =str.remove(TD).toDouble();
            if(str.contains(O1))
                o1 =str.remove(O1).toDouble();
        }
    }
    original = PeakPick::spectrum(spec, -1*(sw_h+o1/2.0)/1E3,-2*(o1-sw_h/2.0)/1E3);
    QStringList path_list  = m_path.split("/");
    
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

bool SpectrumLoader::loadDptFile()
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
    std::vector<double> tmp_entries, entries;
    
    for(const QString &str : filecontent)
    {
        QStringList cells = str.split(",");
        if(cells.size() != 2)
            continue;
        if(i == 1)
            max = cells[0].toDouble();
        i++;
        tmp_entries.push_back(cells[1].remove("\r").toDouble());
        min = cells[0].toDouble();
    }
    
    for(int j = tmp_entries.size(); j > 0; --j)
        entries.push_back(tmp_entries[j]);
    
    y = Vector::Map(&entries[0], entries.size()); 
    original = PeakPick::spectrum(y,min,max); 
    return true;
}


void SpectrumLoader::run()
{
    std::cout << "running file checker" << std::endl;

    QFileInfo f(m_filename);

    if (f.baseName().contains("txt") || f.baseName().contains("dat"))
        m_load = loadAsciiFile();
    else if (f.baseName().contains("jdf"))
        m_load = loadJEOLFile();
    else if (f.baseName().contains("1r"))
        m_load = loadNMRFile();
    else if (f.baseName().contains("fid"))
        m_load = loadFidFile();
    else if (f.baseName().contains("dpt"))
        m_load = loadDptFile();
    else if (f.baseName().contains("SPA"))
        m_load = loadFidFile();

    if(m_load)
    {
        spectrum = PeakPick::spectrum(original);
        if(!(m_filename.contains("dpt")))
        {
            PeakPick::Normalise(&original,0,10);
            PeakPick::Normalise(&spectrum, 0, 10);
            PeakPick::SmoothFunction(&spectrum, 12);
        } else if (m_filename.contains("dat"))
          PeakPick::SmoothFunction(&spectrum, 12);
        else
          m_nmr = false;
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
