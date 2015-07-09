/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QWidget>

#include <medAbstractWorkspace.h>

class medTestWorkspacePrivate;


/**
 * @brief Workspace providing a comparative display of the input and output of image-to-image filtering process plugins
 */
class medTestWorkspace : public medAbstractWorkspace
{
    Q_OBJECT
    MED_WORKSPACE_INTERFACE("Test",
                            "Test workspace.")
public:
    medTestWorkspace(QWidget *parent = 0);
    ~medTestWorkspace();

    static bool isUsable();
    void setupTabbedViewContainer ();


private:
    medTestWorkspacePrivate *d;
};


