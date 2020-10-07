/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractSelectableToolBox.h>
#include "LCCLogDemonsPluginExport.h"

class LCCLogDemonsToolBoxPrivate;

class LCCLogDemonsPLUGIN_EXPORT LCCLogDemonsToolBox : public medAbstractSelectableToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("LCC Log Demons",
                          "LCC Log Demons ToolBox",
                          << "Registration")
    
public:
    LCCLogDemonsToolBox(QWidget *parent = 0);
    ~LCCLogDemonsToolBox();

	dtkPlugin * plugin() override;
	medAbstractData * processOutput() override;

    static bool registered();
    
public slots:
    void chooseUpdateRule(int);
    void run();
    
private:
    LCCLogDemonsToolBoxPrivate *d;
};


