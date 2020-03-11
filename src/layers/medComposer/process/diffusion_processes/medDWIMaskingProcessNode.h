#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkComposer>

#include <medAbstractDWIMaskingProcess.h>

#include <medComposerExport.h>

class medDWIMaskingProcessNodePrivate;

class MEDCOMPOSER_EXPORT medDWIMaskingProcessNode : public dtkComposerNodeObject <medAbstractDWIMaskingProcess>
{
public:
     medDWIMaskingProcessNode();
    ~medDWIMaskingProcessNode();

public:
    void run();
    virtual QWidget* editor();

private:
    const QScopedPointer<medDWIMaskingProcessNodePrivate> d;
};
