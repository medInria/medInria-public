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

#include <medAbstractSingleFilterOperationDoubleProcess.h>
#include <medComposerExport.h>
#include <medSingleFilterOperationProcessNode.h>

class medSingleFilterDoubleOperationProcessNodePrivate;

template <typename T>
class MEDCOMPOSER_EXPORT medSingleFilterDoubleOperationProcessNode : public medSingleFilterOperationProcessNode<T>
{
public:
     medSingleFilterDoubleOperationProcessNode(void);
    ~medSingleFilterDoubleOperationProcessNode(void);

public:
    virtual bool prepareInput(void);
private:
    const QScopedPointer<medSingleFilterDoubleOperationProcessNodePrivate> d;
};

#include <medSingleFilterDoubleOperationProcessNode.tpp>
