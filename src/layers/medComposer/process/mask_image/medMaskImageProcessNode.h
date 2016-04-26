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

#include <medAbstractMaskImageProcess.h>

#include <medComposerExport.h>

class medMaskImageProcessNodePrivate;

class MEDCOMPOSER_EXPORT medMaskImageProcessNode : public dtkComposerNodeObject <medAbstractMaskImageProcess>
{
public:
     medMaskImageProcessNode();
    ~medMaskImageProcessNode();

public:
    void run();

private:
    const QScopedPointer<medMaskImageProcessNodePrivate> d;
};
