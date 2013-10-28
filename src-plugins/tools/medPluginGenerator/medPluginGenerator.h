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
    medPluginGenerator();
   ~medPluginGenerator();

   void setOutputDirectory(const QString& directory);
   void setName(const QString& suffix);
   void setType(const QString& type);
   void setDescription(const QString& desc);
   void setLicense(const QString& desc);
   void setPluginFamily(const QString &family);
   bool run();

   static QStringList pluginFamilies();
   static QStringList pluginTypes();

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
   bool generateReadmeFile();


private:
   medPluginGeneratorPrivate *d;
};


