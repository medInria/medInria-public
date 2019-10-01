/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "vtkDataMeshPluginExport.h"

#include <medAbstractNavigator.h>
#include <QFormLayout>
#include <QSlider>
#include <QWidget>

class vtkDataMeshNavigatorPrivate;

class VTKDATAMESHPLUGIN_EXPORT vtkDataMeshNavigator: public medAbstractNavigator
{
    Q_OBJECT

public:
    vtkDataMeshNavigator(medAbstractView* parent);
    virtual ~vtkDataMeshNavigator();

public:
    virtual QString  identifier() const;
    static QString  s_identifier();
    static bool registered();
    virtual QStringList handled(void) const;
    virtual QString description() const;
    virtual QList<medAbstractParameterL*> linkableParameters();
    virtual QList<medBoolParameterL*> mouseInteractionParameters();

public slots:
    void updateWidgets();

    void enableDepthPeeling(bool);
    void setBackgroundColor(const QString &color);

protected slots:
    void setBackgroundColor(QColor color);

protected:
    virtual QWidget * buildToolBoxWidget();
    virtual QWidget * buildToolBarWidget();

    QStringList createColorList();

private:
    vtkDataMeshNavigatorPrivate *d;

};


