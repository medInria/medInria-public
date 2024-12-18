/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.

 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

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
    QPointer<QLabel> label;
    QString toolTip;
    bool hide;
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

void medAbstractParameterL::toXMLNode(QDomDocument *doc, QDomElement *currentNode)
{
    QDomElement element = doc->createElement("name");
    element.appendChild(doc->createTextNode(name()));
    currentNode->appendChild(element);
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

QString medAbstractStringParameterL::fromString(QString value)
{
    return value;
}

void medAbstractStringParameterL::toXMLNode(QDomDocument *doc, QDomElement *currentNode)
{
    medAbstractParameterL::toXMLNode(doc, currentNode);
    QDomElement type = doc->createElement("type");
    type.appendChild(doc->createTextNode("String"));
    currentNode->appendChild(type);
    QDomElement element = doc->createElement("value");
    element.appendChild(doc->createTextNode(value()));
    currentNode->appendChild(element);
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

int medAbstractIntParameterL::fromString(QString value)
{
    return value.toInt();
}

void medAbstractIntParameterL::toXMLNode(QDomDocument *doc, QDomElement *currentNode)
{
    medAbstractParameterL::toXMLNode(doc, currentNode);
    QDomElement type = doc->createElement("type");
    type.appendChild(doc->createTextNode("Int"));
    currentNode->appendChild(type);
    QDomElement element = doc->createElement("value");
    element.appendChild(doc->createTextNode(QString::number(value())));
    currentNode->appendChild(element);
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

double medAbstractDoubleParameterL::fromString(QString value)
{
    return value.toDouble();
}

void medAbstractDoubleParameterL::toXMLNode(QDomDocument *doc, QDomElement *currentNode)
{
    medAbstractParameterL::toXMLNode(doc, currentNode);
    QDomElement type = doc->createElement("type");
    type.appendChild(doc->createTextNode("Double"));
    currentNode->appendChild(type);
    QDomElement element = doc->createElement("value");
    element.appendChild(doc->createTextNode(QString::number(value())));
    currentNode->appendChild(element);
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

bool medAbstractBoolParameterL::fromString(QString value)
{
    return value == "true";
}

void medAbstractBoolParameterL::toXMLNode(QDomDocument *doc, QDomElement *currentNode)
{
    medAbstractParameterL::toXMLNode(doc, currentNode);
    QDomElement type = doc->createElement("type");
    type.appendChild(doc->createTextNode("Bool"));
    currentNode->appendChild(type);
    QDomElement element = doc->createElement("value");
    if (value())
    {
        element.appendChild(doc->createTextNode("true"));
    }
    else
    {
        element.appendChild(doc->createTextNode("false"));
    }

    currentNode->appendChild(element);
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

void medAbstractVector2DParameterL::toXMLNode(QDomDocument *doc, QDomElement *currentNode)
{
    medAbstractParameterL::toXMLNode(doc, currentNode);
    QDomElement type = doc->createElement("type");
    type.appendChild(doc->createTextNode("Vector2D"));
    currentNode->appendChild(type);
    QDomElement element = doc->createElement("value");
    element.appendChild(doc->createTextNode(QString::number(value().x()) + " " + QString::number(value().y())));
    currentNode->appendChild(element);
}

QVector2D medAbstractVector2DParameterL::fromString(QString value)
{
    QStringList tokens = value.split(" ");
    return QVector2D(tokens[0].toDouble(),tokens[1].toDouble());
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

QVector3D medAbstractVector3DParameterL::fromString(QString value)
{
    QStringList tokens = value.split(" ");
    return QVector3D(tokens[0].toDouble(),tokens[1].toDouble(),tokens[2].toDouble());
}

void medAbstractVector3DParameterL::toXMLNode(QDomDocument *doc, QDomElement *currentNode)
{
    medAbstractParameterL::toXMLNode(doc, currentNode);
    QDomElement type = doc->createElement("type");
    type.appendChild(doc->createTextNode("Vector3D"));
    currentNode->appendChild(type);
    QDomElement element = doc->createElement("value");
    element.appendChild(doc->createTextNode(QString::number(value().x())
                                            + " "
                                            + QString::number(value().y())
                                            + " "
                                            + QString::number(value().z())));
    currentNode->appendChild(element);
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

QVector4D medAbstractVector4DParameterL::fromString(QString value)
{
    QStringList tokens = value.split(" ");
    return QVector4D(tokens[0].toDouble(),tokens[1].toDouble(),tokens[2].toDouble(),tokens[3].toDouble());
}

void medAbstractVector4DParameterL::toXMLNode(QDomDocument *doc, QDomElement *currentNode)
{
    medAbstractParameterL::toXMLNode(doc, currentNode);
    QDomElement type = doc->createElement("type");
    currentNode->appendChild(type);
    type.appendChild(doc->createTextNode("Vector4D"));
    QDomElement element = doc->createElement("value");
    element.appendChild(doc->createTextNode(QString::number(value().x()) 
                                            + " " 
                                            + QString::number(value().y())
                                            + " "
                                            + QString::number(value().z())
                                            + " "
                                            + QString::number(value().w())));
    currentNode->appendChild(element);
}
