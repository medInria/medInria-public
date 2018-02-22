/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractJob.h>

#include <dtkCore>

#include <medCoreExport.h>

class medAbstractParameter;

class medAbstractProcessPrivate;
class MEDCORE_EXPORT medAbstractProcess: public medAbstractJob
{
    Q_OBJECT
public:
    medAbstractProcess(QObject *parent = NULL);
    virtual ~medAbstractProcess();

    virtual QString description() const = 0;

    medAbstractParameter* parameter(QString const& id) const;
    QList<medAbstractParameter*> parameters() const;


protected:
    void registerParameter(medAbstractParameter *parameter);

    //! Utility function to add a process description to outputs
    virtual QString outputNameAddon() const {return "processed";}

private:
    const QScopedPointer<medAbstractProcessPrivate> d;
};

// Why is this here?
// Because unfortunately, the current implementation prevents us from having processes *in workspaces* that are not attached to a abstraction
// Because we can't predict all types of abstractions, we need to provide a generic abstraction that can be used by plugin designers
DTK_DECLARE_PLUGIN        (medAbstractProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractProcess, MEDCORE_EXPORT)
