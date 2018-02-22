/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkPlugin.h>

#include <exampleProcessImageFilterPluginExport.h>

class EXAMPLEPROCESSIMAGEFILTERPLUGIN_EXPORT exampleProcessImageFilterPluginPrivate;

/**
* @class exampleProcessImageFilterPlugin
* @brief A simple example process plugin, the input are 2 UNSIGNED SHORT 3D images that can be added, masked. Region growing can also be applyed to the first image.
* It gets two 3D unsigned short images trough the medDiffusionAbstractToolBox and exampleProcessImageFilterToolBox.h drop sites,
* three operations can be done to thos images:
* 1 MASK the first image with the second one
* 2 ADD the two images
* 3 APPLY CONNECTED THRESHOLD to the first image, the three first spin boxes, specify the seed voxel, the fourth specifies
* the lower intensity threshold used, the upper one is +150 teh lower
**/

class EXAMPLEPROCESSIMAGEFILTERPLUGIN_EXPORT exampleProcessImageFilterPlugin : public dtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
     exampleProcessImageFilterPlugin(QObject *parent = 0);
    ~exampleProcessImageFilterPlugin();

    /**
    * Initializes the exampleProcessImageFilterPlugin by registering the exampleProcessImageFilter type and the
    * exampleProcessImageFilter toolbox
    **/
    virtual bool initialize();
    virtual bool uninitialize();

    virtual QString name() const;
    virtual QString description() const;

    virtual QStringList tags() const;
    virtual QStringList types() const;

private:
     exampleProcessImageFilterPluginPrivate *d;
};


