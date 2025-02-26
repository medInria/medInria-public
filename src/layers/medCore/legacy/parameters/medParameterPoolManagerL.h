#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QtCore/QObject>
#include <QtCore/QHash>

#include <medCoreExport.h>

class medParameterPoolManagerLPrivate;

class medParameterPoolManagerLToolBox;
class medAbstractParameterL;
class medParameterPoolL;
class medAbstractView;

class MEDCORE_EXPORT medParameterPoolManagerL : public QObject
{
    Q_OBJECT

public:
    static medParameterPoolManagerL *instance();

    QList<medParameterPoolL*> pools();
    medParameterPoolL* pool(QString poolId);
    QStringList pools(medAbstractParameterL *param);

public slots:
    void linkParameter(medAbstractParameterL*, QString poolId);
    void unlinkParameter(medAbstractParameterL*);
    void removePool(QString);

protected:
    medParameterPoolManagerL();
    ~medParameterPoolManagerL();

    static medParameterPoolManagerL *s_instance;

private:
    medParameterPoolManagerLPrivate *d;

};

