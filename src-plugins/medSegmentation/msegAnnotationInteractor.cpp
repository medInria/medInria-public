/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <msegAnnotationInteractor.h>

#include <medAbstractData.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkLog/dtkLog.h>

#include <medAnnotationData.h>
#include <medSeedPointAnnotationData.h>
#include <medImageMaskAnnotationData.h>

#include <medImageViewFactory.h>
#include <medDoubleParameter.h>
#include <medBoolParameter.h>

#include <msegAnnIntSeedPointHelper.h>
#include <msegAnnIntImageMaskHelper.h>

#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkSeedWidget.h>
#include <vtkSeedRepresentation.h>
#include <vtkSphereHandleRepresentation.h>
#include <vtkSmartPointer.h>

#include <medVtkViewBackend.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>


// pImpl
class msegAnnotationInteractorPrivate
{
public:
    typedef QVector<msegAnnIntHelper *> HelperVecType;
    HelperVecType helpers;

    typedef std::map< medAnnotationData*, HelperVecType::size_type> HelperMap;
    HelperMap  dataToHelperIdMap;

    typedef QSet< dtkSmartPointer<medAnnotationData> > AnnotationHash;
    AnnotationHash installedAnnotations;


    medAbstractImageView *medVtkView;
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;
    vtkRenderWindow *render;

    medAbstractData *imageData;

    QWidget* toolbox;
    QWidget* toolbar;
    QImage thumbnail;

    medDoubleParameter *opacityParam;
    medBoolParameter *visibiltyParameter;
};

// Implementation
msegAnnotationInteractor::msegAnnotationInteractor(medAbstractImageView* parent):
    medAbstractImageViewInteractor(parent), d(new msegAnnotationInteractorPrivate)
{
    d->helpers.push_back(new msegAnnIntSeedPointHelper(this));
    d->helpers.push_back(new msegAnnIntImageMaskHelper(this));

    d->medVtkView = parent;
    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;

    d->imageData = NULL;

    d->toolbox = NULL;
    d->toolbar = NULL;

    d->opacityParam = NULL;
    d->visibiltyParameter = NULL;

    connect(parent, SIGNAL(currentLayerChanged()), this, SLOT(enableWindowLevelInteraction()));
}

msegAnnotationInteractor::~msegAnnotationInteractor()
{
    qDeleteAll(d->helpers);
    delete d;
    d = NULL;
}

QStringList msegAnnotationInteractor::dataHandled()
{
    QStringList d = QStringList() << "medImageMaskAnnotationData";
    return  d;
}

QStringList msegAnnotationInteractor::handled() const
{
    return msegAnnotationInteractor::dataHandled();
}

QString msegAnnotationInteractor::description() const
{
    return tr("Interactor displaying annotations");
}

QString msegAnnotationInteractor::identifier() const
{
    return s_identifier();
}

bool msegAnnotationInteractor::registered()
{
    medImageViewFactory *factory = medImageViewFactory::instance();
    return factory->registerInteractor<msegAnnotationInteractor>("msegAnnotationInteractor",
                                                                  QStringList () << "medVtkView" <<
                                                                  msegAnnotationInteractor::dataHandled());
}

QString msegAnnotationInteractor::s_identifier()
{
    return "msegAnnotationInteractor";
}


medAbstractImageView * msegAnnotationInteractor::getView()
{
    return d->medVtkView;
}

void msegAnnotationInteractor::setData(medAbstractData *data)
{
    if ( d->imageData ) {
        disconnect(d->imageData, SIGNAL(dataModified(medAbstractData*)), this, SLOT(onDataModified(medAbstractData*)) );
        // Remove annotations
        foreach( dtkSmartPointer<medAnnotationData> key,  d->installedAnnotations ) {
            if ( key->parentData() == data ) {
                this->removeAnnotation( key );
            }
        }
    }
    
    d->imageData = data;

    if (d->imageData)
    {        
        typedef itk::Image<unsigned char, 3> MaskType;
        medImageMaskAnnotationData *maskAnnData = dynamic_cast<medImageMaskAnnotationData*>(data);
        if (MaskType* image = dynamic_cast<MaskType*>((itk::Object*)(maskAnnData->maskData()->data())))
        {
            d->view2d->SetITKInput(image, d->medVtkView->layer(d->imageData));
            d->view3d->SetITKInput(image, d->medVtkView->layer(d->imageData));
        }

        medAnnotationData *annItem = qobject_cast<medAnnotationData*>(data);
        attachData(annItem);
    }

    d->opacityParam = new medDoubleParameter("Opacity", this);
    d->opacityParam->setRange(0,1);
    d->opacityParam->setValue(1);
    d->opacityParam->setSingleStep(0.01);
    connect(d->opacityParam, SIGNAL(valueChanged(double)), this, SLOT(setOpacity(double)));

    d->visibiltyParameter = new medBoolParameter("Visibility", this);
    d->visibiltyParameter->setValue(true);
    connect(d->visibiltyParameter, SIGNAL(valueChanged(bool)), this, SLOT(setVisibility(bool)));
}

void msegAnnotationInteractor::removeData()
{
    d->view2d->RemoveLayer(d->medVtkView->layer(d->imageData));
    d->view3d->RemoveLayer(d->medVtkView->layer(d->imageData));
    if(d->medVtkView->is2D())
        d->view2d->Render();
    else
        d->view3d->Render();
}


bool msegAnnotationInteractor::showIn2dView() const
{
    return true;
}

bool msegAnnotationInteractor::showIn3dView() const
{
    return true;
}

bool msegAnnotationInteractor::isInSlice( const QVector3D & slicePoint, const QVector3D & sliceNormal,
                                            qreal thickness ) const
{
    return true;
}


void msegAnnotationInteractor::onDataModified( medAbstractData* data )
{
    medAnnotationData * annData = qobject_cast<medAnnotationData*>(data);
    if (!annData)
        return;

    msegAnnotationInteractorPrivate::HelperMap::iterator it = d->dataToHelperIdMap.find(annData);

    if ( it == d->dataToHelperIdMap.end() )
        return;

    msegAnnIntHelper * helper = d->helpers.at( it->second );
    helper->annotationModified(annData);
}


void msegAnnotationInteractor::addAnnotation( medAnnotationData * annData )
{
    if ( d->installedAnnotations.contains(annData) ) {
        return;
    }
    
    typedef msegAnnotationInteractorPrivate::HelperVecType::size_type IndexType;
    bool isAdded = false;
    for (IndexType i(0), end(d->helpers.size()); i<end; ++i){
        msegAnnIntHelper * helper = d->helpers[i];
        if ( helper->addAnnotation(annData) ) {
            isAdded = true;
            d->dataToHelperIdMap[annData] = i;
            break;
        }
    }
    
    if ( isAdded ) {
        d->installedAnnotations.insert( annData );
        
        connect(annData, SIGNAL(dataModified(medAbstractData*)), this, SLOT(onDataModified(medAbstractData*)) );
    }
}

void msegAnnotationInteractor::removeAnnotation( medAnnotationData * annData )
{
    if ( !d->installedAnnotations.contains(annData) ) {
        return;
    }

    msegAnnotationInteractorPrivate::HelperMap::iterator it = d->dataToHelperIdMap.find(annData);
    if ( it == d->dataToHelperIdMap.end() )
        return;

    typedef msegAnnotationInteractorPrivate::HelperVecType::size_type IndexType;
    IndexType iHelper = it->second;

    d->helpers[iHelper]->removeAnnotation( annData );
    d->dataToHelperIdMap.erase(annData);

    d->installedAnnotations.remove( annData );
}

void msegAnnotationInteractor::attachData( medAttachedData* data )
{
    medAnnotationData * annData = qobject_cast<medAnnotationData *>(data);
    if ( annData ) {
        this->addAnnotation(annData);
    }
}

void msegAnnotationInteractor::removeAttachedData( medAttachedData* data )
{
    medAnnotationData * annData = qobject_cast<medAnnotationData *>(data);
    if ( annData ) {
        this->removeAnnotation(annData);
    }
}

QPointF msegAnnotationInteractor::worldToScene( const QVector3D & worldVec ) const
{
    return d->medVtkView->mapWorldToDisplayCoordinates( worldVec );
}

QVector3D msegAnnotationInteractor::sceneToWorld( const QPointF & sceneVec ) const
{
    return d->medVtkView->mapDisplayToWorldCoordinates( sceneVec );
}

QVector3D msegAnnotationInteractor::viewUp() const
{
    return d->medVtkView->viewUp( );
}

bool msegAnnotationInteractor::isPointInSlice(const QVector3D & testPoint, const QVector3D & slicePoint,
                                                 const QVector3D & sliceNormal, qreal thickness ) const
{
    qreal distanceToPlane = QVector3D::dotProduct( (testPoint - slicePoint), sliceNormal );
    
    distanceToPlane = distanceToPlane / sliceNormal.length();
    
    return ( distanceToPlane >= -thickness ) && ( distanceToPlane < thickness );
}

bool msegAnnotationInteractor::isPointInCurrentSlice( const QVector3D & testPoint ) const
{
    return this->isPointInSlice(testPoint, d->medVtkView->viewCenter(), d->medVtkView->viewPlaneNormal(),
                                0.5*d->medVtkView->sliceThickness());
}

void msegAnnotationInteractor::setVisibility(bool visible)
{
    if(visible)
    {
        d->view2d->SetVisibility(1, d->medVtkView->layer(d->imageData));
        d->view3d->SetVisibility(1, d->medVtkView->layer(d->imageData));
    }
    else
    {
        d->view2d->SetVisibility(0, d->medVtkView->layer(d->imageData));
        d->view3d->SetVisibility(0, d->medVtkView->layer(d->imageData));
    }

    d->view2d->Render();
    d->view3d->Render();
}

bool msegAnnotationInteractor::visibility() const
{
    return d->visibiltyParameter->value();
}

QWidget* msegAnnotationInteractor::toolBarWidget()
{
    if(!d->toolbar)
    {
        d->toolbar = new QWidget;
    }
    return d->toolbar;
}

QWidget* msegAnnotationInteractor::toolBoxWidget()
{
    if(!d->toolbox)
    {
        d->toolbox = new QWidget;
    }
    return d->toolbox;
}

QWidget* msegAnnotationInteractor::layerWidget()
{
    QSlider *slider = d->opacityParam->getSlider();
    slider->setOrientation(Qt::Horizontal);
    return d->opacityParam->getSlider();
}

void msegAnnotationInteractor::moveToSlice(int slice)
{
    int zslice = d->view2d->GetSlice();

    if(slice!=zslice)
      d->view2d->SetSlice ( slice );

    if(d->medVtkView->is2D())
        d->view2d->Render();
    else
        d->view3d->Render();
}

void msegAnnotationInteractor::moveToSliceAtPosition(const QVector3D &position)
{
    double pos[3];
    pos[0] = position.x();
    pos[1] = position.y();
    pos[2] = position.z();

    d->view2d->SetCurrentPoint ( pos );
    d->view2d->UpdateCursorPosition(pos);

    if(d->medVtkView->is2D())
    {
        //unsigned int zslice = d->view2d->GetSlice();
        //d->slicingParameter->setValue ( zslice );
    }
}

void msegAnnotationInteractor::setWindowLevel (double &window, double &level)
{

}

void msegAnnotationInteractor::windowLevel(double &window, double &level)
{

}

void msegAnnotationInteractor::setOpacity(double opacity)
{
    d->view3d->SetOpacity (opacity, d->medVtkView->layer(d->imageData));
    d->view2d->SetOpacity (opacity, d->medVtkView->layer(d->imageData));

    d->view2d->Render();
    d->view3d->Render();
}

double msegAnnotationInteractor::opacity() const
{
     double opacity = d->opacityParam->value();
     return opacity;
}


QImage msegAnnotationInteractor::generateThumbnail(const QSize &size)
{
    return QImage();
}

QList<medAbstractParameter*> msegAnnotationInteractor::parameters()
{
    QList<medAbstractParameter*> params;
    params.append(d->opacityParam);
    params.append(d->visibiltyParameter);
    return params;
}


void msegAnnotationInteractor::enableWindowLevelInteraction()
{
    if(d->medVtkView->currentLayer() == d->medVtkView->layer(d->imageData))
    {
        d->view2d->SetLeftButtonInteractionStyle ( vtkInteractorStyleImageView2D::InteractionTypeNull );
    }
    else
    {
        d->view2d->SetLeftButtonInteractionStyle ( vtkInteractorStyleImageView2D::InteractionTypeWindowLevel );
    }
}

////////////////////////////
// msegAnnIntHelper
////////////////////////////

msegAnnIntHelper::msegAnnIntHelper( msegAnnotationInteractor * annInt)
    : m_msegAnnInt( annInt )
{

}

msegAnnIntHelper::~msegAnnIntHelper()
{

}


