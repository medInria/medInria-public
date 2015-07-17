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

#include <medAbstractSubtractImageProcessPresenter.h>


class medSubtractImageProcessNodePrivate;

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

class medSubtractImageProcessNode : public dtkComposerNodeObject<medAbstractSubtractImageProcessPresenter>
{
public:
     medSubtractImageProcessNode(void);
    ~medSubtractImageProcessNode(void);

public:
    void run(void);

private:
    medSubtractImageProcessNodePrivate *d;
};
