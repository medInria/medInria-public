/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medStringListParameterL.h>

#include <QComboBox>
#include <dtkLog>


class medStringListParameterLPrivate
{
public:
    QComboBox* comboBox;
    QStringList items;
    QHash <QString, QIcon> iconForItem;

    ~medStringListParameterLPrivate(){delete comboBox;}
};

medStringListParameterL::medStringListParameterL(QString name, QObject* parent):
    medAbstractStringParameterL(name, parent),
    d(new medStringListParameterLPrivate)
{
    d->comboBox = NULL;
}

medStringListParameterL::~medStringListParameterL()
{
    delete d;
}

void medStringListParameterL::addItem(QString item, QIcon icon)
{
    if(item == "")
        return;

    d->items <<  item;
    d->iconForItem.insert(item, icon);

    if(d->comboBox)
        d->comboBox->addItem(icon, item);
}

void medStringListParameterL::addItems(QStringList items)
{
    foreach(QString item, items)
        this->addItem(item);
}

void medStringListParameterL::removeItem(QString item)
{
    if(!d->items.contains(item))
        return;

    int index = d->items.indexOf(QRegExp(item));
    d->items.removeOne(item);

    if(d->comboBox)
        d->comboBox->removeItem(index);
}

void medStringListParameterL::clear()
{
    d->items.clear();

    if(d->comboBox)
        d->comboBox->clear();
}

QStringList medStringListParameterL::items() const
{
    return d->items;
}


QComboBox* medStringListParameterL::getComboBox()
{
    if(!d->comboBox)
    {
        d->comboBox = new QComboBox;
        foreach(QString item, d->items)
            d->comboBox->addItem(d->iconForItem.value(item), item);

        this->addToInternWidgets(d->comboBox);

        updateInternWigets();

        connect(d->comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(setValue(QString)));
        connect(d->comboBox, SIGNAL(destroyed()), this, SLOT(removeInternComboBox()));
    }
    return d->comboBox;
}

void medStringListParameterL::updateInternWigets()
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

QWidget* medStringListParameterL::getWidget()
{
    return this->getComboBox();
}

void medStringListParameterL::removeInternComboBox()
{
    this->removeFromInternWidgets(d->comboBox);
    d->comboBox = NULL;
}

QIcon medStringListParameterL::createIconFromColor(const QString &colorName)
{
    QPixmap iconPixmap(32,32);
    iconPixmap.fill(QColor(colorName));
    QIcon itemIcon(iconPixmap);
    return itemIcon;
}
