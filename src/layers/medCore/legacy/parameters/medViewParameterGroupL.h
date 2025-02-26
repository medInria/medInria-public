#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractParameterGroupL.h>

class medViewParameterGroupLPrivate;

class MEDCORE_EXPORT medViewParameterGroupL : public medAbstractParameterGroupL
{
    Q_OBJECT

public:
    medViewParameterGroupL(QString name = "", QObject *parent = nullptr, QString workspace = "");
    virtual ~medViewParameterGroupL();

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
    medViewParameterGroupLPrivate *d;

};

