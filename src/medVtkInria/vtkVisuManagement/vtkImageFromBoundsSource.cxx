/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkImageFromBoundsSource.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"

#include "vtkImageData.h"

vtkStandardNewMacro(vtkImageFromBoundsSource);

//----------------------------------------------------------------------------
vtkImageFromBoundsSource::vtkImageFromBoundsSource()
{
    this->OutputImageSize[0] = 128;
    this->OutputImageSize[1] = 128;
    this->OutputImageSize[2] = 128;

    this->OutputImageOrigin[0] = 0.0;
    this->OutputImageOrigin[1] = 0.0;
    this->OutputImageOrigin[2] = 0.0;

    this->OutputImageSpacing[0] = 1.0;
    this->OutputImageSpacing[1] = 1.0;
    this->OutputImageSpacing[2] = 1.0;

    this->OutputImageBounds[0] =  0.0;
    this->OutputImageBounds[1] = -1.0;
    this->OutputImageBounds[2] =  0.0;
    this->OutputImageBounds[3] = -1.0;
    this->OutputImageBounds[4] =  0.0;
    this->OutputImageBounds[5] = -1.0;

    this->SpacingSet = 0;
    this->OriginSet = 0;
    this->ImageBoundsSet = 0;

    this->OutValue = 0.0;
    this->OutValueSet =0;

    this->OutputScalarType = VTK_UNSIGNED_CHAR;
    this->SetNumberOfInputPorts(0);
}

//----------------------------------------------------------------------------
vtkImageFromBoundsSource::~vtkImageFromBoundsSource()
{
}

//----------------------------------------------------------------------------
void vtkImageFromBoundsSource::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);
    os << indent << "Image Bounds: ("
       << this->OutputImageBounds[1] << ", " << this->OutputImageBounds[2]
       << this->OutputImageBounds[1] << ", " << this->OutputImageBounds[2]
       << this->OutputImageBounds[1] << ", " << this->OutputImageBounds[2] << ")\n";
    
    os << indent << "OutValue: " << this->OutValue << "\n";
    os << indent << "OutputScalarType: " << this->OutputScalarType << "\n";

    os << indent << "Output Origin: ("
       << this->OutputImageOrigin[0] << ", "
       << this->OutputImageOrigin[1] << ", "
       << this->OutputImageOrigin[2] << ")\n";
}
//----------------------------------------------------------------------------
void vtkImageFromBoundsSource::SetOutputImageBounds(const double bounds[6])
{
    int idx;

    for (idx = 0; idx < 6; ++idx)
    {
        if (this->OutputImageBounds[idx] != bounds[idx])
        {
            this->OutputImageBounds[idx] = bounds[idx];
            this->Modified();
        }
    }
    this->ImageBoundsSet = 1;
}

//----------------------------------------------------------------------------
void vtkImageFromBoundsSource::SetOutputImageBounds(double minX, double maxX,
                                                    double minY, double maxY,
                                                    double minZ, double maxZ)
{
    double bounds[6];

    bounds[0] = minX;  bounds[1] = maxX;
    bounds[2] = minY;  bounds[3] = maxY;
    bounds[4] = minZ;  bounds[5] = maxZ;
    this->SetOutputImageBounds(bounds);
}


//----------------------------------------------------------------------------
void vtkImageFromBoundsSource::GetOutputImageBounds(double bounds[6])
{
    int idx;

    for (idx = 0; idx < 6; ++idx)
    {
        bounds[idx] = this->OutputImageBounds[idx];
    }
}

//----------------------------------------------------------------------------
int vtkImageFromBoundsSource::RequestInformation (
    vtkInformation * vtkNotUsed(request),
    vtkInformationVector** vtkNotUsed( inputVector ),
    vtkInformationVector *outputVector)
{
    if(ImageBoundsSet)
    {
        // Use the bounds to set the image origin
        if (!this->OriginSet)
        {
            if ( this->OutputImageBounds[0] <= this->OutputImageBounds[1] &&
                 this->OutputImageBounds[2] <= this->OutputImageBounds[3] &&
                 this->OutputImageBounds[4] <= this->OutputImageBounds[5] )
            {
                this->OutputImageOrigin[0] = this->OutputImageBounds[0];
                this->OutputImageOrigin[1] = this->OutputImageBounds[2];
                this->OutputImageOrigin[2] = this->OutputImageBounds[4];
            }
        }

        // If spacing has not been set by the user, estimate it.
        if (!this->SpacingSet)
        {
            for (unsigned int i=0; i<3; i++)
                this->OutputImageSpacing[i] = (this->OutputImageBounds[2*i+1] - this->OutputImageBounds[2*i])/(double)this->OutputImageSize[i];
        }

        // get the info objects
        vtkInformation* outInfo = outputVector->GetInformationObject(0);

        outInfo->Set(vtkDataObject::ORIGIN(), this->OutputImageOrigin, 3);
        outInfo->Set(vtkDataObject::SPACING(),this->OutputImageSpacing,3);
        outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),
                     0, this->OutputImageSize[0] - 1,
                     0, this->OutputImageSize[1] - 1,
                     0, this->OutputImageSize[2] - 1);

        vtkDataObject::SetPointDataActiveScalarInfo(outInfo, this->OutputScalarType, -1);
        return 1;
    }
    return 0;
}

template <class T>
void vtkImageFromBoundsSourceExecute(vtkImageFromBoundsSource *self,
                                     vtkImageData *data, int ext[6], T *ptr)
{
    int min0, max0;
    int idx0, idx1, idx2;
    vtkIdType inc0, inc1, inc2;
    T outVal;
    unsigned long count = 0;
    unsigned long target;

    outVal = static_cast<T>(self->GetOutValue());

    min0 = ext[0];
    max0 = ext[1];
    data->GetContinuousIncrements(ext, inc0, inc1, inc2);

    target = static_cast<unsigned long>((ext[5]-ext[4]+1)*(ext[3]-ext[2]+1)/50.0);
    target++;

    for (idx2 = ext[4]; idx2 <= ext[5]; ++idx2)
    {
        for (idx1 = ext[2]; !self->AbortExecute && idx1 <= ext[3]; ++idx1)
        {
            if (!(count%target))
            {
                self->UpdateProgress(count/(50.0*target));
            }
            count++;

            for (idx0 = min0; idx0 <= max0; ++idx0)
            {
                *ptr = outVal;
                ++ptr;
                // inc0 is 0
            }
            ptr += inc1;
        }
        ptr += inc2;
    }
}

//----------------------------------------------------------------------------
int vtkImageFromBoundsSource::RequestData(
    vtkInformation *vtkNotUsed(request),
    vtkInformationVector ** vtkNotUsed( inputVector ),
    vtkInformationVector *outputVector)
{
    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    vtkImageData *data = vtkImageData::SafeDownCast(
                outInfo->Get(vtkDataObject::DATA_OBJECT()));


    if(this->OutValueSet)

    {
        int extent[6];

        outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),extent);

        data->SetExtent(extent);
        data->AllocateScalars();

        void *ptr;
        ptr = data->GetScalarPointerForExtent(extent);

        switch (data->GetScalarType())
        {
        vtkTemplateMacro(
                    vtkImageFromBoundsSourceExecute(this, data, extent,
                                                    static_cast<VTK_TT *>(ptr)));
        default:
            vtkErrorMacro("Execute: Unknown output ScalarType");
        }
    }
    else
    {
        // just allocate data !
        data->SetExtent(
                    outInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()));
        data->AllocateScalars();
    }

    return 1;
}
