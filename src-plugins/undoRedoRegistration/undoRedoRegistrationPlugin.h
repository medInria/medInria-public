/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medPlugin.h>
#include <undoRedoRegistrationPluginExport.h>

class UNDOREDOREGISTRATIONPLUGIN_EXPORT undoRedoRegistrationPlugin : public medPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
    undoRedoRegistrationPlugin(QObject *parent = 0);
    virtual bool initialize(void);
    
    virtual QString name(void) const;
    virtual QString version (void) const;
    virtual QStringList types(void) const;
};
