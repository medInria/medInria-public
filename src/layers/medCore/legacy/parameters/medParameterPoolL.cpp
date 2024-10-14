/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medParameterPoolL.h>

#include <QMultiHash>
#include <QHashIterator>
#include <dtkLog>

#include <medAbstractParameterL.h>
#include <medCompositeParameterL.h>
#include <medDataIndex.h>
#include <medDataListParameterL.h>
#include <medStringListParameterL.h>

class medParameterPoolLPrivate
{
public:
    QMultiHash<QString, medAbstractParameterL*> pool;
    QString name;
    QColor color;
};

medParameterPoolL::medParameterPoolL(QObject* parent): d(new medParameterPoolLPrivate)
{
    this->setParent(parent);
    d->color = QColor("black");
}

medParameterPoolL::~medParameterPoolL()
{
    delete d;
}

QString medParameterPoolL::name() const
{
    return d->name;

}
void medParameterPoolL::setName(QString name)
{
    d->name = name;
}

QColor medParameterPoolL::color()
{
    return d->color;
}

void medParameterPoolL::setColor(QColor color)
{
    d->color = color;
}

void medParameterPoolL::append(medAbstractParameterL *parameter)
{
    if(medAbstractGroupParameterL* group = dynamic_cast<medAbstractGroupParameterL*>(parameter))
    {
        for(medAbstractParameterL* param : group->parametersCandidateToPool())
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

void medParameterPoolL::append(QList<medAbstractParameterL *> parameters)
{
    for(medAbstractParameterL* param : parameters)
    {
        append(param);
    }
}

void medParameterPoolL::remove(medAbstractParameterL* parameter)
{
    if(medAbstractGroupParameterL *group = dynamic_cast<medAbstractGroupParameterL *>(parameter))
    {
        for(medAbstractParameterL *paramInGroup : group->parametersCandidateToPool())
        {
            remove(paramInGroup);
        }
        return;
    }

    disconnectParam(parameter);

    QMutableHashIterator<QString, medAbstractParameterL*> it(d->pool);
    while(it.hasNext())
    {
        it.next();
        if(it.value() && it.value() == parameter)
        {
            it.remove();
        }
    }
}

void medParameterPoolL::removeAll (QString name)
{
    for(medAbstractParameterL* param : parameters(name))
    {
        remove(param);
    }
}

void medParameterPoolL::clear()
{
    for(medAbstractParameterL* param : d->pool)
    {
        disconnectParam(param);
    }

    d->pool.clear();
}

QList<medAbstractParameterL*> medParameterPoolL::parameters(QString name)
{
    return d->pool.values(name);
}

QList<medAbstractParameterL*> medParameterPoolL::parameters()
{
    return d->pool.values();
}

QStringList medParameterPoolL::parametersNames()
{
    return d->pool.keys();
}

void medParameterPoolL::triggerParams()
{
    medAbstractTriggerParameterL *sender = dynamic_cast<medAbstractTriggerParameterL*>(QObject::sender());

    if(!sender)
        return;

    for(medAbstractParameterL *param : d->pool.values(sender->name()))
    {
        medAbstractTriggerParameterL *triggerParam = dynamic_cast<medAbstractTriggerParameterL*>(param);
        if(triggerParam && triggerParam != sender)
        {
            triggerParam->disconnect(this);
            triggerParam->trigger();
            connect(triggerParam, SIGNAL(triggered()), this, SLOT(triggerParams()), Qt::UniqueConnection);
            connect(triggerParam, SIGNAL(aboutToBeDestroyed()), this, SLOT(removeInternParam()), Qt::UniqueConnection);
        }
    }
}

void medParameterPoolL::changeParamsValue(bool value)
{
    medAbstractBoolParameterL *sender = dynamic_cast<medAbstractBoolParameterL*>(QObject::sender());

    if(!sender)
        return;

    for(medAbstractParameterL *param : d->pool.values(sender->name()))
    {
        medAbstractBoolParameterL *boolParam = dynamic_cast<medAbstractBoolParameterL*>(param);
        if(boolParam && boolParam != sender)
        {
            boolParam->disconnect(this);
            boolParam->setValue(value);
            connect(boolParam, SIGNAL(valueChanged(bool)), this, SLOT(changeParamsValue(bool)), Qt::UniqueConnection);
            connect(boolParam, SIGNAL(aboutToBeDestroyed()), this, SLOT(removeInternParam()), Qt::UniqueConnection);
        }
    }
}

void medParameterPoolL::changeParamsValue(int value)
{
    medAbstractIntParameterL *sender = dynamic_cast<medAbstractIntParameterL*>(QObject::sender());

    if(!sender)
        return;

    for(medAbstractParameterL *param : d->pool.values(sender->name()))
    {
        medAbstractIntParameterL *intParam = dynamic_cast<medAbstractIntParameterL*>(param);
        if(intParam && intParam != sender)
        {
            intParam->disconnect(this);
            intParam->setValue(value);
            connect(intParam, SIGNAL(valueChanged(int)), this, SLOT(changeParamsValue(int)), Qt::UniqueConnection);
            connect(intParam, SIGNAL(aboutToBeDestroyed()), this, SLOT(removeInternParam()), Qt::UniqueConnection);
        }
    }
}

void medParameterPoolL::changeParamsValue(double value)
{
    medAbstractDoubleParameterL *sender = dynamic_cast<medAbstractDoubleParameterL*>(QObject::sender());

    if(!sender)
        return;

    for(medAbstractParameterL *param : d->pool.values(sender->name()))
    {
        medAbstractDoubleParameterL *doubleParam = dynamic_cast<medAbstractDoubleParameterL*>(param);
        if(doubleParam && doubleParam != sender)
        {
            doubleParam->disconnect(this);
            doubleParam->setValue(value);
            connect(doubleParam, SIGNAL(valueChanged(double)), this, SLOT(changeParamsValue(double)), Qt::UniqueConnection);
            connect(doubleParam, SIGNAL(aboutToBeDestroyed()), this, SLOT(removeInternParam()), Qt::UniqueConnection);
        }
    }
}

void medParameterPoolL::changeParamsValue(const QString &value)
{
    medAbstractStringParameterL *sender = dynamic_cast<medAbstractStringParameterL*>(QObject::sender());

    if(!sender)
        return;

    for(medAbstractParameterL *param : d->pool.values(sender->name()))
    {
        medAbstractStringParameterL *listParam = dynamic_cast<medAbstractStringParameterL*>(param);
        if(listParam && listParam != sender)
        {
            listParam->disconnect(this);
            listParam->setValue(value);
            connect(listParam, SIGNAL(valueChanged(QString)), this, SLOT(changeParamsValue(QString)), Qt::UniqueConnection);
            connect(listParam, SIGNAL(aboutToBeDestroyed()), this, SLOT(removeInternParam()), Qt::UniqueConnection);
        }
    }
}

void medParameterPoolL::changeParamsValue(const QVector2D& value)
{
    medAbstractVector2DParameterL *sender = dynamic_cast<medAbstractVector2DParameterL*>(QObject::sender());

    if(!sender)
        return;

    for(medAbstractParameterL *param : d->pool.values(sender->name()))
    {
        medAbstractVector2DParameterL *vector2DParam = dynamic_cast<medAbstractVector2DParameterL*>(param);
        if(vector2DParam && vector2DParam != sender)
        {
            vector2DParam->disconnect(this);
            vector2DParam->setValue(value);
            connect(vector2DParam, SIGNAL(valueChanged(QVector2D)), this, SLOT(changeParamsValue(QVector2D)), Qt::UniqueConnection);
            connect(vector2DParam, SIGNAL(aboutToBeDestroyed()), this, SLOT(removeInternParam()), Qt::UniqueConnection);
        }
    }
}

void medParameterPoolL::changeParamsValue(const QVector3D& value)
{
    medAbstractVector3DParameterL *sender = dynamic_cast<medAbstractVector3DParameterL*>(QObject::sender());

    if(!sender)
        return;

    for(medAbstractParameterL *param : d->pool.values(sender->name()))
    {
        medAbstractVector3DParameterL *vector3DParam = dynamic_cast<medAbstractVector3DParameterL*>(param);
        if(vector3DParam && vector3DParam != sender)
        {
            vector3DParam->disconnect(this);
            vector3DParam->setValue(value);
            connect(vector3DParam, SIGNAL(valueChanged(QVector3D)), this, SLOT(changeParamsValue(QVector3D)), Qt::UniqueConnection);
            connect(vector3DParam, SIGNAL(aboutToBeDestroyed()), this, SLOT(removeInternParam()), Qt::UniqueConnection);
        }
    }
}

void medParameterPoolL::changeParamsValue(const QVector4D& value)
{
    medAbstractVector4DParameterL *sender = dynamic_cast<medAbstractVector4DParameterL*>(QObject::sender());

    if(!sender)
        return;

    for(medAbstractParameterL *param : d->pool.values(sender->name()))
    {
        medAbstractVector4DParameterL *vector4DParam = dynamic_cast<medAbstractVector4DParameterL*>(param);
        if(vector4DParam && vector4DParam != sender)
        {
            vector4DParam->disconnect(this);
            vector4DParam->setValue(value);
            connect(vector4DParam, SIGNAL(valueChanged(QVector4D)), this, SLOT(changeParamsValue(QVector4D)), Qt::UniqueConnection);
            connect(vector4DParam, SIGNAL(aboutToBeDestroyed()), this, SLOT(removeInternParam()), Qt::UniqueConnection);
        }
    }
}

void medParameterPoolL::changeParamsValue(const QHash<QString, QVariant>& values)
{
    medCompositeParameterL *sender = dynamic_cast<medCompositeParameterL*>(QObject::sender());

    if(!sender)
        return;

    for(medAbstractParameterL *param : d->pool.values(sender->name()))
    {
        medCompositeParameterL *compositeParam = dynamic_cast<medCompositeParameterL*>(param);
        if(compositeParam && compositeParam != sender)
        {
            compositeParam->disconnect(this);
            compositeParam->setValues(values);
            connect(compositeParam, SIGNAL(valuesChanged(QHash<QString,QVariant>)), this, SLOT(changeParamsValue(QHash<QString,QVariant>)), Qt::UniqueConnection);
            connect(compositeParam, SIGNAL(aboutToBeDestroyed()), this, SLOT(removeInternParam()), Qt::UniqueConnection);
        }
    }

}

void medParameterPoolL::changeParamsValue(const QList<medDataIndex>& values)
{
    medDataListParameterL *sender = dynamic_cast<medDataListParameterL*>(QObject::sender());

    if(!sender)
        return;

    for(medAbstractParameterL *param : d->pool.values(sender->name()))
    {
        medDataListParameterL *dataListParam = dynamic_cast<medDataListParameterL*>(param);
        if(dataListParam && dataListParam != sender)
        {
            dataListParam->disconnect(this);
            dataListParam->setValues(values);
            connect(dataListParam, SIGNAL(valuesChanged(QList<medDataIndex>)), this, SLOT(changeParamsValue(QList<medDataIndex>)), Qt::UniqueConnection);
            connect(dataListParam, SIGNAL(aboutToBeDestroyed()), this, SLOT(removeInternParam()), Qt::UniqueConnection);
        }
    }
}

int medParameterPoolL::count() const
{
   return d->pool.keys().size();
}

void medParameterPoolL::removeInternParam()
{
    medAbstractParameterL *param = qobject_cast<medAbstractParameterL*>(QObject::sender());
    if(param)
        this->remove(param);
}

void medParameterPoolL::connectParam(medAbstractParameterL *parameter)
{
    connect(parameter, SIGNAL(aboutToBeDestroyed()), this, SLOT(removeInternParam()), Qt::UniqueConnection);

    if(medAbstractTriggerParameterL* param = dynamic_cast<medAbstractTriggerParameterL*>(parameter))
        connect(param, SIGNAL(triggered()), this, SLOT(triggerParams()), Qt::UniqueConnection);
    else if(medAbstractBoolParameterL* param = dynamic_cast<medAbstractBoolParameterL*>(parameter))
        connect(param, SIGNAL(valueChanged(bool)), this, SLOT(changeParamsValue(bool)), Qt::UniqueConnection);
    else if(medAbstractDoubleParameterL* param = dynamic_cast<medAbstractDoubleParameterL*>(parameter))
        connect(param, SIGNAL(valueChanged(double)), this, SLOT(changeParamsValue(double)), Qt::UniqueConnection);
    else if(medAbstractIntParameterL* param = dynamic_cast<medAbstractIntParameterL*>(parameter))
        connect(param, SIGNAL(valueChanged(int)), this, SLOT(changeParamsValue(int)), Qt::UniqueConnection);
    else if(medAbstractStringParameterL* param = dynamic_cast<medAbstractStringParameterL*>(parameter))
        connect(param, SIGNAL(valueChanged(QString)), this, SLOT(changeParamsValue(QString)), Qt::UniqueConnection);
    else if(medAbstractVector2DParameterL* param = dynamic_cast<medAbstractVector2DParameterL*>(parameter))
        connect(param, SIGNAL(valueChanged(QVector2D)), this, SLOT(changeParamsValue(QVector2D)), Qt::UniqueConnection);
    else if(medAbstractVector3DParameterL* param = dynamic_cast<medAbstractVector3DParameterL*>(parameter))
        connect(param, SIGNAL(valueChanged(QVector3D)), this, SLOT(changeParamsValue(QVector3D)), Qt::UniqueConnection);
    else if(medAbstractVector4DParameterL* param = dynamic_cast<medAbstractVector4DParameterL*>(parameter))
        connect(param, SIGNAL(valueChanged(QVector4D)), this, SLOT(changeParamsValue(QVector4D)), Qt::UniqueConnection);
    else if(medCompositeParameterL* param = dynamic_cast<medCompositeParameterL*>(parameter))
        connect(param, SIGNAL(valuesChanged(QHash<QString,QVariant>)), this, SLOT(changeParamsValue(QHash<QString,QVariant>)), Qt::UniqueConnection);
    else if(medDataListParameterL* param = dynamic_cast<medDataListParameterL*>(parameter))
        connect(param, SIGNAL(valuesChanged(QList<medDataIndex>)), this, SLOT(changeParamsValue(QList<medDataIndex>)), Qt::UniqueConnection);

}

void medParameterPoolL::disconnectParam(medAbstractParameterL *parameter)
{
    if(!parameter)
        return;

    parameter->disconnect(this);
}



