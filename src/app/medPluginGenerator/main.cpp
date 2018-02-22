/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QApplication>

#include <iostream>

#include <medPluginGeneratorMainWindow.h>
#include <medPluginGenerator.h>

void printUsage() {
    std::cout <<
    QString("Usage: %1 --help | --console --output path --name name --type typeName\n").arg(qApp->arguments()[0]).toStdString() <<
    "--help                  Displays this message.\n"
    "--console               Run the console version.\n"
    "--output [path]         Output directory for the plugin skeleton.\n"
    "--name [name]           Name to use for the plugin.\n"
    "--type [typeName]       Type to use for the plugin.\n"
    "--quiet                 Process quietly (non gui generation only).\n";
}


QString getArgValue(QString arg)
{
    QStringList args = qApp->arguments();
    for(int i = 1; i < args.size() - 1; ++i) {
        if (args.at(i) == arg) {
            return args.at(i + 1);
        }
    }
    return QString();
}


int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    if (app.arguments().contains("--help")) {
        printUsage();
        return 0;
    }

    if(app.arguments().contains("--console")) {
        QString output = getArgValue("--output");
        QString type = getArgValue("--type");
        QString name = getArgValue("--name");

        bool paramsMissing = output.isEmpty() || type.isEmpty() || name.isEmpty();

        if( paramsMissing ) {
            printUsage();
            return 1;
        }

        if( ! app.arguments().contains("--quiet")) {
            qDebug() << "output = " << output;
            qDebug() << "name = " << name;
            qDebug() << "type = " << type;
        }

        medPluginGenerator generator;
        generator.setOutputDirectory(output);
        generator.setName(name);
        generator.setType((pluginType)type.toInt());

        bool resultGenerator = generator.run();

        if( ! app.arguments().contains("--quiet")) {
            if(resultGenerator) {
                qDebug() << "Generation succeeded.";
            } else {
                qDebug() << "Plugin generation: Generation failed.";
            }
        }

        ::exit(resultGenerator ? 0 : 1);

    } else {
        medPluginGeneratorMainWindow generator;
        generator.show();
        return app.exec();

    }
}
