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

#ifndef DTKPLUGINGENERATOR_H
#define DTKPLUGINGENERATOR_H

#include <QtCore>

class medPluginGeneratorPrivate;

class medPluginGenerator
{
public:
    enum PluginFamily {
        GENERIC, /** generic plugin*/
        REGISTRATION /** registration plugin*/
    };
public:
    medPluginGenerator(void);
   ~medPluginGenerator(void);

   void setOutputDirectory(const QString& directory);
   void setNamespace(const QString& prefix);
   void setName(const QString& suffix);
   void setType(const QString& type);
   void setDescription(const QString& desc);
   void setLicense(const QString& desc);
   void setPluginFamily(const PluginFamily family);
   bool run(void);

protected:
   bool generateCMakeLists(void);
   bool generateTypeHeaderFile(void);
   bool generateTypeSourceFile(void);
   bool generateTypeToolBoxHeaderFile(void);
   bool generateTypeToolBoxSourceFile(void);
   bool generatePluginHeaderFile(void);
   bool generatePluginSourceFile(void);
   bool generateExportHeaderFile(void);
   bool generateHelpCollectionFile(void);
   bool generateHelpConfigurationFile(void);

   bool generateReadmeFile(void);
   bool generateCopyingFile(void);

private:
   medPluginGeneratorPrivate *d;
};

#endif
