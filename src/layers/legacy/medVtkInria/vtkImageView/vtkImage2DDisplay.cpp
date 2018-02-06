#include "vtkImage2DDisplay.h"
#include <vtkImageMapper3D.h>
#include <vtkImageProperty.h>
#include <vtkImageAlgorithm.h>
#include <vtkAlgorithmOutput.h>

vtkStandardNewMacro(vtkImage2DDisplay);

vtkImage2DDisplay::vtkImage2DDisplay()
{
    //this->m_bInputSeted = false;
    this->Input = 0;
    this->ImageActor        = vtkImageActor::New();
    this->WindowLevel       = vtkImageMapToColors::New();
    this->WindowLevel->SetOutputFormatToRGBA();
    this->ColorWindow = 1e-3 * VTK_DOUBLE_MAX;
    this->ColorLevel  = 0;
    this->ColorTransferFunction = NULL;
    this->OpacityTransferFunction = NULL;
    this->UseLookupTable = false;
    //this->m_sVtkImageInfo = { 0 };
}

vtkImage2DDisplay::~vtkImage2DDisplay()
{
}

void vtkImage2DDisplay::SetInput(vtkAlgorithmOutput *pi_poVtkAlgoPort)
{
    if (pi_poVtkAlgoPort)
    {
        if (pi_poVtkAlgoPort->GetProducer() && pi_poVtkAlgoPort->GetProducer()->IsA("vtkImageAlgorithm")/* && pi_poVtkAlgoPort->GetProducer()->IsTypeOf("vtkImageAlgorithm")*/)
        {
            vtkAlgorithmOutput *poVtkAlgoPortTmp = pi_poVtkAlgoPort;
            vtkImageAlgorithm *poVtkImgAlgoTmp = static_cast<vtkImageAlgorithm*>(pi_poVtkAlgoPort->GetProducer());
            vtkImageData *poVtkImgTmp = poVtkImgAlgoTmp->GetOutput();

            m_sVtkImageInfo.SetSpacing(poVtkImgTmp->GetSpacing());
            m_sVtkImageInfo.SetOrigin(poVtkImgTmp->GetOrigin());
            m_sVtkImageInfo.SetScalarRange(poVtkImgTmp->GetScalarRange());
            m_sVtkImageInfo.SetExtent(poVtkImgTmp->GetExtent());
            m_sVtkImageInfo.SetDimensions(poVtkImgTmp->GetDimensions());
            m_sVtkImageInfo.scalarType = poVtkImgTmp->GetScalarType();
            m_sVtkImageInfo.nbScalarComponent = poVtkImgTmp->GetNumberOfScalarComponents();
            m_sVtkImageInfo.initialized = true;

            if (!(poVtkImgTmp->GetScalarType() == VTK_UNSIGNED_CHAR && (poVtkImgTmp->GetNumberOfScalarComponents() == 3 || poVtkImgTmp->GetNumberOfScalarComponents() == 4)))
            {
                this->WindowLevel->SetInputConnection(poVtkAlgoPortTmp);
                poVtkAlgoPortTmp = this->WindowLevel->GetOutputPort();
                //poVtkImgTmp = this->WindowLevel->GetOutput();
            }
            this->ImageActor->GetMapper()->SetInputConnection(poVtkAlgoPortTmp);
            //this->ImageActor->GetProperty()->SetInterpolationTypeToCubic();
        }
        else
        {
            vtkErrorMacro(<< "Set input prior to adding layers");
        }
    }
    else
    {
        memset(&m_sVtkImageInfo, 0, sizeof(m_sVtkImageInfo));
    }
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
