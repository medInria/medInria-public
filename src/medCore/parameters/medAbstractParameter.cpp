/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractParameter.h>

#include <QtGui>


class medAbstractParameterPrivate
{
public:
    QString name;
    QList <QWidget *> internWidget;
    QLabel *label;
    QString toolTip;

    bool hide;

    ~medAbstractParameterPrivate() {delete label;}
};

medAbstractParameter::medAbstractParameter(QString name, QObject *parent):
    d(new medAbstractParameterPrivate)
{
    this->setParent(parent);
    d->label = NULL;
    d->toolTip = QString();
    this->setName(name);
    d->hide = false;
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

void medAbstractParameter::setToolTip(QString tooltip)
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
    d->hide = false;

    foreach (QWidget *widget, d->internWidget)
        widget->show();
}

void medAbstractParameter::hide()
{
    d->hide = true;

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
    if(d->hide)
        widget->hide();

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

void medAbstractParameter::toXMLNode(QDomDocument* doc,QDomElement* currentNode)
{
    QDomElement elmt=doc->createElement("name");
    elmt.appendChild(doc->createTextNode(name()));
    currentNode->appendChild(elmt);
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

void medAbstractStringParameter::trigger()
{
    emit valueChanged(m_value);
}

QString medAbstractStringParameter::fromString(QString value)
{
    return value;
}

void medAbstractStringParameter::toXMLNode(QDomDocument* doc,QDomElement* currentNode)
{
    medAbstractParameter::toXMLNode(doc,currentNode);
    QDomElement type=doc->createElement("type");
    type.appendChild(doc->createTextNode("String"));
    currentNode->appendChild(type);
    QDomElement elmt=doc->createElement("value");
    elmt.appendChild(doc->createTextNode(value()));
    currentNode->appendChild(elmt);
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
int medAbstractIntParameter::fromString(QString value)
{
    return value.toInt();
}

void medAbstractIntParameter::toXMLNode(QDomDocument* doc,QDomElement* currentNode)
{
    medAbstractParameter::toXMLNode(doc,currentNode);
    QDomElement type=doc->createElement("type");
    type.appendChild(doc->createTextNode("Int"));
    currentNode->appendChild(type);
    QDomElement elmt=doc->createElement("value");
    elmt.appendChild(doc->createTextNode(QString::number(value())));
    currentNode->appendChild(elmt);
}


void medAbstractIntParameter::trigger()
{
    emit valueChanged(m_value);
}

int medAbstractIntParameter::value() const
{
    return m_value;
}

void medAbstractIntParameter::trigger()
{
    emit valueChanged(m_value);
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

void medAbstractDoubleParameter::trigger()
{
    emit valueChanged(m_value);
}

double medAbstractDoubleParameter::fromString(QString value)
{
    return value.toDouble();
}

void medAbstractDoubleParameter::toXMLNode(QDomDocument* doc,QDomElement* currentNode)
{
    medAbstractParameter::toXMLNode(doc,currentNode);
    QDomElement type=doc->createElement("type");
    type.appendChild(doc->createTextNode("Double"));
    currentNode->appendChild(type);
    QDomElement elmt=doc->createElement("value");
    elmt.appendChild(doc->createTextNode(QString::number(value())));
    currentNode->appendChild(elmt);
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

void medAbstractBoolParameter::trigger()
{
    emit valueChanged(m_value);
}


bool medAbstractBoolParameter::fromString(QString value)
{
    return value=="true";
}

void medAbstractBoolParameter::toXMLNode(QDomDocument* doc,QDomElement* currentNode)
{
    medAbstractParameter::toXMLNode(doc,currentNode);
    QDomElement type=doc->createElement("type");
    type.appendChild(doc->createTextNode("Bool"));
    currentNode->appendChild(type);
    QDomElement elmt=doc->createElement("value");
    if(value())
        elmt.appendChild(doc->createTextNode("true"));
    else
        elmt.appendChild(doc->createTextNode("false"));

    currentNode->appendChild(elmt);
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

void medAbstractVector2DParameter::trigger()
{
    emit valueChanged(m_value);
}

QVector2D medAbstractVector2DParameter::fromString(QString value)
{
    QStringList tokens=value.split(" ");
    return QVector2D(tokens[0].toDouble(),tokens[1].toDouble());
}


void medAbstractVector2DParameter::toXMLNode(QDomDocument* doc,QDomElement* currentNode)
{
    medAbstractParameter::toXMLNode(doc,currentNode);
    QDomElement type=doc->createElement("type");
    type.appendChild(doc->createTextNode("Vector2D"));
    currentNode->appendChild(type);
    QDomElement elmt=doc->createElement("value");
    elmt.appendChild(doc->createTextNode(QString::number(value().x())+" "+QString::number(value().y())));
    currentNode->appendChild(elmt);
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
QVector3D medAbstractVector3DParameter::fromString(QString value)
{
    QStringList tokens=value.split(" ");
    return QVector3D(tokens[0].toDouble(),tokens[1].toDouble(),tokens[2].toDouble());
}

void medAbstractVector3DParameter::toXMLNode(QDomDocument* doc,QDomElement* currentNode)
{
    medAbstractParameter::toXMLNode(doc,currentNode);
    QDomElement type=doc->createElement("type");
    type.appendChild(doc->createTextNode("Vector3D"));
    currentNode->appendChild(type);
    QDomElement elmt=doc->createElement("value");
    elmt.appendChild(doc->createTextNode(QString::number(value().x())+" "+QString::number(value().y())+" "+QString::number(value().z())));
    currentNode->appendChild(elmt);
}

QVector3D medAbstractVector3DParameter::value() const
{
    return m_value;
}

void medAbstractVector3DParameter::trigger()
{
    emit valueChanged(m_value);
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

void medAbstractVector4DParameter::trigger()
{
    emit valueChanged(m_value);
}
    
QVector4D medAbstractVector4DParameter::fromString(QString value)
{
    QStringList tokens=value.split(" ");
    return QVector4D(tokens[0].toDouble(),tokens[1].toDouble(),tokens[2].toDouble(),tokens[3].toDouble());
}

void medAbstractVector4DParameter::toXMLNode(QDomDocument* doc,QDomElement* currentNode)
{
    medAbstractParameter::toXMLNode(doc,currentNode);
    QDomElement type=doc->createElement("type");
    currentNode->appendChild(type);
    type.appendChild(doc->createTextNode("Vector4D"));
    QDomElement elmt=doc->createElement("value");
    elmt.appendChild(doc->createTextNode(QString::number(value().x())+" "+QString::number(value().y())+" "+QString::number(value().z())+" "+QString::number(value().w())));
    currentNode->appendChild(elmt);
}

void medAbstractVector4DParameter::trigger()
{
    emit valueChanged(m_value);
}
