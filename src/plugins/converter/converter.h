#pragma once

#include <dtkCore>
#include <QList>
#include <QString>
#include <QDomDocument>

#include <medCore.h>

class converterPrivate;

class converter : public medAbstractConverter
{
    Q_OBJECT

    public:
        dtkImage* toDtkImage(medAbstractImageData* medImage);
        medAbstractImageData* toMedImage(dtkImage* image);

    private:
        template <typename T, int dim> dtkImage* toDtkImage_helper(medAbstractImageData* medImage);

};

inline medAbstractConverter *converterCreator(void)
{
    return new converter();
}
