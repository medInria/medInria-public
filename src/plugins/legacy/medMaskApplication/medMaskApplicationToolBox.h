/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medMaskApplicationPluginExport.h"

#include <medAbstractSelectableToolBox.h>

class medMaskApplicationToolBoxPrivate;

/*!
 * \brief Toolbox to apply a mask to an image.
 */
class MEDMASKAPPLICATIONPLUGIN_EXPORT medMaskApplicationToolBox : public medAbstractSelectableToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("Mask Application",
                          "Apply a mask to an image.",
                          <<"Filtering Legacy")

public:
    medMaskApplicationToolBox(QWidget *parent = nullptr);
    ~medMaskApplicationToolBox();

    medAbstractData *processOutput() override;
    
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


