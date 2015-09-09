#include <medAbstractMultiplyImageProcess.h>

#include <QString>

medAbstractMultiplyImageProcess::medAbstractMultiplyImageProcess(QObject *parent):
    medAbstractArithmeticOperationProcess(parent)
{
    this->addTags(QStringList() << "multiplication"
                                << "multiply");
}
