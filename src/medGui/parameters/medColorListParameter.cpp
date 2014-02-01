/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medColorListParameter.h>

#include <QComboBox>
#include <QColor>
#include <QList>
#include <QIcon>


class medColorListParameterPrivate
{
public:
    QComboBox* comboBox;
    QStringList colors;

    ~medColorListParameterPrivate(){delete comboBox;}
};

medColorListParameter::medColorListParameter(QString name, QObject* parent):
    medAbstractStringParameter(name,parent),
    d(new medColorListParameterPrivate)
{
    d->comboBox = NULL;
}

medColorListParameter::~medColorListParameter()
{
    delete d;
}

void medColorListParameter::addColor(QString& colorName)
{
    d->colors <<  colorName;

    if(d->comboBox)
        d->comboBox->addItem(createIcon(colorName), colorName);
}

void medColorListParameter::addColors(QStringList &colorNames)
{
    d->colors <<  colorNames;

    if(d->comboBox)
        foreach(QString colorName, colorNames)
            d->comboBox->addItem(createIcon(colorName), colorName);
}

void medColorListParameter::removeColor(QString &colorName)
{
    if(!d->colors.contains(colorName))
        return;

    int index = d->colors.indexOf(QRegExp(colorName));
    d->colors.removeOne(colorName);

    if(d->comboBox)
        d->comboBox->removeItem(index);
}

void medColorListParameter::clear()
{
    d->colors.clear();

    if(d->comboBox)
        d->comboBox->clear();
}

QStringList& medColorListParameter::colors() const
{
    return d->colors;
}


QComboBox* medColorListParameter::getComboBox()
{
    if(!d->comboBox)
    {
        d->comboBox = new QComboBox;
        d->comboBox->addItems(d->colors);

        this->addToInternWidgets(d->comboBox);
        connect(d->comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(setValue(QString)));
        connect(d->comboBox, SIGNAL(destroyed()), this, SLOT(_prvt_removeInternComboBox()));
    }
    return d->comboBox;
}

void medColorListParameter::updateInternWigets()
{
    if(!d->colors.contains(m_value))
        this->addColor(m_value);

    if (d->comboBox)
    {
        int index = d->colors.indexOf(QRegExp(m_value));
        d->comboBox->setCurrentIndex(index);
    }
}


QIcon medColorListParameter::createIcon(QString &colorName) const
{
    QPixmap iconPixmap(32,32);
    iconPixmap.fill(QColor(colorName));
    QIcon itemIcon(iconPixmap);
    return itemIcon;
}

QWidget* medColorListParameter::getWidget()
{
    return this->getComboBox();
}

void medColorListParameter::_prvt_removeInternComboBox()
{
    this->removeFromInternWidgets(d->comboBox);
    d->comboBox = NULL;
}
