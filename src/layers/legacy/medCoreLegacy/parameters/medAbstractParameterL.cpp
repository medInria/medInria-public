/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractParameterL.h>

#include <QtGui>
#include <QtWidgets>

class medAbstractParameterLPrivate
{
public:
    QString name;
    QList <QWidget *> internWidget;
    QLabel *label;
    QString toolTip;

    bool hide;

    ~medAbstractParameterLPrivate() {delete label;}
};

medAbstractParameterL::medAbstractParameterL(QString name, QObject *parent):
    d(new medAbstractParameterLPrivate)
{
    this->setParent(parent);
    d->label = nullptr;
    d->toolTip = QString();
    this->setName(name);
    d->hide = false;
}

medAbstractParameterL::~medAbstractParameterL(void)
{
    emit aboutToBeDestroyed();
    delete d;
}

QString medAbstractParameterL::name() const
{
    return d->name;
}

void medAbstractParameterL::setName(QString &name)
{
    d->name = name;
}

QLabel* medAbstractParameterL::getLabel()
{
    if (!d->label)
    {
        d->label = new QLabel(name());
        this->addToInternWidgets(d->label);
        connect(d->label, SIGNAL(destroyed()), this, SLOT(removeInternLabel()));
    }

    return d->label;
}

void medAbstractParameterL::setToolTip(QString tooltip)
{
    d->toolTip = tooltip;

    for(QWidget *widget : d->internWidget)
    {
        widget->setToolTip(tooltip);
    }
}

bool medAbstractParameterL::match(medAbstractParameterL const *other)
{
    return this->name() == other->name();
}

void medAbstractParameterL::show()
{
    d->hide = false;

    for(QWidget *widget : d->internWidget)
    {
        widget->show();
    }
}

void medAbstractParameterL::hide()
{
    d->hide = true;

    for(QWidget *widget : d->internWidget)
    {
        widget->hide();
    }
}

void medAbstractParameterL::removeInternLabel()
{
    this->removeFromInternWidgets(d->label);
    d->label = nullptr;
}

void medAbstractParameterL::addToInternWidgets(QWidget *widget)
{
    widget->setToolTip(d->toolTip);
    if(d->hide)
        widget->hide();

    d->internWidget << widget;
}
void medAbstractParameterL::removeFromInternWidgets(QWidget *widget)
{
    d->internWidget.removeOne(widget);
}

void medAbstractParameterL::blockInternWidgetsSignals(bool block) const
{
    for(QWidget* widget : d->internWidget)
    {
        widget->blockSignals(block);
    }
}

//--------------------------------------------------------------------------
//  medAbstractTriggerParameterL
void medAbstractTriggerParameterL::trigger()
{
    emit triggered();
}

//--------------------------------------------------------------------------
//  medAbstractStringParameterL
void medAbstractStringParameterL::setValue(QString value)
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

QString medAbstractStringParameterL::value() const
{
    return m_value;
}

void medAbstractStringParameterL::trigger()
{
    emit valueChanged(m_value);
}

//--------------------------------------------------------------------------
//  medAbstractIntParameterL
void medAbstractIntParameterL::setValue(int value)
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

int medAbstractIntParameterL::value() const
{
    return m_value;
}

void medAbstractIntParameterL::trigger()
{
    emit valueChanged(m_value);
}

//--------------------------------------------------------------------------
//  medAbstractDoubleParameterL
void medAbstractDoubleParameterL::setValue(double value)
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

double medAbstractDoubleParameterL::value() const
{
    return m_value;
}

void medAbstractDoubleParameterL::trigger()
{
    emit valueChanged(m_value);
}

//--------------------------------------------------------------------------
//  medAbstractBoolParameterL
void medAbstractBoolParameterL::setValue(bool value)
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

bool medAbstractBoolParameterL::value() const
{
    return m_value;
}

void medAbstractBoolParameterL::trigger()
{
    emit valueChanged(m_value);
}

//--------------------------------------------------------------------------
//  medAbstractVector2DParameterL
void medAbstractVector2DParameterL::setValue(const QVector2D &value)
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

QVector2D medAbstractVector2DParameterL::value() const
{
    return m_value;
}

void medAbstractVector2DParameterL::trigger()
{
    emit valueChanged(m_value);
}

//--------------------------------------------------------------------------
//  medAbstractVector3DParameterL
void medAbstractVector3DParameterL::setValue(const QVector3D &value)
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

QVector3D medAbstractVector3DParameterL::value() const
{
    return m_value;
}

void medAbstractVector3DParameterL::trigger()
{
    emit valueChanged(m_value);
}

//--------------------------------------------------------------------------
//  medAbstractVector4DParameterL
void medAbstractVector4DParameterL::setValue(const QVector4D &value)
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

QVector4D medAbstractVector4DParameterL::value() const
{
    return m_value;
}

void medAbstractVector4DParameterL::trigger()
{
    emit valueChanged(m_value);
}
