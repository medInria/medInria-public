/*=========================================================================

 medInria

 Copyright (c) INRIA 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medStringListParameter.h>


class medStringListParameterPrivate
{
public:
    int iCurrent;
    QStringList items;
    QValidator *poValidator;
    QHash <QString, QIcon> iconForItem;
};

medStringListParameter::medStringListParameter(QString name, QObject* parent): medStringParameter(name, parent), d(new medStringListParameterPrivate)
{
    d->iCurrent = 0;
    d->poValidator = nullptr;
    connect(this, &medStringListParameter::valueChanged, this, &medStringListParameter::triggered);
}


medStringListParameter::~medStringListParameter()
{
    //Nothing because is QScopedPointer
}

void medStringListParameter::addItem(QString item, QIcon icon)
{
    if (item != "")
    {
        QString sOldValue = value();

        d->items <<  item;
        d->iconForItem.insert(item, icon);
        
        reComputeIndex(sOldValue);
        emit compositionChanged();
    }
}

void medStringListParameter::addItems(QStringList items)
{
    if (!items.isEmpty())
    {
        QString sOldValue = value();

        for(QString item : items)
        {
            d->items << item;
            d->iconForItem.insert(item, QIcon());
        }

        reComputeIndex(sOldValue);
        emit compositionChanged();
    }
}

void medStringListParameter::removeItem(QString item)
{
    if (d->items.contains(item))
    {
        QString sOldValue = value();

        d->items.removeOne(item);
        d->iconForItem.remove(item);

        reComputeIndex(sOldValue);
        emit compositionChanged();
    }
}

void medStringListParameter::clear()
{
    d->items.clear();
    d->iCurrent = -1;
    emit compositionChanged();
}

QStringList medStringListParameter::items() const
{
    return d->items;
}

QString medStringListParameter::value() const
{
    QString osRes;
    if (d->iCurrent > -1 && d->iCurrent < d->items.size())
    {
        osRes = d->items[d->iCurrent];
    }
    return QString();
}

int medStringListParameter::getIndex() const
{
    return d->iCurrent;
}

bool medStringListParameter::copyValueTo(medAbstractParameter & dest)
{
    bool bRes = typeid(dest) == typeid(*this);

    if (bRes)
    {
        dynamic_cast<medStringListParameter*>(&dest)->setValue(value());
    }

    return bRes;
}

bool medStringListParameter::setValue(QString const & value)
{
    bool bRes = false;

    int i = 0;
    QString tmpVal = value;

    if (value != this->value() && (d->poValidator == nullptr || d->poValidator->validate(tmpVal, i) == QValidator::Acceptable))
    {
        bRes = true;
        i = d->items.indexOf(value);
        if (i < 0)
        {
            addItem(value);
            i = d->items.indexOf(value);
        }
        d->iCurrent = i;
        emit valueChanged(d->iCurrent);
    }

    return bRes;
}

bool medStringListParameter::setValue(QString const & value, int index)
{
    bool bRes = true;

    int i = 0;
    QString tmpVal = value;

    if (d->poValidator == nullptr || d->poValidator->validate(tmpVal, i) == QValidator::Acceptable)
    {
        if (index > -1 && index < d->items.size()) //modification of an existing value
        {
            if (d->items[index] != value)
            {
                d->items[index] = value;
            }
        }
        else if (index == d->items.size()) //add a new value on last position
        {
            d->items.insert(index, value);
            d->iconForItem.insert(value, QIcon());
        }
        else
        {
            bRes = false;
        }

        if (bRes)
        {
            d->iCurrent = index;
            emit compositionChanged();
        }
    }
    else
    {
        bRes = false;
    }

    return bRes;
}

bool medStringListParameter::setIndex(int index)
{
    bool bRes = index > -1 && index < d->items.size();

    if(bRes)
    {
        d->iCurrent = index;
        emit valueChanged(d->iCurrent);
    }

    return bRes;
}

void medStringListParameter::trigger()
{
    emit valueChanged(d->iCurrent);
}



void medStringListParameter::setValidator(QValidator *pi_poValidator)
{
    if (d->poValidator != pi_poValidator)
    {
        d->poValidator = pi_poValidator;
    }
}

QValidator *medStringListParameter::getValidator() const
{
    return d->poValidator;
}

// ////////////////////////////////////////////////////////////////////////////
// Private functions
// ////////////////////////////////////////////////////////////////////////////
void medStringListParameter::reComputeIndex(QString const& pi_roCurentValue)
{
    int iIndex = d->items.indexOf(pi_roCurentValue);
    if (iIndex != d->iCurrent)
    {
        if (d->items.isEmpty())
        {
            d->iCurrent = -1;
        }
        else if(iIndex > -1)
        {
            d->iCurrent = iIndex;
        }
        else
        {
            d->iCurrent = 0;
        }
        emit valueChanged(d->iCurrent);
    }
}

// ////////////////////////////////////////////////////////////////////////////
// Static functions
// ////////////////////////////////////////////////////////////////////////////
QIcon createIconFromColor(const QString &colorName)
{
    QPixmap iconPixmap(32,32);
    iconPixmap.fill(QColor(colorName));
    QIcon itemIcon(iconPixmap);
    return itemIcon;
}
