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
    medAbstractView(const medAbstractView& view);
    virtual ~medAbstractView();

    //TODO rename?
    virtual QWidget *receiverWidget();

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


    //! Get the coordinates helper
    // TODO: check if this is required, and what is exactly required
    virtual medAbstractViewCoordinates * coordinates() = 0;

    virtual medViewBackend * backend() const;


    //TODO GPR: check datatypes: used in medToolbox::update
    void addDataType(const QString & dataDescription);

    void removeDataType(const QString & dataDescription);

    QHash<QString, unsigned int> dataTypes();

    bool isDataTypeHandled(const QString& dataType) const;

    bool isClosable() const;
    void setClosable(bool closable);

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
    //TODO: à vérifier mais sans doute à bouger dans les container: rename to maximizeRequested
    void maximizeRequested (bool);

    /**
       This signal is emitted when the zoom factor of the view has changed.
     **/
    void zoomChanged      (double zoom, bool propagate);

    /**
       This signal is emitted when the pan (=translation) of the view has
       changed.
     **/
    void panChanged       (const QVector2D &pan, bool propagate);


public slots:

    //TODO: rename to toggleMaximize ?
    void toggleMaximize(bool state);



private:
    medAbstractViewPrivate *d;
};

// derive and implement if you need to provide access to your backend
class medViewBackend {
};
