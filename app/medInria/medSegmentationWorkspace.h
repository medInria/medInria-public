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

public:
    medSegmentationWorkspace(QWidget * parent = NULL);

    virtual ~medSegmentationWorkspace();

    //! Overrides base class.
    QString description() const;
    //! Implement base class
    QString identifier() const;

    static bool isUsable();

    //! Implement abstract method in base class.
    void setupViewContainerStack();

    medSegmentationSelectorToolBox * segmentationToobox();

protected slots:
    void addViewEventFilter(medViewEventFilter * filter );

    void onSuccess();


private:
    static QString s_identifier();
    medSegmentationWorkspacePrivate *d;
};


