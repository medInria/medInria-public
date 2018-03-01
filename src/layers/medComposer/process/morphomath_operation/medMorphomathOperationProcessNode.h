/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include <dtkComposer>


#include <medAbstractMorphomathOperationProcess.h>

#include <medComposerExport.h>

class medMorphomathOperationProcessNodePrivate;

template <typename T>
class MEDCOMPOSER_EXPORT medMorphomathOperationProcessNode : public dtkComposerNodeObject<T>
{
public:
     medMorphomathOperationProcessNode(void);
    ~medMorphomathOperationProcessNode(void);

public:
    void run(void);

private:
    const QScopedPointer<medMorphomathOperationProcessNodePrivate> d;
};

#include <medMorphomathOperationProcessNode.tpp>
