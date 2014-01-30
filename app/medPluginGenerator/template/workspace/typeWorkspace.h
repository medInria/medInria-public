/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medWorkspace.h>

#include <%1PluginExport.h>

class %1WorkspacePrivate;
    
class %2PLUGIN_EXPORT %1Workspace : public medWorkspace
{
    Q_OBJECT
    
public:
    %1Workspace(QWidget *parent = 0);
    virtual ~%1Workspace();
    
    virtual QString description() const;

    virtual QString identifier() const;

    virtual void setupViewContainerStack();

    static bool isUsable();

    static bool registered();

    
private:
    %1WorkspacePrivate *d;
};
