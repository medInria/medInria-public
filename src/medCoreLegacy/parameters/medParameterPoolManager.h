/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <medCoreExport.h>

class medParameterPoolManagerPrivate;

class medParameterPoolManagerToolBox;
class medAbstractParameter;
class medParameterPool;
class medAbstractView;

class MEDCORE_EXPORT medParameterPoolManager : public QObject
{
    Q_OBJECT

public:
    static medParameterPoolManager *instance();

    QList<medParameterPool*> pools();
    medParameterPool* pool(QString poolId);
    QStringList pools(medAbstractParameter *param);

public slots:
    void linkParameter(medAbstractParameter*, QString poolId);
    void unlinkParameter(medAbstractParameter*);
    void removePool(QString);

protected:
    medParameterPoolManager();

protected:
    static medParameterPoolManager *s_instance;

private:
    medParameterPoolManagerPrivate *d;

};

