/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medFilteringAbstractToolBox.h>
#include <%1PluginExport.h>

class %1ToolBoxPrivate;

class %2PLUGIN_EXPORT %1ToolBox : public medFilteringAbstractToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("%1",
                          "TODO Description",
                          << "filtering")
    
public:
    %1ToolBox(QWidget *parent = 0);
    ~%1ToolBox();
    
    medAbstractData *processOutput();
    
    static bool registered();
    dtkPlugin * plugin();

    void update(medAbstractData *data);
    
signals:
    void success();
    void failure();
    
public slots:
    void run();
    
private:
    %1ToolBoxPrivate *d;
};


