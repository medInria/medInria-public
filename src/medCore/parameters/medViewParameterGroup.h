/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractParameterGroup.h>

class medViewParameterGroupPrivate;

class medViewParameterGroup : public medAbstractParameterGroup
{
    Q_OBJECT

public:
    medViewParameterGroup(QString name = "", QObject *parent = 0);
    virtual ~medViewParameterGroup();

    void addImpactedView(medAbstractView *view);
    void removeImpactedView(medAbstractView *view);
    QList<medAbstractView*> impactedViews();

    void setLinkAllParameters(bool linkAll);
    bool linkAll() const;

    virtual void update();

private slots:
    void removeImpactedView();

private:
    medViewParameterGroupPrivate *d;

};

