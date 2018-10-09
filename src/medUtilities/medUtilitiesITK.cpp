#include "medUtilitiesITK.h"

#include <statsROI.h>

double medUtilitiesITK::minimumValue(dtkSmartPointer<medAbstractData> data)
{
    statsROI statsProcess;
    statsProcess.setInput(data, 0);
    statsProcess.setParameter(statsROI::MINMAX);
    statsProcess.update();
    return statsProcess.output().at(0);
}

double medUtilitiesITK::maximumValue(dtkSmartPointer<medAbstractData> data)
{
    statsROI statsProcess;
    statsProcess.setInput(data, 0);
    statsProcess.setParameter(statsROI::MINMAX);
    statsProcess.update();
    return statsProcess.output().at(1);
}

double medUtilitiesITK::volume(dtkSmartPointer<medAbstractData> data)
{
    statsROI statsProcess;
    statsProcess.setInput(data, 0);
    statsProcess.setParameter(statsROI::VOLUMEML);
    statsProcess.update();
    return statsProcess.output().at(0);
}
