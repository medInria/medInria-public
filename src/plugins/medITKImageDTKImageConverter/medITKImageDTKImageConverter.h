#pragma once

#include <dtkCore>
#include <QList>
#include <QString>
#include <QDomDocument>

#include <medCore.h>

class medAbstractDataConverterPrivate;

class medITKImageDTKImageConverter : public medAbstractDataConverter
{
    Q_OBJECT

public:
    dtkImage* toDtkImage(medAbstractImageData* medImage);
    medAbstractImageData* toMedImage(dtkImage* image);

private:
    template <typename T, int dim> dtkImage* toDtkImage_helper(medAbstractImageData* medImage);
};

inline medAbstractDataConverter *medITKImageDTKImageConverterCreator(void)
{
    return new medITKImageDTKImageConverter();
}
