/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medRegistrationAbstractToolBox.h>
#include "iterativeClosestPointPluginExport.h"

class iterativeClosestPointToolBoxPrivate;

class ITERATIVECLOSESTPOINTPLUGIN_EXPORT iterativeClosestPointToolBox : public medRegistrationAbstractToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("iterativeClosestPointToolBox","IterativeClosestPoint (Mesh)",<<"mesh")
    
public:
    iterativeClosestPointToolBox(QWidget *parent = 0);
    ~iterativeClosestPointToolBox();
    
public:
    static bool registered();
    //void setWorkspace(medAbstractWorkspace*);
    void addLayer(unsigned int layer);
    
public slots:
    void run();
    virtual void updateView();
    void resetComboBoxes();
    
private:
    iterativeClosestPointToolBoxPrivate *d;
};


