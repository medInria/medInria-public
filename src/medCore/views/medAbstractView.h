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

#include "medCoreExport.h"

class medAbstractViewPrivate;

class medAbstractViewCoordinates;
class medViewBackend;
class medAbstractData;
class medAbstractViewNavigator;
class medAbstractNavigator;
class medAbstractIntercator;
class medAbstractViewIntercator;

class QColor;

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

    virtual QWidget *receiverWidget() = 0;

    /**
       Set the view zoom factor.

    **/
    virtual void setZoom (double zoom);
    double zoom() const;

    /**
       Set the view pan.
    **/
    virtual void setPan (const QVector2D &pan);
    QVector2D pan() const;

    virtual medViewBackend * backend() const = 0;

    bool isClosable() const;
    void setClosable(bool closable);

    virtual QWidget* toolBar() = 0;
    virtual QWidget* toolBox() = 0;


signals:
    void selected();
    void unselected();

    /**
       This signal is emitted when a view is about to close.
     **/
    void closing();

    /**
       This signal is emitted when the view wants to be displayed in full screen.
     **/
    void maximizeRequested (bool);

    /**
       This signal is emitted when the zoom factor of the view has changed.
     **/
    void zoomChanged      (double zoom);

    /**
       This signal is emitted when the pan (=translation) of the view has
       changed.
     **/
    void panChanged       (const QVector2D &pan);

protected:
    virtual medAbstractViewIntercator* primaryIntercator(medAbstractData* data) = 0;
    virtual QList<medAbstractIntercator*> extraIntercator(medAbstractData* data) = 0;
    virtual medAbstractViewNavigator* primaryNavigator() = 0;
    virtual QList<medAbstractNavigator*> extraNavigator() = 0;

    virtual void getIntercators(medAbstractData* data) = 0;
    virtual void getNavigators() = 0;
    virtual void removeInteractors(medAbstractData *data);

private:
    medAbstractViewPrivate *d;

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
};

// derive and implement if you need to provide access to your backend
class medViewBackend {
};
