/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <medSelectorWorkspace.h>

/**
 * class medMeshingWorkspace
 * Defines the meshing workspace.
 */
class medMeshingWorkspace : public medSelectorWorkspace
{
    Q_OBJECT
    MED_WORKSPACE_INTERFACE("Meshing",
                            "Providing tool for mesh processing.",
                            "Methodology")

public:
    medMeshingWorkspace(QWidget *parent);

    static bool isUsable();
    static bool registered();
};


