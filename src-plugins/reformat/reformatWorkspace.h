/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractWorkspace.h>

#include "reformatPluginExport.h"

class reformatWorkspacePrivate;
    
class REFORMATPLUGIN_EXPORT reformatWorkspace : public medAbstractWorkspace
{
    Q_OBJECT
    MED_WORKSPACE_INTERFACE("Reformat",
                            "Reformat workspace.",
                            "Methodology")
    
public:
    reformatWorkspace(QWidget *parent = 0);
    virtual ~reformatWorkspace();
    
    virtual void setupViewContainerStack();

    static bool isUsable();
    static bool registered();

    void showViewPropertiesToolBox(bool val);
        
private:
    reformatWorkspacePrivate *d;
};
