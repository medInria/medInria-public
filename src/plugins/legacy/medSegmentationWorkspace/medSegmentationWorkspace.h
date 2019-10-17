/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medSegmentationWorkspacePluginExport.h>
#include <medSelectorWorkspace.h>

/**
 * class medSegmentationWorkspace
 * Defines the segmentation workspace.
 */
class MEDSEGMENTATIONWORKSPACEPLUGIN_EXPORT medSegmentationWorkspace : public medSelectorWorkspace
{
    Q_OBJECT
    MED_WORKSPACE_INTERFACE("Segmentation",
                            "Workspace to extract structures from an image using segmentation.",
                            "Methodology")
public:
    medSegmentationWorkspace(QWidget *parent);

    static bool isUsable();
    static bool registered();
};
