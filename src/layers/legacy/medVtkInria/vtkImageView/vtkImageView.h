#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkInriaExport.h>

#include <medVtkImageInfo.h>

#include <vtkObject.h>
#include <vtkCommand.h>
#include <vtkInteractorStyle.h>

#include <cstring>

#include <vtkSmartPointer.h>
#include <vtkImageFromBoundsSource.h>

class vtkRenderer;
class vtkRenderWindow;
class vtkRenderWindowInteractor;
class vtkOrientationAnnotation;
class vtkImageViewCornerAnnotation;
class vtkTextProperty;
class vtkScalarBarActor;
class vtkMatrix4x4;
class vtkMatrixToLinearTransform;
class vtkImageData;
class vtkImageMapToColors;
class vtkAlgorithmOutput;
class vtkLookupTable;
class vtkDataSet;
class vtkPointSet;
class vtkProperty;
class vtkActor;
class vtkColorTransferFunction;
class vtkPiecewiseFunction;
class vtkDataSetCollection;
class vtkProp3DCollection;
class vtkProp3D;
class vtkScalarsToColors;
class vtkAlgorithm;
class vtkImageAlgorithm;


/**

   \class vtkImageView
   \brief This class is a top-level convenience class for displaying a scalar - or RGB
   image in a 2D or 3D scene.

   It inherits is inspired from vtkImageViewer2 class, which is initially designed for 2D scene
   visualization. However, vtkImageView generalizes its purpose to 2D AND 3D scene visualization.

   As a high-level class, it provides the user with convinient functionalities
   such as a colormap (SetLookupTable()), a scalar bar (ScalarBarActor), some corner annotations
   (CornerAnnotation), access to the background color (SetBackground()), the
   annotation text properties (SetTextProperty()), or a call for reseting to default values
   (Reset() or ResetCamera()).


   The principle add-on of this class is to tacke the common issue of placing
   different objects in a same consistent reference frame. In a world coordinates system, an
   volume image can be localized by its origin and its spacing, and an orientation vector defining
   how to rotate the volume to be consistent with reality.

   The vtkImageData class has among its attributes the origin and the spacing information.
   However, the orientation information is missing.

   The vtkImageView class tackle this lack by providing the user the possibility to set an
   orientation matrix with SetOrientationMatrix(). This matrix will directly be applied to the
   actor describing the image in the 2D - or 3D - scene. The rotation 3x3 component of this matrix
   has to be orthogonal (no scaling). The offset component may contain the origin information.
   In this case the user will have to make sure that this information is absent from the vtkImageData
   instance given in SetInputData(). For that you can call : view->GetInput()->SetOrigin(0,0,0).

   \ingroup AdvancedRendering
   \see vtkImageView2D vtkImageViewCollection vtkImageView3D
   \author Pierre Fillard and Nicolas Toussaint, INRIA
*/


/**
   Notes on changes from Nicolas Toussaint:

   A) the instance names:
   Interactor, instead of RenderWindowInteractor : shorter
   ShowXX instead of XXVisibility : shorter


   B) interactorstyle : it is an observer but the most generic instance would
   certainly be a InteractorStyle, and it could be interesting to have it on the
   generic view instead of the view2d or 3d

   C) The OrientationAnnotation will never be shown in anything else than
   the 2D view, so it has been moved there

   D) The LookupTable has to be a lookup table

   E) duplicated methods
   GetCurrentPointDoubleValue() can be obtained with GetValueAtPosition() and GetCurrentPoint()
   GetWorldCoordinates() is GetCurrentPoint()
   Get/Set BackgroundColor() is Get/Set Background() to match VTK

   F) Get/Set Position() and Enable() / Disable() re-installed using the RenderWindow

   G) (Un)Initialize() and (Un)InitializeInteractor() replaced by VTK names (Un)InstallPipeline()

   H) LinkXX moved to the collection which is the instance meant to rule them.

   I) WindowLevel/ImageToColor
   This instance (vtkImageMapToColors) is totally compatible between 2D and 3D view, as
   only one is needed in the 3D view where 3 vtkImageActor will come at its output
   instead of 1 in the 2D view

   J) Events
   The view should not send or have any events. This is the role of Command or Style instances.
   We have to figure out a way of having the same behavior without having events in the view instances

   K) protected
   It is very useful to have local instances in the protected section, so that upper level classes
   can access them directly.

   L) Input
   No actual need to keep the input instance, but preferably directly put it into the ImageToColor instance
   as it will (or should) normally not be used otherwise.
*/
class MEDVTKINRIA_EXPORT vtkImageView : public vtkObject
{
public:
    vtkTypeMacro(vtkImageView, vtkObject)
    void PrintSelf (ostream& os, vtkIndent indent);

    vtkMTimeType GetMTime();

    /**
     List of events the image view is able to invoke.
     - CurrentPointChangedEvent is invoked each time the current point
     of the view is changed.
     - CameraChangedEvent is invoked each time the camera settings are modified
     "by the view". This includes any zoom or pan effect.
     - WindowLevelChangedEvent is invoked each time the window OR the level is
     modified.
  */
    //BTX
    enum EventIds
    {
        CurrentPointChangedEvent = vtkCommand::UserEvent+500,
        CameraChangedEvent,
        WindowLevelChangedEvent,
        ZoomChangedEvent,
        PanChangedEvent
    };
    //ETX

    virtual void Render();


    virtual void SetInput (vtkAlgorithmOutput* pi_poVtkAlgoOutput, vtkMatrix4x4 *matrix = 0, int layer = 0);
    virtual medVtkImageInfo* GetMedVtkImageInfo(int layer = 0) const = 0;

    // Get the internal render window, renderer, image map and interactor instances.
    vtkGetObjectMacro(RenderWindow,    vtkRenderWindow)
    virtual vtkRenderer * GetRenderer() const{return Renderer;}
    vtkGetObjectMacro(OverlayRenderer, vtkRenderer)
    vtkGetObjectMacro(InteractorStyle, vtkInteractorStyle)
    vtkGetObjectMacro(Interactor,      vtkRenderWindowInteractor)

    virtual void SetRenderWindow (vtkRenderWindow *arg);
    virtual void SetRenderer     (vtkRenderer *arg); // LAYER
    virtual void SetOverlayRenderer (vtkRenderer *arg);

    virtual void SetupInteractor           (vtkRenderWindowInteractor* arg);
    virtual void SetRenderWindowInteractor (vtkRenderWindowInteractor* arg) { this->SetupInteractor (arg); }

    // Start/Stop the interactor relation with the view.
    // it basically plug or unplug the interactor.
    // IMPORTANT: pure abstract method to be implemented by subclasses.
    virtual void InstallInteractor()   = 0;
    virtual void UnInstallInteractor() = 0;

    // Attach a specific interactor style to this view.
    vtkSetObjectMacro(InteractorStyle, vtkInteractorStyle)

    /**
     The corner annotation gather information related to the image.
     In vtkImageView2D, it displays slice number, spacing, window-level, position, etc
     Access and change the values with GetCornerAnnotation()->SetText(n, const char*).
     n begins down-right and increases anti-clockwise.
    */
    vtkGetObjectMacro (CornerAnnotation, vtkImageViewCornerAnnotation)

    /**
     Get the scalar bar actor. This instance follows the color window/level
     of the viewer.
    */
    vtkGetObjectMacro (ScalarBar, vtkScalarBarActor)

    /**
     The OrientationMatrix instance (GetOrientationMatrix()) is a very important
     added feature of this viewer. It describes the rotation and translation to
     apply to the image bouding box (axis aligned) to the world coordinate system.

     Rotation part is usually given by the GetDirection() method on an itk::Image
     for instance. Translation usually correspond to the origin of the image given
     by GetOrigin() on an itk::Image.

     CAUTION: if you provide non-zero origin to the viewer vtkImageData input
     (SetInputData()), then don't provide translation to the OrientationMatrix instance,
     otherwise the information is redundant.

     The best behaviour is to force the origin of the vtkImageData input to zero and
     provide this origin information in the OrientationMatrix.

    */
    vtkGetObjectMacro (OrientationMatrix, vtkMatrix4x4) // LAYER
    virtual void SetOrientationMatrix (vtkMatrix4x4* matrix); // LAYER
    vtkGetObjectMacro (InvertOrientationMatrix, vtkMatrix4x4) // LAYER


    virtual void SetTransferFunctions (vtkColorTransferFunction *color,
                                       vtkPiecewiseFunction     *opacity);

    virtual void SetTransferFunctions (vtkColorTransferFunction *color,
                                       vtkPiecewiseFunction     *opacity,
                                       int layer);

    virtual vtkColorTransferFunction * GetColorTransferFunction() const;
    virtual vtkColorTransferFunction * GetColorTransferFunction(int layer) const= 0;

    virtual void SetColorTransferFunction (vtkColorTransferFunction *ctf);
    /** Sets the ColorTransferFunction, but just store the value in the layer,
     doesn't apply anything.
     @todo: call it storeColorTransferFunction perhaps?
    */
    virtual void StoreColorTransferFunction (vtkColorTransferFunction *ctf, int layer) = 0;


    virtual vtkPiecewiseFunction* GetOpacityTransferFunction () const;
    virtual vtkPiecewiseFunction* GetOpacityTransferFunction (int layer) const = 0;
    virtual void SetOpacityTransferFunction (vtkPiecewiseFunction *otf);
    /** Sets the OpacityTransferFunction, but just store the value in the layer,
     doesn't apply anything.
     @todo: call it storeOpacityTransferFunction perhaps?
    */
    virtual void StoreOpacityTransferFunction (vtkPiecewiseFunction *otf,int layer) = 0;


    virtual vtkLookupTable * GetLookupTable() const;
    virtual vtkLookupTable * GetLookupTable(int layer) const = 0;
    virtual void SetLookupTable (vtkLookupTable *lookuptable);
    virtual void SetLookupTable (vtkLookupTable *lookuptable, int layer);
    virtual void StoreLookupTable (vtkLookupTable *lookuptable, int layer) = 0;

    virtual bool GetUseLookupTable() const;
    virtual bool GetUseLookupTable(int layer) const = 0;
    virtual void SetUseLookupTable(bool use);
    virtual void SetUseLookupTable(bool use, int layer) = 0;

    /**
     The TextProperty instance (GetTextProperty()) describes the font and
     other settings of the CornerAnnotation instance (GetCornerAnnotation())
    */
    vtkGetObjectMacro (TextProperty, vtkTextProperty)
    virtual void SetTextProperty (vtkTextProperty* textproperty);

    /**
     The world is not always what we think it is ...

     Use this method to move the viewer position such that the position
     (in world coordinates) given by the arguments is contained by
     the slice plane. If the given position is outside the bounds
     of the image, then the slice will be as close as possible.
    */
    virtual void SetCurrentPoint (double x, double y, double z)
    {
        double pos[3] = {x,y,z};
        this->SetCurrentPoint (pos);
    }
    virtual void SetCurrentPoint (double pos[3]);

    /**
     Get the current position in world coordinate of
     the lastly clicked point.
    */
    vtkGetVector3Macro (CurrentPoint, double)

    /**
     Get/Update the current position of the cursor
     in world coordinate.
     This framework is only used in vtkViewImage2D to
     update corner annotations and cursor position.
    */
    vtkGetVector3Macro (CursorPosition, double)
    virtual void UpdateCursorPosition (double pos[3]);


    virtual void ResetCurrentPoint();


    virtual void GetWorldCoordinatesFromImageCoordinates (int indices[3], double* position);


    virtual void GetImageCoordinatesFromWorldCoordinates (double position[3], int* indices) const;


    virtual double GetValueAtPosition(double worldcoordinates[3], int component=0 );
    virtual double GetValueAtPosition(double worldcoordinates[3],
                                      int component,
                                      int layer );
    /**
     Set the background color. Format is RGB, 0 <= R,G,B <=1
     Example: SetBackground(0.9,0.9,0.9) for grey-white.
    */
    virtual void SetBackground(double rgb[3]);
    virtual void SetBackground(double r, double g, double b)
    {
        double rgb[3] = {r,g,b};
        this->SetBackground (rgb);
    }
    virtual double* GetBackground() const;


    void    SetCameraPosition (double* arg);
    double* GetCameraPosition() const;

    void    SetCameraFocalPoint (double* arg);
    double* GetCameraFocalPoint() const;

    void    SetCameraViewUp (double* arg);
    double* GetCameraViewUp() const;

    void   SetCameraParallelScale (double arg);
    double GetCameraParallelScale() const;

    virtual void   SetZoom(double arg);
    virtual double GetZoom();

    virtual void ResetCamera();

    /**
     * @brief ResetCamera resets the camera onto
     * the parameter dataset
     * @param dataSetToFocusOn
     */
    virtual void ResetCamera(vtkDataSet *dataSetToFocusOn);

    virtual void SetColorWindow(double s);
    virtual void SetColorWindow(double s,int layer);
    virtual void StoreColorWindow(double s,int layer) = 0;

    virtual double GetColorWindow() const;
    virtual double GetColorWindow(int layer) const = 0;

    virtual void SetColorLevel(double s);
    virtual void SetColorLevel(double s, int layer);
    virtual void StoreColorLevel(double s,int layer) = 0;

    virtual double GetColorLevel() const;
    virtual double GetColorLevel(int layer) const = 0;

    void SetColorWindowLevel(double w, double l);
    void SetColorWindowLevel(double w, double l, int layer);

    virtual void SetColorRange( double r[2] );

    virtual void GetColorRange( double r[2] );

    virtual void SetColorRange( double r[2], int layer );
    virtual void GetColorRange( double r[2], int layer );

    virtual void ResetWindowLevel();

    virtual void Reset();

    /**
    * Show/Hide the annotations.
    */
    vtkGetMacro (ShowAnnotations, int)

    /**
    * Show/Hide the annotations.
    */
    virtual void SetShowAnnotations (int);

    /**
    * Show/Hide the annotations.
    */
    vtkBooleanMacro (ShowAnnotations, int)

    /**
    * Show/Hide the scalarbar.
    */
    vtkGetMacro (ShowScalarBar, int)
    virtual void SetShowScalarBar (int);
    vtkBooleanMacro (ShowScalarBar, int)


    virtual int* GetPosition() const;
    virtual void SetPosition(int a,int b);
    virtual void SetPosition(int a[2]) { this->SetPosition(a[0],a[1]); }

    virtual int* GetSize() const;
    virtual void SetSize(int a, int b);
    virtual void SetSize(int a[2]) { this->SetSize(a[0],a[1]); }


    virtual void Enable();
    virtual void Disable();

    virtual int GetEnabled() const;

    virtual void Start();


    virtual void GetInputBounds ( double * bounds );
    virtual void GetInputBoundsInWorldCoordinates ( double * bounds );


    virtual void AddLayer(int);
    virtual void RemoveLayer(int layer);
    virtual void RemoveAllLayers();

    virtual bool HasLayer(int layer) const;

    virtual int GetNumberOfLayers() const;

    virtual void SetCurrentLayer (int layer);
    virtual int GetCurrentLayer() const;

    double * GetScalarRange(unsigned int pi_uiLayer = 0);


public:
    void        SetPatientName (const char* name);
    const char *GetPatientName() const;

    void        SetStudyName (const char* name);
    const char *GetStudyName() const;

    void        SetSeriesName (const char* name);
    const char *GetSeriesName() const;

    /**
     Abstract method to add a dataset to the view (has to be subclass of vtkPointSet).
     A vtkProperty of the dataset can be specified.

     ********* CAUTION ************

     subclasses SHOULD populate dataset and actor collections :
     this->DataSetCollection->AddItem(arg);
     this->DataSetActorCollection->AddItem(actor);

     ******************************
   */

    virtual vtkActor* AddDataSet (vtkPointSet* arg, vtkProperty* prop = nullptr) = 0;

    virtual void RemoveDataSet (vtkPointSet *arg);

    vtkProp3D* FindDataSetActor (vtkDataSet* arg);
    vtkDataSet* FindActorDataSet (vtkProp3D* arg);

    vtkGetObjectMacro (DataSetCollection, vtkDataSetCollection)
    vtkGetObjectMacro (DataSetActorCollection, vtkProp3DCollection)

    vtkGetMacro(IsInteractorInstalled, int)

    virtual vtkAlgorithm* Get2DDisplayMapperInputAlgorithm () const;
    virtual vtkAlgorithm* Get2DDisplayMapperInputAlgorithm(int layer) const;
    virtual vtkImageAlgorithm* GetInputAlgorithm(int layer) const;

protected:
    vtkImageView();
    ~vtkImageView();

    virtual void InstallPipeline();
    virtual void UnInstallPipeline();

    vtkColorTransferFunction * GetDefaultColorTransferFunction();
    vtkPiecewiseFunction * GetDefaultOpacityTransferFunction();

    virtual void SetTransferFunctionRangeFromWindowSettings(vtkColorTransferFunction *cf,
                                                            vtkPiecewiseFunction *of,
                                                            double minRange, double maxRange);
    virtual void SetTransferFunctionRangeFromWindowSettings();
    virtual void SetTransferFunctionRangeFromWindowSettings(int layer);
    virtual void SetWindowSettingsFromTransferFunction();
    virtual void SetWindowSettingsFromTransferFunction(int layer);

    virtual bool Compare(double *array1, double *array2, int size);
    virtual bool Compare(int *array1,    int *array2,    int size);
    virtual bool Compare(vtkMatrix4x4 *mat1, vtkMatrix4x4 *mat2);

    virtual vtkAlgorithmOutput* ResliceImageToInput(vtkAlgorithmOutput* pi_poVtkAlgoPort, vtkMatrix4x4 *matrix);

    virtual void GetWithinBoundsPosition (double* pos1, double* dos2);

    double getImageHalfMaximumSize();

protected:
    /**
     Takes a vtkScalarsToColors pointer
     (ideally from this->GetColorTransferFunction(layer))
     and applies it to the layer.
    */
    virtual void ApplyColorTransferFunction(vtkScalarsToColors * colors, int layer) = 0;

    /**
     The OrientationMatrix instance (GetOrientationMatrix()) is a very important
     added feature of this viewer. It describes the rotation and translation to
     apply to the image bouding box (axis aligned) to the world coordinate system.

     Rotation part is usually given by the GetDirection() method on an itk::Image
     for instance. Translation usually correspond to the origin of the image given
     by GetOrigin() on an itk::Image.

     CAUTION: if you provide non-zero origin to the viewer vtkImageData input
     (SetInputData()), then don't provide translation to the OrientationMatrix instance,
     otherwise the information is redundant.

     The best behaviour is to force the origin of the vtkImageData input to zero and
     provide this origin information in the OrientationMatrix.

    */
    vtkMatrix4x4* OrientationMatrix;
    vtkMatrix4x4* InvertOrientationMatrix;

    /**
     The corner annotation gather information related to the image.
     In vtkViewImage2D, it displays slice number, spacing, window-level, position, etc
     Access and change the values with GetCornerAnnotation()->SetText(n, const char*).
     n begins down-right and increases anti-clockwise.
    */
    vtkImageViewCornerAnnotation* CornerAnnotation;

    /**
     The TextProperty instance (GetTextProperty()) describes the font and
     other settings of the CornerAnnotation instance (GetCornerAnnotation())
    */
    vtkTextProperty* TextProperty;

    /**
     The lookup table is an alternative way to define a transfer
     function.  Internally this is transformed into a
     vtkColorTransferFunction (color) and a vtkPiecewiseFunction
     (opacity/alpha).  Default is a linear black to white table.
    */
    vtkLookupTable* LookupTable;

    /**
     Get the scalar bar actor. This instance follows the color window/level
     of the viewer.
    */
    vtkScalarBarActor* ScalarBar;

    /**
     This vtkTransform instance carries the OrientationMatrix (see GetOrientationMatrix())
     and is used to quickly transform the slice plane in vtkViewImage2D.
    */
    vtkMatrixToLinearTransform* OrientationTransform;


    vtkDataSetCollection* DataSetCollection;
    vtkProp3DCollection* DataSetActorCollection;

    int ShowAnnotations;
    int ShowScalarBar;

    /**
     Get the current position in world coordinate.
     This framework is only used in vtkViewImage2D to
     update corner annotations and cursor position.
    */
    double CurrentPoint[3];
    double CursorPosition[3];

    int CurrentLayer;
    int IsInteractorInstalled;

    vtkRenderer*                    Renderer;
    vtkRenderer*                    OverlayRenderer;  // Same camera as the Renderer, but higher layer.
    vtkRenderWindow*                RenderWindow;
    vtkRenderWindowInteractor*      Interactor;
    vtkInteractorStyle*             InteractorStyle;
    vtkImageMapToColors*            WindowLevel;

    vtkImageData*                   m_poInternalImageFromInput;
    vtkAlgorithmOutput*             m_poInputVtkAlgoOutput;

    std::string PatientName;
    std::string StudyName;
    std::string SeriesName;

    vtkSmartPointer<vtkImageFromBoundsSource> m_vtkImageFromBoundsSourceGenerator;

private:

    unsigned long InternalMTime;

    vtkImageView  (const vtkImageView&); // Not implemented.
    void operator=(const vtkImageView&); // Not implemented.
};
