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
#include <QStandardItemModel>


class medStringListParameterPrivate
{
public:
    QComboBox* comboBox;
    QStringList items;
    QHash <QString, QIcon> iconForItem;

    QStringList m_value;

    bool isExclusive;

    ~medStringListParameterPrivate(){delete comboBox;}
};

medStringListParameter::medStringListParameter(QString name, QObject* parent):
    medAbstractParameter(name, parent),
    d(new medStringListParameterPrivate)
{
    d->comboBox = NULL;
    d->isExclusive = true;
}

medStringListParameter::~medStringListParameter()
{
    delete d;
}

void medStringListParameter::setExclusive(bool exclusive)
{
    d->isExclusive = exclusive;
}

void medStringListParameter::setValues(QStringList value)
{
    if(d->m_value == value)
        return;

    d->m_value = value;

    //  update intern widget
    this->blockInternWidgetsSignals(true);
    this->updateInternWigets();
    this->blockInternWidgetsSignals(false);

    emit valueChanged(value);
}

void medStringListParameter::setValue(QString value)
{
    if(d->m_value == QStringList(value))
        return;

    d->m_value = QStringList(value);

    //  update intern widget
    this->blockInternWidgetsSignals(true);
    this->updateInternWigets();
    this->blockInternWidgetsSignals(false);

    emit valueChanged(value);
}

QStringList medStringListParameter::values() const
{
    return d->m_value;
}

QString medStringListParameter::value() const
{
    return d->m_value[0];
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

        QStandardItemModel *model = new QStandardItemModel(this);
        foreach(QString s, d->items)
        {
            QStandardItem* item = new QStandardItem(s);

            if(!d->isExclusive)
            {
              item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
              item->setData(Qt::Unchecked, Qt::CheckStateRole);
            }

            model->appendRow(item);
        }
        d->comboBox->setModel(model);

        this->addToInternWidgets(d->comboBox);

        updateInternWigets();

        connect(d->comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(setValue(QString)));
        connect(d->comboBox, SIGNAL(destroyed()), this, SLOT(removeInternComboBox()));
    }
    return d->comboBox;
}

void medStringListParameter::updateInternWigets()
{
    if(d->m_value.isEmpty())
        return;

    foreach(QString s, d->m_value)
      if(!d->items.contains(s))
          this->addItem(s);

    if (d->comboBox)
    {
        if(d->isExclusive)
        {
            int index = d->items.indexOf(QRegExp(d->m_value[0]));
            d->comboBox->setCurrentIndex(index);
        }
        else
        {
            foreach(QString s, d->m_value)
            {
                int index = d->items.indexOf(QRegExp(s));
                QStandardItemModel *model = dynamic_cast<QStandardItemModel *>(d->comboBox->model());
                model->item(index)->setCheckState(Qt::Checked);
            }
        }
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
