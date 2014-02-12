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

class medParameterPoolManagerPrivate;

class medParameterPoolManagerToolBox;
class medAbstractParameter;
class medParameterPool;
class medAbstractView;

class medParameterPoolManager : public QObject
{
    Q_OBJECT

public:
    static medParameterPoolManager *instance();

    void setToolBox(medParameterPoolManagerToolBox* tb);

    QList<int> poolsId(QString type);


public slots:
    void removePool(int);
    void linkAll();
    void linkParameter(medAbstractParameter*, int poolId);
    void unlinkParameter(medAbstractParameter*, int poolId);

signals:
    void poolCreated(medParameterPool *selectedPool, int id);
    void poolRemoved(int id);
    void multipleViewsSelected();
    void singleViewSelected();

protected:
    medParameterPoolManager();

protected:
    static medParameterPoolManager *s_instance;

private slots:
    void buildViewPool();
    void addViewParamsToPool(medAbstractView*, int);

private:
    void addColorLabeltoView(medAbstractParameter* parameter, medParameterPool *selectedPool);

private:
    medParameterPoolManagerPrivate *d;

};

