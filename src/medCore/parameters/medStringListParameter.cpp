/*=========================================================================
 medInria
 Copyright (c) INRIA 2013 - 2014. All rights reserved.
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
    QHash <QString, QIcon> iconForItem;

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

void medStringListParameter::addItem(QString item, QIcon icon)
{
    if(item == "")
        return;

    d->items <<  item;
    d->iconForItem.insert(item, icon);

    if(d->comboBox)
        d->comboBox->addItem(icon, item);
}

void medStringListParameter::addItems(QStringList items)
{
    foreach(QString item, items)
        this->addItem(item);
}

void medStringListParameter::removeItem(QString item)
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

QStringList medStringListParameter::items() const
{
    return d->items;
}


QComboBox* medStringListParameter::getComboBox()
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

QIcon medStringListParameter::createIconFromColor(const QString &colorName)
{
    QPixmap iconPixmap(32,32);
    iconPixmap.fill(QColor(colorName));
    QIcon itemIcon(iconPixmap);
    return itemIcon;
}

QString medStringListParameter::fromString(QString value)
{
    /*for(int i=0;i<getComboBox()->count();i++)
		if(this->getComboBox()->itemText(i)==value)
            getComboBox()->setCurrentIndex(i);*/
    return value;
}

void medStringListParameter::toXMLNode(QDomDocument* doc,QDomElement* currentNode)
{
	medAbstractParameter::toXMLNode(doc,currentNode);
	QDomElement type=doc->createElement("type");
	type.appendChild(doc->createTextNode("String"));
	currentNode->appendChild(type);
	QDomElement elmt=doc->createElement("value");
	elmt.appendChild(doc->createTextNode(getComboBox()->currentText()));
	currentNode->appendChild(elmt);
}
