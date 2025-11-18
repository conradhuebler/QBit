/*
 * <QBit About Dialog>
 * Copyright (C) 2017 - 2025 Conrad Hübler <Conrad.Huebler@gmx.net>
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

#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>

#include <QtCore/QCoreApplication>

#include "aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("About QBit"));
    setMinimumWidth(500);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Application info
    m_info = new QLabel(this);
    m_info->setTextFormat(Qt::RichText);
    m_info->setOpenExternalLinksInBrowser(true);
    m_info->setWordWrap(true);

    QString version = QCoreApplication::applicationVersion();
    if(version.isEmpty())
        version = "0.1.0";

    QString gitBranch, gitCommit, gitDate;
#ifdef GIT_BRANCH
    gitBranch = QString(GIT_BRANCH);
#endif
#ifdef GIT_COMMIT_HASH
    gitCommit = QString(GIT_COMMIT_HASH);
#endif
#ifdef GIT_COMMIT_DATE
    gitDate = QString(GIT_COMMIT_DATE);
#endif

    QString infoText = QString(
        "<h2>QBit %1</h2>"
        "<p><b>An Open Source Qt6 based NMR Spectra Software</b></p>"
        "<p>QBit provides visualization, peak fitting, and deconvolution "
        "capabilities for NMR spectroscopy data.</p>"
        "<hr>"
        "<p><b>Author:</b> Conrad Hübler<br>"
        "<b>Email:</b> <a href='mailto:Conrad.Huebler@gmx.net'>Conrad.Huebler@gmx.net</a><br>"
        "<b>Repository:</b> <a href='https://github.com/conradhuebler/QBit'>github.com/conradhuebler/QBit</a></p>"
        "<hr>"
        "<p><b>Version:</b> %1<br>"
        "%2%3%4"
        "<b>Built with:</b> Qt %5<br>"
        "<b>License:</b> GNU GPL v3</p>"
        "<hr>"
        "<p><b>Based on:</b><br>"
        "• libpeakpick (Peak detection and fitting)<br>"
        "• Eigen (Non-linear optimization)<br>"
        "• kissfft (FFT processing)<br>"
        "• CuteChart (Spectrum visualization)</p>"
    ).arg(version)
     .arg(version)
     .arg(gitBranch.isEmpty() ? "" : QString("<b>Branch:</b> %1<br>").arg(gitBranch))
     .arg(gitCommit.isEmpty() ? "" : QString("<b>Commit:</b> %1<br>").arg(gitCommit))
     .arg(gitDate.isEmpty() ? "" : QString("<b>Date:</b> %1<br>").arg(gitDate))
     .arg(qVersion());

    m_info->setText(infoText);
    mainLayout->addWidget(m_info);

    // Close button
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    m_close = new QPushButton(tr("Close"), this);
    connect(m_close, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(m_close);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

AboutDialog::~AboutDialog()
{
}
