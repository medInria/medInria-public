#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.

 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractView.h>

#include <QDomElement>
#include <QMainWindow>

#include <medCoreExport.h>

class medAbstractViewPrivate;

class medViewBackend;
class medAbstractData;
class medAbstractViewNavigator;
class medAbstractNavigator;
class medAbstractInteractor;
class medAbstractViewInteractor;
class medAbstractParameterL;
class medDoubleParameterL;
class medAbstractVector2DParameterL;
class medStringListParameterL;
class QUndoStack;

/**
 * @brief The medViewBackend class
 * derive and implement if you need to provide access to your backend
 */
class medViewBackend {
};

/**
 * @class medAbstractView
 * @brief Base class for view types
 * medAbstractView specializes a dtkAbstractView in the context of the application.
 * A medAbstractView contains medAbstractInteractor and medAbstractNavigator.
 **/
class MEDCORE_EXPORT medAbstractView: public dtkAbstractView
{
    Q_OBJECT

public:
    medAbstractView(QObject* parent = nullptr);
    virtual ~medAbstractView();

    virtual void addData(medAbstractData* data);
    virtual void clear();

    virtual QWidget* navigatorWidget();
    virtual QWidget *viewWidget() = 0;
    virtual QMainWindow* mainWindow();
    virtual QWidget *mouseInteractionWidget();
    virtual QWidget* toolBarWidget();

    virtual QList<medAbstractNavigator*> navigators();
    virtual QList<medAbstractInteractor*> interactors();

    medDoubleParameterL* zoomParameter();
    medAbstractVector2DParameterL* panParameter();

    virtual QList<medAbstractParameterL*> linkableParameters();

    virtual medViewBackend * backend() const = 0;
    virtual QString description() const = 0;

    QImage generateThumbnail(const QSize &size);
    virtual void setOffscreenRendering(bool isOffscreen);

    QUndoStack* undoStack() const;

    virtual void restoreState(QDomElement *element);

public slots:
    virtual void reset() = 0;
    virtual void render() = 0;

signals:
    /**
     * @brief selectedRequest
     * Emitted on a left click event.
     *
     * @param selected
     */
    void selectedRequest(bool selected);

protected:
    virtual medAbstractViewInteractor* primaryInteractor(medAbstractData* data);
    virtual QList<medAbstractInteractor*> extraInteractors(medAbstractData* data);
    virtual medAbstractViewInteractor* primaryInteractor();
    virtual QList<medAbstractInteractor*> extraInteractors();
    virtual medAbstractViewNavigator* primaryNavigator();
    virtual QList<medAbstractNavigator*> extraNavigators();

    virtual bool initialiseInteractors(medAbstractData* data);
    virtual bool initialiseNavigators();
    virtual void removeInteractors(medAbstractData *data);
    virtual bool eventFilter(QObject *, QEvent *);

    virtual void setUpViewForThumbnail();
    virtual QImage buildThumbnail(const QSize &size) = 0;
private:

private:
    medAbstractViewPrivate *d;

    /*=========================================================================
                             NOT IMPLEMENTED ON PURPOSE
    *=========================================================================*/
    using dtkAbstractView::copy;
    using dtkAbstractView::link;
    using dtkAbstractView::unlink;
    using dtkAbstractView::select;
    using dtkAbstractView::unselect;
    using dtkAbstractView::setStereo;
    using dtkAbstractView::setView;
    using dtkAbstractView::setData;
    using dtkAbstractView::setBackgroundColor;
    using dtkAbstractView::view;
    using dtkAbstractView::data;
    using dtkAbstractView::stereo;
    using dtkAbstractView::clear;
    using dtkAbstractView::update;
    using dtkAbstractView::close;
    using dtkAbstractView::showFullScreen;
    using dtkAbstractView::showMinimized;
    using dtkAbstractView::showMaximized;
    using dtkAbstractView::showNormal;
    using dtkAbstractView::show;
    using dtkAbstractView::resize;
    using dtkAbstractView::addAnimator;
    using dtkAbstractView::addNavigator;
    using dtkAbstractView::addInteractor;
    using dtkAbstractView::enableAnimator;
    using dtkAbstractView::disableAnimator;
    using dtkAbstractView::enableNavigator;
    using dtkAbstractView::disableNavigator;
    using dtkAbstractView::enableInteractor;
    using dtkAbstractView::disableInteractor;
    using dtkAbstractView::animator;
    using dtkAbstractView::navigator;
    using dtkAbstractView::interactor;
    using dtkAbstractView::animators;
    using dtkAbstractView::navigators;
    using dtkAbstractView::interactors;
    using dtkAbstractView::initialize;
    using dtkAbstractView::uninitialize;
    using dtkAbstractView::setHeadPosition;
    using dtkAbstractView::setHeadOrientation;
    using dtkAbstractView::setUpperLeft;
    using dtkAbstractView::setLowerLeft;
    using dtkAbstractView::setLowerRight;
    using dtkAbstractView::widget;
};
