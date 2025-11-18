/*
 * <QBit License Dialog>
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

#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QDir>

#include "licensedialog.h"

LicenseDialog::LicenseDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("License Information"));
    resize(700, 500);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Text browser for license
    m_textBrowser = new QTextBrowser(this);
    m_textBrowser->setOpenExternalLinks(true);

    // Try to load LICENSE.md from various possible locations
    QStringList possiblePaths = {
        QDir::currentPath() + "/LICENSE.md",
        QDir::currentPath() + "/../LICENSE.md",
        ":/LICENSE.md",  // If embedded as resource
        QCoreApplication::applicationDirPath() + "/LICENSE.md",
        QCoreApplication::applicationDirPath() + "/../LICENSE.md"
    };

    QString licenseText;
    bool loaded = false;

    for(const QString &path : possiblePaths)
    {
        QFile file(path);
        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file);
            licenseText = in.readAll();
            file.close();
            loaded = true;
            break;
        }
    }

    if(!loaded)
    {
        // Fallback: Display GPL v3 summary
        licenseText = QString(
            "<h2>GNU General Public License v3.0</h2>"
            "<p>QBit is free software: you can redistribute it and/or modify "
            "it under the terms of the GNU General Public License as published by "
            "the Free Software Foundation, either version 3 of the License, or "
            "(at your option) any later version.</p>"
            "<p>QBit is distributed in the hope that it will be useful, "
            "but WITHOUT ANY WARRANTY; without even the implied warranty of "
            "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the "
            "GNU General Public License for more details.</p>"
            "<p>You should have received a copy of the GNU General Public License "
            "along with this program. If not, see "
            "<a href='https://www.gnu.org/licenses/'>https://www.gnu.org/licenses/</a>.</p>"
            "<hr>"
            "<p><b>Full License Text:</b><br>"
            "The full text of the GNU GPL v3 license can be found at:<br>"
            "<a href='https://www.gnu.org/licenses/gpl-3.0.html'>https://www.gnu.org/licenses/gpl-3.0.html</a></p>"
            "<hr>"
            "<h3>Permissions</h3>"
            "<ul>"
            "<li>Commercial use</li>"
            "<li>Distribution</li>"
            "<li>Modification</li>"
            "<li>Patent use</li>"
            "<li>Private use</li>"
            "</ul>"
            "<h3>Conditions</h3>"
            "<ul>"
            "<li>Disclose source</li>"
            "<li>License and copyright notice</li>"
            "<li>Same license</li>"
            "<li>State changes</li>"
            "</ul>"
            "<h3>Limitations</h3>"
            "<ul>"
            "<li>Liability</li>"
            "<li>Warranty</li>"
            "</ul>"
        );
    }

    m_textBrowser->setHtml(licenseText);
    mainLayout->addWidget(m_textBrowser);

    // Close button
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    m_close = new QPushButton(tr("Close"), this);
    connect(m_close, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(m_close);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

LicenseDialog::~LicenseDialog()
{
}
