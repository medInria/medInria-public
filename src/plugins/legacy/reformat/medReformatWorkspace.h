/*=========================================================================

 medInria

 Copyright (c) INRIA 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medSelectorWorkspace.h>
#include <reformatPluginExport.h>
    
class REFORMATPLUGIN_EXPORT medReformatWorkspace : public medSelectorWorkspace
{
    Q_OBJECT
    MED_WORKSPACE_INTERFACE("Reformat",
                            "Reformat workspace.",
                            "Methodology")
    
public:
    medReformatWorkspace(QWidget *parent);

    static bool isUsable();
    static bool registered();
};
