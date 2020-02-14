#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QStringList>
#include <QHash>
#include <medAbstractProcess.h>

template <typename ProcessType, typename PresenterType> class medProcessPresenterFactory
{
public:
    typedef PresenterType *(*creator) (medAbstractProcess *);

public:
    PresenterType *create(medAbstractProcess *process) const;

public:
public:
    void record(const QString& key, creator func);
    QStringList keys(void) const;

private:
    QHash<QString, creator> creators;

};

#include "medProcessPresenterFactory.tpp"

// ///////////////////////////////////////////////////////////////////
// MED_DECLARE_PROCESS_PRESENTER_FACTORY
// ///////////////////////////////////////////////////////////////////
#define MED_DECLARE_PROCESS_PRESENTER_FACTORY(ProcessType,Export)                              \
    class Export ProcessType##PresenterFactory : public medProcessPresenterFactory<ProcessType, ProcessType##Presenter> {};

// ///////////////////////////////////////////////////////////////////
// MED_DECLARE_PROCESS_PRESENTER_CREATOR
// ///////////////////////////////////////////////////////////////////
#define MED_DECLARE_PROCESS_PRESENTER_CREATOR(ProcessAbstractType, ProcessType) \
inline ProcessAbstractType##Presenter* ProcessType##PresenterCreator(medAbstractProcess* abstractProcess) \
{ \
    ProcessAbstractType##Presenter *presenter = nullptr; \
    if(ProcessType *process = qobject_cast<ProcessType *>(abstractProcess)) \
        presenter = new ProcessType##Presenter(process); \
    return presenter; \
}
