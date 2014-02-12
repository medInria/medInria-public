#include "medVector3DParameter.h"
#include <QVector3D>

class medVector3DParameterPrivate
{
public:

};

medVector3DParameter::medVector3DParameter(QString name, QObject *parent)
    : medAbstractVector3DParameter(name, parent), d(new medVector3DParameterPrivate)
{
}

medVector3DParameter::~medVector3DParameter()
{

}

QWidget* medVector3DParameter::getWidget()
{
    return 0;
}

void medVector3DParameter::updateInternWigets()
{
    return;
}
