/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkImage3DDisplay.h"
#include "vtkImageView3D.h"

#ifndef VTK_MAJOR_VERSION
#  include "vtkVersion.h"
#endif

#include <vtkActor.h>
#include <vtkAnnotatedCubeActor.h>
#include <vtkColorTransferFunction.h>
#include <vtkDataSetCollection.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkImageActor.h>
#include <vtkImageAppendComponents.h>
#include <vtkImageCast.h>
#include <vtkImageMapper3D.h>
#include <vtkImageMapToColors.h>
#include <vtkInteractorStyleTrackball.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPointSet.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProp3DCollection.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRendererCollection.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkTextProperty.h>
#include <vtkTransferFunctionPresets.h>
#include <vtkLookupTableManager.h>

#include <vtkMatrix4x4.h>

vtkStandardNewMacro(vtkImageView3D)

vtkImageView3D::vtkImageView3D()
{

    this->Callback    = vtkImageView3DCroppingBoxCallback::New();
    this->BoxWidget   = vtkOrientedBoxWidget::New();
    this->PlaneWidget = vtkPlaneWidget::New();
    this->Marker      = vtkOrientationMarkerWidget::New();
    this->Cube        = vtkAnnotatedCubeActor::New();

    this->PlanarWindowLevelX = vtkSmartPointer<vtkImageMapToColors>::New();
    this->PlanarWindowLevelY = vtkSmartPointer<vtkImageMapToColors>::New();
    this->PlanarWindowLevelZ = vtkSmartPointer<vtkImageMapToColors>::New();
    this->ActorX = vtkImageActor::New();
    this->ActorY = vtkImageActor::New();
    this->ActorZ = vtkImageActor::New();

    this->ExtraPlaneCollection = vtkProp3DCollection::New();
    this->ExtraPlaneInputCollection = vtkProp3DCollection::New();

    this->SetupWidgets();

    this->ShowAnnotationsOn();
    this->TextProperty->SetColor (0,0,0);
    double white[3] = {0.9, 0.9, 0.9};
    this->SetBackground (white);

    this->RenderingMode = PLANAR_RENDERING;
    this->ShowActorX = 1;
    this->ShowActorY = 1;
    this->ShowActorZ = 1;

    this->Opacity    = 1.0;
    this->Visibility = 1;

    this->CroppingMode = CROPPING_OFF;

    auto  styleswitch = vtkInteractorStyleSwitch::New();
    styleswitch->SetCurrentStyleToTrackballCamera();
    this->SetInteractorStyle ( styleswitch );
    styleswitch->Delete();

    this->Renderer= vtkRenderer::New();
}

vtkImageView3D::~vtkImageView3D()
{
    // delete all vtk objects
    this->LayerInfoVec.clear();  // Delete handled by smartpointer

    this->BoxWidget->Delete();
    this->Callback->Delete();
    this->Cube->Delete();
    this->Marker->Delete();
    this->PlaneWidget->Delete();
    this->ActorX->Delete();
    this->ActorY->Delete();
    this->ActorZ->Delete();
    this->ExtraPlaneCollection->Delete();
    this->ExtraPlaneInputCollection->Delete();
}

//----------------------------------------------------------------------------
//                      GEOMETRY
//----------------------------------------------------------------------------
void vtkImageView3D::GetInputBounds ( double * bounds ) {
    bounds [0] = VTK_DOUBLE_MAX;
    bounds [1] = VTK_DOUBLE_MIN;
    bounds [2] = VTK_DOUBLE_MAX;
    bounds [3] = VTK_DOUBLE_MIN;
    bounds [4] = VTK_DOUBLE_MAX;
    bounds [5] = VTK_DOUBLE_MIN;

    for ( size_t layer(0); layer<this->LayerInfoVec.size(); ++layer )
    {
        if (this->HasLayer(layer))
        {
            auto imageDisplay = GetImage3DDisplayForLayer(layer);
            if (imageDisplay)
            {
                const int* wholeExtent = imageDisplay->GetMedVtkImageInfo()->extent ;
                const double * spacing = imageDisplay->GetMedVtkImageInfo()->spacing;
                const double * origin = imageDisplay->GetMedVtkImageInfo ()->origin ;

                for ( int i(0); i < 3; ++i )
                {
                    bounds[2*i] = (spacing[i]*wholeExtent[2*i]+origin[i])<bounds[2*i] ? (spacing[i]*wholeExtent[2*i]+origin[i]) : bounds[2*i];
                    bounds[2*i+1] = (spacing[i]*wholeExtent[2*i+1]+origin[i])>bounds[2*i+1] ? (spacing[i]*wholeExtent[2*i+1]+origin[i]) : bounds[2*i+1] ;
                }
            }
        }
    }
}

void vtkImageView3D::SetOrientationMatrix (vtkMatrix4x4* matrix)
{
    this->Superclass::SetOrientationMatrix (matrix);
    OrientationMatrix->PrintSelf(std::cout,vtkIndent(0));
    for(auto it : VolumesActor)
    {
        it->SetUserMatrix(this->OrientationMatrix);
    }

    this->BoxWidget->SetOrientationMatrix (this->OrientationMatrix);
    this->ActorX->SetUserMatrix (this->OrientationMatrix);
    this->ActorY->SetUserMatrix (this->OrientationMatrix);
    this->ActorZ->SetUserMatrix (this->OrientationMatrix);
}


//----------------------------------------------------------------------------
//                      DATA MANAGEMENT
//----------------------------------------------------------------------------

medVtkImageInfo* vtkImageView3D::GetMedVtkImageInfo(int layer /*= 0*/) const
{
    auto imageDisplay = this->GetImage3DDisplayForLayer(layer);
    medVtkImageInfo *imageInfo = nullptr;

    if (imageDisplay)
    {
        imageInfo = imageDisplay->GetMedVtkImageInfo();
    }

    return imageInfo;
}

//! Get layer specific info
vtkImage3DDisplay * vtkImageView3D::GetImage3DDisplayForLayer( int layer ) const
{
    if ( layer > -1 && (unsigned int)layer < this->LayerInfoVec.size())
    {
        return this->LayerInfoVec.at(layer).ImageDisplay;
    }
    else return nullptr;
}

int vtkImageView3D::GetNumberOfLayers() const
{
    return static_cast<int>(this->LayerInfoVec.size());
}


void vtkImageView3D::RemoveAllLayers()
{
    while (this->LayerInfoVec.size() > 1)
    {
        this->RemoveLayer (static_cast<int>(this->LayerInfoVec.size() -1));
    }
}

void vtkImageView3D::RemoveVolume(int layer)
{
    if(this->Renderer)
    {
        this->Renderer->RemoveViewProp (this->VolumesActor[layer]);
    }

    this->VolumesActor[layer]->SetProperty (nullptr);
    this->VolumesActor[layer]->SetMapper (nullptr);
    VolumesActor[layer]->Delete();
    VolumesActor.erase(VolumesActor.begin()+layer);

    VolumesProperty[layer]->Delete();
    VolumesProperty.erase(VolumesProperty.begin()+layer);

    this->VolumesMapper[layer]->SetInputConnection(nullptr);
    this->VolumesMapper[layer]->Delete();
    VolumesMapper.erase(VolumesMapper.begin()+layer);

}

void vtkImageView3D::RemoveLayer (int layer)
{
    if (this->HasLayer(layer))
    {
        vtkRenderer *renderer = this->Renderer;
        vtkImageView::RemoveLayer(layer);
        RemoveVolume(layer);

        this->LayerInfoVec.erase(this->LayerInfoVec.begin() + layer);
        IsFirstImageLoaded = (layer == 0);
        InternalUpdate();
        if (renderer)
        {
            renderer->RemoveAllViewProps();
            for(int i=0; i< this->DataSetActorCollection->GetNumberOfItems(); ++i)
            {
                auto poWidget = vtkActor::SafeDownCast(this->DataSetActorCollection->GetItemAsObject(i));
                poWidget->VisibilityOff();
            }
            if (this->GetRenderWindow())
            {
                this->GetRenderWindow()->RemoveRenderer(renderer);
                // to see SetRenderWindow(GetRenderWindow()) , maybe only interactor step is needed
                this->UnInstallPipeline();
                this->InstallPipeline();
            }
            this->Modified();
        }

        if(this->LayerInfoVec.size()>0 && layer == 0)
        {
            auto imDisplay = this->GetImage3DDisplayForLayer(0);
            if(imDisplay && imDisplay->GetInputProducer())
            {
                this->Superclass::SetInput(imDisplay->GetInputProducer()->GetOutputPort(), VolumesActor[0]->GetUserMatrix(), layer);
            }
            double *range = m_poInternalImageFromInput->GetScalarRange();
            this->SetColorRange(range, 0);
            this->initializeTransferFunctions(0);
        }
    }
}

void vtkImageView3D::AddLayer (int layer)
{
    if (this->HasLayer(layer) || (layer < 0) )
    {
        return;
    }
    if ( static_cast<size_t>(layer) >= this->LayerInfoVec.size() )
    {
        this->LayerInfoVec.resize(layer + 1);
    }
    // needs to instantiate objects for layers being created
    for ( size_t i(0); i<this->LayerInfoVec.size(); ++i )
    {
        if (!this->LayerInfoVec[i].ImageDisplay)
        {
            this->LayerInfoVec[i].ImageDisplay = vtkSmartPointer<vtkImage3DDisplay>::New();
        }
    }

}


void vtkImageView3D::AddVolume(int layer)
{
    this->VolumesProperty.push_back(vtkVolumeProperty::New());
    this->VolumesActor.push_back(vtkVolume::New());
    this->VolumesMapper.push_back(vtkSmartVolumeMapper::New());

#ifdef __APPLE__
    this->VolumesMapper[layer]->SetRequestedRenderMode( vtkSmartVolumeMapper::RayCastRenderMode );
#else
    this->VolumesMapper[layer]->SetRequestedRenderMode( vtkSmartVolumeMapper::DefaultRenderMode );
#endif

    this->VolumesMapper[layer]->SetInputConnection( GetImage3DDisplayForLayer(layer)->GetInputProducer()->GetOutputPort());
    this->VolumesMapper[layer]->Update();
    this->VolumesMapper[layer]->Modified();

    this->SetCroppingModeToInside();
    this->VolumesProperty[layer]->SetInterpolationTypeToLinear();
    this->VolumesProperty[layer]->SetDiffuse (0.9);
    this->VolumesProperty[layer]->SetAmbient (0.15);
    this->VolumesProperty[layer]->SetSpecular (0.3);
    this->VolumesProperty[layer]->SetSpecularPower (15.0);

    this->VolumesActor[layer]->SetProperty ( this->VolumesProperty[layer]);
    this->VolumesActor[layer]->SetMapper ( this->VolumesMapper[layer] );
    this->VolumesActor[layer]->PickableOff();
    this->VolumesActor[layer]->DragableOff();

    for(auto volActor : VolumesActor)
    {
        volActor->SetVisibility (this->RenderingMode == vtkImageView3D::VOLUME_RENDERING);
    }

    if(this->Renderer)
    {
        this->Renderer->AddViewProp (this->VolumesActor[layer]);
    }

}

void vtkImageView3D::SetInput(vtkAlgorithmOutput* pi_poVtkAlgoOutput, vtkMatrix4x4 *matrix /*= 0*/, int layer /*= 0*/)
{
    if(pi_poVtkAlgoOutput)
    {
        this->AddLayer(layer);

        auto image = static_cast<vtkImageAlgorithm*>(pi_poVtkAlgoOutput->GetProducer())->GetOutput();
        auto cellType = image->GetCellType(0);
        auto imageDisplay = this->GetImage3DDisplayForLayer(layer);
        imageDisplay->SetInputProducer(pi_poVtkAlgoOutput);
        imageDisplay->SetInputData(image);

        double *range = image->GetScalarRange();
        this->SetColorRange(range, layer);

        if(IsFirstImageLoaded && cellType==VTK_VOXEL) {
            IsFirstImageLoaded = false;
            this->Superclass::SetInput(pi_poVtkAlgoOutput, matrix, layer);
        }

        if(cellType==VTK_VOXEL)
        {
            this->GetActorX()->SetOpacity(1.0);
            this->GetActorY()->SetOpacity(1.0);
            this->GetActorZ()->SetOpacity(1.0);
            AddVolume(layer);
        }
        this->InternalUpdate();
    }
}

void vtkImageView3D::SetInput(vtkActor * actor, int layer, vtkMatrix4x4 * matrix, const int imageSize[3], const double imageSpacing[], const double imageOrigin[])
{
    auto numberOfLayers = GetNumberOfLayers();
    if (numberOfLayers == 0 && matrix)
    {
        this->SetOrientationMatrix(matrix);
    }
    vtkDataSet *arg = actor->GetMapper()->GetInput();
    // If this is the first widget to be added, reset camera
    if (!this->GetMedVtkImageInfo() || !this->GetMedVtkImageInfo()->initialized)
    {
        this->ResetCamera(arg);
    }
    this->DataSetCollection->AddItem(arg);
    this->DataSetActorCollection->AddItem(actor);
    this->Renderer->AddViewProp(actor); //same as this->Renderer->AddActor(actor);
}

void vtkImageView3D::RemoveDataSet(vtkPointSet* arg)
{
    vtkProp3D* actor = this->FindDataSetActor (arg);
    if (actor)
        this->Renderer->RemoveViewProp (actor);

    this->Superclass::RemoveDataSet (arg);
}

bool vtkImageView3D::HaveAnyImage(void) const
{
    return VolumesActor.size()>0;
}

vtkActor* vtkImageView3D::AddDataSet (vtkPointSet* arg, vtkProperty* prop)
{
    auto geometryextractor = vtkDataSetSurfaceFilter::New();
    auto mapper = vtkPolyDataMapper::New();
    auto actor = vtkActor::New();

    geometryextractor->SetInputData (arg);
    mapper->SetInputConnection(geometryextractor->GetOutputPort());

    actor->SetMapper (mapper);

    if (prop)
        actor->SetProperty (prop);

    this->Renderer->AddViewProp(actor);

    mapper->Delete();
    geometryextractor->Delete();
    actor->Delete();

    // If this is the first widget to be added, reset camera
    if ( ! this->GetMedVtkImageInfo() || !this->GetMedVtkImageInfo()->initialized)
    {
        this->ResetCamera(arg);
    }

    this->DataSetCollection->AddItem (arg);
    this->DataSetActorCollection->AddItem ( actor);

    // the actor is actually not deleted as it has
    // been referenced in the renderer, so we can
    // safely return it. well hopefully.
    return actor;
}

vtkActor* vtkImageView3D::DataSetToActor(vtkPointSet* arg, vtkProperty* prop)
{
    auto geometryextractor = vtkDataSetSurfaceFilter::New();
    auto normalextractor = vtkPolyDataNormals::New();
    auto mapper = vtkPolyDataMapper::New();
    auto actor = vtkActor::New();

    normalextractor->SetFeatureAngle(90);
    ///\todo try to skip the normal extraction filter in order to
    // enhance the visualization speed when the data is time sequence.
    geometryextractor->SetInputData(arg);
    normalextractor->SetInputConnection(geometryextractor->GetOutputPort());
    mapper->SetInputConnection(normalextractor->GetOutputPort());
    actor->SetMapper(mapper);
    if (prop)
    {
        actor->SetProperty(prop);
    }
    mapper->Delete();
    normalextractor->Delete();
    geometryextractor->Delete();
    return actor;
}


void vtkImageView3D::InternalUpdate()
{
    if(this->LayerInfoVec.size() >0 )
    {

        auto imDisplay = GetImage3DDisplayForLayer(CurrentLayer);
        if(imDisplay && imDisplay->GetInputProducer())
        {
            PlanarWindowLevelX->SetInputConnection(imDisplay->GetInputProducer()->GetOutputPort());
            PlanarWindowLevelX->SetOutputFormatToRGB();
            PlanarWindowLevelX->UpdateInformation();
            PlanarWindowLevelX->Update();

            PlanarWindowLevelY->SetInputConnection(imDisplay->GetInputProducer()->GetOutputPort());
            PlanarWindowLevelY->SetOutputFormatToRGB();
            PlanarWindowLevelY->UpdateInformation();
            PlanarWindowLevelY->Update();

            PlanarWindowLevelZ->SetInputConnection(imDisplay->GetInputProducer()->GetOutputPort());

            PlanarWindowLevelZ->SetOutputFormatToRGB();
            PlanarWindowLevelZ->UpdateInformation();
            PlanarWindowLevelZ->Update();
            this->ActorX->GetMapper()->SetInputConnection(nullptr);
            this->ActorY->GetMapper()->SetInputConnection(nullptr);
            this->ActorZ->GetMapper()->SetInputConnection(nullptr);
            auto image = imDisplay->GetInputProducer()->GetOutput();
            double * bounds = image->GetBounds();
            this->BoxWidget->PlaceWidget (bounds);

            this->PlaneWidget->SetInputConnection(imDisplay->GetInputProducer()->GetOutputPort());
            this->PlaneWidget->PlaceWidget(bounds);
            this->UpdateDisplayExtent();
            auto lut = this->VolumesProperty[CurrentLayer]->GetRGBTransferFunction(0);
            if (lut)
            {
                this->PlanarWindowLevelX->SetLookupTable(lut);
                this->PlanarWindowLevelY->SetLookupTable(lut);
                this->PlanarWindowLevelZ->SetLookupTable(lut);
                this->ActorX->GetMapper()->SetInputConnection(this->PlanarWindowLevelX->GetOutputPort());
                this->ActorY->GetMapper()->SetInputConnection(this->PlanarWindowLevelY->GetOutputPort());
                this->ActorZ->GetMapper()->SetInputConnection(this->PlanarWindowLevelZ->GetOutputPort());
            }
        }
    }
    else
    {
        this->ActorX->SetVisibility(0);
        this->ActorY->SetVisibility(0);
        this->ActorZ->SetVisibility(0);

        this->ActorX->GetMapper()->SetInputConnection (nullptr);
        this->ActorY->GetMapper()->SetInputConnection (nullptr);
        this->ActorZ->GetMapper()->SetInputConnection (nullptr);

        this->BoxWidget->SetInputConnection (nullptr);
        this->PlaneWidget->SetInputConnection(nullptr);

    }
}

//----------------------------------------------------------------------------
//                      VOLUME PROPERTY
//----------------------------------------------------------------------------

//The code below is useful for Volume rendering and more precisely for LUT
void  vtkImageView3D::initializeTransferFunctions(int pi_iLayer)
{
    auto rgb = this->GetDefaultColorTransferFunction();
    auto alpha = this->GetDefaultOpacityTransferFunction();

    this->SetTransferFunctions(rgb, alpha, pi_iLayer);
    this->UpdateVolumeFunctions(pi_iLayer);
    rgb->Delete();
    alpha->Delete();
}



void vtkImageView3D::SetCurrentLayer(int layer) {

    this->Superclass::SetCurrentLayer(layer);

    InternalUpdate();
    Render();

}


void vtkImageView3D::UpdateVolumeFunctions(int layer)
{
    auto lookuptable = this->GetLookupTable(layer);
    if ( !this->GetUseLookupTable(layer) || lookuptable == nullptr )
        return;
    auto imageDisplay = this->GetImage3DDisplayForLayer(layer);
    if (!imageDisplay)
        return ;
    double opacityValue = imageDisplay->GetOpacity();

    auto color   = this->VolumesProperty[layer]->GetRGBTransferFunction(0);
    auto opacity = this->VolumesProperty[layer]->GetScalarOpacity(0);

    const double * range = lookuptable->GetRange();
    double width = range[1] - range[0];

    int numColors = lookuptable->GetNumberOfTableValues();
    double factor = 1.0 / static_cast< double >( numColors - 1 );
    color->RemoveAllPoints();
    opacity->RemoveAllPoints();
    double val[4];
    for ( int i = 0; i < numColors; ++i )
    {
        double x = range[0] + factor * static_cast< double >( i ) * width;
        lookuptable->GetTableValue( i,val );
        color->AddRGBPoint( x, val[0], val[1], val[2]);
        opacity->AddPoint( x, val[3]*opacityValue);
    }

}

//----------
// Get
double vtkImageView3D::GetColorLevel(int layer)const
{
    if(auto imageDisplay = this->GetImage3DDisplayForLayer(layer))
    {
        return imageDisplay->GetColorLevel();
    }
    else
    {
        return 0;
    }
}

double vtkImageView3D::GetColorWindow(int layer)const
{
    if(auto imageDisplay = this->GetImage3DDisplayForLayer(layer))
    {
        return imageDisplay->GetColorWindow();
    }
    else
    {
        return 0;
    }
}

double vtkImageView3D::GetOpacity(int layer) const
{
    double dfRes = 0.0;
    if (this->HasLayer(layer))
    {
        if (auto imageDisplay = GetImage3DDisplayForLayer(layer))
        {
            dfRes = imageDisplay->GetOpacity();
        }
    }
    return dfRes;
}

vtkPiecewiseFunction * vtkImageView3D::GetOpacityTransferFunction(int layer) const
{
    if (this->HasLayer(layer) && this->VolumesProperty.size()>layer)
    {
        return this->VolumesProperty[layer]->GetScalarOpacity(0);
    }
    else
    {
        return nullptr;
    }
}

vtkColorTransferFunction * vtkImageView3D::GetColorTransferFunction(int layer) const
{
    if (this->HasLayer(layer) && this->VolumesProperty.size()>0 &&
            this->VolumesProperty[layer])
    {
        return this->VolumesProperty[layer]->GetRGBTransferFunction(0);
    }
    else
    {
        return nullptr;
    }
}

vtkLookupTable * vtkImageView3D::GetLookupTable(int layer) const
{
    if(auto imageDisplay = this->GetImage3DDisplayForLayer(layer))
    {
        return imageDisplay->GetLookupTable();
    }
    else
    {
        return nullptr;
    }
}

bool vtkImageView3D::GetUseLookupTable(int layer) const
{
    if(auto imageDisplay = this->GetImage3DDisplayForLayer(layer))
    {
        return imageDisplay->GetUseLookupTable();
    }
    else
    {
        return false;
    }
}


int vtkImageView3D::GetVisibility (int layer) const
{
    int iRes = 0;
    if (this->HasLayer(layer))
    {
        if (auto imageDisplay = this->GetImage3DDisplayForLayer(layer))
        {
            iRes = imageDisplay->GetVisibility();
        }
    }
    return iRes;
}

//-----------
// Store

void vtkImageView3D::StoreOpacityTransferFunction(vtkPiecewiseFunction *otf, int layer)
{
    if(this->HasLayer(layer)&& VolumesProperty.size()>layer)
    {
        this->VolumesProperty[layer]->SetScalarOpacity(0,otf);
    }
}

void vtkImageView3D::StoreColorTransferFunction(vtkColorTransferFunction *ctf, int layer)
{
    if(this->HasLayer(layer) && VolumesProperty.size()>layer)
    {
        this->VolumesProperty[layer]->SetColor(0,ctf);
        if (layer ==0)
        {
            //only update multiplanar windowlevel if layer 0 changes
            this->PlanarWindowLevelX->SetLookupTable(this->VolumesProperty[layer]->GetRGBTransferFunction());
            this->PlanarWindowLevelY->SetLookupTable(this->VolumesProperty[layer]->GetRGBTransferFunction());
            this->PlanarWindowLevelZ->SetLookupTable(this->VolumesProperty[layer]->GetRGBTransferFunction());
        }
    }

}

void vtkImageView3D::StoreLookupTable(vtkLookupTable *lookuptable, int layer)
{
                                                    if (auto imageDisplay = this->GetImage3DDisplayForLayer(layer))
    {
        imageDisplay->SetLookupTable(lookuptable);
        InternalUpdate();
    }
}

void vtkImageView3D::StoreColorLevel(double s, int layer)
{
    if (auto imageDisplay = this->GetImage3DDisplayForLayer(layer))
    {
        imageDisplay->SetColorLevel(s);
        this->UpdateVolumeFunctions(layer);
    }
}

void vtkImageView3D::StoreColorWindow(double s, int layer)
{
    if (auto imageDisplay = this->GetImage3DDisplayForLayer(layer))
    {
        imageDisplay->SetColorWindow( s );
        this->UpdateVolumeFunctions(layer);
    }
}


//--------------
// Setters
/**
* Transfer functions define the mapping of the intensity or color
* values in the image to colors and opacity displayed on the screen.
*/
void vtkImageView3D::SetTransferFunctions (vtkColorTransferFunction * color,
                                           vtkPiecewiseFunction * opacity,
                                           int layer)
{
    if (this->HasLayer (layer))
    {
        if (auto imageDisplay = this->GetImage3DDisplayForLayer(layer))
        {
            if (imageDisplay->GetMedVtkImageInfo() && VolumesProperty.size()>layer)
            {
                double *range = imageDisplay->GetMedVtkImageInfo()->scalarRange;
                this->SetTransferFunctionRangeFromWindowSettings(color, opacity, range[0], range[1]);

                this->VolumesProperty[layer]->SetColor(0, color);
                this->VolumesProperty[layer]->SetScalarOpacity(0, opacity);

                if (layer == 0)
                {
                    //update planar window level only if we change layer 0
                    this->PlanarWindowLevelX->SetLookupTable(color);
                    this->PlanarWindowLevelY->SetLookupTable(color);
                    this->PlanarWindowLevelZ->SetLookupTable(color);
                }
            }
        }
    }
}

/** Set a user-defined lookup table */
void vtkImageView3D::SetLookupTable (vtkLookupTable* lookuptable,int layer)
{
    this->Superclass::SetLookupTable (lookuptable,layer);
    this->UpdateVolumeFunctions(layer);
}

/** Set a user-defined lookup table */
void vtkImageView3D::SetLookupTable (vtkLookupTable* lookuptable, QString name,int layer)
{
    if (lookuptable && this->HasLayer(layer))
    {
        if (auto imageDisplay = this->GetImage3DDisplayForLayer(layer))
        {
            imageDisplay->SetLookupTableName(name);
        }
        this->Superclass::SetLookupTable (lookuptable,layer);
        this->UpdateVolumeFunctions(layer);
    }
}

void vtkImageView3D::SetUseLookupTable(bool use, int layer)
{
    if (auto imageDisplay = this->GetImage3DDisplayForLayer(layer))
    {
        imageDisplay->SetUseLookupTable( use );
    }
}

void vtkImageView3D::SetOpacity(double opacity, int layer)
{
    if (this->HasLayer(layer))
    {
        auto imageDisplay = this->GetImage3DDisplayForLayer(layer);
        if (imageDisplay)
        {
            imageDisplay->SetOpacity(opacity);
            QString value = imageDisplay->GetLookupTableName();

            typedef vtkTransferFunctionPresets Presets;
            auto lut = vtkLookupTableManager::GetLookupTable(value.toStdString());
            if(lut)
            {
                SetLookupTable(lut,value, layer);
            }

            auto scalar_opacity = this->VolumesProperty[layer]->GetScalarOpacity(0);
            const unsigned int n = scalar_opacity->GetSize();

            for ( unsigned int i = 0; i < n; ++i )
            {
                double val[4];
                scalar_opacity->GetNodeValue( i, val );
                val[1] = val[1] * opacity;
                scalar_opacity->SetNodeValue( i, val );
            }
        }
    }
}

void vtkImageView3D::SetColorWindowLevel(double w, double l, int layer)
{
    if (this->VolumesProperty.size()>layer)
    {
        this->Superclass::SetColorWindowLevel (w,l,layer);
        this->UpdateVolumeFunctions(layer);
    }

}

void vtkImageView3D::SetColorLevel (double s,int layer)
{
    if (this->VolumesProperty.size()>layer)
    {
        this->Superclass::SetColorLevel(s,layer);
        this->UpdateVolumeFunctions(layer);
    }
}

void vtkImageView3D::SetColorWindow (double s,int layer)
{
    if (this->VolumesProperty.size()>layer)
    {
        this->Superclass::SetColorWindow(s,layer);
        this->UpdateVolumeFunctions(layer);
    }
}

void vtkImageView3D::SetVisibility (int visibility, int layer)
{
    if (layer == 0 && this->RenderingMode == vtkImageView3D::PLANAR_RENDERING)
    {
        this->ActorX->SetVisibility(visibility);
        this->ActorY->SetVisibility(visibility);
        this->ActorZ->SetVisibility(visibility);
    }
    if (this->HasLayer(layer) && VolumesActor.size()>layer)
    {
        this->VolumesActor[layer]->SetVisibility(this->RenderingMode == vtkImageView3D::VOLUME_RENDERING);
        if (auto imageDisplay = this->GetImage3DDisplayForLayer(layer))
        {
            imageDisplay->SetVisibility(visibility);
        }
    }
}

void vtkImageView3D::SetInterpolationToNearestNeighbor()
{
    for(auto volumProp : VolumesProperty)
    {
        volumProp->SetInterpolationTypeToNearest();
    }
}

void vtkImageView3D::SetInterpolationToLinear()
{
    for(auto volumProp : VolumesProperty)
    {
        volumProp->SetInterpolationTypeToLinear();
    }
}

void vtkImageView3D::SetShade (int a)
{
    for(auto volumProp : VolumesProperty)
    {
        volumProp->SetShade(a);
    }
}

bool vtkImageView3D::GetShade()
{
    if(VolumesProperty.size()>0)
        return this->VolumesProperty[0]->GetShade();
    else {
        return false;
    }
}

void vtkImageView3D::ApplyColorTransferFunction(vtkScalarsToColors * colors, int layer)
{
    return;
}

vtkVolume * vtkImageView3D::GetVolumeActor(int layer)
{
    if(layer<VolumesActor.size() && layer>=0)
    {
        return VolumesActor[layer];
    }
    else {
        return nullptr;
    }
}

vtkVolume * vtkImageView3D::GetVolumeActor()
{
    return GetVolumeActor(GetCurrentLayer());
}

vtkVolumeProperty * vtkImageView3D::GetVolumeProperty(int layer)
{
    if(layer<VolumesProperty.size() && layer>=0)
    {
        return VolumesProperty[layer];
    }
    else {
        return nullptr;
    }
}

vtkVolumeProperty * vtkImageView3D::GetVolumeProperty()
{
    return GetVolumeProperty(GetCurrentLayer());
}

//----------------------------------------------------------------------------
//                      VOLUME MAPPER
//----------------------------------------------------------------------------
/** Set the rendering mode. */
void vtkImageView3D::SetRenderingMode(int arg)
{
    this->RenderingMode = arg;
    for(auto volActor:VolumesActor)
    {
        volActor->SetVisibility (arg == vtkImageView3D::VOLUME_RENDERING);
    }
    this->ActorX->SetVisibility ((arg == vtkImageView3D::PLANAR_RENDERING) && this->ShowActorX);
    this->ActorY->SetVisibility ((arg == vtkImageView3D::PLANAR_RENDERING) && this->ShowActorY);
    this->ActorZ->SetVisibility ((arg == vtkImageView3D::PLANAR_RENDERING) && this->ShowActorZ);

    this->ExtraPlaneCollection->InitTraversal();
    vtkProp3D* item = this->ExtraPlaneCollection->GetNextProp3D();
    while(item)
    {
        item->SetVisibility(arg == vtkImageView3D::PLANAR_RENDERING);
        item = this->ExtraPlaneCollection->GetNextProp3D();
    }
}

void vtkImageView3D::SetVolumeMapperToRayCast()
{
    for(auto volMapper : VolumesMapper)
    {
        volMapper->SetRequestedRenderMode(vtkSmartVolumeMapper::RayCastRenderMode );
    }
}
#ifdef MED_USE_OSPRAY_4_VR_BY_CPU
void vtkImageView3D::SetVolumeMapperToOSPRayRenderMode()
{
    for(auto volMapper:VolumesMapper)
    {
        volMapper->SetRequestedRenderMode(vtkSmartVolumeMapper::OSPRayRenderMode);
    }
}
#endif // MED_USE_OSPRAY_4_VR_BY_CPU
void vtkImageView3D::SetVolumeMapperToGPU()
{
    for(auto volMapper:VolumesMapper)
    {
        volMapper->SetRequestedRenderMode(vtkSmartVolumeMapper::GPURenderMode );
    }
}
void vtkImageView3D::SetVolumeMapperToDefault()
{
    for(auto volMapper : VolumesMapper)
    {
        volMapper->SetRequestedRenderMode(vtkSmartVolumeMapper::DefaultRenderMode );
    }
}
void vtkImageView3D::SetVolumeRayCastFunctionToComposite()
{
    for(auto volMapper : VolumesMapper)
    {
        volMapper->SetBlendModeToComposite();
    }
}
void vtkImageView3D::SetVolumeRayCastFunctionToMaximumIntensityProjection()
{
    for(auto volMapper : VolumesMapper)
    {
        volMapper->SetBlendModeToMaximumIntensity();
    }
}
void vtkImageView3D::SetVolumeRayCastFunctionToMinimumIntensityProjection()
{
    for(auto volMapper : VolumesMapper)
    {
        volMapper->SetBlendModeToMinimumIntensity();
    }
}

void vtkImageView3D::SetCroppingModeToOff()
{
    this->SetCroppingMode( CROPPING_OFF );
}

unsigned int vtkImageView3D::GetCroppingMode()
{
    return this->CroppingMode;
}
void vtkImageView3D::SetCroppingModeToInside()
{
    this->SetCroppingMode( CROPPING_INSIDE );
}

void vtkImageView3D::SetCroppingModeToOutside()
{
    this->SetCroppingMode( CROPPING_OUTSIDE );
}

void vtkImageView3D::SetCroppingMode( unsigned int mode )
{
    this->CroppingMode = mode;
    vtkImage3DDisplay *imageDisplay;
    double * bounds;

    switch ( mode )
    {
    case CROPPING_OFF:
        for(auto volMapper:VolumesMapper)
        {
            volMapper->CroppingOff();
        }
        break;
    case CROPPING_OUTSIDE:
        for(auto volMapper : VolumesMapper)
        {
            volMapper->CroppingOn();
            volMapper->SetCroppingRegionFlagsToSubVolume();
        }
        if(imageDisplay = this->GetImage3DDisplayForLayer(this->CurrentLayer) )
        {
            bounds = imageDisplay->GetInputProducer()->GetOutput()->GetBounds();
            this->BoxWidget->PlaceWidget (bounds);
            this->Callback->SetVolumeMapper ( this->VolumesMapper[this->CurrentLayer]);
        }
        break;
    case CROPPING_INSIDE:         // fall through to default
    default:                      // default is CROPPING_INSIDE
        for(auto volMapper : VolumesMapper)
        {
            volMapper->CroppingOn();
            volMapper->SetCroppingRegionFlags( 0x7ffdfff );
        }
        if(imageDisplay = this->GetImage3DDisplayForLayer(this->CurrentLayer) )
        {
            bounds = imageDisplay->GetInputProducer()->GetOutput()->GetBounds();
            this->BoxWidget->PlaceWidget (bounds);
            this->Callback->SetVolumeMapper ( this->VolumesMapper[this->CurrentLayer]);
        }
    }
}

//----------------------------------------------------------------------------
//                      WIDGET
//----------------------------------------------------------------------------
/** Set the box widget visibility */
void vtkImageView3D::SetShowBoxWidget (int a)
{
    if (this->Interactor)
        this->BoxWidget->SetEnabled (a);
}
bool vtkImageView3D::GetShowBoxWidget()
{
    return this->BoxWidget->GetEnabled();
}
/** Set the plane widget on */
void vtkImageView3D::SetShowPlaneWidget (int a)
{
    if (this->Interactor)
        this->PlaneWidget->SetEnabled (a);
}
bool vtkImageView3D::GetShowPlaneWidget()
{
    return this->PlaneWidget->GetEnabled();
}
/** Set the cube widget on */
void vtkImageView3D::SetShowCube (int a)
{
    if (this->Interactor)
        this->Marker->SetEnabled (a);
}
bool vtkImageView3D::GetShowCube()
{
    return this->Marker->GetEnabled();
}

void vtkImageView3D::SetOrientationMarker(vtkActor *actor)
{
    this->Marker->SetOrientationMarker( actor );
}

void vtkImageView3D::SetOrientationMarkerViewport(double x1, double x2, double x3, double x4)
{
    this->Marker->SetViewport(x1, x2, x3, x4);
}

void vtkImageView3D::SetupWidgets()
{
    // Create an annotated cube actor (directions)
    this->Cube->SetXPlusFaceText ("L");
    this->Cube->SetXMinusFaceText ("R");
    this->Cube->SetYPlusFaceText ("P");
    this->Cube->SetYMinusFaceText ("A");
    this->Cube->SetZPlusFaceText ("S");
    this->Cube->SetZMinusFaceText ("I");
    this->Cube->SetZFaceTextRotation (90);
    this->Cube->SetFaceTextScale (0.65);
    this->Cube->GetCubeProperty()->SetColor (0.5, 1, 1);
    this->Cube->GetTextEdgesProperty()->SetLineWidth (1);
    this->Cube->GetTextEdgesProperty()->SetDiffuse (0);
    this->Cube->GetTextEdgesProperty()->SetAmbient (1);
    this->Cube->GetTextEdgesProperty()->SetColor (0.18, 0.28, 0.23);

    this->Cube->SetTextEdgesVisibility (1);
    this->Cube->SetCubeVisibility(1);
    this->Cube->SetFaceTextVisibility(1);

    this->Cube->GetXPlusFaceProperty()->SetColor (1, 0, 0);
    this->Cube->GetXPlusFaceProperty()->SetInterpolationToFlat();
    this->Cube->GetXMinusFaceProperty()->SetColor (1, 0, 0);
    this->Cube->GetXMinusFaceProperty()->SetInterpolationToFlat();
    this->Cube->GetYPlusFaceProperty()->SetColor (0, 1, 0);
    this->Cube->GetYPlusFaceProperty()->SetInterpolationToFlat();
    this->Cube->GetYMinusFaceProperty()->SetColor (0, 1, 0);
    this->Cube->GetYMinusFaceProperty()->SetInterpolationToFlat();
    this->Cube->GetZPlusFaceProperty()->SetColor (0, 0, 1);
    this->Cube->GetZPlusFaceProperty()->SetInterpolationToFlat();
    this->Cube->GetZMinusFaceProperty()->SetColor (0, 0, 1);
    this->Cube->GetZMinusFaceProperty()->SetInterpolationToFlat();

    this->Marker->SetOutlineColor (0.93, 0.57, 0.13);
    this->Marker->SetOrientationMarker (this->Cube);
    this->Marker->SetViewport (0.0, 0.05, 0.15, 0.15);

    this->BoxWidget->RotationEnabledOff();
    this->BoxWidget->SetPlaceFactor (0.5);
    this->BoxWidget->SetKeyPressActivationValue ('b');
    this->BoxWidget->AddObserver (vtkCommand::InteractionEvent, this->Callback);

    this->PlaneWidget->SetKeyPressActivationValue ('p');
}

//----------------------------------------------------------------------------
//                      ActorX ActorY ActorZ
//----------------------------------------------------------------------------

void vtkImageView3D::SetShowActorX(unsigned int arg)
{
    this->ShowActorX = arg;
    this->ActorX->SetVisibility (arg);
}

void vtkImageView3D::SetShowActorY(unsigned int arg)
{
    this->ShowActorY = arg;
    this->ActorY->SetVisibility (arg);
}

void vtkImageView3D::SetShowActorZ(unsigned int arg)
{
    this->ShowActorZ = arg;
    this->ActorZ->SetVisibility (arg);
}

/**
 The world is not always what we think it is ...

 Use this method to move the viewer slice such that the position
 (in world coordinates) given by the arguments is contained by
 the slice plane. If the given position is outside the bounds
 of the image, then the slice will be as close as possible.
*/
void vtkImageView3D::SetCurrentPoint (double pos[3])
{
    this->Superclass::SetCurrentPoint (pos);

    this->UpdateDisplayExtent();
}

void vtkImageView3D::UpdateDisplayExtent()
{
    if (!this->GetMedVtkImageInfo() || !this->GetMedVtkImageInfo()->initialized)
    {
        return;
    }

    int *w_ext = this->GetMedVtkImageInfo(CurrentLayer)->extent;

    int slices[3];
    this->GetImageCoordinatesFromWorldCoordinates (this->CurrentPoint, slices);

    // Is the slice in range ? If not, fix it
    for (unsigned int i=0; i<3; i++)
    {
        slices[i] = std::max (w_ext[2*i], slices[i]);
        slices[i] = std::min (w_ext[2*i + 1], slices[i]);
    }

    // Set the image actors
    this->ActorX->SetDisplayExtent(
                slices[0], slices[0],
            w_ext[2],  w_ext[3],
            w_ext[4],  w_ext[5] );

    this->ActorY->SetDisplayExtent(
                w_ext[0],  w_ext[1],
            slices[1], slices[1],
            w_ext[4],  w_ext[5] );

    this->ActorZ->SetDisplayExtent(
                w_ext[0],  w_ext[1],
            w_ext[2],  w_ext[3],
            slices[2], slices[2]);
}


//----------------------------------------------------------------------------
//                      UTILITIES
//----------------------------------------------------------------------------

void vtkImageView3D::InstallPipeline()
{
    this->Superclass::InstallPipeline();
    if (this->Renderer)
    {
        for(auto volActor : VolumesActor)
        {
            this->Renderer->AddViewProp (volActor);
        }

        this->Renderer->AddViewProp (this->ActorX);
        this->Renderer->AddViewProp (this->ActorY);
        this->Renderer->AddViewProp (this->ActorZ);
    }
}

void vtkImageView3D::UnInstallPipeline()
{
    if (this->Renderer)
    {
        this->Renderer->RemoveViewProp (this->ActorX);
        this->Renderer->RemoveViewProp (this->ActorY);
        this->Renderer->RemoveViewProp (this->ActorZ);
    }

    this->Superclass::UnInstallPipeline();

    this->IsInteractorInstalled = 0;
}

void vtkImageView3D::InstallInteractor()
{
    if (this->Interactor)
    {
        if (this->InteractorStyle)
        {
            this->Interactor->SetInteractorStyle (this->InteractorStyle);
        }

        if (this->RenderWindow)
        {
            this->Interactor->SetRenderWindow(this->RenderWindow);
            this->BoxWidget->SetInteractor ( this->Interactor );
            this->PlaneWidget->SetInteractor ( this->Interactor );

            this->Marker->SetInteractor ( this->Interactor );
            this->Marker->On();
            this->Marker->InteractiveOff ();
        }
    }
    this->IsInteractorInstalled = 1;
}

void vtkImageView3D::UnInstallInteractor()
{
    this->BoxWidget->SetInteractor (nullptr);
    this->PlaneWidget->SetInteractor (nullptr);
    this->Marker->SetInteractor (nullptr);

    // Happening for instance switching from 2D->3D->2D
    if (this->Interactor)
    {
        if (this->Interactor->GetRenderWindow())
        {
            auto poRenderer = this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
            while (poRenderer)
            {
                this->RenderWindow->RemoveRenderer(poRenderer);
                poRenderer = this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
            }
            this->Interactor->SetRenderWindow (nullptr);
        }

        this->Interactor->SetInteractorStyle (nullptr);

        this->IsInteractorInstalled = 0;
    }
}

//----------------------------------------------------------------------------
/** Override vtkObject - return the maximum mtime of this and any objects owned by this. */
vtkMTimeType vtkImageView3D::GetMTime()
{
    vtkMTimeType mTime = Superclass::GetMTime();

    vtkObject * objectsToInclude[] = {
        this->BoxWidget,
        this->Cube,
        this->Marker,
        this->PlaneWidget,
        this->PlanarWindowLevelX,
        this->PlanarWindowLevelY,
        this->PlanarWindowLevelZ,
        this->ActorX,
        this->ActorY,
        this->ActorZ};

    const int numObjects = sizeof(objectsToInclude) / sizeof(vtkObject *);

    for ( int i(0); i<numObjects; ++i )
    {
        vtkObject * object = objectsToInclude[i];
        if (object)
        {
            const vtkMTimeType testMtime = object->GetMTime();
            if ( testMtime > mTime )
            {
                mTime = testMtime;
            }
        }
    }

    this->ExtraPlaneCollection->InitTraversal();
    vtkObject* item = this->ExtraPlaneCollection->GetNextProp3D();
    while(item)
    {
        const vtkMTimeType testMtime = item->GetMTime();
        if ( testMtime > mTime )
            mTime = testMtime;
        item = this->ExtraPlaneCollection->GetNextProp3D();
    }

    return mTime;
}
