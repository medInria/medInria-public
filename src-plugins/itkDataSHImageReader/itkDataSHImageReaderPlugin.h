/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkPlugin.h>
#include <itkDataSHImageReaderPluginExport.h>

class ITKDATASHIMAGEREADERPLUGIN_EXPORT itkDataSHImageReaderPluginPrivate;

class ITKDATASHIMAGEREADERPLUGIN_EXPORT itkDataSHImageReaderPlugin : public dtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
     itkDataSHImageReaderPlugin(QObject *parent = 0);
    ~itkDataSHImageReaderPlugin();

    virtual bool initialize();
    virtual bool uninitialize();

    virtual QString name() const;
    virtual QString description() const;

    virtual QStringList tags() const;
    virtual QStringList types() const;

    virtual QString version() const;
    virtual QString contact(void) const;
    virtual QStringList authors() const;
    virtual QStringList contributors() const;

private:
     itkDataSHImageReaderPluginPrivate *d;
};
