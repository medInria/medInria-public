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
#include <QHash>
#include <QIcon>


class medColorListParameterPrivate
{
public:
    QComboBox* comboBox;
    QHash<QString, QString> colors;

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

void medColorListParameter::addColor(const QString& colorName, const QString& textDisplayed)
{
    QString text = textDisplayed;

    if(text.isEmpty())
        text = colorName;

    d->colors.insert(colorName, text);

    if(d->comboBox)
        d->comboBox->addItem(createIcon(colorName), text, colorName);
}

void medColorListParameter::addColors(const QStringList &colorNames, const QStringList &labels)
{
    int i = 0;
    foreach(QString color, colorNames)
    {
        QString label;
        if(i<labels.size())
          label = labels.at(i);
        addColor(color, label);
    }
}

void medColorListParameter::removeColor(const QString &colorName)
{
    if(!d->colors.contains(colorName))
        return;

    d->colors.remove(colorName);

    if(d->comboBox)
        d->comboBox->removeItem(d->comboBox->findData(colorName));
}

void medColorListParameter::clear()
{
    d->colors.clear();

    if(d->comboBox)
        d->comboBox->clear();
}

QList<QString> medColorListParameter::colors() const
{
    return d->colors.keys();
}


QComboBox* medColorListParameter::getComboBox()
{
    if(!d->comboBox)
    {
        d->comboBox = new QComboBox;

        foreach(QString color, d->colors.keys())
            d->comboBox->addItem(createIcon(color),d->colors.value(color), color);

        this->addToInternWidgets(d->comboBox);
        connect(d->comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(setColor(QString)));
        connect(d->comboBox, SIGNAL(destroyed()), this, SLOT(removeInternComboBox()));
    }
    return d->comboBox;
}

void medColorListParameter::updateInternWigets()
{
    if(!d->colors.contains(m_value))
        this->addColor(m_value);

    if (d->comboBox)
    {
        int index = d->comboBox->findData(m_value);
        d->comboBox->setCurrentIndex(index);
    }
}


QIcon medColorListParameter::createIcon(const QString &colorName) const
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

void medColorListParameter::removeInternComboBox()
{
    this->removeFromInternWidgets(d->comboBox);
    d->comboBox = NULL;
}

void medColorListParameter::setCurrentColor(const QString& colorName)
{
    if (d->comboBox)
    {
        int index = d->comboBox->findData(colorName);
        d->comboBox->setCurrentIndex(index);
    }

    setValue(colorName);
}

void medColorListParameter::setColor(const QString& value)
{
    if (d->comboBox)
    {
        int index = d->comboBox->findText(value);
        QString color = d->comboBox->itemData(index).toString();
        setValue(color);
    }
}
