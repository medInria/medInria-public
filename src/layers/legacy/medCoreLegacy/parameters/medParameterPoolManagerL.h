/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore/QObject>
#include <QtCore/QHash>

#include <medCoreLegacyExport.h>

class medParameterPoolManagerLPrivate;

class medParameterPoolManagerLToolBox;
class medAbstractParameterL;
class medParameterPoolL;
class medAbstractView;

class MEDCORELEGACY_EXPORT medParameterPoolManagerL : public QObject
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

protected:
    static medParameterPoolManagerL *s_instance;

private:
    medParameterPoolManagerLPrivate *d;

};

