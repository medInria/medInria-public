/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <medGuiExport.h>

class medParameterPoolManagerPrivate;

class medParameterPoolManagerToolBox;
class medAbstractParameter;
class medParameterPool;
class medAbstractView;

class MEDGUI_EXPORT medParameterPoolManager : public QObject
{
    Q_OBJECT

public:
    static medParameterPoolManager *instance();

    QList<medParameterPool*> pools();
    medParameterPool* pool(QString poolId);


public slots:
    void linkParameter(medAbstractParameter*, QString poolId);
    void unlinkParameter(medAbstractParameter*);
    void removePool(QString);

signals:

protected:
    medParameterPoolManager();

protected:
    static medParameterPoolManager *s_instance;


private:



private:
    medParameterPoolManagerPrivate *d;

};

