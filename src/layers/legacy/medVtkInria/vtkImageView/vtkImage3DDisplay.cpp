#include "vtkImage3DDisplay.h"
#include <vtkImageData.h>
#include <vtkImageAlgorithm.h>

vtkStandardNewMacro(vtkImage3DDisplay);

vtkImage3DDisplay::vtkImage3DDisplay()
{
    //this->InputImageOld = 0;
    this->InputConnection = 0;
    this->Opacity = 1.0;
    this->Visibility = 1;
    this->ColorWindow = 1e-3 * VTK_DOUBLE_MAX;
    this->ColorLevel = 0;
    this->UseLookupTable = false;
    this->LookupTable = nullptr;
}

vtkImage3DDisplay::~vtkImage3DDisplay()
{
    /*if (this->InputConnection) 
    {
        this->InputConnection->Delete();
    }*/
    if (this->LookupTable)
    {
        this->LookupTable->Delete();
    }
}


void vtkImage3DDisplay::SetInputConnection(vtkAlgorithmOutput* pi_poVtkAlgoPort)
{    
    if (pi_poVtkAlgoPort)
    {
        if (pi_poVtkAlgoPort->GetProducer() && pi_poVtkAlgoPort->GetProducer()->IsA("vtkImageAlgorithm"))
        {
            this->InputConnection = pi_poVtkAlgoPort;
            vtkImageAlgorithm* poVtkImgAlgoTmp = static_cast<vtkImageAlgorithm*>(pi_poVtkAlgoPort->GetProducer());
            vtkImageData* poVtkImgTmp = poVtkImgAlgoTmp->GetOutput();

            m_sVtkImageInfo.SetSpacing(poVtkImgTmp->GetSpacing(), 3);
            m_sVtkImageInfo.SetOrigin(poVtkImgTmp->GetOrigin(), 3);
            m_sVtkImageInfo.SetScalarRange(poVtkImgTmp->GetScalarRange());
            m_sVtkImageInfo.SetExtent(poVtkImgTmp->GetExtent());
            m_sVtkImageInfo.SetDimensions(poVtkImgTmp->GetDimensions(), 3);
            m_sVtkImageInfo.scalarType = poVtkImgTmp->GetScalarType();
            m_sVtkImageInfo.nbScalarComponent = poVtkImgTmp->GetNumberOfScalarComponents();
            m_sVtkImageInfo.initialized = true;

        } 
        else
        {
            vtkErrorMacro(<< "Set input prior to adding layers");
        }
    } 
    else
    {
        memset(&m_sVtkImageInfo, 0, sizeof(m_sVtkImageInfo));
        this->InputConnection = nullptr;
    }
}

medVtkImageInfo* vtkImage3DDisplay::GetVtkImageInfo()
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

vtkAlgorithmOutput* vtkImage3DDisplay::GetOutputPort()
{
    return this->InputConnection;
}
