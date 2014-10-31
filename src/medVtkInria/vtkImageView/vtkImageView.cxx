/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
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

#include <itkExtractImageBufferFilter.h>
#include <itkImageToVTKImageFilter.h>
#include <itkMatrix.h>
#include <vtkRendererCollection.h>
#include <vtkImageReader2.h>

//#include <vnl/algo/vnl_qr.h>

#ifdef WIN32
    #define snprintf _snprintf_s
#endif

vtkCxxRevisionMacro(vtkImageView, "$Revision: 1 $");
//vtkStandardNewMacro(vtkImageView); // pure virtual class


// Enumeration for the supported pixel types
// NT: why not using the vtk IO definitions ?
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


// pIMPL class for vtkImageView
class vtkImageView::vtkImageViewImplementation {
public:
  //! Default constructor
  vtkImageViewImplementation () : TemporalFilterType (IMAGE_VIEW_NONE) {}

  std::map<int, itk::ProcessObject::Pointer> ImageConverter;
  // should not be handled in this class.
  std::map<int, itk::ProcessObject::Pointer> ImageTemporalFilter;
  int TemporalFilterType;

  public :
  template < typename T > static ImageViewType GetImageViewType () ;
public:
};

// Template specialisations return the enumeration corresponding to the pixel type.
// NT: Use the VTK_DOUBLE/SHORT/etc definitions ?... or the itkImageIO:ComponentType since we have ITK there
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < double > () { return     IMAGE_VIEW_DOUBLE ; }
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < float > () { return     IMAGE_VIEW_FLOAT; }
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < int > () { return     IMAGE_VIEW_INT; }
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < unsigned int > () { return     IMAGE_VIEW_UNSIGNED_INT; }
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < short > () { return     IMAGE_VIEW_SHORT; }
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < unsigned short > () { return     IMAGE_VIEW_UNSIGNED_SHORT; }
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < long > () { return     IMAGE_VIEW_LONG; }
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < unsigned long > () { return     IMAGE_VIEW_UNSIGNED_LONG; }
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < char > () { return     IMAGE_VIEW_CHAR; }
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < unsigned char > () { return     IMAGE_VIEW_UNSIGNED_CHAR; }
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < vtkImageView::RGBPixelType > () { return     IMAGE_VIEW_RGBPIXELTYPE; }
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < vtkImageView::RGBAPixelType > () { return     IMAGE_VIEW_RGBAPIXELTYPE; }
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < vtkImageView::UCharVector3Type > () { return     IMAGE_VIEW_UCHARVECTOR3TYPE ; }
template <> ImageViewType vtkImageView::vtkImageViewImplementation::GetImageViewType < vtkImageView::FloatVector3Type > () { return     IMAGE_VIEW_FLOATVECTOR3TYPE ; }

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

  this->ScalarBar               = vtkScalarBarActor::New();

  this->Renderer               = 0;
  this->OverlayRenderer        = 0;
  this->RenderWindow           = 0;
  this->Interactor             = 0;
  this->Input                  = 0;
  this->InternalMTime          = 0;
  this->InteractorStyle        = 0;
  this->IsInteractorInstalled  = 0;

  this->Impl = new vtkImageViewImplementation;
  //this->ITKInput  = 0;
  this->ITKInput4 = 0;

  this->CornerAnnotation->SetNonlinearFontScaleFactor (0.3);
  this->CornerAnnotation->SetTextProperty ( this->TextProperty );
  this->CornerAnnotation->SetMaximumFontSize (46);
  this->CornerAnnotation->SetImageView (this);
  this->CornerAnnotation->PickableOff();
  this->CornerAnnotation->SetText (3, "<patient>\n<study>\n<series>");

  this->ScalarBar->SetLabelFormat ("%.0f");
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

  // use default maps, sets color map of WindowLevel and ScalarBar
  //this->SetTransferFunctions( NULL, NULL );

  //this->WindowLevel->SetOutputFormatToRGB();

  this->TimeIndex = 0;
}


vtkImageView::~vtkImageView()
{
  std::cout<<"deleting a view"<<std::endl;

  this->OrientationTransform->SetInput ( NULL );

  this->OrientationMatrix->Delete();
  this->InvertOrientationMatrix->Delete();
  this->OrientationTransform->Delete();

  this->CornerAnnotation->Delete();
  this->TextProperty->Delete();

  this->DataSetCollection->Delete();
  this->DataSetActorCollection->Delete();

  this->ScalarBar->Delete();

  if( this->RenderWindow )
  {
    this->RenderWindow->Delete();
    this->RenderWindow = 0;
  }
  if( this->Renderer )
  {
      this->Renderer->Delete();
      this->Renderer = 0;
  }
  if( this->OverlayRenderer )
  {
      this->OverlayRenderer->Delete();
      this->OverlayRenderer = 0;
  }
  if( this->Interactor )
  {
    this->Interactor->Delete();
    this->Interactor = 0;
  }
  if (this->InteractorStyle)
  {
    this->InteractorStyle->Delete();
    this->InteractorStyle = 0;
  }
  if (this->Input)
  {
    this->Input->Delete();
    this->Input = 0;
  }

  delete this->Impl;

  std::cout<<"deleting a view. done"<<std::endl;


}

//----------------------------------------------------------------------------
unsigned long vtkImageView::GetMTime()
{
    typedef unsigned long MTimeType;

    MTimeType mTime = Superclass::GetMTime();

    vtkObject * objectsToInclude[] = {
        // Renderer, RenderWindow,Interactor,
        InteractorStyle, OrientationTransform, ScalarBar, OrientationMatrix,
        InvertOrientationMatrix, CornerAnnotation, TextProperty,
        ScalarBar, Input };
        const int numObjects = sizeof(objectsToInclude) / sizeof(vtkObject *);

        for ( int i(0); i<numObjects; ++i ) {
            vtkObject * object = objectsToInclude[i];
            if (object) {
                const MTimeType testMtime = object->GetMTime();
                if ( testMtime > mTime )
                    mTime = testMtime;
            }
        }
        return mTime;
}

//----------------------------------------------------------------------------
void vtkImageView::SetupInteractor(vtkRenderWindowInteractor *arg)
{
  this->UnInstallPipeline();

  vtkSetObjectBodyMacro (Interactor, vtkRenderWindowInteractor, arg);

  this->InstallPipeline();
}

//----------------------------------------------------------------------------
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
void vtkImageView::Render()
{
  if (this->RenderWindow)
  {
    if (!this->RenderWindow->GetNeverRendered())
    {
      if( this->GetMTime() > this->InternalMTime )
      {
        this->RenderWindow->Render();
        this->InternalMTime = this->GetMTime();
      }
    }
    else
    {
      this->RenderWindow->Render();
    }
  }
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
vtkImageData *vtkImageView::ResliceImageToInput(vtkImageData *image, vtkMatrix4x4 *matrix)
{
  if (!image || !this->GetInput())
    return NULL;

  vtkImageData *output = 0;

  if ( this->Compare(image->GetOrigin(),      this->GetInput()->GetOrigin(), 3) &&
       this->Compare(image->GetSpacing(),     this->GetInput()->GetSpacing(), 3) &&
       this->Compare(image->GetWholeExtent(), this->GetInput()->GetWholeExtent(), 6) &&
       (matrix && this->Compare(matrix, this->OrientationMatrix)) )
  {
    output = image;
    output->Register(this); // hack to make vtk believe output is referenced here
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
    reslicer->SetInput         (image);
    reslicer->SetResliceAxes   (auxMatrix);
    reslicer->SetOutputOrigin  (this->GetInput()->GetOrigin());
    reslicer->SetOutputSpacing (this->GetInput()->GetSpacing());
    reslicer->SetOutputExtent  (this->GetInput()->GetWholeExtent());
    reslicer->SetInterpolationModeToLinear();
    reslicer->Update();

    output = reslicer->GetOutput();
    output->Register (this); // hack to make vtk believe output is referenced here
    // otherwise, next call to reslicer->Delete() would discard its output

    reslicer->Delete();
    auxMatrix->Delete();
  }

  return output;
}

//----------------------------------------------------------------------------
void vtkImageView::SetInput(vtkImageData *arg, vtkMatrix4x4 *matrix, int layer)
{
  vtkSetObjectBodyMacro (Input, vtkImageData, arg);
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
void vtkImageView::SetInputConnection(vtkAlgorithmOutput* arg, vtkMatrix4x4 *matrix, int layer)
{
  if (layer==0)
  {
    this->SetOrientationMatrix(matrix);
  }
}

//----------------------------------------------------------------------------
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
void vtkImageView::GetWithinBoundsPosition (double* pos1, double* pos2)
{
  for (unsigned int i=0; i<3; i++) pos2[i] = pos1[i];

  if (!this->GetInput())
    return;

  int indices[3];
  this->GetImageCoordinatesFromWorldCoordinates (pos1, indices);
  int* w_extent = this->GetInput()->GetWholeExtent();
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
  this->InvokeEvent (vtkImageView::CurrentPointChangedEvent, NULL);
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
void vtkImageView::ResetCurrentPoint()
{
  if (!this->GetInput())
    return;

  this->GetInput()->UpdateInformation();
  int *wholeExtent = this->GetInput()->GetWholeExtent();
  //this->GetInput()->PropagateUpdateExtent ();
  //this->GetInput()->Update();

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
  if ( color   == NULL && this->GetColorTransferFunction(layer)   == NULL &&
       opacity == NULL && this->GetOpacityTransferFunction(layer) == NULL &&
       this->GetLookupTable(layer) != NULL )
  {
    //not sure this line is useful:
    this->SetLookupTable( this->GetLookupTable(layer),layer );
    return;
  }

  this->SetUseLookupTable(false,layer);

  // color
  vtkColorTransferFunction * rgb = NULL;
  if ( color != NULL )
  {
    rgb = vtkColorTransferFunction::New();
    rgb->DeepCopy( color );
  }
  else if ( this->GetColorTransferFunction(layer) == NULL )
    rgb = this->GetDefaultColorTransferFunction();

  if ( rgb != NULL )
  {
    this->StoreColorTransferFunction(rgb,layer);
    rgb->Delete();
  }

  // opacity
  vtkPiecewiseFunction * alpha = NULL;
  if ( opacity != NULL )
  {
    alpha = vtkPiecewiseFunction::New();
    alpha->DeepCopy( opacity );
  }
  else if ( this->GetOpacityTransferFunction(layer) == NULL )
    alpha = this->GetDefaultOpacityTransferFunction();

  if ( alpha != NULL )
  {
    this->StoreOpacityTransferFunction(alpha,layer );
    alpha->Delete();
  }

  // clean up
  if ( this->GetLookupTable(layer) != NULL )
  {
    // remove lookup table not in use
//    this->GetLookupTable(layer)->Delete();
    this->SetLookupTable(NULL, layer);
  }

  this->SetTransferFunctionRangeFromWindowSettings(layer);

  this->ApplyColorTransferFunction(this->GetColorTransferFunction(layer),layer);
  this->ScalarBar->SetLookupTable( this->GetColorTransferFunction(layer) );
}

//----------------------------------------------------------------------------
void vtkImageView::SetColorTransferFunction( vtkColorTransferFunction * ctf )
{
  this->SetTransferFunctions( ctf, NULL, this->GetCurrentLayer() );
}

//----------------------------------------------------------------------------
void vtkImageView::SetOpacityTransferFunction( vtkPiecewiseFunction * otf )
{
  this->SetTransferFunctions( NULL, otf,this->GetCurrentLayer() );
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

    if ( this->GetColorTransferFunction(layer) != NULL )
    {
      this->StoreColorTransferFunction(NULL,layer);
    }
    if ( this->GetOpacityTransferFunction(layer) != NULL )
    {
      this->StoreOpacityTransferFunction(NULL,layer);
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

  bool touched = false;

  // lookup table
  vtkScalarsToColors * lookupTable = this->GetLookupTable(layer);
  if ( this->GetUseLookupTable(layer) && lookupTable != NULL )
  {
    const double * currentRange = lookupTable->GetRange();
    if ( currentRange[0] != targetRange[0] ||
        currentRange[1] != targetRange[1] )
    {
      lookupTable->SetRange( targetRange );
      touched = true;
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
      touched = true;
  }

  if ( touched )
  {
    //todo: Call Modified on the right object
//    this->GetWindowLevel(layer)->Modified();

    //probably should change the lookuptable of this scalar bar?
    //no, done in setLookupTable.

    this->SetScalarBarLabelFormat(targetRange);
    this->ScalarBar->Modified();
  }
}

void vtkImageView::SetScalarBarLabelFormat(double* intensityRange)
{
    double diff = fabs(fabs(intensityRange[1])-fabs(intensityRange[0]));

    if (diff>1)
    {
        this->ScalarBar->SetLabelFormat ("%.f");
        return;
    }

    if (diff == 0)
    {
        if (intensityRange[1]==0)
            return;
        diff = fabs(intensityRange[1]);
    }

    int precision = -floor(log10(diff))+1; //+1 for the third value displayed (mean) not to be rounded
    char format[10];
    snprintf(format, 10, "%%.%df", precision);
    this->ScalarBar->SetLabelFormat (format);
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
  double * targetRange = NULL;

  bool touched = false;
  bool useLookupTable = this->GetUseLookupTable(layer);
  // lookup table
  vtkScalarsToColors* lut = this->GetLookupTable(layer);
  if ( useLookupTable &&  lut != NULL )
  {
    targetRange = lut->GetRange();
    if ( currentRange[0] != targetRange[0] ||
         currentRange[1] != targetRange[1] )
      touched = true;
  }

  // color transfer function
  vtkColorTransferFunction * color = this->GetColorTransferFunction(layer);
  if ( !useLookupTable && color != NULL )
  {
    targetRange = color->GetRange();
    if ( currentRange[0] != targetRange[0] ||
         currentRange[1] != targetRange[1] )
      touched = true;
  }

  // opacity transfer function
  vtkPiecewiseFunction* opacity = this->GetOpacityTransferFunction(layer);
  if ( !useLookupTable && opacity != NULL )
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
//TODO call Modified on the right object
//    this->GetWindowLevel(layer)->Modified();
    this->ScalarBar->Modified();
  }
}

double vtkImageView::GetColorLevel() const
{
    int currentLayer = this->GetCurrentLayer();
    return this->GetColorLevel(currentLayer);
}



void vtkImageView::SetColorLevel(double s)
{
    int currentLayer = this->GetCurrentLayer();
    this->SetColorLevel(s,currentLayer);
}


double vtkImageView::GetColorWindow() const
{
    int currentLayer = this->GetCurrentLayer();
    return this->GetColorWindow(currentLayer);
}


void vtkImageView::SetColorWindow(double s)
{
    int currentLayer = this->GetCurrentLayer();
    this->SetColorWindow(s,currentLayer);
}


//----------------------------------------------------------------------------
void vtkImageView::SetColorWindow(double s,int layer)
{
  //layer is not used here yet.
  if (s < 0)
    s = 0;

  if (s == this->GetColorWindow(layer))
    return;

  this->StoreColorWindow(s,layer);

  this->SetTransferFunctionRangeFromWindowSettings(layer);

  this->InvokeEvent (vtkImageView::WindowLevelChangedEvent, NULL);
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkImageView::SetColorLevel(double s,int layer)
{
  if (s == this->GetColorLevel(layer))
    return;

  this->StoreColorLevel (s, layer);

  this->SetTransferFunctionRangeFromWindowSettings(layer);

  this->InvokeEvent (vtkImageView::WindowLevelChangedEvent, NULL);
  this->Modified();
}

void vtkImageView::SetColorRange( double r[2] )
{
    int currentLayer = this->GetCurrentLayer();
    this->SetColorRange(r, currentLayer);
}

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

  this->SetColorLevel( level,layer );
  this->SetColorWindow( window,layer );
}

//----------------------------------------------------------------------------
void vtkImageView::SetTextProperty (vtkTextProperty* textproperty)
{
  vtkSetObjectBodyMacro (TextProperty, vtkTextProperty, textproperty);
  this->CornerAnnotation->SetTextProperty (this->TextProperty);
}

//----------------------------------------------------------------------------
void vtkImageView::GetWorldCoordinatesFromImageCoordinates(int indices[3], double* position)
{
  if (!this->GetInput())
  {
    position[0] = 0; position[1] = 0; position[2] = 0;
    return;
  }

  // Get information
  double* spacing = this->GetInput()->GetSpacing();
  double* origin = this->GetInput()->GetOrigin();

  double orientedposition[4];
  for (unsigned int i=0; i<3; i++)
    orientedposition[i] = origin[i] + spacing[i]*indices[i];
  orientedposition[3] = 1;

  this->GetOrientationMatrix()->MultiplyPoint (orientedposition, orientedposition);
  for( unsigned int i=0; i<3; i++)
    position[i] = orientedposition[i];
}

//----------------------------------------------------------------------------
void vtkImageView::GetImageCoordinatesFromWorldCoordinates(double position[3], int* indices) const
{
  if (!this->GetInput())
  {
    indices[0] = 0; indices[1] = 0; indices[2] = 0;
    return;
  }

  // Get information
  double unorientedposition[4] = {position[0], position[1], position[2], 1};
  double* spacing = this->GetInput()->GetSpacing();
  double* origin = this->GetInput()->GetOrigin();

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

double vtkImageView::GetValueAtPosition(double worldcoordinates[3], int component )
{
  int layer = this->GetCurrentLayer();
  return this->GetValueAtPosition(worldcoordinates,component,layer);
}

//----------------------------------------------------------------------------
double vtkImageView::GetValueAtPosition(double worldcoordinates[3],
                                        int component,
                                        int layer )
{
  vtkImageData* input = this->GetInput(layer);
  if (!input)
    return 0.0;

  int indices[3] = {0,0,0};
  this->GetImageCoordinatesFromWorldCoordinates (worldcoordinates, indices);
  int* w_extent = input->GetWholeExtent();

  if ( (indices[0] < w_extent[0]) ||
      (indices[0] > w_extent[1]) ||
      (indices[1] < w_extent[2]) ||
      (indices[1] > w_extent[3]) ||
      (indices[2] < w_extent[4]) ||
      (indices[2] > w_extent[5]) )
    return 0;

  // Is the requested point in the currently loaded data extent? If not, attempt to update.
  int* extent = input->GetExtent ();
  if ( (indices[0] < extent[0]) ||
      (indices[0] > extent[1]) ||
      (indices[1] < extent[2]) ||
      (indices[1] > extent[3]) ||
      (indices[2] < extent[4]) ||
      (indices[2] > extent[5]) )
  {

    int* u_extent = input->GetUpdateExtent ();
    if ( (indices[0] < u_extent[0]) ||
        (indices[0] > u_extent[1]) ||
        (indices[1] < u_extent[2]) ||
        (indices[1] > u_extent[3]) ||
        (indices[2] < u_extent[4]) ||
        (indices[2] > u_extent[5]) )
    {

      int pointExtent [6] = { indices [0], indices [0], indices [1], indices [1], indices [2], indices [2] };
      input->SetUpdateExtent(pointExtent);
      input->PropagateUpdateExtent();
      input->UpdateData();

    } else {

      input->Update ();

      int* new_extent = input->GetExtent ();
      if ( (indices[0] < new_extent[0]) ||
          (indices[0] > new_extent[1]) ||
          (indices[1] < new_extent[2]) ||
          (indices[1] > new_extent[3]) ||
          (indices[2] < new_extent[4]) ||
          (indices[2] > new_extent[5]) )
      {
        vtkErrorMacro( "data not in slice extent after update" );
      }

    }
  } else {

    // Need to be sure that the input is up to date. Otherwise we may be requesting bad data.
    input->Update ();
  }

  return input->GetScalarComponentAsDouble (indices[0], indices[1], indices[2], component);

}

//----------------------------------------------------------------------------
void vtkImageView::SetPosition(int a,int b)
{
  if (this->RenderWindow)
    this->RenderWindow->SetPosition(a, b);
}

//----------------------------------------------------------------------------
int* vtkImageView::GetPosition() const
{
  if (this->RenderWindow)
    return this->RenderWindow->GetPosition();
  return NULL;
}

//----------------------------------------------------------------------------
void vtkImageView::SetSize(int a,int b)
{
  if (this->RenderWindow)
    this->RenderWindow->SetSize(a, b);
}

//----------------------------------------------------------------------------
int* vtkImageView::GetSize() const
{
  if (!this->RenderWindow)
    return this->RenderWindow->GetSize();
  return NULL;
}

//----------------------------------------------------------------------------
void vtkImageView::Enable()
{
  if (this->Interactor)
    this->Interactor->Enable();
}
//----------------------------------------------------------------------------
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
    return this->GetRenderer()->GetBackground();
  return NULL;
}

//----------------------------------------------------------------------------
void vtkImageView::SetZoom (double arg)
{
  if (!this->GetInput())
    return;

  vtkCamera *cam = this->GetRenderer() ? this->GetRenderer()->GetActiveCamera() : NULL;
  if (!cam)
    return;

  // Ensure that the spacing and dimensions are up-to-date.
  this->GetInput()->UpdateInformation();
  this->GetInput()->PropagateUpdateExtent ();

  // It seems that the above still does not succeed in getting the dimensions in all cases.
  // int* dimensions = this->GetInput()->GetDimensions();
  const int* extent = this->GetInput()->GetUpdateExtent ();

  double* spacing = this->GetInput()->GetSpacing();
  double xyz[3] = {0,0,0};
  for (unsigned int i=0; i<3; i++)
    xyz[i] = (extent [2*i +1] - extent [2*i]) * spacing[i] / 2.0;
  double val = std::max (std::max (xyz[0], xyz[1]), xyz[2]);

  // Just in case no data, avoid setting scale to zero.
  val = ( val == 0. ) ? 1. : val;

  cam->SetParallelScale (val / arg);

  this->InvokeEvent (vtkImageView::ZoomChangedEvent);
  this->Modified();
}

//----------------------------------------------------------------------------
double vtkImageView::GetZoom()
{
  if (!this->GetInput())
    return 1.0;

  vtkCamera *cam = this->GetRenderer() ? this->GetRenderer()->GetActiveCamera() : NULL;
  if (!cam)
    return 1.0;

  // Ensure that the spacing and dimensions are up-to-date.
  this->GetInput()->UpdateInformation();
  this->GetInput()->PropagateUpdateExtent ();

  //int* dimensions = this->GetInput()->GetDimensions();
  const int* extent = this->GetInput()->GetUpdateExtent ();
  double* spacing = this->GetInput()->GetSpacing();
  double xyz[3] = {0,0,0};
  for (unsigned int i=0; i<3; i++)
    xyz[i] = (extent [2*i +1] - extent [2*i]) * spacing[i] / 2.0;
  //xyz[i] = dimensions[i] * spacing[i] / 2.0;
  double val = std::max (std::max (xyz[0], xyz[1]), xyz[2]);

  return (val / cam->GetParallelScale());
}

//----------------------------------------------------------------------------
void vtkImageView::ResetCamera()
{
  if (this->GetRenderer())
  {
    //  ResetCamera calls ResetCameraClippingRange anyway...
    //      this->GetRenderer()->ResetCameraClippingRange();

     if ( this->GetInput () )
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

//----------------------------------------------------------------------------
void vtkImageView::SetCameraPosition (double* arg)
{
  vtkCamera *cam = this->GetRenderer() ? this->GetRenderer()->GetActiveCamera() : NULL;
  if (!cam)
    return;
  cam->SetPosition (arg);
  this->InvokeEvent (vtkImageView::CameraChangedEvent);
  this->Modified();
}

//----------------------------------------------------------------------------
double* vtkImageView::GetCameraPosition() const
{
  vtkCamera *cam = this->GetRenderer() ? this->GetRenderer()->GetActiveCamera() : NULL;
  if (!cam)
    return NULL;
  return cam->GetPosition ();
}

//----------------------------------------------------------------------------
void vtkImageView::SetCameraFocalPoint (double* arg)
{
  vtkCamera *cam = this->GetRenderer() ? this->GetRenderer()->GetActiveCamera() : NULL;
  if (!cam)
    return;
  cam->SetFocalPoint (arg);
  this->InvokeEvent (vtkImageView::CameraChangedEvent);
  this->Modified();
}

//----------------------------------------------------------------------------
double* vtkImageView::GetCameraFocalPoint() const
{
  vtkCamera *cam = this->GetRenderer() ? this->GetRenderer()->GetActiveCamera() : NULL;
  if (!cam)
    return NULL;
  return cam->GetFocalPoint ();
}

//----------------------------------------------------------------------------
void vtkImageView::SetCameraViewUp (double* arg)
{
  vtkCamera *cam = this->GetRenderer() ? this->GetRenderer()->GetActiveCamera() : NULL;
  if (!cam)
    return;
  cam->SetViewUp (arg);
  this->InvokeEvent (vtkImageView::CameraChangedEvent);
  this->Modified();
}

//----------------------------------------------------------------------------
double* vtkImageView::GetCameraViewUp() const
{
  vtkCamera *cam = this->GetRenderer() ? this->GetRenderer()->GetActiveCamera() : NULL;
  if (!cam)
    return NULL;
  return cam->GetViewUp ();
}


//----------------------------------------------------------------------------
void vtkImageView::SetCameraParallelScale (double arg)
{
  vtkCamera *cam = this->GetRenderer() ? this->GetRenderer()->GetActiveCamera() : NULL;
  if (!cam)
    return;
  cam->SetParallelScale (arg);
  this->InvokeEvent (vtkImageView::CameraChangedEvent);
  this->Modified();
}

//----------------------------------------------------------------------------
double vtkImageView::GetCameraParallelScale() const
{
  vtkCamera *cam = this->GetRenderer() ? this->GetRenderer()->GetActiveCamera() : NULL;
  if (!cam)
    return 1.0;
  return cam->GetParallelScale ();
}

//----------------------------------------------------------------------------
void vtkImageView::Reset()
{
  this->ResetCurrentPoint();
  this->ResetWindowLevel();
  // this->SetColorWindow (VTK_DOUBLE_MAX); // NT: ?? --> when i press reset I would like the windowlevels to be "reset" ?
  this->ResetCamera();

}

//----------------------------------------------------------------------------
void vtkImageView::SetShowAnnotations (int val)
{
  this->ShowAnnotations = val;
  this->CornerAnnotation->SetVisibility (val);
}

//----------------------------------------------------------------------------
void vtkImageView::SetShowScalarBar (int val)
{
  this->ShowScalarBar = val;
  this->ScalarBar->SetVisibility (val);
}

//----------------------------------------------------------------------------
void vtkImageView::ResetWindowLevel()
{
  if (!this->GetInput())
  {
    return;
  }

  if( this->GetInput()->GetScalarType()==VTK_UNSIGNED_CHAR  &&
     (this->GetInput()->GetNumberOfScalarComponents()==3 || this->GetInput()->GetNumberOfScalarComponents()==4) )
  {
    return;
  }

  this->GetInput()->Update();

  double* range = this->GetInput()->GetScalarRange();
  double window = range[1]-range[0];
  double level = 0.5*(range[1]+range[0]);

  // std::cerr << "vtkImageView::ResetWindowLevel" << std::endl;
  // std::cerr << "range: " << range[0] << " - " << range[1] << " ("
  //     << range[1] - range[0] << ")" << std::endl;

  this->SetColorWindow ( window );
  this->SetColorLevel ( level );
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
void vtkImageView::AddLayer(int layer)
{
}

//----------------------------------------------------------------------------
void vtkImageView::RemoveLayer(int layer)
{
    if( layer < (int)(this->ITKInputVector.size()) )
    {
        this->ITKInputVector.erase(this->ITKInputVector.begin() + layer);
    }
}

//----------------------------------------------------------------------------
void vtkImageView::RemoveAllLayers()
{
}

//----------------------------------------------------------------------------
bool vtkImageView::HasLayer(int layer) const
{
  return ( layer >= 0 ) && ( layer < this->GetNumberOfLayers());
}

//----------------------------------------------------------------------------
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
    return NULL;
  return vtkProp3D::SafeDownCast (this->DataSetActorCollection->GetItemAsObject (id-1));
}

//----------------------------------------------------------------------------
vtkDataSet* vtkImageView::FindActorDataSet (vtkProp3D* arg)
{
  int id = this->DataSetActorCollection->IsItemPresent (arg);
  if (id == 0)
    return NULL;
  return vtkDataSet::SafeDownCast (this->DataSetCollection->GetItemAsObject (id-1));
}



/////////////////////////////////////////////////////////////////////////////
/////////////////// NOTE ON TIME HANDLING AND ITK-BRIDGE ////////////////////
/////////////////////////////////////////////////////////////////////////////
/// Nicolas Toussaint.

/// All of this pipelines of time handling, extraction etc
/// have been implemented both for images and meshes in
/// a vtk class called vtkMetaDataSetSequence.
/// Note also that in the vtkMetaDataSetSequence implementation
/// the template switch case between scalar type has ALSO been implemented,
/// as well as all setITKInput APIs and the corr. macros...
///
/// I believe that the bridge between ITK and VTK should be implemented
/// outside of the views, from an external class, such as vtkMetaDataSet(Sequence).
/// The fact that all of these are implemented here again will make difficult the
/// maintenance of both APIs...

/// time should not be handled in this class.

//----------------------------------------------------------------------------
template < class T >
void vtkImageView::SetTimeIndex (vtkIdType timeIndex)
{
  typedef typename itk::Image<T, 4> ImageType4d;
  typedef typename itk::Image<T, 3> ImageType3d;
  typedef typename itk::ExtractImageBufferFilter< ImageType4d, ImageType3d > ExtractFilterType;

  // Since we store the extractor type using an enum, the dynamic cast should always succeed,
  // unless the Filter was already NULL.
  if(this->Impl->ImageTemporalFilter.size())
  {
    ExtractFilterType * extractor = dynamic_cast < ExtractFilterType *> ( this->Impl->ImageTemporalFilter[0].GetPointer () ) ;
    if ( extractor )
    {
      unsigned int timeLimit = extractor->GetInput()->GetLargestPossibleRegion().GetSize()[3] -1;
      if (timeIndex<0)
        timeIndex = 0;
      if (timeIndex>(vtkIdType)timeLimit)
        timeIndex = timeLimit;
      typename ImageType4d::RegionType region = extractor->GetExtractionRegion ();
      region.SetIndex (3, timeIndex);
      extractor->SetExtractionRegion (region);
      extractor->UpdateLargestPossibleRegion();
    }
  }
}

//----------------------------------------------------------------------------
void vtkImageView::SetTimeIndex ( vtkIdType index )
{
  if ( this->TimeIndex != index ) {

    if (this->Impl->ImageTemporalFilter.size())
    {
      if ( this->Impl->ImageTemporalFilter[0].IsNotNull ()) {

        switch ( this->Impl->TemporalFilterType ) {

          default:
          case IMAGE_VIEW_NONE : break;
            // Macro calls template method for correct argument type.
            #define ImageViewCaseEntry( type , enumName )		\
          case enumName :				\
          {						\
            this->SetTimeIndex < type > ( index );	\
            break ;					\
          }

            ImageViewCaseEntry( double, IMAGE_VIEW_DOUBLE );
            ImageViewCaseEntry( float, IMAGE_VIEW_FLOAT );
            ImageViewCaseEntry( int, IMAGE_VIEW_INT );
            ImageViewCaseEntry( unsigned int, IMAGE_VIEW_UNSIGNED_INT );
            ImageViewCaseEntry( short, IMAGE_VIEW_SHORT );
            ImageViewCaseEntry( unsigned short, IMAGE_VIEW_UNSIGNED_SHORT );
            ImageViewCaseEntry( long, IMAGE_VIEW_LONG );
            ImageViewCaseEntry( unsigned long, IMAGE_VIEW_UNSIGNED_LONG );
            ImageViewCaseEntry( char, IMAGE_VIEW_CHAR );
            ImageViewCaseEntry( unsigned char, IMAGE_VIEW_UNSIGNED_CHAR );
            ImageViewCaseEntry( RGBPixelType, IMAGE_VIEW_RGBPIXELTYPE );
            ImageViewCaseEntry( RGBAPixelType, IMAGE_VIEW_RGBAPIXELTYPE );
            ImageViewCaseEntry( UCharVector3Type, IMAGE_VIEW_UCHARVECTOR3TYPE  );
            ImageViewCaseEntry( FloatVector3Type, IMAGE_VIEW_FLOATVECTOR3TYPE  );

        };
      }
      this->TimeIndex = index;
      this->GetInput ()->UpdateInformation ();
      this->GetInput ()->PropagateUpdateExtent ();
      this->InvokeEvent( vtkImageView2DCommand::TimeChangeEvent );
      this->Modified ();
    }
  }
}
//----------------------------------------------------------------------------

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

//----------------------------------------------------------------------------
template < class T >
inline void vtkImageView::SetITKInput (typename itk::Image<T, 3>::Pointer itkImage, int layer)
{
  if(itkImage.IsNull())
    return;

  itkImage->UpdateOutputInformation();
 /* if (this->ITKInput==itkImage)
    return;*/
  if (layer < (int)(this->ITKInputVector.size()) && (this->ITKInputVector[layer]==itkImage) )
    return;

  typedef itk::ImageToVTKImageFilter< itk::Image<T, 3> > ConverterType;
  typename ConverterType::Pointer myConverter = ConverterType::New();
  myConverter->SetInput ( itkImage );
  myConverter->UpdateOutputInformation();
  /**
     The origin in ITK pipeline is taken into account in a different
     way than in the VTK equivalent.
     A first hack would be to force the vtkImageData instance to have
     a null origin, and put the ITK origin in the 4th column of the
     OrientationMatrix instance. BUT, when the ITK pipeline is updated,
     then the ITK origin is put back in place in the vtkImageData instance.

     Therefore we need to keep the vtkImageData's origin the same as the
     ITK one. And, we need to correct for this misbehaviour through a hack
     in the OrientationMatrix 4th column, a sort of corrected origin.
  */
  typename itk::ImageBase<3>::DirectionType directions = itkImage->GetDirection();
  typename itk::ImageBase<3>::PointType origin = itkImage->GetOrigin();
  vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
  matrix->Identity();
  for (int i=0; i<3; i++)
    for (int j=0; j<3; j++)
      matrix->SetElement (i, j, directions (i,j));
  double v_origin[4], v_origin2[4];
  for (int i=0; i<3; i++)
    v_origin[i] = origin[i];
  v_origin[3] = 1.0;
  matrix->MultiplyPoint (v_origin, v_origin2);
  for (int i=0; i<3; i++)
    matrix->SetElement (i, 3, v_origin[i]-v_origin2[i]);
  /**
     The matrix instance is from now on corrected.
  */
  this->SetInput ( myConverter->GetOutput(), matrix, layer);
  this->Impl->ImageConverter[layer] = myConverter;

  if((int)(this->ITKInputVector.size()) <= layer) {
      this->ITKInputVector.resize (layer+1);
      this->ITKInputVector[layer] = itkImage;
  }
  else
    this->ITKInputVector[layer] = itkImage;
  this->Modified();
  matrix->Delete();
}


template < class T >
inline void vtkImageView::AddITKInput (typename itk::Image<T, 3>::Pointer itkImage)
{
    int layer = GetNumberOfLayers();
    SetITKInput<T>(itkImage, layer);
}

template < class T >
inline void vtkImageView::SetITKInput4 (typename itk::Image<T, 4>::Pointer itkImage, int layer)
{
  if( itkImage.IsNull() )
  {
    return;
  }

  // if (this->ITKInput4==itkImage) // if SetITKInput is called after SetITKInput4, pointer is not changed
  // return;                        // and image is not displayed

  typedef typename itk::Image<T, 4> ImageType4d;
  typedef typename itk::Image<T, 3> ImageType3d;
  typedef typename itk::ExtractImageBufferFilter< ImageType4d, ImageType3d > ExtractFilterType;

  typename ExtractFilterType::Pointer extractor (ExtractFilterType::New());
  // extractor->SetNumberOfThreads(1); // not thread safe, to correct // extractor is now not multi-threaded
  typename ImageType4d::SizeType size = itkImage->GetLargestPossibleRegion().GetSize();

  unsigned int timeIndex = this->TimeIndex;
  unsigned int timeLimit = itkImage->GetLargestPossibleRegion().GetSize()[3]-1;

  if (timeIndex>timeLimit)
    timeIndex = timeLimit;

  typename ImageType4d::IndexType index = {{0,0,0, timeIndex}};

  size[3] = 0;
  typename ImageType4d::RegionType region;
  region.SetSize (size);
  region.SetIndex (index);

  extractor->SetExtractionRegion (region);
  extractor->SetDirectionCollapseToGuess();
  extractor->SetInput ( itkImage );

  this->Impl->ImageTemporalFilter[layer] = extractor;
  this->Impl->TemporalFilterType = vtkImageView::vtkImageViewImplementation::GetImageViewType <T> ();
  this->ITKInput4 = itkImage;
  typename ImageType3d::Pointer itkImage3 = extractor->GetOutput ();
  extractor->UpdateLargestPossibleRegion();
  this->SetITKInput ( itkImage3, layer);
  // itkImage3->DisconnectPipeline();
}

template < class T >
inline void vtkImageView::AddITKInput4 (typename itk::Image<T, 4>::Pointer itkImage)
{
    int layer = GetNumberOfLayers();
    SetITKInput4<T>(itkImage, layer);
}

#define vtkImplementSetITKInputMacro(type)				\
void vtkImageView::SetITKInput (itk::Image<type, 3>::Pointer itkImage, int layer) \
{									\
SetITKInput < type > (itkImage, layer);					\
}
vtkImplementSetITKInputMacro (double);
vtkImplementSetITKInputMacro (float);
vtkImplementSetITKInputMacro (int);
vtkImplementSetITKInputMacro (unsigned int);
vtkImplementSetITKInputMacro (short);
vtkImplementSetITKInputMacro (unsigned short);
vtkImplementSetITKInputMacro (long);
vtkImplementSetITKInputMacro (unsigned long);
vtkImplementSetITKInputMacro (char);
vtkImplementSetITKInputMacro (unsigned char);
vtkImplementSetITKInputMacro (RGBPixelType);
vtkImplementSetITKInputMacro (RGBAPixelType);
vtkImplementSetITKInputMacro (UCharVector3Type);
vtkImplementSetITKInputMacro (FloatVector3Type);

#define vtkImplementAddITKInputMacro(type)				\
void vtkImageView::AddITKInput (itk::Image<type, 3>::Pointer itkImage) \
{									\
AddITKInput < type > (itkImage);					\
}
vtkImplementAddITKInputMacro (double);
vtkImplementAddITKInputMacro (float);
vtkImplementAddITKInputMacro (int);
vtkImplementAddITKInputMacro (unsigned int);
vtkImplementAddITKInputMacro (short);
vtkImplementAddITKInputMacro (unsigned short);
vtkImplementAddITKInputMacro (long);
vtkImplementAddITKInputMacro (unsigned long);
vtkImplementAddITKInputMacro (char);
vtkImplementAddITKInputMacro (unsigned char);
vtkImplementAddITKInputMacro (RGBPixelType);
vtkImplementAddITKInputMacro (RGBAPixelType);
vtkImplementAddITKInputMacro (UCharVector3Type);
vtkImplementAddITKInputMacro (FloatVector3Type);

itk::ImageBase<3>* vtkImageView::GetITKInput (int layer) const
{
  if (layer < (int)(this->ITKInputVector.size()))
    return this->ITKInputVector[layer];
  return NULL;
}

#define vtkImplementSetITKInput4Macro(type)				\
void vtkImageView::SetITKInput4 (itk::Image<type, 4>::Pointer itkImage, int layer) \
{									\
SetITKInput4 < type > (itkImage, layer);					\
}

vtkImplementSetITKInput4Macro (double);
vtkImplementSetITKInput4Macro (float);
vtkImplementSetITKInput4Macro (int);
vtkImplementSetITKInput4Macro (unsigned int);
vtkImplementSetITKInput4Macro (short);
vtkImplementSetITKInput4Macro (unsigned short);
vtkImplementSetITKInput4Macro (long);
vtkImplementSetITKInput4Macro (unsigned long);
vtkImplementSetITKInput4Macro (char);
vtkImplementSetITKInput4Macro (unsigned char);
vtkImplementSetITKInput4Macro (RGBPixelType);
vtkImplementSetITKInput4Macro (RGBAPixelType);
vtkImplementSetITKInput4Macro (UCharVector3Type);
vtkImplementSetITKInput4Macro (FloatVector3Type);

#define vtkImplementAddITKInput4Macro(type)				\
void vtkImageView::AddITKInput4 (itk::Image<type, 4>::Pointer itkImage) \
{									\
AddITKInput4 < type > (itkImage);					\
}

vtkImplementAddITKInput4Macro (double);
vtkImplementAddITKInput4Macro (float);
vtkImplementAddITKInput4Macro (int);
vtkImplementAddITKInput4Macro (unsigned int);
vtkImplementAddITKInput4Macro (short);
vtkImplementAddITKInput4Macro (unsigned short);
vtkImplementAddITKInput4Macro (long);
vtkImplementAddITKInput4Macro (unsigned long);
vtkImplementAddITKInput4Macro (char);
vtkImplementAddITKInput4Macro (unsigned char);
vtkImplementAddITKInput4Macro (RGBPixelType);
vtkImplementAddITKInput4Macro (RGBAPixelType);
vtkImplementAddITKInput4Macro (UCharVector3Type);
vtkImplementAddITKInput4Macro (FloatVector3Type);

itk::ImageBase<4>* vtkImageView::GetTemporalITKInput() const
{
  return this->ITKInput4;
}


void vtkImageView::GetInputBounds ( double * bounds )
{
  this->GetInput()->UpdateInformation ();

  // GetWholeBoundingBox may not be updated by UpdateInformation
  //this->GetInput()->GetWholeBoundingBox (bounds);

  const int * wholeExtent = this->GetInput ()->GetWholeExtent ();
  const double * spacing = this->GetInput ()->GetSpacing ();
  const double * origin = this->GetInput ()->GetOrigin ();

  for ( int i(0); i < 3; ++i )
  {
    bounds[ 2*i     ] = spacing [ i ]*wholeExtent [ 2*i     ] + origin [ i ];
    bounds[ 2*i + 1 ] = spacing [ i ]*wholeExtent [ 2*i + 1 ] + origin [ i ];
  }
}

void vtkImageView::GetInputBoundsInWorldCoordinates ( double * bounds )
{
  double imageBounds [6];
  this->GetInput()->UpdateInformation();

  const int * wholeExtent = this->GetInput ()->GetWholeExtent ();
  const double * spacing = this->GetInput ()->GetSpacing ();
  const double * origin = this->GetInput ()->GetOrigin ();

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
void vtkImageView::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "OrientationMatrix:\n";
  this->OrientationMatrix->PrintSelf(os,indent.GetNextIndent());

//  if ( this->LookupTable != NULL )
//  {
//    os << indent << "LookupTable:\n";
//    this->LookupTable->PrintSelf(os,indent.GetNextIndent());
//  }

//  if ( this->ColorTransferFunction != NULL )
//  {
//    os << indent << "ColorTransferFunction:\n";
//    this->ColorTransferFunction->PrintSelf(os,indent.GetNextIndent());
//  }

//  if ( this->OpacityTransferFunction != NULL )
//  {
//    os << indent << "OpacityTransferFunction:\n";
//    this->OpacityTransferFunction->PrintSelf(os,indent.GetNextIndent());
//  }

//  os << indent << "WindowLevel:\n";
//  this->WindowLevel->PrintSelf(os,indent.GetNextIndent());

  if (this->Input)
  {
    os << indent << "Input:\n";
    this->Input->PrintSelf(os,indent.GetNextIndent());
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
  }
}

int vtkImageView::GetCurrentLayer() const
{
  return this->CurrentLayer;
}

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
