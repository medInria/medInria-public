/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medJobManagerL.h>
#include <medJobItemL.h>

class medJobManagerLPrivate
{
public:
    QList<medJobItemL*> itemList;
    bool m_IsActive;
};

std::unique_ptr<medJobManagerL> medJobManagerL::s_instance = nullptr;

medJobManagerL &medJobManagerL::instance()
{
    if(!s_instance)
    {
        s_instance = std::unique_ptr<medJobManagerL>(new medJobManagerL());
    }
    return *s_instance.get();
}

medJobManagerL::medJobManagerL( void ) : d(new medJobManagerLPrivate)
{
    d->m_IsActive = true;
}

medJobManagerL::~medJobManagerL( void )
{
    delete d;

    d = nullptr;
}

bool medJobManagerL::registerJobItem( medJobItemL* item, QString jobName)
{
    if(d->m_IsActive)
    {
        d->itemList.append(item);
        connect(this, SIGNAL(cancel(QObject*)), item, SLOT(onCancel(QObject*)) );
        emit jobRegistered(item, jobName);
        return true;
    }
    return false;
}

bool medJobManagerL::unRegisterJobItem( medJobItemL* item )
{
    int index = d->itemList.indexOf(item);
    if (index != -1)
    {
        disconnect(this, SIGNAL(cancel(QObject*)), item, SLOT(onCancel(QObject*)) );
        d->itemList.removeAt(index);
        return true;
    }
    return false;
}

void medJobManagerL::dispatchGlobalCancelEvent(bool ignoreNewJobItems)
{
    if (ignoreNewJobItems)
        d->m_IsActive = false;

    for( medJobItemL* item : d->itemList )
    {
        emit cancel( item );
    }
}
