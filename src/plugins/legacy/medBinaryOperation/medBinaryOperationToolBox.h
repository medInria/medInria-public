/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medBinaryOperationPluginExport.h"

#include <medDataIndex.h>
#include <medAbstractSelectableToolBox.h>

class medBinaryOperationToolBoxPrivate;

/*!
 * \brief Toolbox to compute binary operations between masks (OR, AND, XOR, etc.).
 */
class MEDBINARYOPERATIONPLUGIN_EXPORT medBinaryOperationToolBox : public medAbstractSelectableToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("Binary Operation",
                          "Compute binary operations between masks (OR, AND, XOR, etc.).",
                          <<"Filtering")
    
public:
    medBinaryOperationToolBox(QWidget *parent = nullptr);
    ~medBinaryOperationToolBox();
    
    medAbstractData *processOutput();
    
    static bool registered();
    dtkPlugin * plugin();
  
signals:
    void success();
    void failure();
    
public slots:
    void run();

protected slots:
    void update (medAbstractData*){}

    /** Slot called when external input finishes being imported. */
    virtual void onSecondInputImported(const medDataIndex &index);

    /**
     * Slot called when the @meDropSite is clicked.
     * Will open a @QFileDialog so the user can choose
     * and external ROI image to open.
     */
    virtual void onDropSiteClicked();

    virtual void clearDropsite();

    void onXorButtonToggled(bool value);
    void onAndButtonToggled(bool value);
    void onOrButtonToggled(bool value);
    void onNotButtonToggled(bool value);
    
private:
    medBinaryOperationToolBoxPrivate *d;
};


