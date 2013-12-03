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

#include <itkDataSHImageWriterPluginExport.h>

class ITKDATASHIMAGEWRITERPLUGIN_EXPORT itkDataSHImageWriterPluginPrivate;

class ITKDATASHIMAGEWRITERPLUGIN_EXPORT itkDataSHImageWriterPlugin : public dtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
     itkDataSHImageWriterPlugin(QObject *parent = 0);
    ~itkDataSHImageWriterPlugin();

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
     itkDataSHImageWriterPluginPrivate *d;
};
