/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medAbstractVtkViewInteractor.h"

#include <medAnnotationData.h>
#include <medAttachedData.h>

#include "v3dViewPluginExport.h"

#include <vtkAbstractWidget.h>

class v3dViewAnnotationInteractorPrivate;
class v3dView;
class v3dViewAnnotationInteractor;

// Helper class for v3dViewAnnotationInteractor
class V3DVIEWPLUGIN_EXPORT v3dViewAnnIntHelper {
public :
    v3dViewAnnIntHelper(v3dViewAnnotationInteractor * annInt);
    virtual ~v3dViewAnnIntHelper();
    virtual bool addAnnotation( medAnnotationData* annData ) = 0;
    virtual void removeAnnotation( medAnnotationData * annData ) = 0;
    virtual void annotationModified( medAnnotationData* annData ) = 0;
protected:
    v3dView * getV3dView();
    v3dViewAnnotationInteractor * m_v3dViewAnnInt;
};


//! Interface between annotations and the v3dview.
class V3DVIEWPLUGIN_EXPORT v3dViewAnnotationInteractor: public medAbstractVtkViewInteractor
{
    typedef medAbstractVtkViewInteractor BaseClass;

    Q_OBJECT

public:
    v3dViewAnnotationInteractor();
    virtual ~v3dViewAnnotationInteractor();

    //! Override dtkAbstractObject
    virtual QString description() const;
    virtual QString identifier() const;
    virtual QStringList handled() const;

    virtual bool isDataTypeHandled(QString dataType) const;

    static bool registered();

    virtual void enable();
    virtual void disable();

    //! Override dtkAbstractViewInteractor.
    virtual bool isAutoEnabledWith ( medAbstractData * data );

    //! Implement dtkAbstractViewInteractor
    virtual void setData(medAbstractData *data);
    virtual void setView(dtkAbstractView* view);
    
    //! Whether the interactor should be on when the view is in 2d and 3d mode.
    virtual bool showIn2dView() const;
    virtual bool showIn3dView() const;

    //! Return true if the annotation should be shown in the given slice.
    virtual bool isInSlice( const QVector3D & slicePoint, const QVector3D & sliceNormal, qreal thickness) const;

    static QString s_identifier();

signals:

public slots:
    virtual bool onAddAnnotation( medAnnotationData * annItem );
    virtual void onRemoveAnnotation( medAnnotationData * annItem );

    //! Respond to add / removal of attached data to data items viewed.
    virtual void onAttachedDataAdded(medAttachedData* data);
    virtual void onAttachedDataRemoved(medAttachedData* data);
    
    //! Called when the annotation data is altered.
    virtual void onDataModified(medAbstractData* data);

    // Mandatory implementations from medVtkViewInteractor
    virtual void setOpacity(medAbstractData * data, double opacity);
    virtual double opacity(medAbstractData * data) const;

    virtual void setVisible(medAbstractData * data, bool visible);
    virtual bool isVisible(medAbstractData * data) const;

protected:
    void onPropertySet(const QString& key, const QString& value);
    
    void onVisibilityPropertySet (const QString& value);
    
    v3dView * getV3dView();
    
    void initialize(medAbstractView * view, medAbstractData* data);

    virtual QPointF worldToScene( const QVector3D & worldVec ) const;
    virtual QVector3D sceneToWorld( const QPointF & sceneVec ) const;
    
    //! Get the view plane up vector in world space.
    virtual QVector3D viewUp() const;
    
    bool isPointInSlice( const QVector3D & testPoint, const QVector3D & slicePoint, const QVector3D & sliceNormal,  qreal thickness) const;
    bool isPointInCurrentSlice( const QVector3D & testPoint) const;
    
    virtual medAbstractViewCoordinates * coordinates();
    virtual const medAbstractViewCoordinates * coordinates() const;
    
    void addAnnotation( medAnnotationData * annData );
    void removeAnnotation( medAnnotationData * annData );

    friend class v3dViewAnnotationInteractorPrivate;
    friend class v3dViewAnnIntHelper;

private:
    v3dViewAnnotationInteractorPrivate *d;
};


// Inline this for speed.
inline v3dView * v3dViewAnnIntHelper::getV3dView()
{
    return m_v3dViewAnnInt->getV3dView();
}


