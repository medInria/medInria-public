/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractNavigator.h>

class vtkDataMeshNavigatorPrivate;

class vtkDataMeshNavigator: public medAbstractNavigator
{
    Q_OBJECT

public:
    vtkDataMeshNavigator(medAbstractView* parent);
    virtual ~vtkDataMeshNavigator();

public:
    virtual QString  identifier() const;
    virtual QString name() const;
    static QString  s_identifier();
    static bool registered();
    virtual QStringList handled(void) const;
    virtual QString description() const;
    virtual QList<medAbstractParameter*> linkableParameters();
    virtual QList<medBoolParameter*> mouseInteractionParameters();

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


