/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#ifndef __vtkLandmarkSegmentationController_h
#define __vtkLandmarkSegmentationController_h

#include <itkVariationalFunctionImageToImageFilter.h>
#include <itkImageToVTKImageFilter.h>

#include <QList>

#include <vtkCommand.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkPolyDataAlgorithm.h>

class vtkMatrixToLinearTransform;
class vtkContourFilter;
class vtkLandmarkWidget;
class vtkCollection;
class vtkLandmarkSegmentationController;
class vtkPolyDataMapper;
class vtkActor;

//BTX
class VTK_EXPORT vtkLandmarkSegmentationControllerCommand : public vtkCommand
{

public:
    static vtkLandmarkSegmentationControllerCommand *New()
    {
        return new vtkLandmarkSegmentationControllerCommand;
    }

    virtual void Execute ( vtkObject *caller, unsigned long, void* );

    void SetController (vtkLandmarkSegmentationController *arg);

protected:
    vtkLandmarkSegmentationControllerCommand();
    ~vtkLandmarkSegmentationControllerCommand();

private:
    vtkLandmarkSegmentationController *Controller;

};
//ETX

class VTK_EXPORT vtkLandmarkSegmentationController : public vtkPolyDataAlgorithm
{
public:
    static vtkLandmarkSegmentationController *New();
    vtkTypeMacro(vtkLandmarkSegmentationController, vtkPolyDataAlgorithm)
    void PrintSelf(ostream &os, vtkIndent indent);

    typedef double ScalarType;
    typedef itk::Image<ScalarType, 3> ImageType;
    typedef itk::Image<unsigned char,3> binaryType;
    typedef itk::Image<int, 3> BooleanImageType;
    typedef itk::VariationalFunctionImageToImageFilter<ImageType> FilterType;
    typedef itk::ImageToVTKImageFilter<ImageType> ConverterType;
    typedef FilterType::ConstraintListType ConstraintListType;
    typedef FilterType::ConstraintType ConstraintType;

    virtual void SetInput(ImageType::Pointer input);
    ImageType::Pointer GetInput();

    void SetConstraints (ConstraintListType arg);
    void SetConstraints (vtkPointSet *arg);

    void SetInteractorCollection (vtkCollection *arg);
    vtkGetObjectMacro (InteractorCollection, vtkCollection)

    vtkGetObjectMacro (LandmarkCollection, vtkCollection)
    vtkGetObjectMacro (TotalLandmarkCollection, vtkCollection)

    void SetEnabled (unsigned int arg);
    vtkGetMacro (Enabled, unsigned int)
    vtkBooleanMacro (Enabled, unsigned int)

    ImageType::Pointer GetImplicitImage()
    {
        return m_Filter->GetOutput();
    }
    void GetLandmarkSet (vtkPolyData *arg);

    void RefreshConstraints();

    vtkLandmarkWidget* AddConstraint (double *pos, int type);
    bool RemoveConstraint (vtkLandmarkWidget *arg);

    void setViews2D(QList<vtkImageView2D*> *views);
    QList<vtkImageView2D*> * getViews2D()
    {
        return Views2D;
    }

    void setViews3D(QList<vtkImageView3D*> *views);
    QList<vtkImageView3D*> * getViews3D()
    {
        return Views3D;
    }

    void setMode3D(bool val)
    {
        mode3D = val;
    }

    bool getMode3D()
    {
        return mode3D;
    }

    bool getModeMPR()
    {
        return modeMPR;
    }

    binaryType::Pointer GetBinaryImage();

    void setOutputSize(int x, int y, int z)
    {
        outputSize[0] = x;
        outputSize[1] = y;
        outputSize[2] = z;
    }

    void DeleteAllLandmarks();

protected:
    vtkLandmarkSegmentationController();
    ~vtkLandmarkSegmentationController();

    int RequestData ( vtkInformation *vtkNotUsed(request),
                      vtkInformationVector **inputVector,
                      vtkInformationVector *outputVector);

    void LinkInteractions();

private:
    vtkLandmarkSegmentationController(const vtkLandmarkSegmentationController&);  // Not implemented.
    void operator=(const vtkLandmarkSegmentationController&);  // Not implemented.
    ImageType::Pointer          m_Input;
    FilterType::Pointer         m_Filter;
    ConverterType::Pointer      m_Converter;
    vtkMatrixToLinearTransform *Transformer;
    vtkContourFilter           *SurfaceExtractor;
    ConstraintListType          Constraints;
    vtkCollection              *LandmarkCollection;
    vtkCollection              *TotalLandmarkCollection;
    vtkCollection              *InteractorCollection;
    unsigned int                Enabled;
    vtkLandmarkSegmentationControllerCommand *Command;
    vtkPolyDataMapper         *Mapper;
    vtkActor                  *Actor;
    double                      LandmarkRadius;
    QList<vtkImageView2D*> *Views2D;
    QList<vtkImageView3D*> *Views3D;
    bool mode3D;
    bool modeMPR;
    ImageType::Pointer implicitFunction;
    int outputSize[3]; // size expected for the output
};

#endif


