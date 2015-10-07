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


#include <medAbstractOpeningImageProcess.h>

#include <medComposerExport.h>

class medOpeningImageProcessNodePrivate;

class MEDCOMPOSER_EXPORT medOpeningImageProcessNode : public dtkComposerNodeObject<medAbstractOpeningImageProcess>
{
public:
     medOpeningImageProcessNode(void);
    ~medOpeningImageProcessNode(void);

public:
    void run(void);

private:
    medOpeningImageProcessNodePrivate *d;
};


