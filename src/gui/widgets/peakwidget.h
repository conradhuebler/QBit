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

#include <vector>

#include <QtCore/QObject>

#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>


#include <libpeakpick/spectrum.h>
#include <libpeakpick/peakpick.h>

#include "src/core/nmrspec.h"

class QSpinBox;
class PeakWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PeakWidget(QWidget *parent = nullptr);

    inline void setPeaks(QVector<std::vector<PeakPick::Peak> > *peaks) { m_peaks = peaks; Update(); }
    inline void setManualPeaks(QVector<std::vector<PeakPick::Peak> > *peaks) { m_manual = peaks; has_manual = true; Update(); }
    inline void setSpectraList(QVector<NMRSpec *> *spectra) { m_spectra = spectra; }

    void Update();

public slots:



private:
    QVector<std::vector<PeakPick::Peak> > *m_peaks, *m_manual;
    QVector<NMRSpec *> *m_spectra;
    QTableWidget *m_peak_list;
    QPushButton *m_show_peaks;
    QSpinBox *m_precision;

    void BuildList(const QVector<std::vector<PeakPick::Peak> > *peaks, bool value);
    bool has_manual;
signals:
    void ShowPeaks(bool show);
    void PrecisionChanged(int precision);
};

