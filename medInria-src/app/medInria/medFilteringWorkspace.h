/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore>
#include <medAbstractWorkspaceLegacy.h>

class medFilteringWorkspacePrivate;
class medViewContainerStack;
class medAbstractData;
class dtkAbstractView;

/**
 * @brief Workspace providing a comparative display of the input and output of image-to-image filtering process plugins
 */
class medFilteringWorkspace : public medAbstractWorkspaceLegacy
{
    Q_OBJECT
    MED_WORKSPACE_INTERFACE("Filtering",
                            "Workspace to apply filters to images.",
                            "Methodology")
public:
    medFilteringWorkspace(QWidget *parent = 0);
    ~medFilteringWorkspace();

    static bool isUsable();

    void setupTabbedViewContainer ();

    virtual void open(const medDataIndex &index);

signals:

    /**
     * @brief signal emitted to refresh the output view with the data resulting from a successful filtering process
     *
     * This is a connection between the medFilteringSelectorToolBox and the medFilteringViewContainer which displays input/output images
     *
     */
    void outputDataChanged ( medAbstractData * );

protected slots:

    void changeToolBoxInput();
    void onProcessSuccess();

private:
    medFilteringWorkspacePrivate *d;
};


