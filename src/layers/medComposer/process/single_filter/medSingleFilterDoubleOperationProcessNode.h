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


#include <medAbstractSingleFilterOperationDoubleProcess.h>

#include <medSingleFilterOperationProcessNode.h>

#include <medComposerExport.h>

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
