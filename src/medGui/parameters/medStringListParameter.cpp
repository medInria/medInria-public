/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medStringListParameter.h>

#include <QComboBox>
#include <QDebug>


class medStringListParameterPrivate
{
public:
    QComboBox* comboBox;
    QStringList items;

    ~medStringListParameterPrivate(){delete comboBox;}
};

medStringListParameter::medStringListParameter(QString name, QObject* parent):
    medAbstractStringParameter(name, parent),
    d(new medStringListParameterPrivate)
{
    d->comboBox = NULL;
}

medStringListParameter::~medStringListParameter()
{
    delete d;
}

void medStringListParameter::addItem(QString& item)
{
    if(item == "")
        return;

    d->items <<  item;

    if(d->comboBox)
        d->comboBox->addItem(item);
}

void medStringListParameter::addItems(QStringList &items)
{
    d->items <<  items;

    if(d->comboBox)
        d->comboBox->addItems(items);
}

void medStringListParameter::removeItem(QString &item)
{
    if(!d->items.contains(item))
        return;

    int index = d->items.indexOf(QRegExp(item));
    d->items.removeOne(item);

    if(d->comboBox)
        d->comboBox->removeItem(index);
}

void medStringListParameter::clear()
{
    d->items.clear();

    if(d->comboBox)
        d->comboBox->clear();
}

QStringList& medStringListParameter::items() const
{
    return d->items;
}


QComboBox* medStringListParameter::getComboBox()
{
    if(!d->comboBox)
    {
        d->comboBox = new QComboBox;
        d->comboBox->addItems(d->items);

        this->addToInternWidgets(d->comboBox);
        connect(d->comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(setValue(QString)));
        connect(d->comboBox, SIGNAL(destroyed()), this, SLOT(removeInternComboBox()));
    }
    return d->comboBox;
}

void medStringListParameter::updateInternWigets()
{
    if(m_value == "")
        return;

    if(!d->items.contains(m_value))
        this->addItem(m_value);

    if (d->comboBox)
    {
        int index = d->items.indexOf(QRegExp(m_value));
        d->comboBox->setCurrentIndex(index);
    }
}

QWidget* medStringListParameter::getWidget()
{
    return this->getComboBox();
}

void medStringListParameter::removeInternComboBox()
{
    this->removeFromInternWidgets(d->comboBox);
    d->comboBox = NULL;
}
