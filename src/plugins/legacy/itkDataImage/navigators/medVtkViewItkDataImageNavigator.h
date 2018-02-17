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

class medVtkViewItkDataImageNavigatorPrivate;

class medVtkViewItkDataImageNavigator : public medAbstractNavigator
{
    Q_OBJECT

public:
    medVtkViewItkDataImageNavigator(medAbstractView* parent);
    virtual ~medVtkViewItkDataImageNavigator();

public:
    virtual QString  identifier() const;
    static QString  s_identifier();
    static bool registered();
    virtual QStringList handled(void) const;
    virtual QString description() const;
    virtual QList<medAbstractParameterL*> linkableParameters();
    QList<medBoolParameterL*> mouseInteractionParameters();

public slots:
    void updateWidgets();

    void setMode3D(QString);
    void setRenderer(QString);
    void enableCropping(bool);

    // Handle hide show planes in MSR mode
    void toggleMSRXPlane(bool);
    void toggleMSRYPlane(bool);
    void toggleMSRZPlane(bool);

protected:
    virtual QWidget * buildToolBoxWidget();
    virtual QWidget * buildToolBarWidget();

private:
    medVtkViewItkDataImageNavigatorPrivate *d;

};
