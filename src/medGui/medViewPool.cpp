/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medViewPool.h"
#include <medMessageController.h>
#include <medAbstractView.h>
#include <medAbstractViewCollection.h>

#include <medAbstractData.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include "medMessageController.h"

class medViewPoolPrivate
{
public:
    QList< dtkSmartPointer<medAbstractView> > views;
    QMap< dtkSmartPointer<medAbstractView>, dtkSmartPointer<medAbstractData> > viewData;
    QHash<QString, QString>                  propertySet;
};

medViewPool::medViewPool(QObject *parent) : QObject (parent), d (new medViewPoolPrivate)
{
    connect(this,SIGNAL(showInfo(const QString&,unsigned int)),
            medMessageController::instance(),SLOT(showInfo(const QString&,unsigned int)));
    connect(this,SIGNAL(showError(const QString&,unsigned int)),
            medMessageController::instance(),SLOT(showError(const QString&,unsigned int)));
}

medViewPool::~medViewPool(void)
{
    delete d;
    d = NULL;
}

void medViewPool::appendView (medAbstractView *vview)
{
    if (!vview)
        return;
    
    QList <medAbstractView *> viewsToAdd;

    if (medAbstractViewCollection *collection = qobject_cast <medAbstractViewCollection *> (vview)) {
        viewsToAdd << collection->views();
        connect (collection, SIGNAL (viewAdded   (medAbstractView*)), this, SLOT (appendView (medAbstractView *)), Qt::UniqueConnection);
        connect (collection, SIGNAL (viewRemoved (medAbstractView*)), this, SLOT (removeView (medAbstractView *)), Qt::UniqueConnection);
    }
    else
        viewsToAdd << vview;

    foreach (medAbstractView *view, viewsToAdd) {

        if (d->views.contains (view)) {
            continue;
        }

        d->views.append (view);
    
        connect (view, SIGNAL (propertySet(const QString&, const QString&)), this, SLOT (onViewPropertySet(const QString&, const QString &)));        
        
        connect (view, SIGNAL (positionChanged  (const QVector3D  &, bool)), this, SLOT (onViewPositionChanged  (const QVector3D  &, bool)));
        connect (view, SIGNAL (zoomChanged      (double, bool)),             this, SLOT (onViewZoomChanged      (double, bool)));
        connect (view, SIGNAL (panChanged       (const QVector2D &, bool)),  this, SLOT (onViewPanChanged       (const QVector2D  &, bool)));
        connect (view, SIGNAL (windowingChanged (double, double, bool)),     this, SLOT (onViewWindowingChanged (double, double, bool)));
    
        connect (view, SIGNAL (cameraChanged     (const QVector3D &, const QVector3D &, const QVector3D &, double, bool)),
                 this, SLOT (onViewCameraChanged (const QVector3D &, const QVector3D &, const QVector3D &, double, bool)));

        connect(view, SIGNAL(  obliqueSettingsChanged    (const medAbstractView *)), 
                this,   SLOT(onViewObliqueSettingsChanged(const medAbstractView *)));

        connect (this, SIGNAL (viewAppended (medAbstractView *)),
                 view, SLOT (onAppendViewToPool (medAbstractView *)));

        connect (this, SIGNAL (viewRemoved (medAbstractView *)),
                 view, SLOT (onRemoveViewFromPool (medAbstractView *)));

        // set properties
        QHashIterator<QString, QString> it(d->propertySet);
        while (it.hasNext()) {
            it.next();
            view->setProperty (it.key(), it.value());
        }

        emit viewAppended (view);
    }    
}

void medViewPool::removeView (medAbstractView *vview)
{
    if (!vview)
        return;

    QList <medAbstractView *> viewsToRemove;

    if (medAbstractViewCollection *collection = qobject_cast <medAbstractViewCollection *> (vview)) {
        viewsToRemove << collection->views();
        disconnect (collection, SIGNAL (viewAdded   (medAbstractView*)), this, SLOT (appendView (medAbstractView *)));
        disconnect (collection, SIGNAL (viewRemoved (medAbstractView*)), this, SLOT (removeView (medAbstractView *)));
    }
    else
        viewsToRemove << vview;

    foreach(medAbstractView *view, viewsToRemove) {

        if (!d->views.contains (view)) {
            continue;
        }

        disconnect (view, SIGNAL (propertySet(const QString&, const QString&)), this, SLOT (onViewPropertySet(const QString&, const QString &)));

        disconnect (view, SIGNAL (positionChanged  (const QVector3D  &, bool)), this, SLOT (onViewPositionChanged  (const QVector3D  &, bool)));
        disconnect (view, SIGNAL (zoomChanged      (double, bool)),             this, SLOT (onViewZoomChanged      (double, bool)));
        disconnect (view, SIGNAL (panChanged       (const QVector2D &, bool)),  this, SLOT (onViewPanChanged       (const QVector2D  &, bool)));
        disconnect (view, SIGNAL (windowingChanged (double, double, bool)),     this, SLOT (onViewWindowingChanged (double, double, bool)));
    
        disconnect (view, SIGNAL (cameraChanged     (const QVector3D &, const QVector3D &, const QVector3D &, double, bool)),
                    this, SLOT (onViewCameraChanged (const QVector3D &, const QVector3D &, const QVector3D &, double, bool)));

        disconnect( view, SIGNAL(  obliqueSettingsChanged    (const medAbstractView *)), 
                    this,   SLOT(onViewObliqueSettingsChanged(const medAbstractView *)));

        disconnect (this, SIGNAL (viewAppended (medAbstractView *)),
                    view, SLOT (onAppendViewToPool (medAbstractView *)));

        disconnect (this, SIGNAL (viewRemoved (medAbstractView *)),
                    view, SLOT (onRemoveViewFromPool (medAbstractView *)));

        d->views.removeOne (view);
        emit viewRemoved (view);
    }
}

void medViewPool::onViewPropertySet (const QString &key, const QString &value)
{
    // property that we do not want to synchronize
    if (key == "PositionLinked"  ||
        key == "CameraLinked"    ||
        key == "WindowingLinked" ||
        key == "Orientation"     ||
        key == "LookupTable"     ||
        key == "Preset"          ||
        key == "3DMode")
        return;
    
    d->propertySet[key] = value;
    
    // first, block all signals
    foreach (medAbstractView *lview, d->views)
        lview->blockSignals (true);    
    
    // second, propagate properties
    foreach (medAbstractView *lview, d->views) {
        if (lview!=this->sender()) {
            lview->setProperty (key, value);
	    if (lview->widget()->isVisible())
	        lview->update();
        }
    }
    
    // third, restore signals
    foreach (medAbstractView *lview, d->views)
        lview->blockSignals (false); 
}

void medViewPool::onViewPositionChanged (const QVector3D &position, bool propagate)
{
    if (!propagate)
        return;

    medAbstractView *vsender = dynamic_cast<medAbstractView*>(this->sender());
    
    if (!vsender) {
        qDebug() << "Sender seems not to be a medAbstractView";
        return;
    }

    foreach (medAbstractView *lview, d->views) {
        if ( lview != vsender && lview->positionLinked() ) {
            lview->setPosition(position);
            if (lview->widget()->isVisible())
                lview->update();
        }
    }
}

void medViewPool::onViewCameraChanged (const QVector3D &position,
                                       const QVector3D &viewup,
                                       const QVector3D &focal,
                                       double parallelScale,
                                       bool propagate)
{
    if (!propagate)
        return;

    medAbstractView *vsender = dynamic_cast<medAbstractView*>(this->sender());
    
    if (!vsender) {
        qDebug() << "Sender seems not to be a medAbstractView";
        return;
    }
    
    foreach (medAbstractView *lview, d->views) {
        if (lview!=this->sender() && lview->cameraLinked()) {
            lview->setCamera(position, viewup, focal, parallelScale);
            if (lview->widget()->isVisible())
                lview->update();
        }
    }
}

void medViewPool::onViewZoomChanged (double zoom, bool propagate)
{
    if (!propagate)
        return;

    medAbstractView *vsender = dynamic_cast<medAbstractView*>(this->sender());
    
    if (!vsender) {
        qDebug() << "Sender seems not to be a medAbstractView";
        return;
    }
    
    foreach (medAbstractView *lview, d->views) {
        if (lview!=this->sender() && lview->cameraLinked()) {
            lview->setZoom(zoom);
            if (lview->widget()->isVisible())
                lview->update();
        }
    }
}

void medViewPool::onViewPanChanged (const QVector2D &pan, bool propagate)
{
    if (!propagate)
        return;

    medAbstractView *vsender = dynamic_cast<medAbstractView*>(this->sender());
    
    if (!vsender) {
        qDebug() << "Sender seems not to be a medAbstractView";
        return;
    }
    
    foreach (medAbstractView *lview, d->views) {
        if (lview!=this->sender() && lview->cameraLinked()) {
            lview->setPan(pan);
            if (lview->widget()->isVisible())
                lview->update();
        }
    }
}

void medViewPool::onViewWindowingChanged (double level, double window, bool propagate)
{
    if (!propagate)
        return;

    medAbstractView *vsender = dynamic_cast<medAbstractView*>(this->sender());
    
    if (!vsender) {
        qDebug() << "Sender seems not to be a medAbstractView";
        return;
    }
    
    foreach (medAbstractView *lview, d->views) {
        if (lview!=this->sender() && lview->windowingLinked()) {
            lview->setWindowLevel(level, window);
            if (lview->widget()->isVisible())
                lview->update();
        }
    }
}

int medViewPool::count()
{
    return d->views.count();
}

void medViewPool::onViewObliqueSettingsChanged(const medAbstractView *view)
{
    const medAbstractView *vsender = dynamic_cast<medAbstractView *>(this->sender());

    if (!vsender) {
        qDebug() << "Sender seems not to be a medAbstractView";
        return;
    }

    foreach (medAbstractView *lview, d->views) {
        if ( lview != vsender ) {
            lview->onObliqueSettingsChanged( view );
            if (lview->widget()->isVisible())
                lview->update();
        }
    }
}
