#include "medCompositeParameter.h"

#include <QtGui>


class medCompositeParameterPrivate
{
public:

    QList<QVariant> value;
    QList<medAbstractParameter*> parameters;
    QHash<QString, QVariant> variants;
    QHash<QString, QWidget*> widgets;

    ~medCompositeParameterPrivate()
    {
    }
};

medCompositeParameter::medCompositeParameter(QString name, QObject* parent):
        medAbstractParameter(name, parent),
        d(new medCompositeParameterPrivate)
{

}

medCompositeParameter::~medCompositeParameter()
{

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

void medCompositeParameter::setValue(const QList<QVariant> value)
{
    if(d->value == value)
        return;
    d->value = value;

    //  update intern widget
    this->blockInternWidgetsSignals(true);
    this->updateInternWigets();
    this->blockInternWidgetsSignals(false);

    emit valueChanged(value);
}

QList<QVariant> medCompositeParameter::value() const
{
    return d->value;
}

void medCompositeParameter::updateInternWigets()
{
    QHash<QString, QVariant>::const_iterator i = d->variants.constBegin();
    while (i != d->variants.constEnd())
    {
        QString name = i.key();
        QVariant var = i.value();
        QWidget* widget = d->widgets.value(name);

        if(QCheckBox *checkbox = dynamic_cast<QCheckBox*>(widget))
        {
            checkbox->setChecked(var.toBool());
        }
        else if(QSpinBox *spinBox = dynamic_cast<QSpinBox*>(widget))
        {
            spinBox->setValue(var.toInt());
        }
        else if(QDoubleSpinBox *doubleSpinBox = dynamic_cast<QDoubleSpinBox*>(widget))
        {
            doubleSpinBox->setValue(var.toDouble());
        }
        ++i;
    }
}

void medCompositeParameter::addVariant(QString name, QVariant variant, QVariant min, QVariant max, QVariant step)
{
    d->value.append(QVariant());
    d->variants.insert(name, variant);

    if(variant.type() == QVariant::Bool)
    {
        QCheckBox *checkbox = new QCheckBox(name);
        d->widgets.insert(name, checkbox);
        connect(checkbox, SIGNAL(toggled(bool)), this, SLOT(updateValue(bool)));
    }
    else if(variant.type() == QVariant::Int)
    {
        QSpinBox *spinbox = new QSpinBox;
        spinbox->setValue(variant.toInt());
        if(min != QVariant() && max != QVariant())
        {
            spinbox->setMinimum(min.toInt());
            spinbox->setMaximum(max.toInt());
        }
        if(step != QVariant())
        {
            spinbox->setSingleStep(step.toInt());
        }
        d->widgets.insert(name, spinbox);
        connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(updateValue(int)));
    }
    else if(variant.type() == QVariant::Double )
    {
        QDoubleSpinBox *spinbox = new QDoubleSpinBox;
        spinbox->setValue(variant.toDouble());
        if(min != QVariant() && max != QVariant())
        {
            spinbox->setMinimum(min.toDouble());
            spinbox->setMaximum(max.toDouble());
        }
        if(step != QVariant())
        {
            spinbox->setSingleStep(step.toDouble());
        }
        d->widgets.insert(name, spinbox);
        connect(spinbox, SIGNAL(valueChanged(double)), this, SLOT(updateValue(double)));
    }

    //TODO: to complete with other QVariant types
}


void medCompositeParameter::updateValue(bool value)
{
    QCheckBox *checkbox = dynamic_cast<QCheckBox*>(QObject::sender());
    if(checkbox)
    {
        QString name = d->widgets.key(checkbox);
        d->variants[name] = QVariant(value);
        emit valueChanged(d->variants.values());
    }

}

void medCompositeParameter::updateValue(double value)
{
    QDoubleSpinBox *spinbox = dynamic_cast<QDoubleSpinBox*>(QObject::sender());
    if(spinbox)
    {
        QString name = d->widgets.key(spinbox);
        d->variants[name] = QVariant(value);
        emit valueChanged(d->variants.values());
    }
}

void medCompositeParameter::updateValue(int value)
{
    QSpinBox *spinbox = dynamic_cast<QSpinBox*>(QObject::sender());
    if(spinbox)
    {
        QString name = d->widgets.key(spinbox);
        d->variants[name] = QVariant(value);
        emit valueChanged(d->variants.values());
    }
}
