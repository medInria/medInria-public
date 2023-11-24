/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDataParameter.h>

#include <dtkLog>

class medDataParameterPrivate
{
public:
    medAbstractData *value;
    int  iRow, iColomun;
    int  iTab;
    bool bIn, bOut;
};

medDataParameter::medDataParameter(QString const& name,  QObject *parent)
    : medAbstractParameter(name, parent), d(new medDataParameterPrivate)
{
    d->value   = nullptr;
    d->iRow     = 0;
    d->iColomun = 0;
    d->iTab     = 0;
    d->bIn      = true;
    d->bOut     = false;
    connect(this, &medDataParameter::valueChanged, this, &medDataParameter::triggered);
}

medDataParameter::~medDataParameter()
{

}

medAbstractData* medDataParameter::value() const
{
    return d->value;
}

bool medDataParameter::copyValueTo(medAbstractParameter & dest)
{
    bool bRes = typeid(dest) == typeid(*this);

    if (bRes)
    {
        dynamic_cast<medDataParameter*>(&dest)->setValue(value());
    }

    return bRes;
}

QVariantMap medDataParameter::toVariantMap() const
{
    return QVariantMap(); //TODO
}

bool medDataParameter::fromVariantMap(QVariantMap const & pi_variantMap)
{
    return false; //TODO
}

void medDataParameter::setRowPosition(int iRow)
{
    d->iRow = iRow;
}

int medDataParameter::rowPosition()
{
    return d->iRow;
}

void medDataParameter::setColPosition(int iCol)
{
    d->iColomun = iCol;
}

int medDataParameter::colPosition()
{
    return d->iColomun;
}

void medDataParameter::setTabPosition(int iTab)
{
    d->iTab = iTab;
}

int medDataParameter::tabPosition()
{
    return d->iTab;
}

void medDataParameter::setDirection(bool bIn, bool bOut)
{
    d->bIn  = bIn;
    d->bOut = bOut;
}

bool medDataParameter::isInput()
{
    return d->bIn;
}

bool medDataParameter::isOuput()
{
    return d->bOut;
}

void medDataParameter::setValue(medAbstractData *value)
{
    if(value != d->value)
    {
        d->value = value;
        emit valueChanged(d->value);
    }
}

void medDataParameter::trigger()
{
    emit valueChanged(d->value);
}
