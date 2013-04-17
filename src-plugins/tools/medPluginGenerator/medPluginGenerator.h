/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

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


