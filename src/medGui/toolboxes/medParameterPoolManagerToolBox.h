/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medToolBox.h>

class medAbstractParameter;
class medParameterPool;
class medAbstractView;

class medParameterPoolManagerToolBoxPrivate;

class medParameterPoolManagerToolBox : public medToolBox
{
    Q_OBJECT

public:
    medParameterPoolManagerToolBox();

signals:
    void poolDeletionRequested(int);
    void linkParamRequested(medAbstractParameter*, int poolId = -1);
    void unlinkParamRequested(medAbstractParameter*, int poolId);
    void linkAllRequested();
    void addViewParamsToPoolRequested(medAbstractView*, int poolId);

private slots:
    void addPool(medParameterPool *selectedPool,int);
    void removePool();
    void removePoolFromWidget(int);
    void linkAll();
    void showLinkParametersWidget();
    void showPoolsSelectorWidget();
    void buildPool(QListWidgetItem*);
    void addViewParamsToPool();
    void displayPoolParameters();

private:
    QIcon createIcon(QString &colorName) const;
    QIcon createIcon(QColor color) const;

    QList<medAbstractParameter*> commonParameters(QList<medAbstractView*> views);

private:
    medParameterPoolManagerToolBoxPrivate *d;
};

