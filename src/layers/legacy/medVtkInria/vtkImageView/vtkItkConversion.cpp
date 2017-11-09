/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2017. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include <cmath>

#include "vtkItkConversion.h"
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

#include <itkExtractImageBufferFilter.h>
#include <itkImageToVTKImageFilter.h>
#include <itkMatrix.h>
#include <vtkRendererCollection.h>
#include <vtkImageReader2.h>

//#include <vnl/algo/vnl_qr.h>

#ifdef WIN32
#define snprintf _snprintf_s
#endif


// Enumeration for the supported pixel types
// NT: why not using the vtk IO definitions ?
namespace 
{
    enum ImageViewType
    {
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
        IMAGE_VIEW_FLOATVECTOR3TYPE
    };
}


//----------------------------------------------------------------------------
template < class T >
inline bool vtkItkConversion::SetITKInput (typename itk::Image<T, 3>::Pointer itkImage, int pi_iLayer)
{
    bool bRes = true;

    //Return without any action if image is already attached to a layer.
    if (!(pi_iLayer < (int)(this->ITKInputVector.size()) && (this->ITKInputVector[pi_iLayer] == itkImage)))
    {
        //////////////////////////////////////////////////////////////////////////
        //Initialization phase
        itkImage->UpdateOutputInformation();

        typedef itk::ImageToVTKImageFilter< itk::Image<T, 3> > ConverterType;
        typename ConverterType::Pointer myConverter = ConverterType::New();
        myConverter->SetInput(itkImage);
        myConverter->UpdateOutputInformation();

        //////////////////////////////////////////////////////////////////////////
        // bli
        this->ImageConverter[pi_iLayer] = myConverter;
        if ((int)(this->ITKInputVector.size()) <= pi_iLayer)
        {
            this->ITKInputVector.resize(pi_iLayer + 1);
            this->ITKInputVector[pi_iLayer] = itkImage;
        }
        else
        {
            this->ITKInputVector[pi_iLayer] = itkImage;
        }

        //////////////////////////////////////////////////////////////////////////
        // bli
        myConverter->GetImporter()->UpdateInformation();
        myConverter->GetImporter()->Update();
        this->ImageConverterOutput[pi_iLayer] = myConverter->GetImporter()->GetOutputPort();
    }

    return bRes;
}

bool vtkItkConversion::GetConversion(int pi_iLayer, vtkAlgorithmOutput *& po_poAlgoOut, vtkMatrix4x4 *&po_poMatrix)
{
    bool bRes = true;

    po_poAlgoOut = nullptr;
    po_poMatrix = nullptr;

    if (pi_iLayer < (int)(this->ITKInputVector.size()))
    {
        //////////////////////////////////////////////////////////////////////////
        // Copy the output port
        po_poAlgoOut = this->ImageConverterOutput[pi_iLayer];

        //////////////////////////////////////////////////////////////////////////
        // Orientation matrix correction phase
        /**
            The origin in ITK pipeline is taken into account in a different
            way than in the VTK equivalent.
            A first hack would be to force the vtkImageData instance to have
            a null origin, and put the ITK origin in the 4th column of the
            OrientationMatrix instance. BUT, when the ITK pipeline is updated,
            then the ITK origin is put back in place in the vtkImageData instance.

            Therefore we need to keep the vtkImageData's origin the same as the
            ITK one. And, we need to correct for this misbehavior through a hack
            in the OrientationMatrix 4th column, a sort of corrected origin.
        */
        typename itk::ImageBase<3>::DirectionType directions = this->ITKInputVector[pi_iLayer]->GetDirection();
        typename itk::ImageBase<3>::PointType origin = this->ITKInputVector[pi_iLayer]->GetOrigin();
        po_poMatrix = vtkMatrix4x4::New();
        po_poMatrix->Identity();
        for (int i = 0; i<3; i++)
        {
            for (int j = 0; j<3; j++)
            {
                po_poMatrix->SetElement(i, j, directions(i, j));
            }
        }
        double v_origin[4], v_origin2[4];
        for (int i = 0; i<3; i++)
        {
            v_origin[i] = origin[i];
        }
        v_origin[3] = 1.0;
        po_poMatrix->MultiplyPoint(v_origin, v_origin2);
        for (int i = 0; i<3; i++)
        {
            po_poMatrix->SetElement(i, 3, v_origin[i] - v_origin2[i]);
        }
        /**
            The po_poMatrix instance is from now on corrected.
        */
    }
    else
    {
        bRes = false;
    }

    return bRes;
}

#define vtkImplementSetITKInputMacro(type)                                                    \
    bool vtkItkConversion::SetITKInput (itk::Image<type, 3>::Pointer itkImage, int pi_iLayer) \
    {                                                                                         \
        return SetITKInput < type > (itkImage, pi_iLayer);                                    \
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


itk::ImageBase<3>*vtkItkConversion::GetITKInput (int layer) const
{
    if (layer < (int)(this->ITKInputVector.size()))
        return this->ITKInputVector[layer];
    return NULL;
}

