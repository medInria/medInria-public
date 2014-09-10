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
#include <medCoreExport.h>

class medViewParameterGroupPrivate;

class MEDCORE_EXPORT medViewParameterGroup : public medAbstractParameterGroup
{
    Q_OBJECT

public:
    medViewParameterGroup(QString name = "", QObject *parent = 0, QString workspace = "");
    virtual ~medViewParameterGroup();

    void addImpactedView(medAbstractView *view);
    void removeImpactedView(medAbstractView *view);
    QList<medAbstractView*> impactedViews();

    void setLinkAllParameters(bool linkAll);

protected:
    virtual void updatePool();
    void updateParameterToLinkList(medAbstractView *view);

private slots:
    void removeImpactedView();
    void updateGroupIndicators(QColor, QColor);

private:
    medViewParameterGroupPrivate *d;

};

