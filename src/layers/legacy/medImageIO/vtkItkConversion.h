#pragma once
/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2018. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/
#include <medImageIOExport.h>

#include <itkImage.h>
#include <itkImageToVTKImageFilter.h>
#include <itkExtractImageFilter.h>

#include <vtkAlgorithmOutput.h>
#include <vtkMatrix4x4.h>

#include <vector>

class medAbstractData;

// ///////////////////////////////////////////////////////////////////////////
// Interface for vtkItkConversion different class
/**
* @class vtkItkConversionInterface "Interface class for itk to vtk converter" ServiceDescr.h
* @brief Interface for itk to vtk converter. It is the base of template class vtkItkConversion.
* @detail the static function createInstance is here to provide an object to match with this interface.
*/
class MEDIMAGEIO_EXPORT vtkItkConversionInterface
{
public:
    virtual ~vtkItkConversionInterface() {};

    virtual bool SetITKInput(itk::DataObject::Pointer pi_input) = 0;
    virtual bool GetConversion(vtkAlgorithmOutput *& po_poAlgoOut, vtkMatrix4x4  *&po_poMatrix) = 0;

    virtual bool setTimeIndex(unsigned int pi_uiTimeIndex) = 0;

    virtual unsigned int getTimeIndex() = 0;
    virtual unsigned int getNumberOfVolumes() = 0;
    virtual float getTotalTime() = 0;

    virtual double * getCurrentScalarRange() = 0;

    static vtkItkConversionInterface * createInstance(medAbstractData* pi_poData);

protected:
    vtkItkConversionInterface() {};
};


// ///////////////////////////////////////////////////////////////////////////
// Template for concrete implementation of vtkItkConversionInterface
/**
* @class vtkItkConversion "Template class for itk to vtk converter" ServiceDescr.h
* @brief Template for itk to vtk converter. It inherits vtkItkConversionInterface.
* @detail It keep inside the ITK input image and provide for output an vtk port.<br>The hearth of this class is a itk::ImageToVTKImageFilter
*/
template <typename volumeType, unsigned int imageDim>
class vtkItkConversion final : public vtkItkConversionInterface
{
    typedef typename itk::Image<volumeType, 4> Image4DType;
    typedef typename itk::Image<volumeType, 3> Image3DType;
    typedef itk::ImageToVTKImageFilter< itk::Image<volumeType, 3> > ConverterType;

private:
    // ///////////////////////////////////////////////////////////////////////
    // 3D
    typename ConverterType::Pointer m_ImageConverter; /*!<Keep ITK->VTK converter inside, to prevent the image buffer to be deleted unexpectedly. */
    typename itk::Image<volumeType, 3>::Pointer m_ItkInputImage; /*!<Keep 3D ITK input image or current used extracted volume from 4D ITK image. */

    // ///////////////////////////////////////////////////////////////////////
    // 4D
    typename itk::Image<volumeType, 4>::Pointer m_ItkInputImage4D;  /*!<Keep 4D ITK input image. */
    std::vector<typename itk::Image<volumeType, 3>::Pointer> m_oVolumeVectorFrom4D; /*!<Internal vector for each 3D ITK volume extracted from 4D ITK input image. */
    unsigned int m_uiCurrentTimeIndex; /*!<Keep current index of the time line. */
    unsigned int m_uiNbVolume; /*!<Number of extracted volume. */
    float m_fTotalTime; /*!<Time line in second. */

public:
    vtkItkConversion();
    virtual ~vtkItkConversion();
    
    // ///////////////////////////////////////////////////////////////////////
    // Mains functions
    virtual bool SetITKInput(typename itk::DataObject::Pointer input);

    virtual bool GetConversion(vtkAlgorithmOutput *& po_poAlgoOut, vtkMatrix4x4  *&po_poMatrix);

    // ///////////////////////////////////////////////////////////////////////
    // Getters and Setters functions
    virtual bool setTimeIndex(unsigned int pi_uiTimeIndex);

    virtual unsigned int getTimeIndex();
    virtual unsigned int getNumberOfVolumes();
    virtual float getTotalTime();

    virtual double * getCurrentScalarRange();

private:
    bool initializeImage(typename itk::ImageBase<imageDim>::Pointer &input);
    bool volumeExtraction();
    void conversion();
};

#include <vtkItkConversion.tpp>