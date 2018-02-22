/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractWorkspace.h>

#include <%1PluginExport.h>

class %1Private;
    
class %2PLUGIN_EXPORT %1 : public medAbstractWorkspace
{
    Q_OBJECT
    MED_WORKSPACE_INTERFACE("TODO Title",
                            "TODO Description")
    
public:
    %1(QWidget *parent = 0);
    virtual ~%1();
    
    virtual void setupViewContainerStack();

    static bool isUsable();

    static bool registered();

    
private:
    %1Private *d;
};
