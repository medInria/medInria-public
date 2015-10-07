/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkPlugin.h>

#include <medCompositeDataSetsPluginExport.h>
#include <medCompositeDataSetsBase.h>

class MEDCOMPOSITEDATASETSPLUGIN_EXPORT medCompositeDataSetsPlugin : public dtkPlugin {

    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:

     medCompositeDataSetsPlugin(QObject *parent = 0);
    ~medCompositeDataSetsPlugin();

    virtual bool initialize();
    virtual bool uninitialize();

    virtual QString name() const;
    virtual QString description() const;

    virtual QStringList tags() const;
    virtual QStringList types() const;

private:
    
    QStringList registered_types;
};


