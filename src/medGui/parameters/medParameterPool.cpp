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
#include <QHashIterator>
#include <QDebug>

#include <medAbstractParameter.h>
#include <medCompositeParameter.h>

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
            if(!d->pool.values(param->name()).contains(param))
            {
                d->pool.insert(param->name(), param);
                connectParam(param);
            }
        }
    }
    else
    {
        if(!d->pool.values(parameter->name()).contains(parameter))
        {
            d->pool.insert(parameter->name(), parameter);
            connectParam(parameter);
        }
    }
}

void medParameterPool::append(QList<medAbstractParameter *> parameters)
{
    foreach(medAbstractParameter* param, parameters)
    {
        append(param);
    }
}

void medParameterPool::remove(medAbstractParameter* parameter)
{
    if(medAbstractGroupParameter *group = dynamic_cast<medAbstractGroupParameter *>(parameter))
    {
        foreach(medAbstractParameter *paramInGroup, group->parametersCandidateToPool())
        {
            remove(paramInGroup);
        }
        return;
    }

    disconnectParam(parameter);

    QHashIterator<QString, medAbstractParameter*> it(d->pool);

    while(it.hasNext())
    {
        it.next();
        if(it.value() == parameter)
            d->pool.remove(it.key(), it.value());
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
        disconnectParam(param);

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
        if(triggerParam && triggerParam != sender)
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
        if(boolParam && boolParam != sender)
        {
            boolParam->disconnect(this);
            boolParam->setValue(value);
            connect(boolParam, SIGNAL(valueChanged(bool)), this, SLOT(changeParamsValue(bool)));
        }
    }
}

void medParameterPool::changeParamsValue(int value)
{
    medAbstractIntParameter *sender = dynamic_cast<medAbstractIntParameter*>(QObject::sender());

    if(!sender)
        return;

    qDebug() << "pool size :"<<  d->pool.count();

    foreach(medAbstractParameter *param, d->pool.values(sender->name()))
    {
        medAbstractIntParameter *intParam = dynamic_cast<medAbstractIntParameter*>(param);
        if(intParam && intParam != sender)
        {
            intParam->disconnect(this);
            intParam->setValue(value);
            connect(intParam, SIGNAL(valueChanged(int)), this, SLOT(changeParamsValue(int)));
        }
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
        if(doubleParam && doubleParam != sender)
        {
            doubleParam->disconnect(this);
            doubleParam->setValue(value);
            connect(doubleParam, SIGNAL(valueChanged(double)), this, SLOT(changeParamsValue(double)));
        }
    }
}

void medParameterPool::changeParamsValue(const QString &value)
{
    medAbstractStringParameter *sender = dynamic_cast<medAbstractStringParameter*>(QObject::sender());

    if(!sender)
        return;

    foreach(medAbstractParameter *param, d->pool.values(sender->name()))
    {
        medAbstractStringParameter *listParam = dynamic_cast<medAbstractStringParameter*>(param);
        if(listParam && listParam != sender)
        {
            listParam->disconnect(this);
            listParam->setValue(value);
            connect(listParam, SIGNAL(valueChanged(QString)), this, SLOT(changeParamsValue(QString)));
        }
    }
}

void medParameterPool::changeParamsValue(const QVector2D& value)
{
    medAbstractVector2DParameter *sender = dynamic_cast<medAbstractVector2DParameter*>(QObject::sender());

    if(!sender)
        return;

    foreach(medAbstractParameter *param, d->pool.values(sender->name()))
    {
        medAbstractVector2DParameter *vector2DParam = dynamic_cast<medAbstractVector2DParameter*>(param);
        if(vector2DParam && vector2DParam != sender)
        {
            vector2DParam->disconnect(this);
            vector2DParam->setValue(value);
            connect(vector2DParam, SIGNAL(valueChanged(QVector2D)), this, SLOT(changeParamsValue(QVector2D)));
        }
    }
}

void medParameterPool::changeParamsValue(const QVector3D& value)
{
    medAbstractVector3DParameter *sender = dynamic_cast<medAbstractVector3DParameter*>(QObject::sender());

    if(!sender)
        return;

    foreach(medAbstractParameter *param, d->pool.values(sender->name()))
    {
        medAbstractVector3DParameter *vector3DParam = dynamic_cast<medAbstractVector3DParameter*>(param);
        if(vector3DParam && vector3DParam != sender)
        {
            vector3DParam->disconnect(this);
            vector3DParam->setValue(value);
            connect(vector3DParam, SIGNAL(valueChanged(QVector3D)), this, SLOT(changeParamsValue(QVector3D)));
        }
    }
}

void medParameterPool::changeParamsValue(const QVector4D& value)
{
    medAbstractVector4DParameter *sender = dynamic_cast<medAbstractVector4DParameter*>(QObject::sender());

    if(!sender)
        return;

    foreach(medAbstractParameter *param, d->pool.values(sender->name()))
    {
        medAbstractVector4DParameter *vector4DParam = dynamic_cast<medAbstractVector4DParameter*>(param);
        if(vector4DParam && vector4DParam != sender)
        {
            vector4DParam->disconnect(this);
            vector4DParam->setValue(value);
            connect(vector4DParam, SIGNAL(valueChanged(QVector4D)), this, SLOT(changeParamsValue(QVector4D)));
        }
    }
}

void medParameterPool::changeParamsValue(const QList<QVariant>& value)
{
    medCompositeParameter *sender = dynamic_cast<medCompositeParameter*>(QObject::sender());

    if(!sender)
        return;

    qDebug() << "pool size :"<<  d->pool.count();

    foreach(medAbstractParameter *param, d->pool.values(sender->name()))
    {
        medCompositeParameter *compositeParam = dynamic_cast<medCompositeParameter*>(param);
        if(compositeParam && compositeParam != sender)
        {
            compositeParam->disconnect(this);
            compositeParam->setValue(value);
            connect(compositeParam, SIGNAL(valueChanged(QList<QVariant>)), this, SLOT(changeParamsValue(QList<QVariant>)));
        }
    }

}

int medParameterPool::count() const
{
   return d->pool.keys().size();
}

void medParameterPool::removeInternParam()
{
    medAbstractParameter *param = dynamic_cast<medAbstractParameter*>(sender());
    if(param)
    {
        this->remove(param);
    }
}

void medParameterPool::connectParam(medAbstractParameter *parameter)
{
    connect(parameter, SIGNAL(aboutToBeDestroyed()), this, SLOT(removeInternParam()));

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
    else if(medCompositeParameter* param = dynamic_cast<medCompositeParameter*>(parameter))
        connect(param, SIGNAL(valueChanged(QList<QVariant>)), this, SLOT(changeParamsValue(QList<QVariant>)));
}

void medParameterPool::disconnectParam(medAbstractParameter *parameter)
{
    if(!parameter)
        return;

    parameter->disconnect(this);
}
