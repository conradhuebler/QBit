/*
 * <QBit lets go.>
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

#include "gui/qbit.h"

#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>
#include <QtCore/QDir>

#include <QtWidgets/QApplication>



void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtInfoMsg:
        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        if(context.line != 0)
            fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        else
            fprintf(stderr, "Warning: %s \n", localMsg.constData());
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
}


int main(int argc, char** argv)
{
    
    qInstallMessageHandler(myMessageOutput);
    
    QApplication app(argc, argv);
    
    app.setApplicationName("QBit");
    app.setApplicationDisplayName("QBit");
    app.setOrganizationName("Conrad Huebler");
    
//     app.setApplicationVersion(version);
    
    QCommandLineParser parser;
    parser.setApplicationDescription ( "A Open Source Qt5 based NMR Spectra Software, based on libpeakpick." );
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("input file", QCoreApplication::translate("main", "File to open."));
    parser.process(app);

    QStringList args = parser.positionalArguments();
    for(int i = 0; i < args.size(); ++i)
    {
        args[i] = QDir::currentPath() + QDir::separator() + args[i];
    }
#ifdef _DEBUG
        qDebug() << "Debug output enabled, good fun!";
#endif

    QBit qbit;
    qbit.show();
    qbit.LoadFiles(args);
    
    return app.exec();
}
