/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medGuiExport.h>
#include <medToolBox.h>
#include <medParameter.h>

class medVtkView;
class medView3dParamsToolBoxPrivate;

class MEDGUI_EXPORT medView3dParamsToolBox : public medToolBox
{
    Q_OBJECT
public:
    explicit medView3dParamsToolBox(QWidget *parent = 0);

public:
    void update(dtkAbstractView * view);
    static bool registered();
    QList<medAbstractParameter*> getParameters(medVtkView* view);

private slots:
    void set3DMode(QString mode);
    void setRenderer(QString mode);
    void setCropping(bool checked);
    void setDepthPeeling(bool checked);

private:
    medView3dParamsToolBoxPrivate *d;

};
