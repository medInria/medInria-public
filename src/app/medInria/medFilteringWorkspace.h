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

    enum ProcessTypes
    {
        MorphoMath = 1,
        SingleFilter,
        Arithmetic
    };

public slots:
    void setProcessType(int index);
    void setProcessSelection(int index);

private:
    medFilteringWorkspacePrivate *d;
};


