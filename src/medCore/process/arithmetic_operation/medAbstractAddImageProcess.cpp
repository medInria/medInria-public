#include <medAbstractAddImageProcess.h>

#include <QString>

medAbstractAddImageProcess::medAbstractAddImageProcess(QObject *parent):
    medAbstractArithmeticOperationProcess(parent)
{
    this->addTags(QStringList() << "addition"
                                << "plus");
}
