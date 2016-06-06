#pragma once

#include <QObject>
#include <medCoreExport.h>
#include <dtkCore>

class dtkImage;
class medAbstractImageData;

class MEDCORE_EXPORT medAbstractDataConverter : public QObject
{
    Q_OBJECT
    public:
        virtual dtkImage* toDtkImage(medAbstractImageData* medImage) = 0;
        virtual medAbstractImageData* toMedImage(dtkImage* image) = 0;
};

DTK_DECLARE_PLUGIN        (medAbstractDataConverter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractDataConverter, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractDataConverter, MEDCORE_EXPORT)
