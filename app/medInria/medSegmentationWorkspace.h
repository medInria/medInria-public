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


class medAbstractView;
class medViewEventFilter;

class medSegmentationWorkspacePrivate;
class medSegmentationSelectorToolBox;

/**
 * class medSegmentationWorkspace
 * Defines the segmentation workspace.
 */
class medSegmentationWorkspace : public medAbstractWorkspace
{
    Q_OBJECT
    MED_WORKSPACE_INTERFACE("Segmentation",
                            "Segmentation workspace.")
public:
    medSegmentationWorkspace(QWidget * parent = NULL);

    virtual ~medSegmentationWorkspace();

    static bool isUsable();

    //! Implement abstract method in base class.
    void setupTabbedViewContainer();

    medSegmentationSelectorToolBox * segmentationToobox();

protected slots:
    void addViewEventFilter(medViewEventFilter * filter );

    void onSuccess();


private:
    medSegmentationWorkspacePrivate *d;
};


