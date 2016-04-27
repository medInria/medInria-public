/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include <dtkComposer>

#include <medAbstractDiffusionScalarMapsProcess.h>

#include <medComposerExport.h>

class medDiffusionScalarMapsProcessNodePrivate;

class MEDCOMPOSER_EXPORT medDiffusionScalarMapsProcessNode : public dtkComposerNodeObject <medAbstractDiffusionScalarMapsProcess>
{
public:
     medDiffusionScalarMapsProcessNode();
    ~medDiffusionScalarMapsProcessNode();

public:
    void run();
    virtual QWidget* editor();

private:
    const QScopedPointer<medDiffusionScalarMapsProcessNodePrivate> d;
};
