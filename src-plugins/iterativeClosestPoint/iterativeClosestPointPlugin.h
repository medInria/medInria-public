/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medPlugin.h>
#include "iterativeClosestPointPluginExport.h"

class ITERATIVECLOSESTPOINTPLUGIN_EXPORT iterativeClosestPointPlugin : public medPlugin
{
    Q_OBJECT
    
public:
    virtual bool initialize();
    
    virtual QString name() const;
    virtual QString identifier() const;
    virtual QString version() const;
    virtual QStringList types() const;
};


