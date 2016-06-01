#pragma once

#include <medCoreExport.h>

class dtkImage;
class medAbstractImageData;

class MEDCORE_EXPORT medAbstractConverter : public QObject
{
    Q_OBJECT
    public:
        virtual dtkImage* toDtkImage(medAbstractImageData* medImage) = 0;
        virtual medAbstractImageData* toMedImage(dtkImage* image) = 0;
};


DTK_DECLARE_PLUGIN        (medAbstractConverter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractConverter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractConverter, MEDCORE_EXPORT)
