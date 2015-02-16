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

class medVtkViewItkScalarImageNavigatorPrivate;

class medVtkViewItkScalarImageNavigator : public medAbstractNavigator
{
    Q_OBJECT

public:
    medVtkViewItkScalarImageNavigator(medAbstractView* parent);
    virtual ~medVtkViewItkScalarImageNavigator();

public:
    virtual QString  identifier() const;
    static QString  s_identifier();
    static bool registered();
    virtual QStringList handled(void) const;
    virtual QString description() const;
    virtual QList<medAbstractParameter*> linkableParameters();
    QList<medBoolParameter*> mouseInteractionParameters();

public slots:
    void updateWidgets();

    void setMode3D(QString);
    void setRenderer(QString);
    void enableCropping(bool);

protected:
    virtual QWidget * buildToolBoxWidget();
    virtual QWidget * buildToolBarWidget();

private:
    medVtkViewItkScalarImageNavigatorPrivate *d;

};
