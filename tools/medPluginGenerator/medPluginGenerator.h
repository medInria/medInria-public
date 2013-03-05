/* medPluginGenerator.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Mar  9 21:40:22 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Apr 26 16:51:11 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 28
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#pragma once

#include <QtCore>

class medPluginGeneratorPrivate;

class medPluginGenerator
{
public:
    enum PluginFamily {
        GENERIC, /** generic plugin*/
        FILTERING, /** filtering plugin*/
        REGISTRATION /** registration plugin*/
    };
public:
    medPluginGenerator();
   ~medPluginGenerator();

   void setOutputDirectory(const QString& directory);
   void setName(const QString& suffix);
   void setType(const QString& type);
   void setDescription(const QString& desc);
   void setLicense(const QString& desc);
   void setPluginFamily(const PluginFamily family);
   bool run();

protected:
   bool generateCMakeLists();
   bool generateTypeHeaderFile();
   bool generateTypeSourceFile();
   bool generateTypeToolBoxHeaderFile();
   bool generateTypeToolBoxSourceFile();
   bool generateRPIHeaderFile();
   bool generateRPISourceFile();
   bool generatePluginHeaderFile();
   bool generatePluginSourceFile();
   bool generateExportHeaderFile();
   bool generateHelpCollectionFile();
   bool generateHelpConfigurationFile();

   bool generateReadmeFile();
   bool generateCopyingFile();

private:
   medPluginGeneratorPrivate *d;
};


