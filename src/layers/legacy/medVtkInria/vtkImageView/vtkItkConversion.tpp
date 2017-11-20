
template <typename volumeType, unsigned int imageDim>
vtkItkConversion<volumeType, imageDim>::vtkItkConversion() :m_ImageConverter(ConverterType::New()), m_uiCurrentTimeIndex(0), m_uiNbVolume(0), m_fTotalTime(0) {}

template <typename volumeType, unsigned int imageDim>
vtkItkConversion<volumeType, imageDim>::~vtkItkConversion() {}

template <typename volumeType, unsigned int imageDim>
bool vtkItkConversion<volumeType, imageDim>::SetITKInput(typename itk::DataObject::Pointer input)
{
    typename itk::ImageBase<imageDim>::Pointer inputImage = dynamic_cast<  itk::ImageBase<imageDim>*> (input.GetPointer());
    
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
/*
template <typename volumeType, unsigned int imageDim>
bool vtkItkConversion<volumeType, imageDim>::SetITKInput4D(itk::ImageBase<4>::Pointer input)
{
    bool bRes = initializeImage4D(input);

    if (bRes)
    {
        bRes = volumeExtraction();
        if (bRes)
        {
            conversion();
        }
    }

    return bRes;
}*/

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
        /**
        The origin in ITK pipeline is taken into account in a different way than
        in the VTK equivalent.
        A first hack would be to force the vtkImageData instance to have
        a null origin, and put the ITK origin in the 4th column of the
        OrientationMatrix instance. BUT, when the ITK pipeline is updated,
        then the ITK origin is put back in place in the vtkImageData instance.

        Therefore we need to keep the vtkImageData's origin the same as the
        ITK one. And, we need to correct for this misbehavior through a hack
        in the OrientationMatrix 4th column, a sort of corrected origin.
        */
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


template <typename volumeType, unsigned int imageDim>
bool vtkItkConversion<volumeType, imageDim>::initializeImage(typename itk::ImageBase<imageDim>::Pointer &input)
{
    bool bRes = false;

    if (input.IsNotNull())
    {
        switch (imageDim)
        {
            case 3:        
            {
                m_ItkInputImage = reinterpret_cast<itk::Image<typename volumeType, 3>*>(input.GetPointer());
                bRes = m_ItkInputImage.IsNotNull();
                break;
            }
            case 4:
            {
                m_ItkInputImage4D = reinterpret_cast<itk::Image<typename volumeType, 4>*>(input.GetPointer());
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
        m_fTotalTime = dTimeResolution * m_uiNbVolume;

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
                //FloTODO Redo throw vtkErrorCode::UserError; //vtkErrorMacro(<< "no volume into 4D image with exception : " << e << endl);
            }

            try
            {
                m_oVolumeVectorFrom4D[n] = myExtractor->GetOutput();
            }
            catch (itk::ExceptionObject &e)
            {
                bRes = false;
                // vtkErrorMacro(<< "error when extracting volume from 4D itk image on volume : " << n << " \n with exception" << e << endl);
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
        }
        else
        {
            bRes = false;
        }
    }

    return bRes;
}

template <typename volumeType, unsigned int imageDim>
unsigned int vtkItkConversion<volumeType, imageDim>::getTimeIndex()
{
    return m_uiCurrentTimeIndex;
}

template <typename volumeType, unsigned int imageDim>
unsigned int vtkItkConversion<volumeType, imageDim>::getNumberOfVolumes()
{
    return m_uiNbVolume;
}

template <typename volumeType, unsigned int imageDim>
float vtkItkConversion<volumeType, imageDim>::getTotalTime()
{
    return m_fTotalTime;
}
/*
template <typename volumeType, unsigned int imageDim>
bool vtkItkConversion<volumeType, imageDim>::initializeImage3D(itk::ImageBase<3>::Pointer &input)
{
    bool bRes = false;

    if (input.IsNotNull())
    {
        m_ItkInputImage = static_cast<itk::Image<typename volumeType, 3>*>(input.GetPointer());
        bRes = m_ItkInputImage.IsNotNull();
        bRes ? m_uiNbVolume = 1 : m_uiNbVolume = 0;
    }

    return bRes;
}

template <typename volumeType, unsigned int imageDim>
bool vtkItkConversion<volumeType, imageDim>::initializeImage4D(itk::ImageBase<4>::Pointer &input)
{
    bool bRes = false;

    if (input.IsNotNull())
    {
        m_ItkInputImage4D = static_cast<itk::Image<typename volumeType, 4>*>(input.GetPointer());
        bRes = m_ItkInputImage4D.IsNotNull();
        bRes ? m_uiNbVolume = 1 : m_uiNbVolume = 0;
    }

    return bRes;
}
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
