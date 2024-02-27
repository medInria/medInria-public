/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVariantListParameter.h>
#include <medVariantListParameterPresenter.h>
#include <medVariantListParameterWidgetPrivate.h>

#include <QWidget>
#include <QCheckBox>
#include <QSpinBox>
#include <QFormLayout>
#include <QDebug>


medVariantListParameterWidgetPrivate::medVariantListParameterWidgetPrivate(QWidget * parent, const char * name)
{
    layout.setParent(this);

    QHash<QString, QWidget*>::const_iterator i = widgets.constBegin();
    while (i != widgets.constEnd())
    {
        layout.addRow(i.key(), i.value());
        ++i;
    }
}

medVariantListParameterWidgetPrivate::~medVariantListParameterWidgetPrivate()
{
}

void medVariantListParameterWidgetPrivate::changeValue(QString const & key, QVariant const & Value)
{

    blockInternWidgetsSignals(true);
    if (widgets.contains(key))
    {
        QWidget* widget = widgets.value(key);

        if (QCheckBox *checkbox = qobject_cast<QCheckBox*>(widget))
            checkbox->setChecked(Value.toBool());
        else if (QSpinBox *spinBox = qobject_cast<QSpinBox*>(widget))
            spinBox->setValue(Value.toInt());
        else if (QDoubleSpinBox *doubleSpinBox = qobject_cast<QDoubleSpinBox*>(widget))
            doubleSpinBox->setValue(Value.toDouble());
    }
    blockInternWidgetsSignals(false);
}

void medVariantListParameterWidgetPrivate::addVariant(QString const & key, QVariant const & Value)
{
    blockInternWidgetsSignals(true);
    if (!widgets.contains(key))
    {
        switch (Value.type())
        {
        case QVariant::Bool:
        {
            QCheckBox *checkbox = new QCheckBox(key);
            widgets.insert(key, checkbox);
            //connect(checkbox, &QCheckBox::toggled, [=](bool val) {emit valueChanged(key, QVariant(val)); });
            break;
        }
        case QVariant::Int:
        {
            QSpinBox *spinbox = new QSpinBox;
            //if (min != QVariant() && max != QVariant())
            //{
            //    spinbox->setMinimum(min.toInt());
            //    spinbox->setMaximum(max.toInt());
            //}
            //if (step != QVariant())
            //{
            //    spinbox->setSingleStep(step.toInt());
            //}
            spinbox->setValue(Value.toInt());
            widgets.insert(key, spinbox);
            void (QSpinBox::* mySignal)(int) = &QSpinBox::valueChanged;
            //connect(spinbox, mySignal, [=](int val) {emit valueChanged(key, QVariant(val)); });
            break;
        }
        case QVariant::Double:
        {
            QDoubleSpinBox *spinbox = new QDoubleSpinBox;
            //if (min != QVariant() && max != QVariant())
            //{
            //    spinbox->setMinimum(min.toDouble());
            //    spinbox->setMaximum(max.toDouble());
            //}
            //if (step != QVariant())
            //{
            //    spinbox->setSingleStep(step.toDouble());
            //}
            spinbox->setValue(Value.toDouble());
            widgets.insert(key, spinbox);
            void (QDoubleSpinBox::* mySignal)(double) = &QDoubleSpinBox::valueChanged;
            //connect(spinbox, mySignal, [=](double val) {emit valueChanged(key, QVariant(val)); });
            break;
        }
        default:
            qWarning() << "medVariantListParameterWidgetPrivate::addVariant don't accept " << Value.typeName() << "\n";
            break;
        }
    }
    changeValue(key, Value);
    blockInternWidgetsSignals(false);
}

void medVariantListParameterWidgetPrivate::removeVariant(QString const & key)
{
    QWidget * widget = widgets[key];
    widget->setParent(nullptr);
    widget->disconnect();
    widgets.remove(key);
}


inline void medVariantListParameterWidgetPrivate::blockInternWidgetsSignals(bool block) const
{
    for (QWidget* widget : widgets)
    {
        widget->blockSignals(block);
    }
}
