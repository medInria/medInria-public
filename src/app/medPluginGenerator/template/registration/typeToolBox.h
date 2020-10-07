/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medRegistrationAbstractToolBox.h>
#include <%1PluginExport.h>

class %1ToolBoxPrivate;

class %2PLUGIN_EXPORT %1ToolBox : public medRegistrationAbstractToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("%1",
                          "Description",
                          << "registration")
    
public:
    %1ToolBox(QWidget *parent = 0);
    ~%1ToolBox();
    
public:
    static bool registered();
    
public slots:
    void run();
    
private:
    %1ToolBoxPrivate *d;
};


