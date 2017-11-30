#include "medVtkImageInfo.h"
#include <string.h>


medVtkImageInfo::medVtkImageInfo()
{
    memset(dimensions, 0, sizeof(dimensions));
    memset(spacing, 0, sizeof(spacing));
    memset(origin, 0, sizeof(origin));
    memset(extent, 0, sizeof(extent));
    memset(scalarRange, 0, sizeof(scalarRange));
    nbScalarComponent = 0;
    initialized = false;
}

medVtkImageInfo::~medVtkImageInfo()
{
    memset(dimensions, 0, sizeof(dimensions));
    memset(spacing, 0, sizeof(spacing));
    memset(origin, 0, sizeof(origin));
    memset(extent, 0, sizeof(extent));
    memset(scalarRange, 0, sizeof(scalarRange));
    nbScalarComponent = 0;
    initialized = false;
}

void medVtkImageInfo::SetScalarRange(double pi_pdScalarRange[2])
{
    memcpy(scalarRange, pi_pdScalarRange, sizeof(double) * 2);
}

void medVtkImageInfo::SetExtent(int pi_piExtent[6])
{
    memcpy(extent, pi_piExtent, sizeof(int) * 6);
}

void medVtkImageInfo::SetDimensions(int pi_piDimensions[3])
{
     memcpy(dimensions, pi_piDimensions, sizeof(int) * 3);
}

void medVtkImageInfo::SetSpacing(double pi_pdSpacing[3])
{
    memcpy(spacing, pi_pdSpacing, sizeof(double) * 3);
}

void medVtkImageInfo::SetOrigin(double pi_pdOrigin[3])
{
    memcpy(origin, pi_pdOrigin, sizeof(double) * 3);
}

void medVtkImageInfo::GetScalarRange(double po_pdScalarRange[2])
{
    memcpy(po_pdScalarRange, scalarRange, sizeof(double) * 2);
}

void medVtkImageInfo::GetSpacing(double po_pdSpacing[3])
{
    memcpy(po_pdSpacing, spacing, sizeof(double) * 3);
}

void medVtkImageInfo::GetOrigin(double po_pdOrigin[3])
{
    memcpy(po_pdOrigin, origin, sizeof(double) * 3);
}

void medVtkImageInfo::GetExtent(int po_piExtent[6])
{
    memcpy(po_piExtent, extent, sizeof(int) * 6);
}

void medVtkImageInfo::GetDimensions(int po_piDimensions[3])
{
    memcpy(po_piDimensions, dimensions, sizeof(int) * 3);
}
