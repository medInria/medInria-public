/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
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

    virtual QWidget* widgetForToolBox() const;
    virtual QWidget* widgetForToolBar() const;

    virtual QList<medAbstractParameter*> parameters();

public slots:
    void set3DMode(QString);
    void setRenderer(QString);
    void enableCropping(bool);

private slots:
    void updateVisibility();


private:
    medVtkViewItkDataImageNavigatorPrivate *d;

};
