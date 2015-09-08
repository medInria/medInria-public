#include <medAbstractSubtractImageProcess.h>

#include <QString>

medAbstractSubtractImageProcess::medAbstractSubtractImageProcess(QObject *parent):
    medAbstractArithmeticOperationProcess(parent)
{
    this->addTags(QStringList() << "subtraction"
                                << "minus");
}
