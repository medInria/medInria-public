/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore>

class medPluginGeneratorPrivate;

enum pluginType
{
    DATA = 1,

    VIEW = 3,
    LAYERED_VIEW,
    IMAGE_VIEW,

    VIEW_INTERACTOR = 7,
    LAYERED_INTERACTOR,
    IMAGE_INTERACTOR,
    EXTRA_INTERACTOR,

    VIEW_NAVIGATOR = 12,
    LAYERED_NAVIGATOR,
    IMAGE_NAVIGATOR,
    EXTRA_NAVIGATOR,

    FILTERING = 17,
    REGISTRATION,
    DIFFUSION,

    DATA_READER = 21,
    DATA_WRITER,

    WORKSPACE = 24
};

class medPluginGenerator
{
public:
    medPluginGenerator();
   ~medPluginGenerator();

   void setOutputDirectory(const QString& directory);
   void setName(const QString& suffix);
   void setType(pluginType type);
   void setDescription(const QString& desc);
   void setLicense(const QString& desc);
   void setGenerateClassOnly(bool);
   bool run();

protected:
   bool generateCMakeLists();
   bool generateFile(QString targetFilename, QString templateFilename);
   bool generateReadmeFile();

private:
   medPluginGeneratorPrivate *d;
};


