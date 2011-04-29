/* main.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Jan 30 00:19:02 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Sun Feb  7 16:35:52 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 116
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include <QApplication>

#include "anyoption.h"

#include "medPluginGeneratorMainWindow.h"
#include "medPluginGenerator.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    AnyOption options;
    options.addUsage(QString("Usage: %1 --help | --console | [--output path --prefix prefixName --suffix suffixName --type typeName").arg(argv[0]).toAscii());
    options.addUsage("");
    options.addUsage("--help                Displays this message.");
    options.addUsage("--console             Run the gui version.");
    options.addUsage("--output [path]       Output directory for the plugin skeleton.");
    options.addUsage("--prefix [prefixName] Prefix to use for the plugin.");
    options.addUsage("--suffix [suffixName] Suffix to use for the plugin.");
    options.addUsage("--type [typeName]     Type to use for the plugin.");
    options.addUsage("--quiet               Process quietly (non gui generation only).");

    options.setFlag("help");
    options.setFlag("console");

    options.setOption("output");
    options.setOption("prefix");
    options.setOption("suffix");
    options.setOption("type");
    options.setOption("quiet");

    options.processCommandArgs(argc, argv);

    if(options.getFlag("help")) {
        options.printUsage();
        return 0;
    }

    if(options.getFlag("console")) {

        bool paramsOk = options.getValue("output") && options.getValue("prefix") && options.getValue("type") && options.getValue("suffix");

        if( !paramsOk ) {
            options.printUsage();
            return 1;
        }

        if(!options.getFlag("quiet")) {
            qDebug() << "output = " << options.getValue("output");
            qDebug() << "prefix = " << options.getValue("prefix");
            qDebug() << "suffix = " << options.getValue("suffix");
            qDebug() << "type = " << options.getValue("type");
        }

        medPluginGenerator generator;
	generator.setOutputDirectory(options.getValue("output"));
	generator.setPrefix(options.getValue("prefix"));
	generator.setSuffix(options.getValue("suffix"));
	generator.setType(options.getValue("type"));

	bool resultGenerator = generator.run();

	if(!options.getFlag("quiet")) {
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
