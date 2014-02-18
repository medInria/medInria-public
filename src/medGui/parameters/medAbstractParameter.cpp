/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractParameter.h>

#include <QLabel>


class medAbstractParameterPrivate
{
public:
    QString name;
    QList <QWidget *> internWidget;
    QLabel *label;
    QString toolTip;

    ~medAbstractParameterPrivate() {delete label;}
};

medAbstractParameter::medAbstractParameter(QString name, QObject *parent):
    d(new medAbstractParameterPrivate)
{
    this->setParent(parent);
    d->label = NULL;
    d->toolTip = QString();
    this->setName(name);
}

medAbstractParameter::~medAbstractParameter(void)
{
    emit aboutToBeDestroyed();
    delete d;
}

QString medAbstractParameter::name() const
{
    return d->name;
}

void medAbstractParameter::setName(QString &name)
{
    d->name = name;
}

QLabel* medAbstractParameter::getLabel()
{
    if (!d->label)
    {
        d->label = new QLabel(name());
        this->addToInternWidgets(d->label);
        connect(d->label, SIGNAL(destroyed()), this, SLOT(removeInternLabel()));
    }

    return d->label;
}

void medAbstractParameter::setToolTip(QString &tooltip)
{
    d->toolTip = tooltip;

    foreach(QWidget *widget, d->internWidget)
        widget->setToolTip(tooltip);
}

bool medAbstractParameter::match(medAbstractParameter const *other)
{
    return this->name() == other->name();
}

void medAbstractParameter::show()
{
    foreach (QWidget *widget, d->internWidget)
        widget->show();
}

void medAbstractParameter::hide()
{
    foreach (QWidget *widget, d->internWidget)
        widget->hide();
}

void medAbstractParameter::removeInternLabel()
{
    this->removeFromInternWidgets(d->label);
    d->label = NULL;
}

void medAbstractParameter::addToInternWidgets(QWidget *widget)
{
    widget->setToolTip(d->toolTip);
    d->internWidget << widget;
}
void medAbstractParameter::removeFromInternWidgets(QWidget *widget)
{
    d->internWidget.removeOne(widget);
}

void medAbstractParameter::blockInternWidgetsSignals(bool block) const
{
    foreach(QWidget* widget, d->internWidget)
        widget->blockSignals(block);
}



//------------------ medTestParameter -------------------------------------

class medTestParameterPrivate
{
public:
    QWidget *widget;

    ~medTestParameterPrivate() {delete widget;}
};
medTestParameter::medTestParameter(QString name):
    medAbstractParameter(name),
    d(new medTestParameterPrivate)
{
    d->widget = NULL;
}
medTestParameter::~medTestParameter()
{
    delete d;
}

QWidget* medTestParameter::getWidget()
{
    if(!d->widget)
    {
        d->widget = new QWidget;
        this->addToInternWidgets(d->widget);
        connect(d->widget, SIGNAL(destroyed()), this, SLOT(removeInternWidget()));
    }

    return d->widget;
}

void medTestParameter::removeInternWidget()
{
    this->removeFromInternWidgets(d->widget);
    d->widget = NULL;
}


//--------------------------------------------------------------------------
//  medAbstractTriggerParameter
void medAbstractTriggerParameter::trigger()
{
    emit triggered();
}

//--------------------------------------------------------------------------
//  medAbstractStringParameter
void medAbstractStringParameter::setValue(QString value)
{
    if(m_value == value)
        return;

    m_value = value;

    //  update intern widget
    this->blockInternWidgetsSignals(true);
    this->updateInternWigets();
    this->blockInternWidgetsSignals(false);

    emit valueChanged(value);
}
QString medAbstractStringParameter::value() const
{
    return m_value;
}


//--------------------------------------------------------------------------
//  medAbstractIntParameter
void medAbstractIntParameter::setValue(int value)
{
    if(m_value == value)
        return;

    m_value = value;

    //  update intern widget
    this->blockInternWidgetsSignals(true);
    this->updateInternWigets();
    this->blockInternWidgetsSignals(false);

    emit valueChanged(value);
}
int medAbstractIntParameter::value() const
{
    return m_value;
}

//--------------------------------------------------------------------------
//  medAbstractDoubleParameter
void medAbstractDoubleParameter::setValue(double value)
{
    if(m_value == value)
        return;

    m_value = value;

    //  update intern widget
    this->blockInternWidgetsSignals(true);
    this->updateInternWigets();
    this->blockInternWidgetsSignals(false);

    emit valueChanged(value);
}
double medAbstractDoubleParameter::value() const
{
    return m_value;
}


//--------------------------------------------------------------------------
//  medAbstractBoolParameter
void medAbstractBoolParameter::setValue(bool value)
{
    if(m_value == value)
        return;

    m_value = value;

    //  update intern widget
    this->blockInternWidgetsSignals(true);
    this->updateInternWigets();
    this->blockInternWidgetsSignals(false);

    emit valueChanged(value);
}
bool medAbstractBoolParameter::value() const
{
    return m_value;
}


//--------------------------------------------------------------------------
//  medAbstractVector2DParameter
void medAbstractVector2DParameter::setValue(const QVector2D &value)
{
    if(m_value == value)
        return;

    m_value = value;

    //  update intern widget
    this->blockInternWidgetsSignals(true);
    this->updateInternWigets();
    this->blockInternWidgetsSignals(false);

    emit valueChanged(value);
}
QVector2D medAbstractVector2DParameter::value() const
{
    return m_value;
}

//--------------------------------------------------------------------------
//  medAbstractVector3DParameter
void medAbstractVector3DParameter::setValue(const QVector3D &value)
{
    if(m_value == value)
        return;

    m_value = value;

    //  update intern widget
    this->blockInternWidgetsSignals(true);
    this->updateInternWigets();
    this->blockInternWidgetsSignals(false);

    emit valueChanged(value);
}
QVector3D medAbstractVector3DParameter::value() const
{
    return m_value;
}

//--------------------------------------------------------------------------
//  medAbstractVector4DParameter
void medAbstractVector4DParameter::setValue(const QVector4D &value)
{
    if(m_value == value)
        return;

    m_value = value;

    //  update intern widget
    this->blockInternWidgetsSignals(true);
    this->updateInternWigets();
    this->blockInternWidgetsSignals(false);

    emit valueChanged(value);
}
QVector4D medAbstractVector4DParameter::value() const
{
    return m_value;
}
