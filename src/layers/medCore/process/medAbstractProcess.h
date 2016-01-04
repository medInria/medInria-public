/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractJob.h>


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

private:
    const QScopedPointer<medAbstractProcessPrivate> d;
};
