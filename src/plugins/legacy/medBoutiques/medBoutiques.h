/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCoreSupport/dtkAbstractProcess.h>
#include <dtkCoreSupport/dtkAbstractData.h>

#include <medBoutiquesPluginExport.h>

// medBoutiques is a plugin to run Boutiques tools in medInria

class MEDBOUTIQUESPLUGIN_EXPORT medBoutiques : public dtkAbstractProcess
{
    Q_OBJECT

public:
             medBoutiques() = default;
    virtual ~medBoutiques() = default;

    virtual QString description() const;

    static bool registered();

};

dtkAbstractProcess *createmedBoutiques();


