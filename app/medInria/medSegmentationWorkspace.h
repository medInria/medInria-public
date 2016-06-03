/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractWorkspace.h>

class medSegmentationWorkspacePrivate;
class medSelectorToolBox;

/**
 * class medSegmentationWorkspace
 * Defines the segmentation workspace.
 */
class medSegmentationWorkspace : public medAbstractWorkspace
{
    Q_OBJECT
    MED_WORKSPACE_INTERFACE("Segmentation",
                            "Workspace to extract structures from an image using segmentation.",
                            "Methodology")
public:
    medSegmentationWorkspace(QWidget * parent);

    virtual ~medSegmentationWorkspace();

    static bool isUsable();

    medSelectorToolBox *segmentationToobox();

protected slots:
    void onSuccess();

private:
    medSegmentationWorkspacePrivate *d;
};


