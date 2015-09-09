#include <medAbstractDivideImageProcess.h>

#include <QString>

medAbstractDivideImageProcess::medAbstractDivideImageProcess(QObject *parent):
    medAbstractArithmeticOperationProcess(parent)
{
    this->addTags(QStringList() << "division"
                                << "divide");
}
