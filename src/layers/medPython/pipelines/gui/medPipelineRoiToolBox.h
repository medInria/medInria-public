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
#include <medAbstractData.h>
#include <medAbstractView.h>
#include <medAbstractWorkspaceLegacy.h>

#include "medPipelineExport.h"

/**
 * Main segmentation toolbox. The user may choose the
 * segmentation method: paintBrush or PolygonRoi
 */

namespace med::pipeline
{

class MEDPIPELINE_EXPORT RoiToolBox : public medAbstractSelectableToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("Choose ROI tool", "Various methods to perform a segmentation", )

public:
    RoiToolBox(QWidget *parent = 0);

    static bool registered();

    medAbstractSelectableToolBox* getCurrentRoiTool();

    medAbstractData* processOutput();

signals:
    void inputChanged();

public slots:
    void updateView();

    void changeCurrentRoiTool(int index);

    /******* Variables *************/
protected:
    // combo box with all the mesh generators
    QComboBox* roiComboBox;

    medAbstractSelectableToolBox* currentRoiTool;

    QHash<QString, medAbstractSelectableToolBox*> roiTools;

    QVBoxLayout* mainLayout;

    medAbstractView* currentView;

private:
    void initializeToolbox();
};

} // namespace med::pipeline
