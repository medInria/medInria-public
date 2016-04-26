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

#include <medAbstractTractographyProcess.h>

#include <medComposerExport.h>

class medTractographyProcessNodePrivate;

class MEDCOMPOSER_EXPORT medTractographyProcessNode : public dtkComposerNodeObject <medAbstractTractographyProcess>
{
public:
     medTractographyProcessNode();
    ~medTractographyProcessNode();

public:
    void run();

private:
    const QScopedPointer<medTractographyProcessNodePrivate> d;
};
