/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medCompositeParameter.h"

#include <QtGui>
#include <QtWidgets>
#include <QPair>


class medCompositeParameterPrivate
{
public:

    QHash<QString, QVariant> variants;
    QHash<QString, QPair <QVariant, QVariant> > ranges;
    QHash<QString, QVariant> steps;
    QHash<QString, QWidget*> widgets;

    ~medCompositeParameterPrivate()
    {
        variants.clear();
        QHashIterator<QString, QWidget*> i(widgets);
        while (i.hasNext())
        {
            i.next();
            delete i.value();
        }
        widgets.clear();
    }
};

medCompositeParameter::medCompositeParameter(QString name, QObject* parent):
        medAbstractParameter(name, parent),
        d(new medCompositeParameterPrivate)
{

}

medCompositeParameter::~medCompositeParameter()
{
    delete d;
}

QWidget* medCompositeParameter::getWidget()
{
    QWidget *mainWidget = new QWidget;
    QFormLayout *layout = new QFormLayout(mainWidget);

    QHash<QString, QWidget*>::const_iterator i = d->widgets.constBegin();
    while (i != d->widgets.constEnd())
    {
        layout->addRow(i.key(), i.value());
        ++i;
    }
    return mainWidget;
}

void medCompositeParameter::setValues(const QHash<QString, QVariant> value)
{
    QHash<QString, QVariant>::const_iterator valuesIterator = value.constBegin();
    bool valueUpdated = false;

    while (valuesIterator != value.constEnd())
    {
        QString key = valuesIterator.key();
        if (!d->variants.contains(key))
        {
            valuesIterator++;
            continue;
        }

        if(d->variants[key].type() == QVariant::Double)
        {
            double previousValue = d->variants[key].toDouble();

            if(valuesIterator.value().toDouble() < d->ranges.value(key).first.toDouble())
                d->variants[key] = d->ranges.value(key).first;
            else if(valuesIterator.value().toDouble() > d->ranges.value(key).second.toDouble())
                d->variants[key] = d->ranges.value(key).second;
            else
                d->variants[key] = valuesIterator.value();

            if( previousValue != d->variants[key] )
                valueUpdated = true;
        }
        else if(d->variants[key].type() == QVariant::Int)
        {
            int previousValue = d->variants[key].toInt();

            if(valuesIterator.value().toInt() < d->ranges.value(key).first.toInt())
                d->variants[key] = d->ranges.value(key).first;
            else if(valuesIterator.value().toInt() > d->ranges.value(key).second.toInt())
                d->variants[key] = d->ranges.value(key).second;
            else
                d->variants[key] = valuesIterator.value();

            if( previousValue != d->variants[key] )
                valueUpdated = true;
        }
        else
        {
            d->variants[key] = valuesIterator.value();
            valueUpdated = true;
        }

        valuesIterator++;
    }

    if(!valueUpdated)
        return;

    //  update intern widget
    this->blockInternWidgetsSignals(true);
    this->updateInternWigets();
    this->blockInternWidgetsSignals(false);

    emit valuesChanged(d->variants);
}

QList<QVariant> medCompositeParameter::values() const
{
    return d->variants.values();
}

void medCompositeParameter::updateInternWigets()
{
    QHash<QString, QVariant>::const_iterator i = d->variants.constBegin();
    while (i != d->variants.constEnd())
    {
        QString name = i.key();
        QVariant var = i.value();
        QWidget* widget = d->widgets.value(name);

        if(QCheckBox *checkbox = qobject_cast<QCheckBox*>(widget))
            checkbox->setChecked(var.toBool());
        else if(QSpinBox *spinBox = qobject_cast<QSpinBox*>(widget))
            spinBox->setValue(var.toInt());
        else if(QDoubleSpinBox *doubleSpinBox = qobject_cast<QDoubleSpinBox*>(widget))
            doubleSpinBox->setValue(var.toDouble());

        ++i;
    }
}

void medCompositeParameter::addVariant(QString name, QVariant variant, QVariant min, QVariant max, QVariant step)
{
    d->variants.insert(name, variant);

    if(variant.type() == QVariant::Bool)
    {
        QCheckBox *checkbox = new QCheckBox(name);
        d->widgets.insert(name, checkbox);
        this->addToInternWidgets(checkbox);
        connect(checkbox, SIGNAL(toggled(bool)), this, SLOT(updateValue(bool)));
        connect(checkbox, SIGNAL(destroyed(QObject*)), this, SLOT(removeInternWidget(QObject*)));
    }
    else if(variant.type() == QVariant::Int)
    {
        QSpinBox *spinbox = new QSpinBox;
        if(min != QVariant() && max != QVariant())
        {
            spinbox->setMinimum(min.toInt());
            spinbox->setMaximum(max.toInt());
            d->ranges.insert(name, QPair<QVariant, QVariant>(min, max));
        }
        if(step != QVariant())
        {
            spinbox->setSingleStep(step.toInt());
            d->steps.insert(name, step);
        }
        spinbox->setValue(variant.toInt());
        d->widgets.insert(name, spinbox);
        this->addToInternWidgets(spinbox);
        connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(updateValue(int)));
        connect(spinbox, SIGNAL(destroyed(QObject*)), this, SLOT(removeInternWidget(QObject*)));
    }
    else if(variant.type() == QVariant::Double )
    {
        QDoubleSpinBox *spinbox = new QDoubleSpinBox;
        if(min != QVariant() && max != QVariant())
        {
            spinbox->setMinimum(min.toDouble());
            spinbox->setMaximum(max.toDouble());
            d->ranges.insert(name, QPair<QVariant, QVariant>(min, max));
        }
        if(step != QVariant())
        {
            spinbox->setSingleStep(step.toDouble());
            d->steps.insert(name, step);
        }
        spinbox->setValue(variant.toDouble());
        d->widgets.insert(name, spinbox);
        this->addToInternWidgets(spinbox);
        connect(spinbox, SIGNAL(valueChanged(double)), this, SLOT(updateValue(double)));
        connect(spinbox, SIGNAL(destroyed(QObject*)), this, SLOT(removeInternWidget(QObject*)));
    }

    //TODO: to complete with other QVariant types
}


void medCompositeParameter::updateValue(bool value)
{
    QCheckBox *checkbox = qobject_cast<QCheckBox*>(QObject::sender());
    if(checkbox)
    {
        QString name = d->widgets.key(checkbox);
        d->variants[name] = QVariant(value);
        emit valuesChanged(d->variants);
    }

}

void medCompositeParameter::updateValue(double value)
{
    QDoubleSpinBox *spinbox = qobject_cast<QDoubleSpinBox*>(QObject::sender());
    if(spinbox)
    {
        QString name = d->widgets.key(spinbox);
        d->variants[name] = QVariant(value);
        emit valuesChanged(d->variants);
    }
}

void medCompositeParameter::updateValue(int value)
{
    QSpinBox *spinbox = qobject_cast<QSpinBox*>(QObject::sender());
    if(spinbox)
    {
        QString name = d->widgets.key(spinbox);
        d->variants[name] = QVariant(value);
        emit valuesChanged(d->variants);
    }
}

void medCompositeParameter::removeInternWidget(QObject *widget)
{
    QWidget *w = qobject_cast<QWidget*>(widget);
    if(!w)
        return;

    this->removeFromInternWidgets(w);
    QHashIterator<QString, QWidget*> i(d->widgets);
    while (i.hasNext())
    {
        i.next();
        if(w == i.value())
            d->widgets.remove(i.key());
    }
}

QList<QPair<QVariant, QVariant> > medCompositeParameter::ranges() const
{
    return d->ranges.values();
}

QList<QVariant> medCompositeParameter::steps() const
{
    return d->steps.values();
}

void medCompositeParameter::trigger()
{
    emit valuesChanged(d->variants);
}
