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


#include <medAbstractDilateImageProcess.h>

#include <medComposerExport.h>

class medDilateImageProcessNodePrivate;

class MEDCOMPOSER_EXPORT medDilateImageProcessNode : public dtkComposerNodeObject<medAbstractDilateImageProcess>
{
public:
     medDilateImageProcessNode(void);
    ~medDilateImageProcessNode(void);

public:
    void run(void);

private:
    medDilateImageProcessNodePrivate *d;
};
