/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QApplication>

#include <iostream>

#include "medPluginGeneratorMainWindow.h"
#include "medPluginGenerator.h"

void printUsage() {
    std::cout <<
    QString("Usage: %1 --help | --console [--output path --name name --type typeName\n").arg(qApp->argv()[0]).toStdString() <<
    "--help                Displays this message.\n"
    "--console             Run the console version.\n"
    "--output [path]       Output directory for the plugin skeleton.\n"
    "--name [name] name to use for the plugin.\n"
    "--type [typeName]     Type to use for the plugin.\n"
    "--quiet               Process quietly (non gui generation only).\n";
}


QString getArgValue(QString arg)
{
    QStringList args = qApp->arguments();
    QString value;
    for(int i = 0; i < args.size() - 1; ++i) {
        if (args.at(i) == arg) {
            value = args.at(i + 1);
        }
    }
    return value;
}


int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    if (app.arguments().contains("--help")) {
        printUsage();
        return 0;
    }

    if(app.arguments().contains("--console")) {
        QString output = getArgValue("output");
        QString type = getArgValue("type");
        QString name = getArgValue("name");
        bool paramsMissing = output.isNull() || type.isNull() || name.isNull();

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
    generator.setType(type);

	bool resultGenerator = generator.run();

    if( ! app.arguments().contains("--quiet")) {
            if(resultGenerator)
                qDebug() << "Generation succeeded.";
            else
                qDebug() << "Plugin generation: Generation failed.";
	}

    } else {
        medPluginGeneratorMainWindow generator;
        generator.show();
        return app.exec();

    }
}
