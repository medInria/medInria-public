/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
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


class msegAnnotationInteractorPrivate;
class msegAnnotationInteractor;

// Helper class for msegAnnotationInteractor
class MEDVIEWSEGMENTATIONPLUGIN_EXPORT msegAnnIntHelper {
public :
    msegAnnIntHelper(msegAnnotationInteractor * annInt);
    virtual ~msegAnnIntHelper();
    virtual bool addAnnotation( medAnnotationData* annData ) = 0;
    virtual void removeAnnotation( medAnnotationData * annData ) = 0;
    virtual void annotationModified( medAnnotationData* annData ) = 0;
protected:
    medAbstractImageView * getView();
    msegAnnotationInteractor * m_msegAnnInt;
};


//! Interface between annotations and the mseg.
class MEDVIEWSEGMENTATIONPLUGIN_EXPORT msegAnnotationInteractor: public medAbstractImageViewInteractor
{
    typedef medAbstractImageViewInteractor BaseClass;

    Q_OBJECT

public:
    msegAnnotationInteractor(medAbstractView *parent);
    virtual ~msegAnnotationInteractor();

    //! Override dtkAbstractObject
    virtual QString description() const;
    virtual QString identifier() const;
    virtual QStringList handled() const;

    static bool registered();

    virtual QWidget* buildLayerWidget();
    virtual QWidget* buildToolBarWidget();
    virtual QWidget* buildToolBoxWidget();
    virtual QList<medAbstractParameter*> linkableParameters();
    virtual QList<medBoolParameter*> mouseInteractionParameters();
    virtual void setInputData(medAbstractData * data);

    virtual void setUpViewForThumbnail();

    //! Whether the interactor should be on when the view is in 2d and 3d mode.
    virtual bool showIn2dView() const;
    virtual bool showIn3dView() const;

    //! Return true if the annotation should be shown in the given slice.
    virtual bool isInSlice( const QVector3D & slicePoint, const QVector3D & sliceNormal, qreal thickness) const;

    static QString s_identifier();

signals:

public slots:
    //! Respond to add / removal of attached data to data items viewed.
    virtual void attachData(medAttachedData* data);
    virtual void removeAttachedData(medAttachedData* data);

    //! Called when the annotation data is altered.
    virtual void onDataModified(medAbstractData* data);

    // Mandatory implementations from medVtkViewInteractor
    virtual void setOpacity(double opacity);

    virtual void setVisibility (bool visibility);

    virtual void setWindowLevel (QHash<QString, QVariant>);

    void moveToSlice(int slice);

    virtual void removeData();

    virtual void updateWidgets();

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

    friend class msegAnnotationInteractorPrivate;
    friend class msegAnnIntHelper;

private slots:
    void enableWindowLevelInteraction();
    void updateSlicingParam();

private:
     static QStringList dataHandled();

private:
    msegAnnotationInteractorPrivate *d;
};


// Inline this for speed.
inline medAbstractImageView * msegAnnIntHelper::getView()
{
    return m_msegAnnInt->getView();
}


