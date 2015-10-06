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


#include <medAbstractClosingImageProcess.h>

#include <medComposerExport.h>

class medClosingImageProcessNodePrivate;

class MEDCOMPOSER_EXPORT medClosingImageProcessNode : public dtkComposerNodeObject<medAbstractClosingImageProcess>
{
public:
     medClosingImageProcessNode(void);
    ~medClosingImageProcessNode(void);

public:
    void run(void);

private:
    medClosingImageProcessNodePrivate *d;
};


