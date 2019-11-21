/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAnnotationInteractor.h>

#include <medAbstractData.h>
#include <dtkLog/dtkLog.h>

#include <medAnnotationData.h>
#include <medSeedPointAnnotationData.h>
#include <medImageMaskAnnotationData.h>

#include <medViewFactory.h>
#include <medAbstractImageView.h>
#include <medDoubleParameterL.h>
#include <medBoolParameterL.h>
#include <medIntParameterL.h>

#include <medAnnIntSeedPointHelper.h>
#include <medAnnIntImageMaskHelper.h>

#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkSeedWidget.h>
#include <vtkSeedRepresentation.h>
#include <vtkSphereHandleRepresentation.h>
#include <vtkSmartPointer.h>

#include <medVtkViewBackend.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>

#include <vtkItkConversion.h>

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
    medIntParameterL *slicingParameter;

    medAbstractData *imageData;

    QImage thumbnail;
};

// Implementation
medAnnotationInteractor::medAnnotationInteractor(medAbstractView *parent):
    medAbstractImageViewInteractor(parent), d(new msegAnnotationInteractorPrivate), m_poConv(nullptr)
{
    d->helpers.push_back(new medAnnIntSeedPointHelper(this));
    d->helpers.push_back(new medAnnIntImageMaskHelper(this));

    d->medVtkView = dynamic_cast<medAbstractImageView*>(parent);
    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;

    d->imageData = NULL;

    d->slicingParameter = new medIntParameterL("Slicing", this);
    connect(d->slicingParameter, SIGNAL(valueChanged(int)), this, SLOT(moveToSlice(int)));

    connect(parent, SIGNAL(currentLayerChanged()), this, SLOT(enableWindowLevelInteraction()));
}

medAnnotationInteractor::~medAnnotationInteractor()
{
    qDeleteAll(d->helpers);
    delete d;
    d = NULL;
    delete(m_poConv);
    m_poConv = nullptr;
}

QStringList medAnnotationInteractor::dataHandled()
{
    QStringList d = QStringList() << "medImageMaskAnnotationData";
    return  d;
}

QStringList medAnnotationInteractor::handled() const
{
    return medAnnotationInteractor::dataHandled();
}

QString medAnnotationInteractor::description() const
{
    return tr("Interactor displaying annotations");
}

QString medAnnotationInteractor::identifier() const
{
    return s_identifier();
}

bool medAnnotationInteractor::registered()
{
    medViewFactory *factory = medViewFactory::instance();
    return factory->registerInteractor<medAnnotationInteractor>("medAnnotationInteractor",
                                                                  QStringList () << "medVtkView" <<
                                                                  medAnnotationInteractor::dataHandled());
}

QString medAnnotationInteractor::s_identifier()
{
    return "medAnnotationInteractor";
}


medAbstractImageView * medAnnotationInteractor::getView()
{
    return d->medVtkView;
}

void medAnnotationInteractor::setInputData(medAbstractData *data)
{
    this->medAbstractInteractor::setInputData(data);

    if ( d->imageData )
    {
        disconnect(d->imageData, SIGNAL(dataModified(medAbstractData*)), this, SLOT(onDataModified(medAbstractData*)) );
        // Remove annotations
        foreach( dtkSmartPointer<medAnnotationData> key,  d->installedAnnotations )
        {
            if ( key->parentData() == data )
            {
                this->removeAnnotation( key );
            }
        }
    }
    
    if (data)
    {
        SetViewInput(data);
        medAnnotationData *annItem = qobject_cast<medAnnotationData*>(data);
        attachData(annItem);
        this->updateSlicingParam();
    }
}

bool medAnnotationInteractor::SetViewInput(medAbstractData *data)
{
    bool bRes = data;

    d->imageData = data;

    if (bRes)
    {
        int layer = d->medVtkView->layer(d->imageData);

        typedef itk::Image<unsigned char, 3> MaskType;
        medImageMaskAnnotationData *maskAnnData = dynamic_cast<medImageMaskAnnotationData*>(d->imageData);

        if (MaskType* image = dynamic_cast<MaskType*>((itk::Object*)(maskAnnData->maskData()->data())))
        {
            vtkAlgorithmOutput *poVtkAlgoOutputPort = nullptr;
            vtkMatrix4x4 *poMatrix = nullptr;
            auto *poOldConv = m_poConv;
            m_poConv = new vtkItkConversion<unsigned char, 3>();
            delete poOldConv;
            bRes = m_poConv->SetITKInput(image);
            if (bRes)
            {
                bRes = m_poConv->GetConversion(poVtkAlgoOutputPort, poMatrix);
                if (bRes)
                {
                    d->view2d->SetInput(poVtkAlgoOutputPort, poMatrix, layer);
                    d->view3d->SetInput(poVtkAlgoOutputPort, poMatrix, layer);
                }
            }
        }
        else
        {
            bRes = false;
        }
    }

    return bRes;
}

void medAnnotationInteractor::removeData()
{
    d->view2d->RemoveLayer(d->medVtkView->layer(d->imageData));
    d->view3d->RemoveLayer(d->medVtkView->layer(d->imageData));
    if(d->medVtkView->is2D())
        d->view2d->Render();
    else
        d->view3d->Render();
}


bool medAnnotationInteractor::showIn2dView() const
{
    return true;
}

bool medAnnotationInteractor::showIn3dView() const
{
    return true;
}

bool medAnnotationInteractor::isInSlice( const QVector3D & slicePoint, const QVector3D & sliceNormal,
                                            qreal thickness ) const
{
    return true;
}


void medAnnotationInteractor::onDataModified( medAbstractData* data )
{
    medAnnotationData * annData = qobject_cast<medAnnotationData*>(data);
    if (!annData)
        return;

    msegAnnotationInteractorPrivate::HelperMap::iterator it = d->dataToHelperIdMap.find(annData);

    if ( it == d->dataToHelperIdMap.end() )
        return;

    msegAnnIntHelper * helper = d->helpers.at( it->second );
    helper->annotationModified(annData);
    if(d->medVtkView->is2D())
        d->view2d->Render();
    else
        d->view3d->Render();
}


void medAnnotationInteractor::addAnnotation( medAnnotationData * annData )
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

void medAnnotationInteractor::removeAnnotation( medAnnotationData * annData )
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

void medAnnotationInteractor::attachData( medAttachedData* data )
{
    medAnnotationData * annData = qobject_cast<medAnnotationData *>(data);
    if ( annData ) {
        this->addAnnotation(annData);
    }
}

void medAnnotationInteractor::removeAttachedData( medAttachedData* data )
{
    medAnnotationData * annData = qobject_cast<medAnnotationData *>(data);
    if ( annData ) {
        this->removeAnnotation(annData);
    }
}

QPointF medAnnotationInteractor::worldToScene( const QVector3D & worldVec ) const
{
    return d->medVtkView->mapWorldToDisplayCoordinates( worldVec );
}

QVector3D medAnnotationInteractor::sceneToWorld( const QPointF & sceneVec ) const
{
    return d->medVtkView->mapDisplayToWorldCoordinates( sceneVec );
}

QVector3D medAnnotationInteractor::viewUp() const
{
    return d->medVtkView->viewUp( );
}

bool medAnnotationInteractor::isPointInSlice(const QVector3D & testPoint, const QVector3D & slicePoint,
                                                 const QVector3D & sliceNormal, qreal thickness ) const
{
    qreal distanceToPlane = QVector3D::dotProduct( (testPoint - slicePoint), sliceNormal );

    distanceToPlane = distanceToPlane / sliceNormal.length();

    return ( distanceToPlane >= -thickness ) && ( distanceToPlane < thickness );
}

bool medAnnotationInteractor::isPointInCurrentSlice( const QVector3D & testPoint ) const
{
    return this->isPointInSlice(testPoint, d->medVtkView->viewCenter(), d->medVtkView->viewPlaneNormal(),
                                0.5*d->medVtkView->sliceThickness());
}

void medAnnotationInteractor::setVisibility(bool visible)
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

QWidget* medAnnotationInteractor::buildToolBarWidget()
{
    d->slicingParameter->getSlider()->setOrientation(Qt::Horizontal);
    return d->slicingParameter->getSlider();
}

QWidget* medAnnotationInteractor::buildToolBoxWidget()
{

    return NULL;
}

QWidget* medAnnotationInteractor::buildLayerWidget()
{
    QSlider *slider = this->opacityParameter()->getSlider();
    slider->setOrientation(Qt::Horizontal);
    return slider;
}

void medAnnotationInteractor::moveToSlice(int slice)
{
    //TODO find a way to get woorldCoordinate for slice from vtkInria.
    // instead of moving to the slice corresponding on the first layer dropped.
    if(d->medVtkView->is2D() && slice != d->view2d->GetSlice())
    {
        d->view2d->SetSlice(slice);
        d->view2d->Render();
     }
}

void medAnnotationInteractor::setWindowLevel (QHash<QString,QVariant>)
{

}

void medAnnotationInteractor::setOpacity(double opacity)
{
    d->view3d->SetOpacity (opacity, d->medVtkView->layer(d->imageData));
    d->view2d->SetOpacity (opacity, d->medVtkView->layer(d->imageData));

    d->view2d->Render();
    d->view3d->Render();
}


void medAnnotationInteractor::setUpViewForThumbnail()
{

}

QList<medAbstractParameterL*> medAnnotationInteractor::linkableParameters()
{
    QList<medAbstractParameterL*> params;
    params.append(this->opacityParameter());
    params.append(this->visibilityParameter());
    return params;
}

void medAnnotationInteractor::updateWidgets()
{
    if(!d->medVtkView->is2D())
        d->slicingParameter->getSlider()->setEnabled(false);
    else
    {
        d->slicingParameter->getSlider()->setEnabled(true);
        this->updateSlicingParam();
    }
}

void medAnnotationInteractor::updateSlicingParam()
{
    if(!d->medVtkView->is2D())
        return;

    d->slicingParameter->blockSignals(true);
    d->slicingParameter->setRange(d->view2d->GetSliceMin(), d->view2d->GetSliceMax());
    d->slicingParameter->blockSignals(false);

    d->slicingParameter->setValue(d->view2d->GetSlice());
}

QList<medBoolParameterL*> medAnnotationInteractor::mouseInteractionParameters()
{
    // no parameters related to mouse interactions
    // TODO: actually segmentation is directly related to mouse interaction
    // maybe we should investigate to add a "paint" button in the mouse interaction widget
    return QList<medBoolParameterL*>();
}

void medAnnotationInteractor::enableWindowLevelInteraction()
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

msegAnnIntHelper::msegAnnIntHelper( medAnnotationInteractor * annInt)
    : m_msegAnnInt( annInt )
{

}

msegAnnIntHelper::~msegAnnIntHelper()
{

}


