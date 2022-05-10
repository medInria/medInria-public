#pragma once
/*
 * medInria
 * Copyright (c) INRIA 2013. All rights reserved.
 *
 * medInria is under BSD-2-Clause license. See LICENSE.txt for details in the root of the sources or:
 * https://github.com/medInria/medInria-public/blob/master/LICENSE.txt
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "medUtilitiesExport.h"

#include <dtkCoreSupport/dtkSmartPointer.h>

#include <medAbstractData.h>

class MEDUTILITIES_EXPORT imageFilters
{
public:
    enum filtersParameter
    {
        BINARIZE
    };

    imageFilters();

    void setInput(medAbstractData *data);

    void setParameter(int method);

    bool update();

    dtkSmartPointer<medAbstractData> dataOutput();

    template <class ImageType>
    bool runProcess();

    template <class ImageType>
    bool runBinarize();

private:
    dtkSmartPointer<medAbstractData> input0;
    int chosenMethod;
    dtkSmartPointer<medAbstractData> computedDataOutput;
};
