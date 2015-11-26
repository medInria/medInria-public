/*=========================================================================

 Plugin medInria - Super Resolution

 Copyright (c) IHU LIRYC 2013. All rights reserved.

 Author : Mathilde Merle -- mathilde.merle @ ihu-liryc.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medSegmentationAbstractToolBox.h>
//#include <medFilteringAbstractToolBox.h>
#include <reformatPluginExport.h>
#include "superResolutionProcess.h"
#include <medDataManager.h>
#include <medAbstractWorkspace.h>

class superResolutionToolBoxPrivate;

class REFORMATPLUGIN_EXPORT superResolutionToolBox : public medSegmentationAbstractToolBox
{
    Q_OBJECT;
    MED_TOOLBOX_INTERFACE("superResolutionToolBox","shape based interpolation algorithm", << "reformat");

public:
    superResolutionToolBox(QWidget *parentToolBox = 0);
    ~superResolutionToolBox();
    static bool registered();
    dtkPlugin* plugin();
    void setWorkspace(medAbstractWorkspace * workspace);

    void onRoiImportedDo(const medDataIndex& index, int inputNumber);
    medAbstractData* processOutput();

public slots:
    void enableRunButton(const bool enable);

    void runProcessRequest();

protected slots:

    /**
     * Clears the toolbox. Removes any bundle in the fiber bundle treeview,
     * any ROI previously loaded (if any), etc.
     */
    virtual void onClearRoiButtonClicked();

    /** Slot called when external ROI image finishes being imported. */
    virtual void onVol0Imported(const medDataIndex &index);
    virtual void onVol1Imported(const medDataIndex &index);
    virtual void onVol2Imported(const medDataIndex &index);
    virtual void onVol3Imported(const medDataIndex &index);

    /**
     * Sets the image passed as parameter as the @medDropSite image.
     */
    void setImage0(const QImage& thumbnail);
    void setImage1(const QImage& thumbnail);
    void setImage2(const QImage& thumbnail);
    void setImage3(const QImage& thumbnail);

    /** Open the process output using the current view container of the workspace*/
    void setOutput();


private:
    superResolutionToolBoxPrivate *d;
};
