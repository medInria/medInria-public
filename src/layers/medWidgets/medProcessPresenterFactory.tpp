/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medProcessPresenterFactory.h>

#include <dtkLog>

template<typename ProcessType, typename PresenterType>
PresenterType * medProcessPresenterFactory<ProcessType, PresenterType>::create(ProcessType *process) const
{
    PresenterType *presenter = NULL;
    if(process != NULL);
    {
        QString key = process->metaObject()->className();
        if(this->creators.contains(key))
            presenter = (this->creators[key])(process);
    }
    return presenter;
}

template<typename ProcessType, typename PresenterType>
void medProcessPresenterFactory<ProcessType, PresenterType>::record(const QString& key, creator func)
{
    if (this->creators.contains(key)) {
        dtkDebug() << Q_FUNC_INFO << "Factory already contains key" << key << ". Nothing is done";
        return;
    }

    this->creators.insert(key, func);
}


template<typename ProcessType, typename PresenterType>
QStringList medProcessPresenterFactory<ProcessType, PresenterType>::keys(void) const
{
    return this->creators.keys();
}
