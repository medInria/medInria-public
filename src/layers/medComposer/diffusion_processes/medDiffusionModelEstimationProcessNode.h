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

#include <medAbstractDiffusionModelEstimationProcess.h>

#include <medComposerExport.h>

class medDiffusionModelEstimationProcessNodePrivate;

class MEDCOMPOSER_EXPORT medDiffusionModelEstimationProcessNode : public dtkComposerNodeObject <medAbstractDiffusionModelEstimationProcess>
{
public:
     medDiffusionModelEstimationProcessNode();
    ~medDiffusionModelEstimationProcessNode();

public:
    void run();

private:
    const QScopedPointer<medDiffusionModelEstimationProcessNodePrivate> d;
};
