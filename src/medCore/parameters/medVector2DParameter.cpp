#include "medVector2DParameter.h"
#include <QVector2D>

class medVector2DParameterPrivate
{
public:

};

medVector2DParameter::medVector2DParameter(QString name, QObject *parent)
    : medAbstractVector2DParameter(name, parent), d(new medVector2DParameterPrivate)
{
}

medVector2DParameter::~medVector2DParameter()
{

}

QWidget* medVector2DParameter::getWidget()
{
    return 0;
}

void medVector2DParameter::updateInternWigets()
{
    return;
}
