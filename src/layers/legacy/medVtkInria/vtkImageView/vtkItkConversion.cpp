/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2017. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include "vtkItkConversion.h"
#include <itkImageToVTKImageFilter.h>

//----------------------------------------------------------------------------
template < class T >
inline bool vtkItkConversion::SetITKInput (typename itk::Image<T, 3>::Pointer itkImage)
{
    bool bRes = itkImage;

    //Return without any action if image is already attached to a layer.
    if (bRes)
    {
        //////////////////////////////////////////////////////////////////////////
        // Initialization phase
        typedef itk::ImageToVTKImageFilter< itk::Image<T, 3> > ConverterType;
        typename ConverterType::Pointer itk2VtkConverter = ConverterType::New();
        itkImage->UpdateOutputInformation();
        itk2VtkConverter->SetInput(itkImage);
        
        //////////////////////////////////////////////////////////////////////////
        // Update phase
        itk2VtkConverter->UpdateOutputInformation();
        itk2VtkConverter->GetImporter()->UpdateInformation();
        itk2VtkConverter->GetImporter()->Update();

        //////////////////////////////////////////////////////////////////////////
        // Register phase
        m_ImageConverter = itk2VtkConverter;
        m_ItkInputImage = itkImage;
        m_ImageConverterOutput = itk2VtkConverter->GetImporter()->GetOutputPort();
    }

    return bRes;
}

bool vtkItkConversion::GetConversion(vtkAlgorithmOutput *& po_poAlgoOut, vtkMatrix4x4 *&po_poMatrix)
{
    bool bRes = true;

    po_poAlgoOut = nullptr;
    po_poMatrix = nullptr;

    if (m_ImageConverterOutput)
    {

        //////////////////////////////////////////////////////////////////////////
        // Copy the output port
        po_poAlgoOut = m_ImageConverterOutput;

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
        typename itk::ImageBase<3>::DirectionType directions = m_ItkInputImage->GetDirection();
        typename itk::ImageBase<3>::PointType origin = m_ItkInputImage->GetOrigin();
        po_poMatrix = vtkMatrix4x4::New();
        po_poMatrix->Identity();
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                po_poMatrix->SetElement(i, j, directions(i, j));
            }
        }
        double v_origin[4], v_origin2[4];
        for (int i = 0; i < 3; i++)
        {
            v_origin[i] = origin[i];
        }
        v_origin[3] = 1.0;
        po_poMatrix->MultiplyPoint(v_origin, v_origin2);
        for (int i = 0; i < 3; i++)
        {
            po_poMatrix->SetElement(i, 3, v_origin[i] - v_origin2[i]);
        }
        //The po_poMatrix instance is from now on corrected.
    }
    else
    {
        bRes = false;
    }

    return bRes;
}

#define vtkImplementSetITKInputMacro(type)\
    bool vtkItkConversion::SetITKInput (itk::Image<type, 3>::Pointer itkImage)\
    {\
        return SetITKInput < type > (itkImage);\
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

/*itk::ImageBase<3>*vtkItkConversion::GetITKInput (int layer) const
{
    if (layer < (int)(m_ItkInputImage.size()))
        return m_ItkInputImage[layer];
    return NULL;
}*/