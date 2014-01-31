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

    virtual QWidget *receiverWidget() = 0;

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

    bool isClosable() const;
    void setClosable(bool closable);

    virtual QWidget* toolBar() = 0;
    virtual QWidget* toolBox() = 0;
    virtual QWidget* widget() = 0;

//TODO shouldbe the role of container IMO - RDE
signals:
    void selected();
    void unselected();

protected:
    virtual medAbstractViewInteractor* primaryInteractor(medAbstractData* data);
    virtual QList<medAbstractInteractor*> extraInteractor(medAbstractData* data);
    virtual medAbstractViewNavigator* primaryNavigator();
    virtual QList<medAbstractNavigator*> extraNavigator();

    virtual void initialiseInteractors(medAbstractData* data);
    virtual void initialiseNavigators();
    virtual void removeInteractors(medAbstractData *data);



public:
    /**
     * @brief implementationOf
     * @return Upper abstract class it derives from.
     * Do NOT reimplement it in non abstract class.
     * Used by the factory to kwnow what can be create.
     */
    static QString implementationOf()
    {
        return "medAbstractView";
    }

private:
    medAbstractViewPrivate *d;

    /*=========================================================================
                             NOT IMPLEMENTED ON PURPOSE
    *=========================================================================*/
     virtual void copy(const dtkAbstractObject& other)
{DTK_UNUSED(other);}
     virtual void   link(dtkAbstractView *other)
{DTK_UNUSED(other);}
     virtual void unlink(dtkAbstractView *other)
{DTK_UNUSED(other);}
     virtual void   select(dtkAbstractData *data)
{DTK_UNUSED(data);}
     virtual void unselect(dtkAbstractData *data)
{DTK_UNUSED(data);}
     virtual void setStereo(bool on)
{DTK_UNUSED(on);}
     virtual void setView(void *view)
{DTK_UNUSED(view);}
    virtual void setData(dtkAbstractData *data)
{DTK_UNUSED(data);}
    virtual void setData(dtkAbstractData *data, int inputId)
{DTK_UNUSED(data); DTK_UNUSED(inputId);}
    virtual void setBackgroundColor(int red, int green, int blue)
{DTK_UNUSED(red);DTK_UNUSED(green);DTK_UNUSED(blue);}
    virtual void setBackgroundColor(double red, double green, double blue)
{DTK_UNUSED(red);DTK_UNUSED(green);DTK_UNUSED(blue);}
    virtual void *view(void)
{return 0;}
    virtual void *data(void)
{return 0;}
    virtual void *data(int channel)
{DTK_UNUSED(channel);return 0;}
    virtual bool stereo(void)
{return false;}
    virtual void  clear(void)
{}
    virtual void  reset(void)
{}
    virtual void update(void)
{}
    virtual void close(void)
{}
    void showFullScreen(void)
{}
    void showMinimized(void)
{}
    void showMaximized(void)
{}
    void showNormal(void)
{}
    void show(void)
{}
    void resize(int width, int height)
{DTK_UNUSED(width);DTK_UNUSED(height);}
    void addAnimator  (dtkAbstractViewAnimator   *animator)
{DTK_UNUSED(animator);}
    void addNavigator (dtkAbstractViewNavigator  *navigator)
{DTK_UNUSED(navigator);}
    void addInteractor(dtkAbstractViewInteractor *interactor)
{DTK_UNUSED(interactor);}
    void    enableAnimator(const QString& animator)
{DTK_UNUSED(animator);}
    void   disableAnimator(const QString& animator)
{DTK_UNUSED(animator);}
    void   enableNavigator(const QString& navigator)
{DTK_UNUSED(navigator);}
    void  disableNavigator(const QString& navigator)
{DTK_UNUSED(navigator);}
    void  enableInteractor(const QString& interactor)
{DTK_UNUSED(interactor);}
    void disableInteractor(const QString& interactor)
{DTK_UNUSED(interactor);}
    dtkAbstractViewAnimator   *animator  (const QString& type)
{DTK_UNUSED(type);return 0;}
     dtkAbstractViewNavigator  *navigator (const QString& type)
{DTK_UNUSED(type);return 0;}
     dtkAbstractViewInteractor *interactor(const QString& type)
{DTK_UNUSED(type);return 0;}
     QList<dtkAbstractViewAnimator   *> animators(void) const
{return QList<dtkAbstractViewAnimator   *>();}
     QList<dtkAbstractViewNavigator  *> navigators(void) const
     {return QList<dtkAbstractViewNavigator *>();}
     QList<dtkAbstractViewInteractor *> interactors(void) const
     {return QList<dtkAbstractViewInteractor *>();}
     virtual void   initialize(void)
{}
     virtual void uninitialize(void)
{}
     virtual void setHeadPosition(dtkVector3D<double> position)
{DTK_UNUSED(position);}
     virtual void setHeadOrientation(dtkQuaternion<double> orientation)
{DTK_UNUSED(orientation);}
     virtual void setUpperLeft(dtkVector3D<double> position)
{DTK_UNUSED(position);}
     virtual void setLowerLeft(dtkVector3D<double> position)
{DTK_UNUSED(position);}
     virtual void setLowerRight(dtkVector3D<double> position)
{DTK_UNUSED(position);}
};


