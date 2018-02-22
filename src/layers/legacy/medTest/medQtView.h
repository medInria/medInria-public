/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medTestExport.h>

#include <dtkCoreSupport/dtkSmartPointer.h>

#include <medAbstractLayeredView.h>

class medAbstractData;
class medQtViewPrivate;

class MEDTEST_EXPORT medQtView : public medAbstractLayeredView
{
    Q_OBJECT

public:
             medQtView(QObject* parent = 0);
    virtual ~medQtView();

    virtual QString description() const;

    static QString s_description();
    static bool registered();

    virtual medAbstractLayeredViewInteractor * primaryInteractor(medAbstractData* data);
    virtual QList<medAbstractInteractor *> extraInteractors(medAbstractData* data);
    virtual medAbstractLayeredViewInteractor * primaryInteractor(unsigned int layer);
    virtual QList<medAbstractInteractor *> extraInteractors(unsigned int layer);

    virtual medAbstractLayeredViewNavigator * primaryNavigator();
    virtual QList<medAbstractNavigator *> extraNavigators();

    virtual QWidget *viewWidget();
    virtual QWidget* navigatorWidget();
    virtual QWidget* mouseInteractionWidget();

    virtual bool initialiseInteractors(medAbstractData* data);
    virtual bool initialiseNavigators();

    virtual medViewBackend * backend() const;

    virtual void removeInteractors(medAbstractData *data);

    virtual QWidget* buildToolBarWidget();

public slots:
    virtual void reset();
    virtual void render();

private:
    virtual QImage buildThumbnail(const QSize &size);

private:
    medQtViewPrivate *d;
};


