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

class medParameterPoolManagerToolBoxPrivate;

class medParameterPoolManagerToolBox : public medToolBox
{
    Q_OBJECT

public:
    medParameterPoolManagerToolBox();

    void addParameter(medAbstractParameter* parameter);

signals:
    void poolSelected(medAbstractParameter*, int);
    void poolDeletionRequested(int);
    void linkAllRequested();

private slots:
    void addToPool();
    void addPool(medParameterPool *selectedPool,int);
    void removePool();
    void removePoolFromWidget(int);
    void showLinkAllWidget();
    void hideLinkAllWidget();
    void linkAll();

private:
    QIcon createIcon(QString &colorName) const;

private:
    medParameterPoolManagerToolBoxPrivate *d;
};

