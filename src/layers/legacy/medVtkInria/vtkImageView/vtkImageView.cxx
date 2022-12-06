/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <cmath>

#include "vtkImageView.h"
#include "vtkObjectFactory.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkMatrixToLinearTransform.h"

#include "vtkImageData.h"
#include "vtkPointSet.h"
#include <vtkInformation.h>
#include <vtkStreamingDemandDrivenPipeline.h>

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyle.h"
#include "vtkImageViewCornerAnnotation.h"
#include "vtkTextProperty.h"
#include "vtkCamera.h"
#include "vtkDataSetCollection.h"
#include "vtkProp3DCollection.h"

#include "vtkLookupTable.h"
#include "vtkColorTransferFunction.h"
#include "vtkPiecewiseFunction.h"
#include "vtkImageMapToColors.h"
#include "vtkScalarBarActor.h"
#include <vtkImageReslice.h>
#include <vtkSmartPointer.h>

#include "vtkCommand.h"
#include "vtkImageView2DCommand.h"

#include <vtkAlgorithmOutput.h>
#include <vtkImageReader2.h>
#include <vtkBoundingBox.h>
#include <vtkRendererCollection.h>

#ifdef WIN32
#define snprintf _snprintf_s
#endif

namespace {
enum ImageViewType {
    IMAGE_VIEW_NONE = 0,
    IMAGE_VIEW_DOUBLE = 1,
    IMAGE_VIEW_FLOAT,
    IMAGE_VIEW_INT,
    IMAGE_VIEW_UNSIGNED_INT,
    IMAGE_VIEW_SHORT,
    IMAGE_VIEW_UNSIGNED_SHORT,
    IMAGE_VIEW_LONG,
    IMAGE_VIEW_UNSIGNED_LONG,
    IMAGE_VIEW_CHAR,
    IMAGE_VIEW_UNSIGNED_CHAR,
    IMAGE_VIEW_RGBPIXELTYPE,
    IMAGE_VIEW_RGBAPIXELTYPE,
    IMAGE_VIEW_UCHARVECTOR3TYPE,
    IMAGE_VIEW_FLOATVECTOR3TYPE  };
}


vtkImageView::vtkImageView()
{
    //by default the CurrentLayer is always the bottom one.
    this->CurrentLayer = 0;

    this->OrientationMatrix       = vtkMatrix4x4::New();
    this->InvertOrientationMatrix = vtkMatrix4x4::New();
    this->OrientationTransform    = vtkMatrixToLinearTransform::New();

    this->CornerAnnotation        = vtkImageViewCornerAnnotation::New();
    this->TextProperty            = vtkTextProperty::New();

    this->DataSetCollection       = vtkDataSetCollection::New();
    this->DataSetActorCollection  = vtkProp3DCollection::New();

    this->LookupTable             = vtkLookupTable::New();
    this->WindowLevel             = vtkImageMapToColors::New();

    this->ScalarBar               = vtkScalarBarActor::New();

    this->Renderer               = 0;
    this->OverlayRenderer        = 0;
    this->RenderWindow           = 0;
    this->Interactor             = 0;
    this->m_poInternalImageFromInput = nullptr;
    this->m_poInputVtkAlgoOutput = nullptr;
    this->InternalMTime          = 0;
    this->InteractorStyle        = 0;
    this->IsInteractorInstalled  = 0;

    this->CornerAnnotation->SetNonlinearFontScaleFactor (0.3);
    this->CornerAnnotation->SetTextProperty ( this->TextProperty );
    this->CornerAnnotation->SetMaximumFontSize (46);
    this->CornerAnnotation->SetImageView (this);
    this->CornerAnnotation->PickableOff();
    this->CornerAnnotation->SetText (3, "<patient>\n<study>\n<series>");

    this->ScalarBar->SetLabelFormat ("%.3E");
    this->ScalarBar->SetNumberOfLabels (3);
    this->ScalarBar->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
    this->ScalarBar->SetLabelTextProperty (this->TextProperty);
    this->ScalarBar->GetLabelTextProperty()->SetFontSize (1);
    this->ScalarBar->GetLabelTextProperty()->BoldOff();
    this->ScalarBar->GetLabelTextProperty()->ShadowOff();
    this->ScalarBar->GetLabelTextProperty()->ItalicOff();
    this->ScalarBar->SetWidth (0.1);
    this->ScalarBar->SetHeight (0.5);
    this->ScalarBar->SetPosition (0.9,0.3);
    this->ScalarBar->PickableOff();
    this->ScalarBar->VisibilityOn();

    for(int i=0; i<3; i++)
        this->CurrentPoint[i] = 0.0; //VTK_DOUBLE_MIN;

    this->PatientName = "";
    this->StudyName   = "";
    this->SeriesName  = "";

    this->ShowAnnotations = true;
    this->ShowScalarBar = true;

    this->OrientationTransform->SetInput (this->OrientationMatrix);

    this->WindowLevel->SetLookupTable( this->LookupTable );
    this->WindowLevel->SetOutputFormatToRGB();
    this->ScalarBar->SetLookupTable( this->WindowLevel->GetLookupTable() );

    this->LookupTable->SetTableRange (0, 1);
    this->LookupTable->SetSaturationRange (0, 0);
    this->LookupTable->SetHueRange (0, 0);
    this->LookupTable->SetValueRange (0, 1);
    this->LookupTable->SetAlphaRange (0, 1);
    this->LookupTable->Build();
}

vtkImageView::~vtkImageView()
{
    this->OrientationTransform->SetInput(nullptr);

    this->OrientationMatrix->Delete();
    this->InvertOrientationMatrix->Delete();
    this->OrientationTransform->Delete();

    this->CornerAnnotation->Delete();
    this->TextProperty->Delete();

    this->DataSetCollection->Delete();
    this->DataSetActorCollection->Delete();

    this->ScalarBar->Delete();

    this->LookupTable->Delete();
    this->WindowLevel->Delete();

    if( this->RenderWindow )
    {
        this->RenderWindow->Delete();
        this->RenderWindow = nullptr;
    }
    if( this->Renderer )
    {
        this->Renderer->Delete();
        this->Renderer = nullptr;
    }
    if( this->OverlayRenderer )
    {
        this->OverlayRenderer->Delete();
        this->OverlayRenderer = nullptr;
    }
    if( this->Interactor )
    {
        this->Interactor->Delete();
        this->Interactor = nullptr;
    }
    if (this->InteractorStyle)
    {
        this->InteractorStyle->Delete();
        this->InteractorStyle = nullptr;
    }
    if (this->m_poInternalImageFromInput)
    {
        this->m_poInternalImageFromInput = nullptr;
        this->m_poInputVtkAlgoOutput = nullptr;
    }
}

//----------------------------------------------------------------------------
/** Override vtkObject - return the maximum mtime of this and any objects owned by this. */
vtkMTimeType vtkImageView::GetMTime()
{
    vtkMTimeType mTime = Superclass::GetMTime();

    vtkObject * objectsToInclude[] = {
        // Renderer, RenderWindow,Interactor,
        InteractorStyle, WindowLevel, OrientationTransform, ScalarBar, OrientationMatrix,
        InvertOrientationMatrix, CornerAnnotation, TextProperty, LookupTable,
        ScalarBar, m_poInternalImageFromInput };
    const int numObjects = sizeof(objectsToInclude) / sizeof(vtkObject *);

    for ( int i(0); i<numObjects; ++i ) {
        vtkObject * object = objectsToInclude[i];
        if (object) {
            const vtkMTimeType testMtime = object->GetMTime();
            if ( testMtime > mTime )
                mTime = testMtime;
        }
    }
    return mTime;
}

//----------------------------------------------------------------------------
/** Attach an interactor for the internal render window. */
void vtkImageView::SetupInteractor(vtkRenderWindowInteractor *arg)
{
    this->UnInstallPipeline();

    vtkSetObjectBodyMacro (Interactor, vtkRenderWindowInteractor, arg);

    this->InstallPipeline();
}

//----------------------------------------------------------------------------
/** Set your own renderwindow and renderer */
void vtkImageView::SetRenderWindow(vtkRenderWindow *arg)
{
    this->UnInstallPipeline();
    vtkSetObjectBodyMacro (RenderWindow, vtkRenderWindow, arg);

    if (this->RenderWindow && this->RenderWindow->GetInteractor())
    {
        this->SetupInteractor (this->RenderWindow->GetInteractor());
    }
    this->InstallPipeline();
}

//----------------------------------------------------------------------------
void vtkImageView::SetRenderer(vtkRenderer *arg)
{
    this->UnInstallPipeline();

    vtkSetObjectBodyMacro (Renderer, vtkRenderer, arg);

    this->InstallPipeline();
}

//----------------------------------------------------------------------------
/** Render the resulting image. */
void vtkImageView::Render()
{
    if(this->Interactor)
        this->Interactor->Render();
}

//----------------------------------------------------------------------------
bool vtkImageView::Compare(double *array1, double *array2, int size)
{
    bool result = true;
    for (int i=0; i<size; i++)
        if (array1[i]!=array2[i])
        {
            result = false;
            break;
        }

    return result;
}

//----------------------------------------------------------------------------
bool vtkImageView::Compare(int *array1, int *array2, int size)
{
    bool result = true;
    for (int i=0; i<size; i++)
        if (array1[i]!=array2[i])
        {
            result = false;
            break;
        }

    return result;
}

//----------------------------------------------------------------------------
bool vtkImageView::Compare(vtkMatrix4x4 *mat1, vtkMatrix4x4 *mat2)
{
    bool result = true;
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            if (mat1->GetElement(i,j)!=mat2->GetElement(i,j))
            {
                result = false;
                break;
            }

    return result;
}

//----------------------------------------------------------------------------
/**
* Reslice an image onto the input image. Internal use only.
*/
vtkAlgorithmOutput* vtkImageView::ResliceImageToInput(vtkAlgorithmOutput* pi_poVtkAlgoPort, vtkMatrix4x4 *matrix)
{
    vtkAlgorithmOutput *poResOutput = nullptr;
    vtkImageData* image = ((vtkImageAlgorithm*)pi_poVtkAlgoPort->GetProducer())->GetOutput();

    if (!pi_poVtkAlgoPort || !this->GetMedVtkImageInfo() || !this->GetMedVtkImageInfo()->initialized)
    {
        return nullptr;
    }

    if ( pi_poVtkAlgoPort &&
         this->Compare(image->GetOrigin(),  this->GetMedVtkImageInfo()->origin, 3) &&
         this->Compare(image->GetSpacing(), this->GetMedVtkImageInfo()->spacing, 3) &&
         this->Compare(image->GetExtent(),  this->GetMedVtkImageInfo()->extent, 6) &&
         (matrix && this->Compare(matrix, this->OrientationMatrix)) )
    {
        poResOutput = pi_poVtkAlgoPort;
    }
    else
    {
        vtkMatrix4x4 *auxMatrix = vtkMatrix4x4::New();
        if (matrix)
        {
            auxMatrix->DeepCopy(matrix);
            vtkMatrix4x4::Invert(auxMatrix, auxMatrix);
        }
        else
        {
            auxMatrix->Identity();
        }

        vtkMatrix4x4::Multiply4x4(auxMatrix, this->OrientationMatrix, auxMatrix);

        vtkImageReslice *reslicer = vtkImageReslice::New();
        reslicer->SetInputConnection (pi_poVtkAlgoPort);
        reslicer->SetResliceAxes     (auxMatrix);
        reslicer->SetOutputOrigin    (this->GetMedVtkImageInfo()->origin);
        reslicer->SetOutputSpacing   (this->GetMedVtkImageInfo()->spacing);
        reslicer->SetOutputExtent    (this->GetMedVtkImageInfo()->extent);
        reslicer->SetInterpolationModeToLinear();
        reslicer->Update();

        poResOutput = reslicer->GetOutputPort();

        auxMatrix->Delete();
    }

    return poResOutput;
}

//----------------------------------------------------------------------------
/**  Set the input image to the viewer. */
void vtkImageView::SetInput(vtkAlgorithmOutput* pi_poVtkAlgoOutput, vtkMatrix4x4 *matrix /*= 0*/, int layer /*= 0*/)
{
    m_poInputVtkAlgoOutput = pi_poVtkAlgoOutput;
    m_poInternalImageFromInput = static_cast<vtkImageAlgorithm*>(pi_poVtkAlgoOutput->GetProducer())->GetOutput();

    if (pi_poVtkAlgoOutput)
    {
       this->WindowLevel->SetInputConnection(pi_poVtkAlgoOutput);
    }

    if (layer==0)
    {
        if (matrix)
        {
            this->SetOrientationMatrix(matrix);
        }
        else
        {
            vtkMatrix4x4 *identity = vtkMatrix4x4::New();
            identity->Identity();
            this->SetOrientationMatrix (identity);
            identity->Delete();
        }
    }
}

//----------------------------------------------------------------------------
/**
* This function is called right after setting both Renderer and RenderWindow.
* It allows a class to add actors for instance without knowing when the Renderer
* and RenderWindow are set. For example, vtkImageView will add the corner annotations
* during the call to the Initialize function.
*/
void vtkImageView::InstallPipeline()
{
    if (this->RenderWindow && this->GetRenderer() && !this->RenderWindow->HasRenderer (this->GetRenderer()))
    {
        this->RenderWindow->AddRenderer(this->GetRenderer());
    }

    if (this->Interactor)
    {
        this->Interactor->SetRenderWindow(this->RenderWindow);
    }

    if (this->GetRenderer())
    {
        this->GetRenderer()->AddViewProp(this->CornerAnnotation);
        this->GetRenderer()->AddViewProp(this->ScalarBar);
        this->GetRenderer()->GetActiveCamera()->ParallelProjectionOn();
    }

    this->InstallInteractor();
}

//----------------------------------------------------------------------------
void vtkImageView::UnInstallPipeline()
{
    this->UnInstallInteractor();

    if (this->GetRenderer())
    {
        this->GetRenderer()->RemoveViewProp(this->CornerAnnotation);
        this->GetRenderer()->RemoveViewProp(this->ScalarBar);
    }

    if (this->RenderWindow && this->GetRenderer())
    {
        this->RenderWindow->RemoveRenderer(this->GetRenderer());
    }
}

//----------------------------------------------------------------------------
/**
* From any point in space \arg pos1, this method will return in \arg pos2
* the closest position that lies within the image boundaries.
*/
void vtkImageView::GetWithinBoundsPosition (double* pos1, double* pos2)
{
    for (unsigned int i=0; i<3; i++) pos2[i] = pos1[i];

    if (!this->GetMedVtkImageInfo() || !this->GetMedVtkImageInfo()->initialized)
        return;

    int indices[3];
    this->GetImageCoordinatesFromWorldCoordinates (pos1, indices);

    int* w_extent = this->GetMedVtkImageInfo()->extent;
    bool out_of_bounds = false;

    for (unsigned int i=0; i<3; i++)
    {
        if (indices[i] > w_extent[2 * i + 1])
        {
            indices[i] = w_extent[2 * i + 1];
            out_of_bounds=true;
        }
        if (indices[i] < w_extent[2 * i])
        {
            indices[i] = w_extent[2 * i];
            out_of_bounds=true;
        }
    }

    if (out_of_bounds)
        this->GetWorldCoordinatesFromImageCoordinates (indices, pos2);
    else
        pos2 = pos1;
}


//----------------------------------------------------------------------------
void vtkImageView::SetCurrentPoint (double pos[3])
{
    double inside_pos[3];
    this->GetWithinBoundsPosition (pos, inside_pos);

    this->CurrentPoint[0] = inside_pos[0];
    this->CurrentPoint[1] = inside_pos[1];
    this->CurrentPoint[2] = inside_pos[2];
    this->InvokeEvent (vtkImageView::CurrentPointChangedEvent, nullptr);
    this->Modified();
}

void vtkImageView::UpdateCursorPosition (double pos[3])
{
    double inside_pos[3];
    this->GetWithinBoundsPosition (pos, inside_pos);

    this->CursorPosition[0] = inside_pos[0];
    this->CursorPosition[1] = inside_pos[1];
    this->CursorPosition[2] = inside_pos[2];
    this->Modified();
}

//----------------------------------------------------------------------------
/**
 Reset the 3D position to center of the image
*/
void vtkImageView::ResetCurrentPoint()
{
    if (!this->GetMedVtkImageInfo() ||!this->GetMedVtkImageInfo()->initialized)
        return;

    int *wholeExtent = this->GetMedVtkImageInfo()->extent;

    int center[3];
    for (unsigned int i=0; i<3; i++)
        center[i] = vtkMath::Round((double)(wholeExtent [2*i+1] - wholeExtent[2*i])/2.0);
    double position[3];
    this->GetWorldCoordinatesFromImageCoordinates (center, position);
    this->SetCurrentPoint (position);
}

//----------------------------------------------------------------------------
void vtkImageView::SetOrientationMatrix (vtkMatrix4x4* matrix)
{
    if (!matrix)
        return;

    vtkMatrix4x4* matrixcopy = vtkMatrix4x4::New();
    matrixcopy->DeepCopy (matrix);
    vtkSetObjectBodyMacro (OrientationMatrix, vtkMatrix4x4, matrixcopy);
    this->OrientationTransform->SetInput (this->OrientationMatrix);
    vtkMatrix4x4::Invert (this->OrientationMatrix, this->InvertOrientationMatrix);

    matrixcopy->Delete();
}

//----------------------------------------------------------------------------
vtkColorTransferFunction * vtkImageView::GetDefaultColorTransferFunction()
{
    vtkColorTransferFunction * rgb = vtkColorTransferFunction::New();
    rgb->AddRGBPoint( 0.0, 0.0, 0.0, 0.0 );
    rgb->AddRGBPoint( 1.0, 1.0, 1.0, 1.0 );
    return rgb;
}

//----------------------------------------------------------------------------
vtkPiecewiseFunction * vtkImageView::GetDefaultOpacityTransferFunction()
{
    vtkPiecewiseFunction * alpha = vtkPiecewiseFunction::New();
    alpha->AddPoint( 0.0, 0.0 );
    alpha->AddPoint( 1.0, 1.0 );
    return alpha;
}

//----------------------------------------------------------------------------
/**
* Transfer functions define the mapping of the intensity or color
* values in the image to colors and opacity displayed on the screen.
*/
void vtkImageView::SetTransferFunctions (vtkColorTransferFunction *color,
                                         vtkPiecewiseFunction     *opacity)
{
    this->SetTransferFunctions (color, opacity,this->GetCurrentLayer());
}

//----------------------------------------------------------------------------
void vtkImageView::SetTransferFunctions (vtkColorTransferFunction *color,
                                         vtkPiecewiseFunction     *opacity,
                                         int layer)
{
    if ( !color   && !this->GetColorTransferFunction(layer)
         && !opacity
         && !this->GetOpacityTransferFunction(layer)
         && this->GetLookupTable(layer) )
    {
        //not sure this line is useful:
        this->SetLookupTable( this->GetLookupTable(layer),layer );
        return;
    }

    this->SetUseLookupTable(false,layer);

    // color
    vtkColorTransferFunction * rgb = nullptr;
    if ( color)
    {
        rgb = vtkColorTransferFunction::New();
        rgb->DeepCopy( color );
    }
    else if ( !this->GetColorTransferFunction(layer) )
    {
        rgb = this->GetDefaultColorTransferFunction();
    }

    if ( rgb )
    {
        this->StoreColorTransferFunction(rgb,layer);
        rgb->Delete();
    }

    // opacity
    vtkPiecewiseFunction * alpha = nullptr;
    if ( opacity)
    {
        alpha = vtkPiecewiseFunction::New();
        alpha->DeepCopy( opacity );
    }
    else if ( !this->GetOpacityTransferFunction(layer) )
    {
        alpha = this->GetDefaultOpacityTransferFunction();
    }

    if ( alpha )
    {
        this->StoreOpacityTransferFunction(alpha,layer );
        alpha->Delete();
    }

    // clean up
    if ( this->GetLookupTable(layer))
    {
        // remove lookup table not in use
        this->SetLookupTable(nullptr, layer);
    }

    this->SetTransferFunctionRangeFromWindowSettings(layer);

    this->ApplyColorTransferFunction(this->GetColorTransferFunction(layer),layer);
    this->ScalarBar->SetLookupTable( this->GetColorTransferFunction(layer) );
}

//----------------------------------------------------------------------------
void vtkImageView::SetColorTransferFunction( vtkColorTransferFunction * ctf )
{
    this->SetTransferFunctions( ctf, nullptr, this->GetCurrentLayer() );
}

//----------------------------------------------------------------------------
void vtkImageView::SetOpacityTransferFunction( vtkPiecewiseFunction * otf )
{
    this->SetTransferFunctions( nullptr, otf,this->GetCurrentLayer() );
}

//----------------------------------------------------------------------------
void vtkImageView::SetLookupTable (vtkLookupTable* lookuptable)
{
    int layer = this->GetCurrentLayer();
    this->SetLookupTable(lookuptable,layer);
}

//----------------------------------------------------------------------------
void vtkImageView::SetLookupTable (vtkLookupTable* lookuptable,int layer)
{
    if (lookuptable && this->HasLayer(layer))
    {
        this->SetUseLookupTable(true,layer);
        this->StoreLookupTable(lookuptable,layer);
        this->ScalarBar->SetLookupTable( lookuptable );

        if ( this->GetColorTransferFunction(layer) != nullptr )
        {
            this->StoreColorTransferFunction(nullptr, layer);
        }
        if ( this->GetOpacityTransferFunction(layer) != nullptr )
        {
            this->StoreOpacityTransferFunction(nullptr, layer);
        }
        this->SetTransferFunctionRangeFromWindowSettings(layer);
    }
}

void vtkImageView::SetUseLookupTable(bool use)
{
    int layer = this->GetCurrentLayer();
    this->SetUseLookupTable(use,layer);
}

//----------------------------------------------------------------------------
void vtkImageView::SetTransferFunctionRangeFromWindowSettings(
        vtkColorTransferFunction * cf,
        vtkPiecewiseFunction     * of,
        double minRange, double maxRange)
{
    double targetWidth  = maxRange  - minRange;
    // in case targetWidth is too small causing numerical precision issues,
    // or too large causing overflow problems, return otherwise all values will collapse and further
    // windowing will become impossible

    double widthTol = std::abs(targetWidth);
    widthTol = std::max( widthTol, std::abs(maxRange) );
    widthTol = std::max( widthTol, std::abs(minRange) );
    widthTol = widthTol * 1.e-7;   // Fractional tolerance
    widthTol = std::max( widthTol, 1.e-30 ); // Absolute tolerance

    if (!(targetWidth > widthTol && targetWidth < VTK_DOUBLE_MAX))
        return;

    // For both cf & of we take a copy and put the values back, as altering them
    // can cause the order to and invalidate our iteration, in VTK > 5.9.0
    if (cf)
    {
        // Take a copy of the range since we use & modify the object later.
        double currentRange[2];
        cf->GetRange(currentRange);
        if ( currentRange[0] != minRange ||
             currentRange[1] != maxRange )
        {

            double currentWidth = currentRange[1] - currentRange[0];

            const unsigned int n = cf->GetSize();
            if ( n > 0 && currentWidth == 0.0 )
                currentWidth = 1.0;

            vtkSmartPointer<vtkColorTransferFunction> cfCopy = vtkSmartPointer<vtkColorTransferFunction>::New();
            cfCopy->DeepCopy( cf );
            cf->RemoveAllPoints();

            for ( unsigned int i = 0; i < n; ++i )
            {
                double val[6];
                cfCopy->GetNodeValue( i, val );
                // from current range to [0,1] interval
                val[0] = ( val[0] - currentRange[0] ) / currentWidth;
                // from [0,1] interval to target range
                val[0] = val[0] * targetWidth + minRange;
                cf->AddRGBPoint( val[0], val[1], val[2], val[3], val[4], val[5] );
            }

        }
    }

    if (of)
    {
        // Take a copy of the range since we use & modify the object later.
        double currentRange[2];
        of->GetRange(currentRange);
        if ( currentRange[0] != minRange ||
             currentRange[1] != maxRange )
        {
            double currentWidth = currentRange[1] - currentRange[0];

            const unsigned int n = of->GetSize();
            if ( n > 0 && currentWidth == 0.0 )
                currentWidth = 1.0;

            vtkSmartPointer<vtkPiecewiseFunction> ofCopy = vtkSmartPointer<vtkPiecewiseFunction>::New();
            ofCopy->DeepCopy( of );
            of->RemoveAllPoints();

            for ( unsigned int i = 0; i < n; ++i )
            {
                double val[4];
                ofCopy->GetNodeValue( i, val );
                // from current range to [0,1] interval
                val[0] = ( val[0] - currentRange[0] ) / currentWidth;
                // from [0,1] interval to target range
                val[0] = val[0] * targetWidth + minRange;
                of->AddPoint( val[0], val[1], val[2], val[3] );
            }

        }
    }
}

//----------------------------------------------------------------------------
void vtkImageView::SetTransferFunctionRangeFromWindowSettings()
{
    int currentLayer = this->GetCurrentLayer();
    this->SetTransferFunctionRangeFromWindowSettings(currentLayer);
}

//----------------------------------------------------------------------------
void vtkImageView::SetTransferFunctionRangeFromWindowSettings(int layer)
{
    double targetRange[2];
    this->GetColorRange( targetRange,layer );
    if (targetRange[1] - targetRange[0] <= 0.0)
    {
        targetRange[0] = 0.0;
        targetRange[1] = 1.0;
    }

    // lookup table
    vtkScalarsToColors * lookupTable = this->GetLookupTable(layer);
    if ( this->GetUseLookupTable(layer) && lookupTable != nullptr )
    {
        const double * currentRange = lookupTable->GetRange();
        if ( currentRange[0] != targetRange[0] ||
             currentRange[1] != targetRange[1] )
        {
            lookupTable->SetRange( targetRange );
        }
    }

    // color transfer function
    if ( !this->GetUseLookupTable(layer))
    {
        this->SetTransferFunctionRangeFromWindowSettings(
                    this->GetColorTransferFunction(layer),
                    this->GetOpacityTransferFunction(layer),
                    targetRange[0],
                targetRange[1]);
    }
}

//----------------------------------------------------------------------------
void vtkImageView::SetWindowSettingsFromTransferFunction()
{
    this->SetWindowSettingsFromTransferFunction(this->GetCurrentLayer());
}

//----------------------------------------------------------------------------
void vtkImageView::SetWindowSettingsFromTransferFunction(int layer)
{
    double currentRange[2];
    this->GetColorRange( currentRange, layer );
    double * targetRange = nullptr;

    bool touched = false;
    bool useLookupTable = this->GetUseLookupTable(layer);
    // lookup table
    vtkScalarsToColors* lut = this->GetLookupTable(layer);
    if ( useLookupTable &&  lut != nullptr )
    {
        targetRange = lut->GetRange();
        if ( currentRange[0] != targetRange[0] ||
             currentRange[1] != targetRange[1] )
            touched = true;
    }

    // color transfer function
    vtkColorTransferFunction * color = this->GetColorTransferFunction(layer);
    if ( !useLookupTable && color != nullptr )
    {
        targetRange = color->GetRange();
        if ( currentRange[0] != targetRange[0] ||
             currentRange[1] != targetRange[1] )
            touched = true;
    }

    // opacity transfer function
    vtkPiecewiseFunction* opacity = this->GetOpacityTransferFunction(layer);
    if ( !useLookupTable && opacity != nullptr )
    {
        double * targetRange2 = opacity->GetRange();
        if ( touched )
        {
            if ( targetRange[0] != targetRange2[0] ||
                 targetRange[1] != targetRange2[1] )
            {
                vtkErrorMacro( "ranges of color and opacity function don't match!" );
                return;
            }
        }
        else
        {
            targetRange[0] = targetRange2[0];
            targetRange[1] = targetRange2[1];

            if ( currentRange[0] != targetRange[0] ||
                 currentRange[1] != targetRange[1] )
                touched = true;
        }
    }

    if ( touched )
    {
        this->SetColorRange( targetRange, layer );
        this->ScalarBar->Modified();
    }
}

/**
* Gets the color level of the current layer.
*/
double vtkImageView::GetColorLevel() const
{
    int currentLayer = this->GetCurrentLayer();
    return this->GetColorLevel(currentLayer);
}


/**
* Sets the color level of the current layer.
*/
void vtkImageView::SetColorLevel(double s)
{
    int currentLayer = this->GetCurrentLayer();
    this->SetColorLevel(s,currentLayer);
}

/**
* Gets the color window of the current layer.
*/
double vtkImageView::GetColorWindow() const
{
    int currentLayer = this->GetCurrentLayer();
    return this->GetColorWindow(currentLayer);
}

/**
* Sets the color window of the current layer.
*/
void vtkImageView::SetColorWindow(double s)
{
    int currentLayer = this->GetCurrentLayer();
    this->SetColorWindow(s,currentLayer);
}


//----------------------------------------------------------------------------
void vtkImageView::SetColorWindow(double s,int layer)
{
    if (s == this->GetColorWindow(layer))
        return;

    this->StoreColorWindow(s,layer);
    this->SetTransferFunctionRangeFromWindowSettings(layer);

    this->InvokeEvent (vtkImageView::WindowLevelChangedEvent, nullptr);
}

//----------------------------------------------------------------------------
void vtkImageView::SetColorLevel(double s,int layer)
{
    if (s == this->GetColorLevel(layer))
        return;

    this->StoreColorLevel (s, layer);
    this->SetTransferFunctionRangeFromWindowSettings(layer);

    this->InvokeEvent (vtkImageView::WindowLevelChangedEvent, nullptr);
}

void vtkImageView::SetColorWindowLevel(double w, double l)
{
    int currentLayer = this->GetCurrentLayer();
    this->SetColorWindowLevel(w, l, currentLayer);
}

void vtkImageView::SetColorWindowLevel(double w, double l, int layer)
{
    if (w == this->GetColorWindow(layer) && l == this->GetColorLevel(layer))
    {
        return;
    }
    this->StoreColorWindow(w,layer);
    this->StoreColorLevel (l, layer);
    this->SetTransferFunctionRangeFromWindowSettings(layer);

    this->InvokeEvent (vtkImageView::WindowLevelChangedEvent, nullptr);
}

/**
* Sets the color range of the current layer.
*/
void vtkImageView::SetColorRange( double r[2] )
{
    int currentLayer = this->GetCurrentLayer();
    this->SetColorRange(r, currentLayer);
}

/**
* Gets the color range of the current layer.
*/
void vtkImageView::GetColorRange( double r[2] )
{
    int currentLayer = this->GetCurrentLayer();
    this->GetColorRange(r, currentLayer);
}

void vtkImageView::GetColorRange(double r[], int layer)
{
    r[0] = this->GetColorLevel(layer) - 0.5 * this->GetColorWindow(layer);
    r[1] = this->GetColorLevel(layer) + 0.5 * this->GetColorWindow(layer);
}

void vtkImageView::SetColorRange( double r[2],int layer )
{
    double level  = 0.5 * ( r[0] + r[1] );
    double window = r[1] - r[0];

    this->SetColorWindowLevel( window,  level,  layer);

}

//----------------------------------------------------------------------------
void vtkImageView::SetTextProperty (vtkTextProperty* textproperty)
{
    vtkSetObjectBodyMacro (TextProperty, vtkTextProperty, textproperty);
    this->CornerAnnotation->SetTextProperty (this->TextProperty);
}

//----------------------------------------------------------------------------
/**
* Convert an indices coordinate point (image coordinates) into a world coordinate point
*/
void vtkImageView::GetWorldCoordinatesFromImageCoordinates(int indices[3], double* position)
{
    if (!this->GetMedVtkImageInfo() || !this->GetMedVtkImageInfo()->initialized)
    {
        position[0] = 0; position[1] = 0; position[2] = 0;
        return;
    }

    // Get information
    double* spacing = this->GetMedVtkImageInfo()->spacing;
    double* origin = this->GetMedVtkImageInfo()->origin;

    double orientedposition[4];
    for (unsigned int i=0; i<3; i++)
        orientedposition[i] = origin[i] + spacing[i]*indices[i];
    orientedposition[3] = 1;

    this->GetOrientationMatrix()->MultiplyPoint (orientedposition, orientedposition);
    for( unsigned int i=0; i<3; i++)
        position[i] = orientedposition[i];
}

//----------------------------------------------------------------------------
/**
* Convert a world coordinate point into an image indices coordinate point
*/
void vtkImageView::GetImageCoordinatesFromWorldCoordinates(double position[3], int* indices) const
{
    if (!this->GetMedVtkImageInfo() || !this->GetMedVtkImageInfo()->initialized)
    {
        indices[0] = 0; indices[1] = 0; indices[2] = 0;
        return;
    }

    // Get information
    double unorientedposition[4] = {position[0], position[1], position[2], 1};
    double* spacing = this->GetMedVtkImageInfo()->spacing;
    double* origin = this->GetMedVtkImageInfo()->origin;

    // apply inverted orientation matrix to the world-coordinate position
    this->InvertOrientationMatrix->MultiplyPoint (unorientedposition, unorientedposition);

    for (unsigned int i=0; i<3;i++)
    {
        if (fabs (spacing[i]) > 1e-15)
            indices[i] = vtkMath::Round((unorientedposition[i]-origin[i])/spacing[i]);
        else
            indices[i] = 0;
    }
}

/**
* Get the pixel value at a given world coordinate point in space, return
* zero if out of bounds.
*/
double vtkImageView::GetValueAtPosition(double worldcoordinates[3], int component )
{
    int layer = this->GetCurrentLayer();
    return this->GetValueAtPosition(worldcoordinates,component,layer);
}

//----------------------------------------------------------------------------
double vtkImageView::GetValueAtPosition(double worldcoordinates[3], int component, int layer )
{
    vtkImageAlgorithm *poAlgoTmp = this->GetInputAlgorithm(layer);

    if (!poAlgoTmp || !this->GetMedVtkImageInfo(layer) || !this->GetMedVtkImageInfo(layer)->initialized)
    {
        return 0.0;
    }

    int indices[3];
    this->GetImageCoordinatesFromWorldCoordinates (worldcoordinates, indices);
    this->Get2DDisplayMapperInputAlgorithm(layer)->UpdateInformation();
    vtkImageData* inputImage = poAlgoTmp->GetOutput();

    return inputImage->GetScalarComponentAsDouble(indices[0], indices[1], indices[2], component);
}

//----------------------------------------------------------------------------
void vtkImageView::SetPosition(int a,int b)
{
    if (this->RenderWindow)
        this->RenderWindow->SetPosition(a, b);
}

//----------------------------------------------------------------------------
/**  Set/Get the position in screen coordinates of the rendering window. */
int* vtkImageView::GetPosition() const
{
    if (this->RenderWindow)
    {
        return this->RenderWindow->GetPosition();
    }
    return nullptr;
}

//----------------------------------------------------------------------------
void vtkImageView::SetSize(int a,int b)
{
    if (this->RenderWindow)
        this->RenderWindow->SetSize(a, b);
}

//----------------------------------------------------------------------------
/** Set/Get the size of the window in screen coordinates in pixels. */
int* vtkImageView::GetSize() const
{
    if (!this->RenderWindow)
    {
        return this->RenderWindow->GetSize();
    }
    return nullptr;
}

//----------------------------------------------------------------------------
/**
* Enable interaction on the view.
*/
void vtkImageView::Enable()
{
    if (this->Interactor)
        this->Interactor->Enable();
}
//----------------------------------------------------------------------------
/**
* Disable interaction on the view.
*/
void vtkImageView::Disable()
{
    if (this->Interactor)
        this->Interactor->Disable();
}
//----------------------------------------------------------------------------
int vtkImageView::GetEnabled() const
{
    if (this->Interactor)
        return this->Interactor->GetEnabled();
    return -1;
}

//----------------------------------------------------------------------------
/**
* Start the interactor.
*/
void vtkImageView::Start()
{
    if (this->Interactor)
        this->Interactor->Start();
}

//----------------------------------------------------------------------------
void vtkImageView::SetBackground(double rgb[3])
{
    if (this->GetRenderer())
        this->GetRenderer()->SetBackground(rgb);
}
//----------------------------------------------------------------------------
double* vtkImageView::GetBackground() const
{
    if (GetRenderer())
    {
        return this->GetRenderer()->GetBackground();
    }
    return nullptr;
}

//----------------------------------------------------------------------------
/**
* Get/Set the zoom factor of the view
*/
void vtkImageView::SetZoom (double arg)
{
    if (!this->GetMedVtkImageInfo() || !this->GetMedVtkImageInfo()->initialized)
    {
        return;
    }

    vtkCamera *cam = this->GetRenderer() ? this->GetRenderer()->GetActiveCamera() : nullptr;
    if (cam)
    {
        double val = getImageHalfMaximumSize();

        // Just in case no data, avoid setting scale to zero.
        val = ( val == 0. ) ? 1. : val;

        cam->SetParallelScale (val / arg);

        this->InvokeEvent (vtkImageView::ZoomChangedEvent);
        this->Modified();
    }
}

//----------------------------------------------------------------------------
double vtkImageView::GetZoom()
{
    double res = 1.0;
    if (!this->GetMedVtkImageInfo() || !this->GetMedVtkImageInfo()->initialized)
    {
        return res;
    }
    if (!this->Get2DDisplayMapperInputAlgorithm() || !this->Get2DDisplayMapperInputAlgorithm()->GetOutputInformation(0))
    {
        return res;
    }

    vtkCamera *cam = this->GetRenderer() ? this->GetRenderer()->GetActiveCamera() : nullptr;
    if (cam)
    {
        res = getImageHalfMaximumSize() / cam->GetParallelScale();
    }

    return res;
}

/**
 * @brief Compute the maximum between both x and y size (divided by 2) of the image.
 This is going to be used to compute a proper zoom for the camera.
 * 
 * @return double 
 */
double vtkImageView::getImageHalfMaximumSize()
{
    // Ensure that the spacing and dimensions are up-to-date.
    int* extent = this->GetMedVtkImageInfo()->extent;
    double* spacing = this->GetMedVtkImageInfo()->spacing;
    double xy[2] = {0,0};

    for (unsigned int i=0; i<2; i++)
    {
        xy[i] = (extent [2*i +1] - extent [2*i]) * spacing[i] / 2.0;
    }
    //val = dimension * spacing / 2;

    return std::max (xy[0], xy[1]);
}

//----------------------------------------------------------------------------
/**
* Reset the camera in a nice way for the 2D view
*/
void vtkImageView::ResetCamera()
{
    if (this->GetRenderer())
    {
        if ( this->GetMedVtkImageInfo () )
        {
            double bounds [6];
            this->GetInputBoundsInWorldCoordinates (bounds);
            this->GetRenderer()->ResetCamera (bounds);
        } else {
            // No op.
            this->GetRenderer()->ResetCamera();
        }
    }
    this->SetZoom (1.0);
    this->InvokeEvent (vtkImageView2DCommand::CameraZoomEvent);
    this->InvokeEvent (vtkImageView2DCommand::CameraPanEvent);
}

void vtkImageView::ResetCamera(vtkDataSet *arg)
{
    vtkBoundingBox box;
    box.AddBounds(arg->GetBounds());

    double center[3];
    box.GetCenter(center);
    this->SetCurrentPoint(center);

    double bounds[6];
    box.GetBounds(bounds);
    this->GetRenderer()->ResetCamera(bounds);
}

//----------------------------------------------------------------------------
/**
* Get/Set the camera settings, position
*/
void vtkImageView::SetCameraPosition (double* arg)
{
    vtkCamera *cam = this->GetRenderer() ? this->GetRenderer()->GetActiveCamera() : nullptr;
    if (cam)
    {
        cam->SetPosition (arg);
        this->InvokeEvent (vtkImageView::CameraChangedEvent);
        this->Modified();
    }
}

//----------------------------------------------------------------------------
double* vtkImageView::GetCameraPosition() const
{
    vtkCamera *cam = this->GetRenderer() ? this->GetRenderer()->GetActiveCamera() : nullptr;
    if (cam)
    {
        return cam->GetPosition ();
    }
    return nullptr;
}

//----------------------------------------------------------------------------
/**
* Get/Set the camera settings, focal point
*/
void vtkImageView::SetCameraFocalPoint (double* arg)
{
    vtkCamera *cam = this->GetRenderer() ? this->GetRenderer()->GetActiveCamera() : nullptr;
    if (cam)
    {
        cam->SetFocalPoint (arg);
        this->InvokeEvent (vtkImageView::CameraChangedEvent);
        this->Modified();
    }
}

//----------------------------------------------------------------------------
double* vtkImageView::GetCameraFocalPoint() const
{
    vtkCamera *cam = this->GetRenderer() ? this->GetRenderer()->GetActiveCamera() : nullptr;
    if (cam)
    {
        return cam->GetFocalPoint ();
    }
    else
    {
        return nullptr;
    }
}

//----------------------------------------------------------------------------
/**
* Get/Set the camera settings, ViewUp
*/
void vtkImageView::SetCameraViewUp (double* arg)
{
    vtkCamera *cam = this->GetRenderer() ? this->GetRenderer()->GetActiveCamera() : nullptr;
    if (cam)
    {
        cam->SetViewUp (arg);
        this->InvokeEvent (vtkImageView::CameraChangedEvent);
        this->Modified();
    }
}

//----------------------------------------------------------------------------
double* vtkImageView::GetCameraViewUp() const
{
    vtkCamera *cam = this->GetRenderer() ? this->GetRenderer()->GetActiveCamera() : nullptr;
    if (cam)
    {
        return cam->GetViewUp ();
    }
    return nullptr;
}

//----------------------------------------------------------------------------
/**
* Get/Set the camera settings, parallel scale
*/
void vtkImageView::SetCameraParallelScale (double arg)
{
    vtkCamera *cam = this->GetRenderer() ? this->GetRenderer()->GetActiveCamera() : nullptr;
    if (cam)
    {
        cam->SetParallelScale (arg);
        this->InvokeEvent (vtkImageView::CameraChangedEvent);
        this->Modified();
    }
}

//----------------------------------------------------------------------------
double vtkImageView::GetCameraParallelScale() const
{
    vtkCamera *cam = this->GetRenderer() ? this->GetRenderer()->GetActiveCamera() : nullptr;
    if (cam)
    {
        return cam->GetParallelScale ();
    }
    return 1.0;
}

//----------------------------------------------------------------------------
/**
* Reset position - zoom - window/level to default
*/
void vtkImageView::Reset()
{
    this->ResetCurrentPoint();
    this->ResetWindowLevel();
    this->ResetCamera();
}

//----------------------------------------------------------------------------
void vtkImageView::SetShowAnnotations (int val)
{
    this->ShowAnnotations = val;
    this->CornerAnnotation->SetVisibility (val);
}

//----------------------------------------------------------------------------
/**
* Show/Hide the scalarbar.
*/
void vtkImageView::SetShowScalarBar (int val)
{
    this->ShowScalarBar = val;
    this->ScalarBar->SetVisibility (val);
}

//----------------------------------------------------------------------------
/**
* Reset the window level
*/
void vtkImageView::ResetWindowLevel()
{
    if (!this->GetMedVtkImageInfo() || !this->GetMedVtkImageInfo()->initialized)
    {
        return;
    }

    if( this->GetMedVtkImageInfo()->scalarType==VTK_UNSIGNED_CHAR  && (this->GetMedVtkImageInfo()->nbScalarComponent==3 || this->GetMedVtkImageInfo()->nbScalarComponent ==4) )
    {
        return;
    }

    double* range = this->GetMedVtkImageInfo()->scalarRange;
    double window = range[1]-range[0];
    double level = 0.5*(range[1]+range[0]);

    int layer = this->GetCurrentLayer();
    this->SetColorWindowLevel(window, level, layer);

}

//----------------------------------------------------------------------------
void vtkImageView::SetPatientName (const char *name)
{
    this->PatientName = name;
    this->Modified();
}

//----------------------------------------------------------------------------
const char *vtkImageView::GetPatientName() const
{
    return this->PatientName.c_str();
}

//----------------------------------------------------------------------------
void vtkImageView::SetStudyName (const char *name)
{
    this->StudyName = name;
    this->Modified();
}

//----------------------------------------------------------------------------
const char *vtkImageView::GetStudyName() const
{
    return this->StudyName.c_str();
}

//----------------------------------------------------------------------------
void vtkImageView::SetSeriesName (const char *name)
{
    this->SeriesName = name;
    this->Modified();
}

//----------------------------------------------------------------------------
const char *vtkImageView::GetSeriesName() const
{
    return this->SeriesName.c_str();
}

//----------------------------------------------------------------------------
/**
* Add a new layer. Returns the id of the layer created. Should be reimplemented in subclasses.
*/
void vtkImageView::AddLayer(int layer)
{
    Q_UNUSED(layer);
}

//----------------------------------------------------------------------------
/**
* Remove a layer. Should be reimplemented in subclasses.
*/
void vtkImageView::RemoveLayer(int layer)
{
    Q_UNUSED(layer);
}

//----------------------------------------------------------------------------
/**
* Remove all layers. Should be reimplemented in subclasses.
*/
void vtkImageView::RemoveAllLayers()
{
}

//----------------------------------------------------------------------------
/**
* Check whether layer exists. Should be reimplemented in subclasses.
*/
bool vtkImageView::HasLayer(int layer) const
{
    return ( layer >= 0 ) && ( layer < this->GetNumberOfLayers());
}

//----------------------------------------------------------------------------
/**
* Returns the number of layers. Should be reimplemented in subclasses.
*/
int vtkImageView::GetNumberOfLayers() const
{
    return 0;
}

//----------------------------------------------------------------------------
void vtkImageView::RemoveDataSet (vtkPointSet *arg)
{
    this->DataSetActorCollection->RemoveItem (this->FindDataSetActor (arg));
    this->DataSetCollection->RemoveItem (arg);
}

//----------------------------------------------------------------------------
vtkProp3D* vtkImageView::FindDataSetActor (vtkDataSet* arg)
{
    int id = this->DataSetCollection->IsItemPresent (arg);
    if (id == 0)
    {
        return nullptr;
    }
    return vtkProp3D::SafeDownCast (this->DataSetActorCollection->GetItemAsObject (id-1));
}

//----------------------------------------------------------------------------
vtkDataSet* vtkImageView::FindActorDataSet (vtkProp3D* arg)
{
    int id = this->DataSetActorCollection->IsItemPresent (arg);
    if (id == 0)
    {
        return nullptr;
    }
    return vtkDataSet::SafeDownCast (this->DataSetCollection->GetItemAsObject (id-1));
}

/////////////////////////////////////////////////////////////////////
///////////////// One comment on the origin handling ////////////////
/////////////////////////////////////////////////////////////////////

/// The 4th column of the OrientationMatrix instance is intensively
/// used in this class in order to estimate world coordinate position
/// and other conversions. The fact that vtkImageData class does not
/// handle any orientation matrix makes this class weak, compared to
/// ITK version. Therefore I think that ALL orientation AND origin
/// information should be stored in the same instance, i.e. a vtkMatrix4x4.
/// Thus impliing that the origin in a vtkImageData should be forced to 0.
/// If we mix the information between vtkImageData "origin" and the
/// vtkMatrix4x4 instances, it will be VERY confusing. and it will
/// introduce misbehaviours in the position evaluation in this class.


/**
*  Get the bounding box of the input image
*/
void vtkImageView::GetInputBounds ( double * bounds )
{
    const int* wholeExtent = this->GetMedVtkImageInfo()->extent ;
    const double * spacing = this->GetMedVtkImageInfo()->spacing;
    const double * origin = this->GetMedVtkImageInfo ()->origin ;

    for ( int i(0); i < 3; ++i )
    {
        bounds[ 2*i     ] = spacing [ i ]*wholeExtent [ 2*i     ] + origin [ i ];
        bounds[ 2*i + 1 ] = spacing [ i ]*wholeExtent [ 2*i + 1 ] + origin [ i ];
    }
}

/**
*  Get the bounding box of the input image in world coordinates.
*/
void vtkImageView::GetInputBoundsInWorldCoordinates ( double * bounds )
{
    double imageBounds [6];
    const int* wholeExtent = this->GetMedVtkImageInfo()->extent;
    const double * spacing = this->GetMedVtkImageInfo()->spacing;
    const double * origin = this->GetMedVtkImageInfo ()->origin;

    for ( int i(0); i < 3; ++i )
    {
        imageBounds[ 2*i     ] = spacing [ i ]*wholeExtent [ 2*i     ] + origin [i];
        imageBounds[ 2*i + 1 ] = spacing [ i ]*wholeExtent [ 2*i + 1 ] + origin [i];
    }

    // Loop over the eight points that define the external vertices of the bounding cuboid.
    double testPoint[4];
    bounds [0] = VTK_DOUBLE_MAX;
    bounds [1] = VTK_DOUBLE_MIN;
    bounds [2] = VTK_DOUBLE_MAX;
    bounds [3] = VTK_DOUBLE_MIN;
    bounds [4] = VTK_DOUBLE_MAX;
    bounds [5] = VTK_DOUBLE_MIN;
    for ( int k(0); k<2; ++k ) {
        for ( int j(0); j<2; ++j ) {
            for ( int i(0); i<2; ++i ) {

                testPoint [0] = imageBounds [i  ];   // x coordinate
                testPoint [1] = imageBounds [2+j];   // y coordinate
                testPoint [2] = imageBounds [4+k];   // z coordinate
                testPoint [3] = 1.;

                // Transform to world coordinates
                this->GetOrientationMatrix()->MultiplyPoint (testPoint, testPoint);

                // Compare min / max for each coordinate.
                for ( int m(0); m < 3; ++m ) {

                    if ( bounds [2*m] > testPoint [m] )
                        bounds [2*m] = testPoint [m];
                    if ( bounds [2*m + 1] < testPoint [m] )
                        bounds [2*m + 1] = testPoint [m];

                }
            }
        }
    }
}

//----------------------------------------------------------------------------
vtkAlgorithm* vtkImageView::Get2DDisplayMapperInputAlgorithm() const
{
    int layer = this->GetCurrentLayer();
    return this->Get2DDisplayMapperInputAlgorithm(layer);
}

vtkAlgorithm* vtkImageView::Get2DDisplayMapperInputAlgorithm (int layer) const
{
    Q_UNUSED(layer);
    return this->WindowLevel->GetInputAlgorithm();
}

vtkImageAlgorithm* vtkImageView::GetInputAlgorithm(int layer) const
{
    Q_UNUSED(layer);
    return dynamic_cast<vtkImageAlgorithm*>(this->WindowLevel->GetInputAlgorithm());
}

//----------------------------------------------------------------------------
void vtkImageView::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);

    os << indent << "OrientationMatrix:\n";
    this->OrientationMatrix->PrintSelf(os,indent.GetNextIndent());

    if (this->m_poInternalImageFromInput)
    {
        os << indent << "Input:\n";
        this->m_poInternalImageFromInput->PrintSelf(os,indent.GetNextIndent());
    }
    if (this->RenderWindow)
    {
        os << indent << "RenderWindow:\n";
        this->RenderWindow->PrintSelf(os,indent.GetNextIndent());
    }
    if (this->GetRenderer())
    {
        os << indent << "Renderer:\n";
        this->GetRenderer()->PrintSelf(os,indent.GetNextIndent());
    }
    if (this->InteractorStyle)
    {
        os << indent << "InteractorStyle:\n";
        this->InteractorStyle->PrintSelf(os,indent.GetNextIndent());
    }
}

void vtkImageView::SetOverlayRenderer( vtkRenderer *arg )
{
    vtkSetObjectBodyMacro( OverlayRenderer, vtkRenderer, arg);
    if ( arg ) {
        arg->SetLayer(this->GetNumberOfLayers());
        if ( this->GetRenderer() ) {
            arg->SetActiveCamera(this->GetRenderer()->GetActiveCamera());
        }
    }
}

/**
* Sets the current active layer.
*
* @param layer If HasLayer(layer) returns false, does nothing.
*/
void vtkImageView::SetCurrentLayer(int layer)
{
    if (this->HasLayer(layer) && this->CurrentLayer != layer)
    {
        this->CurrentLayer = layer;
        if (this->GetUseLookupTable(layer))
        {
            this->ScalarBar->SetLookupTable(this->GetLookupTable(layer));
        }
        else
        {
            this->ScalarBar->SetLookupTable(this->GetColorTransferFunction(layer));
        }
        this->ScalarBar->Modified();
        this->Modified();

        this->Render();
    }
}

/**
* Gets the current active layer.
*/
int vtkImageView::GetCurrentLayer() const
{
    return this->CurrentLayer;
}

double * vtkImageView::GetScalarRange(unsigned int pi_uiLayer /*= 0*/)
{
    return GetMedVtkImageInfo(pi_uiLayer)->scalarRange;
}

/**
* The lookup table is an alternative way to define a transfer
* function.  Internally this is transformed into a
* vtkColorTransferFunction (color) and a vtkPiecewiseFunction
* (opacity/alpha).  Default is a linear black to white table.
*/
vtkLookupTable * vtkImageView::GetLookupTable() const
{
    int layer = this->GetCurrentLayer();
    return this->GetLookupTable(layer);
}

vtkColorTransferFunction * vtkImageView::GetColorTransferFunction() const
{
    int layer = this->GetCurrentLayer();
    return this->GetColorTransferFunction(layer);
}

/**
* Composite volume rendering takes into account the opacity (alpha)
* value to add transparency.
*/
vtkPiecewiseFunction * vtkImageView::GetOpacityTransferFunction() const
{
    int layer = this->GetCurrentLayer();
    return this->GetOpacityTransferFunction(layer);
}

bool vtkImageView::GetUseLookupTable() const
{
    int layer = this->GetCurrentLayer();
    return this->GetUseLookupTable(layer);
}
