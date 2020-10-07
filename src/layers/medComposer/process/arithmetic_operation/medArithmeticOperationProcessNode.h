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


#include <medAbstractArithmeticOperationProcess.h>

#include <medComposerExport.h>

class medArithmeticOperationProcessNodePrivate;

template <typename T>
class MEDCOMPOSER_EXPORT medArithmeticOperationProcessNode : public dtkComposerNodeObject<T>
{
public:
     medArithmeticOperationProcessNode(void);
    ~medArithmeticOperationProcessNode(void);

public:
    void run(void);

private:
    const QScopedPointer<medArithmeticOperationProcessNodePrivate> d;
};

#include <medArithmeticOperationProcessNode.tpp>
