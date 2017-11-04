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
#include <libpeakpick/spectrum.h>
#include <libpeakpick/analyse.h>

#include <QtWidgets/QDialog>

class QLabel;
class QDoubleSpinBox;
class QLineEdit;
class QPushButton;

class SelectGuess : public QDialog
{
    Q_OBJECT

public:
    SelectGuess();
    ~SelectGuess();
    void setUi();
    
    void setStart(double start);
    void setEnd(double end);
    
    QString PeakList() const;
    
public slots:
    void addMaxima(const QPointF &point);
    
private:
    QLabel *textfield;
    QDoubleSpinBox *m_start, *m_end;
    QLineEdit *m_maxima;
    QPushButton *m_fit;
    
    QString *maxima;
    
signals:
    void MinChanged(double val);
    void MaxChanged(double val);
    void Fit();
};
