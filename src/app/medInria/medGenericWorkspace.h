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
#include <medAbstractWorkspaceLegacy.h>

class medGenericWorkspacePrivate;

/**
 * @brief Workspace providing a generic framework to test all process plugins
 */
class medGenericWorkspace : public medAbstractWorkspaceLegacy
{
    Q_OBJECT
    MED_WORKSPACE_INTERFACE("Generic",
                            "Generic workspace.",
                            "Generic")
public:
    medGenericWorkspace(QWidget *parent = 0);
    ~medGenericWorkspace();

    static bool isUsable();
    void setupTabbedViewContainer ();

    enum ProcessTypes
    {
        Generic
    };

public slots:
    void setProcessType(int index);
    void setProcessSelection(int index);

private:
    medGenericWorkspacePrivate *d;
};
