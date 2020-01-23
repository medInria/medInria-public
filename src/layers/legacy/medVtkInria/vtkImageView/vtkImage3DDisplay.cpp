/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkImage3DDisplay.h"

#include <vtkImageAlgorithm.h>
#include <vtkImageData.h>

vtkStandardNewMacro(vtkImage3DDisplay)

vtkImage3DDisplay::vtkImage3DDisplay()
{
<<<<<<< HEAD
    this->InputProducer = nullptr;
=======
    this->InputConnection = 0;
>>>>>>> be44c23ec (- change some vtk SetInput to SetInputData)
    this->Opacity = 1.0;
    this->Visibility = 1;
    this->ColorWindow = 1e-3 * VTK_DOUBLE_MAX;
    this->ColorLevel = 0;
    this->UseLookupTable = false;
    this->LookupTable = nullptr;
}

vtkImage3DDisplay::~vtkImage3DDisplay()
{
    if (this->LookupTable)
    {
        this->LookupTable->Delete();
    }
}

void vtkImage3DDisplay::SetInputData(vtkImageData *pi_poVtkImage)
{
    if (pi_poVtkImage)
    {
<<<<<<< HEAD
        m_sVtkImageInfo.SetSpacing(pi_poVtkImage->GetSpacing());
        m_sVtkImageInfo.SetOrigin(pi_poVtkImage->GetOrigin());
        m_sVtkImageInfo.SetScalarRange(pi_poVtkImage->GetScalarRange());
        m_sVtkImageInfo.SetExtent(pi_poVtkImage->GetExtent());
        m_sVtkImageInfo.SetDimensions(pi_poVtkImage->GetDimensions());
        m_sVtkImageInfo.scalarType = pi_poVtkImage->GetScalarType();
        m_sVtkImageInfo.nbScalarComponent = pi_poVtkImage->GetNumberOfScalarComponents();
        m_sVtkImageInfo.initialized = true;
    }
=======
        if (pi_poVtkAlgoPort->GetProducer() && pi_poVtkAlgoPort->GetProducer()->IsA("vtkImageAlgorithm"))
        {
            this->InputConnection = pi_poVtkAlgoPort;
            vtkImageAlgorithm* poVtkImgAlgoTmp = static_cast<vtkImageAlgorithm*>(pi_poVtkAlgoPort->GetProducer());
            vtkImageData* poVtkImgTmp = poVtkImgAlgoTmp->GetOutput();

            m_sVtkImageInfo.SetSpacing(poVtkImgTmp->GetSpacing());
            m_sVtkImageInfo.SetOrigin(poVtkImgTmp->GetOrigin());
            m_sVtkImageInfo.SetScalarRange(poVtkImgTmp->GetScalarRange());
            m_sVtkImageInfo.SetExtent(poVtkImgTmp->GetExtent());
            m_sVtkImageInfo.SetDimensions(poVtkImgTmp->GetDimensions());
            m_sVtkImageInfo.scalarType = poVtkImgTmp->GetScalarType();
            m_sVtkImageInfo.nbScalarComponent = poVtkImgTmp->GetNumberOfScalarComponents();
            m_sVtkImageInfo.initialized = true;
        } 
        else
        {
            vtkErrorMacro(<< "Set input prior to adding layers");
        }
    } 
>>>>>>> be44c23ec (- change some vtk SetInput to SetInputData)
    else
    {
        memset(&m_sVtkImageInfo, 0, sizeof(m_sVtkImageInfo));
    }
}

void vtkImage3DDisplay::SetInputProducer(vtkAlgorithmOutput *pi_poAlgorithmOutput)
{
    InputProducer = static_cast<vtkImageAlgorithm*>(pi_poAlgorithmOutput->GetProducer());
}

medVtkImageInfo* vtkImage3DDisplay::GetMedVtkImageInfo()
{
    return &m_sVtkImageInfo;
}

bool vtkImage3DDisplay::isInputSet()
{
    return m_sVtkImageInfo.initialized;
}

vtkLookupTable* vtkImage3DDisplay::GetLookupTable()
{
    return this->LookupTable;
}
