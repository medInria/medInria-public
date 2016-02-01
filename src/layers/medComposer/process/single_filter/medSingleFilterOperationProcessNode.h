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


#include <medAbstractSingleFilterOperationProcess.h>

#include <medComposerExport.h>

class medSingleFilterOperationProcessNodePrivate;

template <typename T>
class MEDCOMPOSER_EXPORT medSingleFilterOperationProcessNode : public dtkComposerNodeObject<T>
{
public:
     medSingleFilterOperationProcessNode(void);
    ~medSingleFilterOperationProcessNode(void);

public:
    void run(void);

private:
    const QScopedPointer<medSingleFilterOperationProcessNodePrivate> d;
};

#include <medSingleFilterOperationProcessNode.tpp>
