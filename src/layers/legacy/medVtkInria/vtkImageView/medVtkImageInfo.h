#pragma once
#include <string.h>

class medVtkImageInfo
{
private:

public:
    unsigned int    nbDim;
    int    *dimensions;
    int    extent[6];
    double scalarRange[2];
    double *spacing;
    double *origin;
    int    nbScalarComponent;
    int    scalarType;
    bool   initialized;
    
    medVtkImageInfo();
    medVtkImageInfo(medVtkImageInfo &&pi_rModel);
    ~medVtkImageInfo();
    
    void GetScalarRange (double po_pdScalarRange[2]);
    void GetExtent      (int po_piExtent[6]);
    void GetSpacing     (double *po_pdSpacing);
    void GetOrigin      (double *po_pdOrigin);
    void GetDimensions  (int *po_piDimensions);

    void SetScalarRange (double pi_pdScalarRange[2]);
    void SetExtent      (int pi_piExtent[6]);
    void SetSpacing     (double *pi_pdSpacing, unsigned int pi_uiNbDimensions);
    void SetOrigin      (double *pi_pdOrigin, unsigned int pi_uiNbDimensions);
    void SetDimensions  (int *pi_piDimensions, unsigned int pi_uiNbDimensions);

    unsigned int GetDataDimensions();

};

inline medVtkImageInfo::medVtkImageInfo()
{
    nbDim = 0;
    dimensions = nullptr;
    nbScalarComponent = 0;
    memset(extent, 0, sizeof(extent));
    memset(scalarRange, 0, sizeof(scalarRange));
    spacing = nullptr;
    origin = nullptr;
    initialized = false;
}


inline medVtkImageInfo::medVtkImageInfo(medVtkImageInfo &&pi_rModel)
{
    nbDim = pi_rModel.nbDim;
    dimensions = nullptr;
    nbScalarComponent = 0;
    memset(extent, 0, sizeof(extent));
    memset(scalarRange, 0, sizeof(scalarRange));
    spacing = nullptr;
    origin = nullptr;
    (*((int*)0))++;
}

inline medVtkImageInfo::~medVtkImageInfo()
{
    if (dimensions)
    {
        delete[](dimensions);
    }
    if (spacing)
    {
        delete[](spacing);
    }
    if (origin)
    {
        delete[](origin);
    }
}

inline void medVtkImageInfo::SetScalarRange(double pi_pdScalarRange[2])
{
    memcpy(scalarRange, pi_pdScalarRange, sizeof(double) * 2);
}

inline void medVtkImageInfo::SetExtent(int pi_piExtent[6])
{
    memcpy(extent, pi_piExtent, sizeof(int) * 6);
}

inline void medVtkImageInfo::SetDimensions(int *pi_piDimensions, unsigned int pi_uiNbDimensions)
{
    if ((nbDim==0 || nbDim ==pi_uiNbDimensions) && pi_piDimensions )
    {
        if (!dimensions)
        {
            nbDim = pi_uiNbDimensions;
            dimensions = new int[nbDim];
        }
        memcpy(dimensions, pi_piDimensions, sizeof(int) * nbDim);
    }
}

inline void medVtkImageInfo::SetSpacing(double* pi_pdSpacing, unsigned int pi_uiNbDimensions)
{
    if ((nbDim == 0 || nbDim == pi_uiNbDimensions) && pi_pdSpacing)
    {
        if (!spacing)
        {
            nbDim = pi_uiNbDimensions;
            spacing = new double[nbDim];
        }
        memcpy(spacing, pi_pdSpacing, sizeof(double) * nbDim);
    }
}

inline void medVtkImageInfo::SetOrigin(double *pi_pdOrigin, unsigned int pi_uiNbDimensions)
{
    if ((nbDim == 0 || nbDim == pi_uiNbDimensions) && pi_pdOrigin)
    {
        if (!origin)
        {
            nbDim = pi_uiNbDimensions;
            origin = new double[nbDim];
        }
        memcpy(origin, pi_pdOrigin, sizeof(double) * nbDim);
    }
}



inline void medVtkImageInfo::GetScalarRange(double po_pdScalarRange[2])
{
    memcpy(po_pdScalarRange, scalarRange, sizeof(double) * 2);
}

inline void medVtkImageInfo::GetSpacing(double *po_pdSpacing)
{
    memcpy(po_pdSpacing, spacing, sizeof(double) * nbDim);
}

inline void medVtkImageInfo::GetOrigin(double *po_pdOrigin)
{
    memcpy(po_pdOrigin, origin, sizeof(double) * nbDim);
}

inline void medVtkImageInfo::GetExtent(int po_piExtent[6])
{
    memcpy(po_piExtent, extent, sizeof(int) * 6);
}

inline void medVtkImageInfo::GetDimensions(int *po_piDimensions)
{
    memcpy(po_piDimensions, dimensions, sizeof(int) * nbDim);
}

inline unsigned int medVtkImageInfo::GetDataDimensions()
{
    return nbDim;
}
