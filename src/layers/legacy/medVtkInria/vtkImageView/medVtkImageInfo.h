#pragma once

struct medVtkImageInfo
{
    int    dimensions[3];
    int    extent[6];
    double scalarRange[2];
    double spacing[3];
    double origin[3];
    int    nbScalarComponent;
    int    scalarType;
    bool   initialized;
    
    medVtkImageInfo();
    ~medVtkImageInfo();
    
    void GetScalarRange (double po_pdScalarRange[2]);
    void GetExtent      (int po_piExtent[6]);
    void GetSpacing     (double po_pdSpacing[3]);
    void GetOrigin      (double po_pdOrigin[3]);
    void GetDimensions  (int po_piDimensions[3]);
    
    void SetScalarRange (double pi_pdScalarRange[2]);
    void SetExtent      (int pi_piExtent[6]);
    void SetSpacing     (double pi_pdSpacing[3]);
    void SetOrigin      (double pi_pdOrigin[3]);
    void SetDimensions (int pi_piDimensions[3]);
};