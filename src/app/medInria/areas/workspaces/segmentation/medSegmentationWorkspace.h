/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractWorkspaceLegacy.h>

class medAbstractView;
class medViewEventFilter;

class medSegmentationWorkspacePrivate;
class medSegmentationSelectorToolBox;

/**
 * class medSegmentationWorkspace
 * Defines the segmentation workspace.
 */
class medSegmentationWorkspace : public medAbstractWorkspaceLegacy
{
    Q_OBJECT
    MED_WORKSPACE_INTERFACE("Segmentation",
                            "Workspace to extract structures from an image using segmentation.",
                            "Methodology")
public:
    medSegmentationWorkspace(QWidget *parent = nullptr);

    virtual ~medSegmentationWorkspace();

    static bool isUsable();

    //! Implement abstract method in base class.
    void setupTabbedViewContainer();

    medSegmentationSelectorToolBox *selectorToolBox();

private:
    medSegmentationWorkspacePrivate *d;
};


