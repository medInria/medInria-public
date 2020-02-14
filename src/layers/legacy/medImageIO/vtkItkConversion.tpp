/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2018. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

template <typename volumeType, unsigned int imageDim>
vtkItkConversion<volumeType, imageDim>::vtkItkConversion() :m_ImageConverter(ConverterType::New()), m_uiCurrentTimeIndex(0), m_uiNbVolume(0), m_fTotalTime(0) {}

template <typename volumeType, unsigned int imageDim>
vtkItkConversion<volumeType, imageDim>::~vtkItkConversion() {}

/**
* @brief  This function set the input itk::image casted into itk::DataOject.
* @param  pi_poData [in] must be the itk::image to convert into vtk world.
* @return True if the inpute image is well take into account.<br>False if the type of image or data is not managed by this convertor.
* @todo manage a null pointer / empty smart pointer in input.
* @callgraph
*/
template <typename volumeType, unsigned int imageDim>
bool vtkItkConversion<volumeType, imageDim>::SetITKInput(typename itk::DataObject::Pointer pi_input)
{
    typename itk::ImageBase<imageDim>::Pointer inputImage = dynamic_cast<  itk::ImageBase<imageDim>*> (pi_input.GetPointer());
    
    bool bRes = inputImage.IsNotNull();
   
    bRes = initializeImage(inputImage);

    if (bRes && imageDim == 4)
    {
        bRes = volumeExtraction();
    }
    if (bRes)
    {
        conversion();
    }

    return bRes;
}

/**
* @brief  This function provied an acces to the results of the convertor.
* @details The origin in ITK pipeline is taken into account in a different way than in the VTK equivalent.
*          A first hack would be to force the vtkImageData instance to have a null origin, and put the ITK origin in the 4th column of the OrientationMatrix instance.
*          BUT, when the ITK pipeline is updated, then the ITK origin is put back in place in the vtkImageData instance.
*
*          Therefore we need to keep the vtkImageData's origin the same as the ITK one.
*          And, we need to correct for this misbehavior through a hack in the OrientationMatrix 4th column, a sort of corrected origin.
* @param  po_poAlgoOut [out] is a pointer on an VTK output port.
* @param  po_poMatrix [out] is a pointer, dynamiclly allocated, on a well computed orientation matrix (Caller must deallocate it after use).
* @return True if a conversion is done.<br>False in other cases.
* @callgraph
*/
template <typename volumeType, unsigned int imageDim>
bool vtkItkConversion<volumeType, imageDim>::GetConversion(vtkAlgorithmOutput *& po_poAlgoOut, vtkMatrix4x4 *&po_poMatrix)
{
    bool bRes = true;

    po_poAlgoOut = nullptr;
    po_poMatrix = nullptr;

    if (m_ItkInputImage.IsNotNull())
    {

        //////////////////////////////////////////////////////////////////////////
        // Copy the output port
        po_poAlgoOut = m_ImageConverter->GetImporter()->GetOutputPort();

        //////////////////////////////////////////////////////////////////////////
        // Orientation matrix correction phase
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

/**
* @brief  This internal function register into the good variable member the input image.
* @param  pi_spInputImage [in] smart pointer on the itk input image.
* @return True if succeed. False in other cases.
*/
template <typename volumeType, unsigned int imageDim>
bool vtkItkConversion<volumeType, imageDim>::initializeImage(typename itk::ImageBase<imageDim>::Pointer &pi_spInputImage)
{
    bool bRes = false;

    if (pi_spInputImage.IsNotNull())
    {
        switch (imageDim)
        {
            case 3:        
            {
                m_ItkInputImage = reinterpret_cast<itk::Image<volumeType, 3>*>(pi_spInputImage.GetPointer());
                bRes = m_ItkInputImage.IsNotNull();
                break;
            }
            case 4:
            {
                m_ItkInputImage4D = reinterpret_cast<itk::Image<volumeType, 4>*>(pi_spInputImage.GetPointer());
                bRes = m_ItkInputImage4D.IsNotNull();
                break;
            }
            default:
                break;
        }
        bRes ? m_uiNbVolume = 1 : m_uiNbVolume = 0;
    }

    return bRes;
}

/**
* @brief  This internal function process the extraction of diffrents volume from 4D image input.
* @return True if succed. False in other cases.
*/
template <typename volumeType, unsigned int imageDim>
bool vtkItkConversion<volumeType, imageDim>::volumeExtraction()
{
    bool bRes = true;

    auto size = m_ItkInputImage4D->GetLargestPossibleRegion().GetSize();
    m_uiNbVolume = size[3];
    m_oVolumeVectorFrom4D.reserve(m_uiNbVolume);

    // split the 4D volume into 3D volumes
    typename Image4DType::RegionType regionToExtract = m_ItkInputImage4D->GetLargestPossibleRegion();
    typename Image4DType::IndexType index;
    index[0] = 0;
    index[1] = 0;
    index[2] = 0;
    index[3] = 0;
    size[3] = 0;

    regionToExtract.SetSize(size);
    regionToExtract.SetIndex(index);

    if (m_uiNbVolume > 0)
    {
        double dTimeResolution = m_ItkInputImage4D->GetSpacing()[3];
        m_fTotalTime = dTimeResolution * (m_uiNbVolume-1);
        m_oVolumeVectorFrom4D.resize(m_uiNbVolume);

        for (unsigned int n = 0; (n < m_uiNbVolume) && bRes; n++)
        {
            typedef typename itk::ExtractImageFilter<Image4DType, Image3DType> ExtractImageType;
            regionToExtract.SetIndex(3, n);
            typename ExtractImageType::Pointer myExtractor = ExtractImageType::New();
            myExtractor->SetExtractionRegion(regionToExtract);
            myExtractor->SetDirectionCollapseToGuess();
            myExtractor->SetInput(m_ItkInputImage4D);

            try
            {
                myExtractor->Update();
            }
            catch (itk::ExceptionObject &e)
            {
                bRes = false;
                std::cerr << "no volume into 4D image with exception : " << e << std::endl;
            }

            try
            {
                myExtractor->GetOutput();
                m_oVolumeVectorFrom4D[n] = myExtractor->GetOutput();
            }
            catch (itk::ExceptionObject &e)
            {
                bRes = false;
                std::cerr<< "error when extracting volume from 4D itk image on volume : " << n << " \n with exception" << e << std::endl;
            }
        }

        m_ItkInputImage = m_oVolumeVectorFrom4D[0];
    }
    else
    {
        bRes = false;
    }

    return bRes;

}

/**
* @brief  This function change the current volume of 4D image.
* @param  pi_uiTimeIndex [in] cardinal number of extracter volume (0..N-1).
* @return True if succed. False in other cases.
*/
template <typename volumeType, unsigned int imageDim>
bool vtkItkConversion<volumeType, imageDim>::setTimeIndex(unsigned int pi_uiTimeIndex)
{
    bool bRes = true;

    if (pi_uiTimeIndex != m_uiCurrentTimeIndex)
    {
        if (pi_uiTimeIndex < m_uiNbVolume)
        {
            m_ItkInputImage = m_oVolumeVectorFrom4D[pi_uiTimeIndex];
            conversion();
            m_uiCurrentTimeIndex = pi_uiTimeIndex;
        }
        else
        {
            bRes = false;
        }
    }

    return bRes;
}

/**
* @brief  This function get the current volume number of 4D image.
* @return The current volume number of 4D image or 0 if no input image or simple 3D image.
*/
template <typename volumeType, unsigned int imageDim>
unsigned int vtkItkConversion<volumeType, imageDim>::getTimeIndex()
{
    return m_uiCurrentTimeIndex;
}

/**
* @brief  This function get number of volumes.
* @return The current volume number of 4D image or 0 if no input image or 1 for a simple 3D image.
*/
template <typename volumeType, unsigned int imageDim>
unsigned int vtkItkConversion<volumeType, imageDim>::getNumberOfVolumes()
{
    return m_uiNbVolume;
}

/**
* @brief  This function get the total time of 4D image.
* @return The total time of 4D image or 0 in other cases.
*/
template <typename volumeType, unsigned int imageDim>
float vtkItkConversion<volumeType, imageDim>::getTotalTime()
{
    return m_fTotalTime;
}

/**
* @brief  This internal function process the conversion.
*/
template <typename volumeType, unsigned int imageDim>
void vtkItkConversion<volumeType, imageDim>::conversion()
{
    //////////////////////////////////////////////////////////////////////////
    // Initialization phase
    m_ImageConverter->SetInput(m_ItkInputImage);

    //////////////////////////////////////////////////////////////////////////
    // Update phase
    m_ImageConverter->UpdateOutputInformation();
    m_ImageConverter->GetImporter()->UpdateInformation();
    m_ImageConverter->GetImporter()->Update();
}

/**
* @brief  This function get the total time of 4D image.
* @return The a table of 2 cases with the scalar range (compute with vtk method).
*/
template <typename volumeType, unsigned int imageDim>
double * vtkItkConversion<volumeType, imageDim>::getCurrentScalarRange()
{
    double *dResScalarRange = nullptr;
    double scalarRangeTmp[2];
    unsigned int uiPreviousTimeIndex = m_uiCurrentTimeIndex;

    if (m_ItkInputImage.IsNotNull() || m_ItkInputImage4D.IsNotNull())
    {
        dResScalarRange = new double[2]; //FloTODO check potential memory leak
        dResScalarRange[0] = VTK_DOUBLE_MAX;
        dResScalarRange[1] = VTK_DOUBLE_MIN;

        for (unsigned int i = 0; i < m_uiNbVolume; ++i)
        {
            setTimeIndex(i);
            m_ImageConverter->GetImporter()->GetOutput()->GetScalarRange(scalarRangeTmp);
            if (dResScalarRange[0] > scalarRangeTmp[0]) dResScalarRange[0] = scalarRangeTmp[0];
            if (dResScalarRange[1] < scalarRangeTmp[1]) dResScalarRange[1] = scalarRangeTmp[1];
        }
    }

    setTimeIndex(uiPreviousTimeIndex);

    return dResScalarRange;
}
