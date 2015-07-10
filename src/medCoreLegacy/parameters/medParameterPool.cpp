/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
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
#include <medDataListParameter.h>
#include <medStringListParameter.h>

#include <medDataIndex.h>

class medParameterPoolPrivate
{
public:
    QMultiHash<QString, medAbstractParameter*> pool;
    QString name;
    QColor color;

    //~medParameterPoolPrivate() {pool.clear();}
};

medParameterPool::medParameterPool(QObject* parent): d(new medParameterPoolPrivate)
{
    this->setParent(parent);
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
            connect(triggerParam, SIGNAL(triggered()), this, SLOT(triggerParams()), Qt::UniqueConnection);
            connect(triggerParam, SIGNAL(aboutToBeDestroyed()), this, SLOT(removeInternParam()), Qt::UniqueConnection);
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
            connect(boolParam, SIGNAL(valueChanged(bool)), this, SLOT(changeParamsValue(bool)), Qt::UniqueConnection);
            connect(boolParam, SIGNAL(aboutToBeDestroyed()), this, SLOT(removeInternParam()), Qt::UniqueConnection);
        }
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
        if(intParam && intParam != sender)
        {
            intParam->disconnect(this);
            intParam->setValue(value);
            connect(intParam, SIGNAL(valueChanged(int)), this, SLOT(changeParamsValue(int)), Qt::UniqueConnection);
            connect(intParam, SIGNAL(aboutToBeDestroyed()), this, SLOT(removeInternParam()), Qt::UniqueConnection);
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
            connect(doubleParam, SIGNAL(valueChanged(double)), this, SLOT(changeParamsValue(double)), Qt::UniqueConnection);
            connect(doubleParam, SIGNAL(aboutToBeDestroyed()), this, SLOT(removeInternParam()), Qt::UniqueConnection);
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
            connect(listParam, SIGNAL(valueChanged(QString)), this, SLOT(changeParamsValue(QString)), Qt::UniqueConnection);
            connect(listParam, SIGNAL(aboutToBeDestroyed()), this, SLOT(removeInternParam()), Qt::UniqueConnection);
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
            connect(vector2DParam, SIGNAL(valueChanged(QVector2D)), this, SLOT(changeParamsValue(QVector2D)), Qt::UniqueConnection);
            connect(vector2DParam, SIGNAL(aboutToBeDestroyed()), this, SLOT(removeInternParam()), Qt::UniqueConnection);
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
            connect(vector3DParam, SIGNAL(valueChanged(QVector3D)), this, SLOT(changeParamsValue(QVector3D)), Qt::UniqueConnection);
            connect(vector3DParam, SIGNAL(aboutToBeDestroyed()), this, SLOT(removeInternParam()), Qt::UniqueConnection);
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
            connect(vector4DParam, SIGNAL(valueChanged(QVector4D)), this, SLOT(changeParamsValue(QVector4D)), Qt::UniqueConnection);
            connect(vector4DParam, SIGNAL(aboutToBeDestroyed()), this, SLOT(removeInternParam()), Qt::UniqueConnection);
        }
    }
}

void medParameterPool::changeParamsValue(const QHash<QString, QVariant>& values)
{
    medCompositeParameter *sender = dynamic_cast<medCompositeParameter*>(QObject::sender());

    if(!sender)
        return;

    foreach(medAbstractParameter *param, d->pool.values(sender->name()))
    {
        medCompositeParameter *compositeParam = dynamic_cast<medCompositeParameter*>(param);
        if(compositeParam && compositeParam != sender)
        {
            compositeParam->disconnect(this);
            compositeParam->setValues(values);
            connect(compositeParam, SIGNAL(valuesChanged(QHash<QString,QVariant>)), this, SLOT(changeParamsValue(QHash<QString,QVariant>)), Qt::UniqueConnection);
            connect(compositeParam, SIGNAL(aboutToBeDestroyed()), this, SLOT(removeInternParam()), Qt::UniqueConnection);
        }
    }

}

void medParameterPool::changeParamsValue(const QList<medDataIndex>& values)
{
    medDataListParameter *sender = dynamic_cast<medDataListParameter*>(QObject::sender());
    
    if(!sender)
        return;
    
    foreach(medAbstractParameter *param, d->pool.values(sender->name()))
    {
        medDataListParameter *dataListParam = dynamic_cast<medDataListParameter*>(param);
        if(dataListParam && dataListParam != sender)
        {
            dataListParam->disconnect(this);
            dataListParam->setValues(values);
            connect(dataListParam, SIGNAL(valuesChanged(QList<medDataIndex>)), this, SLOT(changeParamsValue(QList<medDataIndex>)), Qt::UniqueConnection);
            connect(dataListParam, SIGNAL(aboutToBeDestroyed()), this, SLOT(removeInternParam()), Qt::UniqueConnection);
        }
    }
}

int medParameterPool::count() const
{
   return d->pool.keys().size();
}

void medParameterPool::removeInternParam()
{
    medAbstractParameter *param = qobject_cast<medAbstractParameter*>(QObject::sender());
    if(param)
        this->remove(param);
}

void medParameterPool::connectParam(medAbstractParameter *parameter)
{
    connect(parameter, SIGNAL(aboutToBeDestroyed()), this, SLOT(removeInternParam()), Qt::UniqueConnection);

    if(medAbstractTriggerParameter* param = dynamic_cast<medAbstractTriggerParameter*>(parameter))
        connect(param, SIGNAL(triggered()), this, SLOT(triggerParams()), Qt::UniqueConnection);
    else if(medAbstractBoolParameter* param = dynamic_cast<medAbstractBoolParameter*>(parameter))
        connect(param, SIGNAL(valueChanged(bool)), this, SLOT(changeParamsValue(bool)), Qt::UniqueConnection);
    else if(medAbstractDoubleParameter* param = dynamic_cast<medAbstractDoubleParameter*>(parameter))
        connect(param, SIGNAL(valueChanged(double)), this, SLOT(changeParamsValue(double)), Qt::UniqueConnection);
    else if(medAbstractIntParameter* param = dynamic_cast<medAbstractIntParameter*>(parameter))
        connect(param, SIGNAL(valueChanged(int)), this, SLOT(changeParamsValue(int)), Qt::UniqueConnection);
    else if(medAbstractStringParameter* param = dynamic_cast<medAbstractStringParameter*>(parameter))
        connect(param, SIGNAL(valueChanged(QString)), this, SLOT(changeParamsValue(QString)), Qt::UniqueConnection);
    else if(medAbstractVector2DParameter* param = dynamic_cast<medAbstractVector2DParameter*>(parameter))
        connect(param, SIGNAL(valueChanged(QVector2D)), this, SLOT(changeParamsValue(QVector2D)), Qt::UniqueConnection);
    else if(medAbstractVector3DParameter* param = dynamic_cast<medAbstractVector3DParameter*>(parameter))
        connect(param, SIGNAL(valueChanged(QVector3D)), this, SLOT(changeParamsValue(QVector3D)), Qt::UniqueConnection);
    else if(medAbstractVector4DParameter* param = dynamic_cast<medAbstractVector4DParameter*>(parameter))
        connect(param, SIGNAL(valueChanged(QVector4D)), this, SLOT(changeParamsValue(QVector4D)), Qt::UniqueConnection);
    else if(medCompositeParameter* param = dynamic_cast<medCompositeParameter*>(parameter))
        connect(param, SIGNAL(valuesChanged(QHash<QString,QVariant>)), this, SLOT(changeParamsValue(QHash<QString,QVariant>)), Qt::UniqueConnection);
	else if(medDataListParameter* param = dynamic_cast<medDataListParameter*>(parameter))
        connect(param, SIGNAL(valuesChanged(QList<medDataIndex>)), this, SLOT(changeParamsValue(QList<medDataIndex>)), Qt::UniqueConnection);

}

void medParameterPool::disconnectParam(medAbstractParameter *parameter)
{
    if(!parameter)
        return;

    parameter->disconnect(this);
}



