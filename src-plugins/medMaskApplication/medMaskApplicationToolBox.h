/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medFilteringAbstractToolBox.h>

#include "medMaskApplicationPluginExport.h"
#include "medToolBox.h"

#include <medDataManager.h>
#include <medAbstractDataFactory.h>



class medMaskApplicationToolBoxPrivate;

class MEDMASKAPPLICATIONPLUGIN_EXPORT medMaskApplicationToolBox : public medFilteringAbstractToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("medMaskApplicationToolBox","Apply a mask to an image",<<"filtering")

public:
    medMaskApplicationToolBox(QWidget *parent = 0);
    ~medMaskApplicationToolBox();
    
    medAbstractData *processOutput();
    
    static bool registered();
    dtkPlugin * plugin();
    
signals:

    void success();
    void failure();
    
public slots:
    void run();



protected slots:

    void update(medAbstractData *){}

    /**
     * Clears the toolbox. Removes any bundle in the fiber bundle treeview,
     * any ROI previously loaded (if any), etc.
     */
    virtual void clearMask();

    /** Slot called when external ROI image finishes being imported. */
    virtual void importMask(const medDataIndex &index);

private:
    medMaskApplicationToolBoxPrivate *d;
};


