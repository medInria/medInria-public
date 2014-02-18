/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
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

class medAbstractViewCoordinates;
class medViewBackend;
class medAbstractData;
class medAbstractViewNavigator;
class medAbstractNavigator;
class medAbstractInteractor;
class medAbstractViewInteractor;
class medAbstractParameter;

// derive and implement if you need to provide access to your backend
class medViewBackend {
};



/**
 * @class medAbstractView
 * @brief Base class for view types in medInria
 * medAbstractView specializes a dtkAbstractView in the context of medInria.
 * Two concepts are added: view synchronization and color lookup.
 *
 * View synchronization is achieved via the Qt signal/slot mecanism. 5 signals
 * were added: positionChanged(), zoomChanged(), panChanged(), windowingChanged(),
 * cameraChanged() with the corresponding slots: setPosition(), setZoom(),
 * setPan(), setWindowLevel() and setCamera(). Any subclass of medAbstractView
 * should implement those slots and emit signals when apropriate. One can activate/
 * deactivate synchronization using the corresponding setLinkPosition(),
 * setLinkWindowing(), setLinkPan(), setLinkZoom() and setLinkCamera() slots. This
 * class should be used in conjunction with the class @medViewPool, which realizes
 * the synchronization.
 *
 * Color lookup tables are specified with a QList of scalars and corresponding
 * colors using the setColorLookupTable() or setTransferFunctions() methods.
 **/

class MEDCORE_EXPORT medAbstractView: public dtkAbstractView
{
    Q_OBJECT

public:    
    medAbstractView(QObject* parent = 0);
    virtual ~medAbstractView();

    virtual QString description() const =0;

    /**
       Set the view zoom factor.

    **/
    virtual void setZoom (double zoom);
    double zoom();

    /**
       Set the view pan.
    **/
    virtual void setPan (const QVector2D &pan);
    QVector2D pan();

    virtual medViewBackend * backend() const = 0;

    virtual QWidget* toolBar() = 0;
    virtual QWidget* toolBox() = 0;
    virtual QWidget *viewWidget() = 0;


    virtual QList<medAbstractParameter*> viewParameters() = 0;
    virtual QList<medAbstractParameter*> navigatorsParameters();

    virtual QImage& generateThumbnail(const QSize &size) = 0;

public slots:
    virtual void reset() = 0;

signals:
    void selectedRequest(bool selected);

    void zoomChanged(double);
    void panChanged(const QVector2D&);

protected:
    virtual medAbstractViewInteractor* primaryInteractor(medAbstractData* data);
    virtual QList<medAbstractInteractor*> extraInteractors(medAbstractData* data);
    virtual medAbstractViewNavigator* primaryNavigator();
    virtual QList<medAbstractNavigator*> extraNavigators();

    virtual void initialiseInteractors(medAbstractData* data);
    virtual void initialiseNavigators();
    virtual void removeInteractors(medAbstractData *data);
    virtual bool eventFilter(QObject *, QEvent *);


private:
    medAbstractViewPrivate *d;

    /*=========================================================================
                             NOT IMPLEMENTED ON PURPOSE
    *=========================================================================*/
    virtual void copy(const dtkAbstractObject& other);
    virtual void link(dtkAbstractView *other);
    virtual void unlink(dtkAbstractView *other);
    virtual void select(dtkAbstractData *data);
    virtual void unselect(dtkAbstractData *data);
    virtual void setStereo(bool on);
    virtual void setView(void *view);
    virtual void setData(dtkAbstractData *data);
    virtual void setData(dtkAbstractData *data, int inputId);
    virtual void setBackgroundColor(int red, int green, int blue);
    virtual void setBackgroundColor(double red, double green, double blue);
    virtual void *view(void);
    virtual void *data(void);
    virtual void *data(int channel);
    virtual bool stereo(void);
    virtual void clear(void);
    virtual void update(void);
    virtual void close(void);
    void showFullScreen(void);
    void showMinimized(void);
    void showMaximized(void);
    void showNormal(void);
    void show(void);
    void resize(int width, int height);
    void addAnimator  (dtkAbstractViewAnimator   *animator);
    void addNavigator (dtkAbstractViewNavigator  *navigator);
    void addInteractor(dtkAbstractViewInteractor *interactor);
    void enableAnimator(const QString& animator);
    void disableAnimator(const QString& animator);
    void enableNavigator(const QString& navigator);
    void disableNavigator(const QString& navigator);
    void enableInteractor(const QString& interactor);
    void disableInteractor(const QString& interactor);
    dtkAbstractViewAnimator   *animator  (const QString& type);
    dtkAbstractViewNavigator  *navigator (const QString& type);
    dtkAbstractViewInteractor *interactor(const QString& type);
    QList<dtkAbstractViewAnimator   *> animators(void) const;
    QList<dtkAbstractViewNavigator  *> navigators(void) const;
    QList<dtkAbstractViewInteractor *> interactors(void) const;
    virtual void   initialize(void);
    virtual void uninitialize(void);
    virtual void setHeadPosition(dtkVector3D<double> position);
    virtual void setHeadOrientation(dtkQuaternion<double> orientation);
    virtual void setUpperLeft(dtkVector3D<double> position);
    virtual void setLowerLeft(dtkVector3D<double> position);
    virtual void setLowerRight(dtkVector3D<double> position);

};


