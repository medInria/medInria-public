/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medParameterPool.h>

#include <QMultiHash>
#include <QHashIterator>
#include <dtkLog>

#include <medAbstractParameter.h>
#include <medVariantListParameter.h>
#include <medDataIndex.h>
#include <medDataListParameter.h>
#include <medStringListParameter.h>

class medParameterPoolPrivate
{
public:
    QMultiHash<QString, medAbstractParameter*> pool;
    QString name;
    QColor color;
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
    if(!d->pool.values(parameter->id()).contains(parameter))
    {
        d->pool.insert(parameter->id(), parameter);
        connectParam(parameter);
    }
}

void medParameterPool::append(QList<medAbstractParameter *> parameters)
{
    for(medAbstractParameter* param : parameters)
    {
        append(param);
    }
}

void medParameterPool::remove(medAbstractParameter* parameter)
{
    disconnectParam(parameter);

    QHashIterator<QString, medAbstractParameter*> it(d->pool);

    while(it.hasNext())
    {
        it.next();
        if (it.value() == parameter)
        {
            d->pool.remove(it.key(), it.value());
        }
    }
}

void medParameterPool::removeAll (QString name)
{
    for(medAbstractParameter* param : parameters(name))
    {
        remove(param);
    }
}

void medParameterPool::clear()
{
    for(medAbstractParameter* param : d->pool)
    {
        disconnectParam(param);
    }

    d->pool.clear();
}

QList<medAbstractParameter*> medParameterPool::parameters(QString id)
{
    return d->pool.values(id);
}

QList<medAbstractParameter*> medParameterPool::parameters()
{
    return d->pool.values();
}

QStringList medParameterPool::parametersNames()
{
    return d->pool.keys();
}

void medParameterPool::changeParamsValue()
{
    medAbstractParameter *sender = dynamic_cast<medAbstractParameter*>(QObject::sender());

    if (sender)
    {
        for(medAbstractParameter *param : d->pool.values(sender->id()))
        {
            if(param != sender)
            {
                param->disconnect(this);
                sender->copyValueTo(*param);
                connect(param, SIGNAL(triggered()), this, SLOT(changeParamsValue()), Qt::UniqueConnection);
                connect(param, SIGNAL(aboutToBeDestroyed()), this, SLOT(removeInternParam()), Qt::UniqueConnection);
            }
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
    if (param)
    {
        this->remove(param);
    }
}

void medParameterPool::connectParam(medAbstractParameter *parameter)
{
    connect(parameter, SIGNAL(aboutToBeDestroyed()), this, SLOT(removeInternParam()), Qt::UniqueConnection);
    connect(parameter, SIGNAL(triggered()), this, SLOT(changeParamsValue()), Qt::UniqueConnection);
}

void medParameterPool::disconnectParam(medAbstractParameter *parameter)
{
    if (parameter)
    {
        parameter->disconnect(this);
    }
}
