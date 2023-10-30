/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkImage2DDisplay.h"

#include <vtkAlgorithmOutput.h>
#include <vtkImageAlgorithm.h>
#include <vtkImageMapper3D.h>
#include <vtkImageProperty.h>

vtkStandardNewMacro(vtkImage2DDisplay)

vtkImage2DDisplay::vtkImage2DDisplay()
{
    this->InputProducer     = nullptr;
    this->ImageActor        = vtkSmartPointer<vtkImageActor>::New();
    this->WindowLevel       = vtkSmartPointer<vtkImageMapToColors>::New();
    this->WindowLevel->SetOutputFormatToRGBA();
    this->ColorWindow       = 1e-3 * VTK_DOUBLE_MAX;
    this->ColorLevel        = 0;
    this->ColorTransferFunction   = nullptr;
    this->OpacityTransferFunction = nullptr;
    this->UseLookupTable    = false;
}

void vtkImage2DDisplay::SetInputData(vtkImageData *pi_poVtkImage)
{
    if (pi_poVtkImage)
    {
        m_sVtkImageInfo.SetSpacing(pi_poVtkImage->GetSpacing());
        m_sVtkImageInfo.SetOrigin(pi_poVtkImage->GetOrigin());
        m_sVtkImageInfo.SetScalarRange(pi_poVtkImage->GetScalarRange());
        m_sVtkImageInfo.SetExtent(pi_poVtkImage->GetExtent());
        m_sVtkImageInfo.SetDimensions(pi_poVtkImage->GetDimensions());
        m_sVtkImageInfo.scalarType = pi_poVtkImage->GetScalarType();
        m_sVtkImageInfo.nbScalarComponent = pi_poVtkImage->GetNumberOfScalarComponents();
        m_sVtkImageInfo.initialized = true;

        if (m_sVtkImageInfo.scalarType == VTK_UNSIGNED_CHAR && (m_sVtkImageInfo.nbScalarComponent == 3 || m_sVtkImageInfo.nbScalarComponent == 4))
        {
            this->ImageActor->GetMapper()->SetInputData(pi_poVtkImage);
        }
        else
        {
            this->WindowLevel->SetInputData(pi_poVtkImage);
            this->ImageActor->GetMapper()->SetInputConnection(this->WindowLevel->GetOutputPort());
        }
    }
    else
    {
        m_sVtkImageInfo = medVtkImageInfo();
    }
}

void vtkImage2DDisplay::SetInputProducer(vtkAlgorithmOutput *pi_poAlgorithmOutput)
{
    InputProducer = static_cast<vtkImageAlgorithm*>(pi_poAlgorithmOutput->GetProducer());
}

vtkLookupTable*vtkImage2DDisplay::GetLookupTable() const
{
    return vtkLookupTable::SafeDownCast(this->GetWindowLevel()->GetLookupTable());
}

void vtkImage2DDisplay::SetColorTransferFunction(vtkColorTransferFunction *function)
{
    this->ColorTransferFunction = function;
}

void vtkImage2DDisplay::SetOpacityTransferFunction(vtkPiecewiseFunction *function)
{
    this->OpacityTransferFunction = function;
}

medVtkImageInfo* vtkImage2DDisplay::GetMedVtkImageInfo()
{
    return &m_sVtkImageInfo;
}

bool vtkImage2DDisplay::isInputSet()
{
    return m_sVtkImageInfo.initialized;
}
