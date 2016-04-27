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

#include "manualRegistrationPluginExport.h"
#include "medToolBox.h"

#include <medDataManager.h>
#include <medAbstractDataFactory.h>



class manualRegistrationToolBoxPrivate;

class MANUALREGISTRATIONPLUGIN_EXPORT manualRegistrationToolBox : public medRegistrationAbstractToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("manualRegistrationToolBox","register two images manually",<<"registration")

public:
    manualRegistrationToolBox(QWidget *parent = 0);
    ~manualRegistrationToolBox();
    
    medAbstractData* getOutput();
    
    static bool registered();
    dtkPlugin * plugin();

    void updateLabels(int left,int right);

protected slots:
    void updateView();    
    void update(medAbstractData *){}
    void synchroniseMovingFuseView();
    void save();
    void exportTransformation();

public slots:
    void startManualRegistration();
    void stopManualRegistration();
    void computeRegistration();
    void reset();

private:
    void displayButtons(bool);
    void constructContainers(medTabbedViewContainers *);
    void disableSaveButtons(bool);
    manualRegistrationToolBoxPrivate *d;
};


