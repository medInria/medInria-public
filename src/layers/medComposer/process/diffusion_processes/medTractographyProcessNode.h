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
    virtual QWidget* editor();

private:
    const QScopedPointer<medTractographyProcessNodePrivate> d;
};
