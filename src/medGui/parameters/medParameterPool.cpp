/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medParameterPool.h>

#include <QMultiHash>
#include <QDebug>

#include <medAbstractParameter.h>

class medParameterPoolPrivate
{
public:
    QMultiHash<QString, medAbstractParameter*> pool;
    QString name;
    QColor color;

    ~medParameterPoolPrivate() {pool.clear();}
};

medParameterPool::medParameterPool(QObject* parent): d(new medParameterPoolPrivate)
{
    d->color = QColor("black");
}

medParameterPool::~medParameterPool()
{
    delete d;
}

QString medParameterPool::name() const
{
    return d->name;

}
void medParameterPool::setName(QString name)
{
    d->name = name;
}

QColor medParameterPool::color()
{
    return d->color;
}

void medParameterPool::setColor(QColor color)
{
    d->color = color;
}

void medParameterPool::append(medAbstractParameter *parameter)
{
    if(medAbstractGroupParameter* group = dynamic_cast<medAbstractGroupParameter*>(parameter))
    {
        foreach(medAbstractParameter* param, group->parametersCandidateToPool())
        {
            d->pool.insert(param->name(), param);
            _prvt_connectParam(param);
        }
    }
    else
    {
        d->pool.insert(parameter->name(), parameter);
        _prvt_connectParam(parameter);
    }
}

void medParameterPool::remove(medAbstractParameter* parameter)
{
    QMultiHash<QString, medAbstractParameter*>::Iterator it;
    for(it = d->pool.begin(); it != d->pool.end(); ++it)
    {
        if(it.value() == parameter)
            it = d->pool.erase(it);

        if( d->pool.count() == 0 )
            break;
    }
}

void medParameterPool::removeAll (QString name)
{
    foreach(medAbstractParameter* param, parameters(name))
        remove(param);
}

void medParameterPool::clear()
{
    foreach(medAbstractParameter* param, d->pool)
        _prvt_disconnectParam(param);

    d->pool.clear();
}

QList<medAbstractParameter*> medParameterPool::parameters(QString name)
{
    return d->pool.values(name);
}

QList<medAbstractParameter*> medParameterPool::parameters()
{
    return d->pool.values();
}

QStringList medParameterPool::parametersNames()
{
    return d->pool.keys();
}

void medParameterPool::triggerParams()
{
    medAbstractTriggerParameter *sender = dynamic_cast<medAbstractTriggerParameter*>(QObject::sender());

    if(!sender)
        return;

    foreach(medAbstractParameter *param, d->pool.values(sender->name()))
    {
        medAbstractTriggerParameter *triggerParam = dynamic_cast<medAbstractTriggerParameter*>(param);
        if(triggerParam != sender)
        {
            triggerParam->disconnect(this);
            triggerParam->trigger();
            connect(triggerParam, SIGNAL(triggered()), this, SLOT(triggerParams()));
        }
    }
}

void medParameterPool::changeParamsValue(bool value)
{
    medAbstractBoolParameter *sender = dynamic_cast<medAbstractBoolParameter*>(QObject::sender());

    if(!sender)
        return;

    foreach(medAbstractParameter *param, d->pool.values(sender->name()))
    {
        medAbstractBoolParameter *boolParam = dynamic_cast<medAbstractBoolParameter*>(param);
        if(boolParam != sender)
            boolParam->setValue(value);
    }
}

void medParameterPool::changeParamsValue(int value)
{
    medAbstractIntParameter *sender = dynamic_cast<medAbstractIntParameter*>(QObject::sender());

    if(!sender)
        return;

    foreach(medAbstractParameter *param, d->pool.values(sender->name()))
    {
        medAbstractIntParameter *intParam = dynamic_cast<medAbstractIntParameter*>(param);
        if(intParam != sender)
            intParam->setValue(value);
    }
}

void medParameterPool::changeParamsValue(double value)
{
    medAbstractDoubleParameter *sender = dynamic_cast<medAbstractDoubleParameter*>(QObject::sender());

    if(!sender)
        return;

    foreach(medAbstractParameter *param, d->pool.values(sender->name()))
    {
        medAbstractDoubleParameter *doubleParam = dynamic_cast<medAbstractDoubleParameter*>(param);
        if(doubleParam != sender)
            doubleParam->setValue(value);
    }
}

void medParameterPool::changeParamsValue(QString& value)
{
    medAbstractStringParameter *sender = dynamic_cast<medAbstractStringParameter*>(QObject::sender());

    if(!sender)
        return;

    foreach(medAbstractParameter *param, d->pool.values(sender->name()))
    {
        medAbstractStringParameter *listParam = dynamic_cast<medAbstractStringParameter*>(param);
        if(listParam != sender)
            listParam->setValue(value);
    }
}

void medParameterPool::changeParamsValue(const QVector2D& value)
{
    medAbstractVector2DParameter *sender = dynamic_cast<medAbstractVector2DParameter*>(QObject::sender());

    if(!sender)
        return;

    foreach(medAbstractParameter *param, d->pool.values(sender->name()))
    {
        medAbstractVector2DParameter *listParam = dynamic_cast<medAbstractVector2DParameter*>(param);
        if(listParam != sender)
            listParam->setValue(value);
    }
}

void medParameterPool::changeParamsValue(const QVector3D& value)
{
    medAbstractVector3DParameter *sender = dynamic_cast<medAbstractVector3DParameter*>(QObject::sender());

    if(!sender)
        return;

    foreach(medAbstractParameter *param, d->pool.values(sender->name()))
    {
        medAbstractVector3DParameter *listParam = dynamic_cast<medAbstractVector3DParameter*>(param);
        if(listParam != sender)
            listParam->setValue(value);
    }
}

void medParameterPool::changeParamsValue(const QVector4D& value)
{
    medAbstractVector4DParameter *sender = dynamic_cast<medAbstractVector4DParameter*>(QObject::sender());

    if(!sender)
        return;

    foreach(medAbstractParameter *param, d->pool.values(sender->name()))
    {
        medAbstractVector4DParameter *listParam = dynamic_cast<medAbstractVector4DParameter*>(param);
        if(listParam != sender)
            listParam->setValue(value);
    }
}

int medParameterPool::count() const
{
   return d->pool.keys().size();
}

void medParameterPool::_prvt_removeInternParam()
{
    medAbstractParameter *param = dynamic_cast<medAbstractParameter*>(sender());
    if(param)
    {
        this->remove(param);
        qDebug()<< "remove param";
    }
}

void medParameterPool::_prvt_connectParam(medAbstractParameter *parameter)
{
    connect(parameter, SIGNAL(aboutToBeDestroyed()), this, SLOT(_prvt_removeInternParam()));

    if(medAbstractTriggerParameter* param = dynamic_cast<medAbstractTriggerParameter*>(parameter))
        connect(param, SIGNAL(triggered()), this, SLOT(triggerParams()));
    else if(medAbstractBoolParameter* param = dynamic_cast<medAbstractBoolParameter*>(parameter))
        connect(param, SIGNAL(valueChanged(bool)), this, SLOT(changeParamsValue(bool)));
    else if(medAbstractDoubleParameter* param = dynamic_cast<medAbstractDoubleParameter*>(parameter))
        connect(param, SIGNAL(valueChanged(double)), this, SLOT(changeParamsValue(double)));
    else if(medAbstractIntParameter* param = dynamic_cast<medAbstractIntParameter*>(parameter))
        connect(param, SIGNAL(valueChanged(int)), this, SLOT(changeParamsValue(int)));
    else if(medAbstractStringParameter* param = dynamic_cast<medAbstractStringParameter*>(parameter))
        connect(param, SIGNAL(valueChanged(QString)), this, SLOT(changeParamsValue(QString)));
    else if(medAbstractVector2DParameter* param = dynamic_cast<medAbstractVector2DParameter*>(parameter))
        connect(param, SIGNAL(valueChanged(QVector2D)), this, SLOT(changeParamsValue(QVector2D)));
    else if(medAbstractVector3DParameter* param = dynamic_cast<medAbstractVector3DParameter*>(parameter))
        connect(param, SIGNAL(valueChanged(QVector3D)), this, SLOT(changeParamsValue(QVector3D)));
    else if(medAbstractVector4DParameter* param = dynamic_cast<medAbstractVector4DParameter*>(parameter))
        connect(param, SIGNAL(valueChanged(QVector4D)), this, SLOT(changeParamsValue(QVector4D)));
}

void medParameterPool::_prvt_disconnectParam(medAbstractParameter *parameter)
{
    parameter->disconnect(this);
}
