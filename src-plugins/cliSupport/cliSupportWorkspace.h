/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractWorkspace.h>
#include <cliSupportWorkspacePluginExport.h>

class cliSupportWorkspacePrivate;

class cliSupportWorkspace : public medAbstractWorkspace
{
    Q_OBJECT
    MED_WORKSPACE_INTERFACE("CLI Modules",
                            "CLI Modules support using CLI implementation from CTK.")

public:
     cliSupportWorkspace(QWidget *parent = 0);
    ~cliSupportWorkspace(void);

    virtual void setupViewContainerStack();

    static bool isUsable() {return true;}

private:
    cliSupportWorkspacePrivate *d;
};
