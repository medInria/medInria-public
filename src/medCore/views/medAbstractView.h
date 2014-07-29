/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medCoreExport.h>

class medAbstractViewPrivate;

class medViewBackend;
class medAbstractData;
class medAbstractViewNavigator;
class medAbstractNavigator;
class medAbstractInteractor;
class medAbstractViewInteractor;
class medAbstractParameter;
class medDoubleParameter;
class medAbstractVector2DParameter;
class medStringListParameter;

//

/**
 * @brief The medViewBackend class
 * derive and implement if you need to provide access to your backend
 */
class medViewBackend {
};


/**
 * @class medAbstractView
 * @brief Base class for view types in medInria
 * medAbstractView specializes a dtkAbstractView in the context of medInria.
 **/
class MEDCORE_EXPORT medAbstractView: public dtkAbstractView
{
    Q_OBJECT

public:
    medAbstractView(QObject* parent = 0);
    virtual ~medAbstractView();

    virtual void addData(medAbstractData* data);
    virtual void clear();

    medDoubleParameter* zoomParameter();
    medAbstractVector2DParameter* panParameter();

    virtual QWidget* navigatorWidget() = 0;
    virtual QWidget *viewWidget() = 0;
    virtual QWidget *mouseInteractionWidget() = 0;

    virtual QList<medAbstractNavigator*> navigators();
    virtual QList<medAbstractInteractor*> interactors();

    virtual QList<medAbstractParameter*> linkableParameters();

    virtual medViewBackend * backend() const = 0;
    virtual QString description() const = 0;
    QImage generateThumbnail(const QSize &size);

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
    /**
     * @brief aboutToBuildThumbnail
     * Emitted right before to generate a thumbnail.
     * Used to setup the view for a thumbnail, by default connected to medAbstrctView::setupViewForThumbnail().
     */
    void aboutToBuildThumbnail();

protected:
    virtual medAbstractViewInteractor* primaryInteractor(medAbstractData* data);
    virtual QList<medAbstractInteractor*> extraInteractors(medAbstractData* data);
    virtual medAbstractViewNavigator* primaryNavigator();
    virtual QList<medAbstractNavigator*> extraNavigators();

    virtual bool initialiseInteractors(medAbstractData* data);
    virtual bool initialiseNavigators();
    virtual void removeInteractors(medAbstractData *data);
    virtual bool eventFilter(QObject *, QEvent *);

private:
    virtual QImage buildThumbnail(const QSize &size) = 0;

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


