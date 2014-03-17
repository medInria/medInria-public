/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractImageViewInteractor.h>

#include <medAnnotationData.h>
#include <medAttachedData.h>

#include <msegPluginExport.h>


class v3dViewAnnotationInteractorPrivate;
class v3dViewAnnotationInteractor;

// Helper class for v3dViewAnnotationInteractor
class MEDVIEWSEGMENTATIONPLUGIN_EXPORT v3dViewAnnIntHelper {
public :
    v3dViewAnnIntHelper(v3dViewAnnotationInteractor * annInt);
    virtual ~v3dViewAnnIntHelper();
    virtual bool addAnnotation( medAnnotationData* annData ) = 0;
    virtual void removeAnnotation( medAnnotationData * annData ) = 0;
    virtual void annotationModified( medAnnotationData* annData ) = 0;
protected:
    medAbstractImageView * getView();
    v3dViewAnnotationInteractor * m_v3dViewAnnInt;
};


//! Interface between annotations and the v3dview.
class MEDVIEWSEGMENTATIONPLUGIN_EXPORT v3dViewAnnotationInteractor: public medAbstractImageViewInteractor
{
    typedef medAbstractImageViewInteractor BaseClass;

    Q_OBJECT

public:
    v3dViewAnnotationInteractor(medAbstractImageView *parent);
    virtual ~v3dViewAnnotationInteractor();

    //! Override dtkAbstractObject
    virtual QString description() const;
    virtual QString identifier() const;
    virtual QStringList handled() const;

    static bool registered();

    virtual QWidget* toolBarWidget();
    virtual QWidget* toolBoxWidget();
    virtual QWidget* layerWidget();
    virtual QList<medAbstractParameter*> parameters();
    virtual void setData(medAbstractData * data);
    virtual void setWindowLevel (double &window, double &level);
    virtual void windowLevel(double &window, double &level);
    virtual bool visibility() const;
    virtual double opacity() const;

    virtual QImage generateThumbnail(const QSize &size);
    
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
    virtual void setOpacity(double opacity);

    virtual void setVisibility (bool visibility);

    void moveToSlice(int slice);
    virtual void moveToSliceAtPosition(const QVector3D &position);

    virtual void removeData();

protected:    
    medAbstractImageView * getView();

    virtual QPointF worldToScene( const QVector3D & worldVec ) const;
    virtual QVector3D sceneToWorld( const QPointF & sceneVec ) const;
    
    //! Get the view plane up vector in world space.
    virtual QVector3D viewUp() const;
    
    bool isPointInSlice( const QVector3D & testPoint, const QVector3D & slicePoint, const QVector3D & sliceNormal,  qreal thickness) const;
    bool isPointInCurrentSlice( const QVector3D & testPoint) const;
    
    void addAnnotation( medAnnotationData * annData );
    void removeAnnotation( medAnnotationData * annData );

    friend class v3dViewAnnotationInteractorPrivate;
    friend class v3dViewAnnIntHelper;

private slots:
    void enableWindowLevelInteraction();

private:
     static QStringList dataHandled();

private:
    v3dViewAnnotationInteractorPrivate *d;
};


// Inline this for speed.
inline medAbstractImageView * v3dViewAnnIntHelper::getView()
{
    return m_v3dViewAnnInt->getView();
}


