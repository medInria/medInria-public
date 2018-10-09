#pragma once

#include <dtkCore/dtkSmartPointer>
#include <medUtilities.h>
#include <medUtilitiesExport.h>

class medAbstractData;

class MEDUTILITIES_EXPORT medUtilitiesITK
{
public:

    /**
     * @brief minimumValue computes the minimum pixel intensity in a volume
     * @param the input volume
     * @return the minimum intensity value (double)
     */
    static double minimumValue(dtkSmartPointer<medAbstractData> data);

    /**
     * @brief maximumValue computes the maximum pixel intensity in a volume
     * @param the input volume
     * @return the maximum intensity value (double)
     */
    static double maximumValue(dtkSmartPointer<medAbstractData> data);

    static double volume(dtkSmartPointer<medAbstractData> data);
};
