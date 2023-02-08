#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include <itkMultiThreadedImageIOBase.h>
#include <itkMetaDataObject.h>

#include <medImageIOExport.h>

#include <iterator>
#include <map>
#include <vector>
#include <set>
#include <functional>


class DcmElement;

class double_fuzzy_less
{
public:
    double_fuzzy_less( double arg_ = 1e-7 ) : epsilon(arg_) {}
    bool operator()( const double &left, const double &right  ) const
    {
        return (fabs(left - right) > epsilon) && (left < right);
    }
    double epsilon;
};


namespace itk
{

class MEDIMAGEIO_EXPORT DCMTKImageIO : public MultiThreadedImageIOBase
{

public:
    typedef DCMTKImageIO             Self;
    typedef MultiThreadedImageIOBase Superclass;
    typedef SmartPointer<Self>       Pointer;
    typedef SmartPointer<const Self> ConstPointer;

    itkNewMacro (Self)
    itkTypeMacro(DCMTKImageIO, MultiThreadedImageIOBase)

    typedef std::map< std::string, std::vector< std::string > >     StringMap;

    typedef MetaDataObject <std::string>                            MetaDataStringType;
    typedef MetaDataObject <std::vector<std::string> >              MetaDataVectorStringType;

    typedef MultiThreadedImageIOBase::RegionType                    RegionType;

    typedef std::vector<std::string>                                StringVectorType;

    typedef std::map<std::string, int>                              NameToIndexMapType;
    typedef std::map<int, std::list<std::string> >                  IndexToNamesMapType;
    typedef std::set< double >                                      SliceLocationSetType;
    typedef std::set< std::string >                                 NameSetType;
    typedef std::multimap< double, std::string, double_fuzzy_less > SliceLocationToNamesMultiMapType;

    static double MAXIMUM_GAP;

    bool CanReadFile(const char*)  override;
    void ReadImageInformation()    override;

    bool CanWriteFile(const char*) override;
    void WriteImageInformation()   override;
    void Write(const void* buffer) override;

    // DICOM related stuff
    std::string GetPatientName() const;
    std::string GetPatientID() const;
    std::string GetPatientSex() const;
    std::string GetPatientAge() const;
    std::string GetStudyID() const;
    std::string GetStudyInstanceUID() const;
    std::string GetPatientDOB() const;
    std::string GetStudyDescription() const;
    std::string GetSeriesDescription() const;
    std::string GetBodyPart() const;
    std::string GetNumberOfSeriesInStudy() const;
    std::string GetNumberOfStudyRelatedSeries() const;
    std::string GetStudyDate() const;
    std::string GetStudyTime() const;
    std::string GetSeriesDate() const;
    std::string GetSeriesTime() const;
    std::string GetModality() const;
    std::string GetManufacturer() const;
    std::string GetInstitution() const;
    std::string GetModel() const;
    std::string GetScanOptions() const;
    std::string GetAcquisitionDate() const;
    std::string GetReferringPhysicianName () const;
    std::string GetPerformingPhysicianName () const;
    std::string GetProtocolName() const;
    std::string GetAcquisitionComments() const;
    std::string GetPatientStatus() const;

    // new
    std::string GetSeriesInstanceUID() const;
    std::string GetOrientation() const;
    std::string GetSeriesNumber() const;
    std::string GetSequenceName() const;
    std::string GetSliceThickness() const;
    std::string GetRows() const;
    std::string GetColumns() const;

    // MR
    std::string GetAcquisitionTime() const;
    std::string GetFlipAngle() const;
    std::string GetEchoTime() const;
    std::string GetRepetitionTime() const;

    const StringVectorType& GetOrderedFileNames() const
    { return m_OrderedFileNames; }


    inline std::string GetMetaDataValueString (const char* key, int index) const
    {
        std::string value = "";
        const MetaDataDictionary& dicomDictionary = this->GetMetaDataDictionary();
        MetaDataDictionary::ConstIterator it = dicomDictionary.Find ( key );
        if( it!=dicomDictionary.End() )
        {
            if( MetaDataVectorStringType* metaData = dynamic_cast<MetaDataVectorStringType*>( it->second.GetPointer() ) )
            {
                value = metaData->GetMetaDataObjectValue()[index];
            }
        }
        return value;
    }

    inline const StringVectorType& GetMetaDataValueVectorString (const char* key) const
    {
        const MetaDataDictionary& dicomDictionary = this->GetMetaDataDictionary();
        MetaDataDictionary::ConstIterator it = dicomDictionary.Find ( key );
        if( it!=dicomDictionary.End() )
        {
            if( MetaDataVectorStringType* metaData = dynamic_cast<MetaDataVectorStringType*>( it->second.GetPointer() ) )
            {
                return metaData->GetMetaDataObjectValue();
            }
            else
            {
                return m_EmptyVector;
            }
        }
        return m_EmptyVector;
    }

protected:

    DCMTKImageIO();
    ~DCMTKImageIO();
    void PrintSelf(std::ostream& os, Indent indent) const override;

    void ThreadedRead (void* buffer, RegionType region, int threadId) override;
    void InternalRead (void* buffer, int slice, unsigned long pixelCount);

    void SwapBytesIfNecessary(void* buffer, unsigned long numberOfPixels);

    void DetermineNumberOfPixelComponents();
    void DeterminePixelType();

    struct pair_comparator
    {
        bool operator()(std::pair <unsigned int, double> &lhs, std::pair <unsigned int, double> &rhs) const
        {
            return lhs.second < rhs.second;
        }
    };

    void DetermineSpacing();
    void DetermineDimensions();
    void DetermineOrigin();
    void DetermineOrientation();

    double GetPositionOnStackingAxisForImage(int);
    double GetPositionFromPrincipalAxisIndex(int, int);
    double GetSliceLocation(std::string);

    void ReadHeader( const std::string& name, const int& fileIndex, const int& fileCount );
    inline void ReadDicomElement(DcmElement* element, const int &fileIndex, const int &fileCount );

private:
    DCMTKImageIO(const Self&);
    void operator=(const Self&);

    StringVectorType           m_OrderedFileNames;
    std::string                m_Directory;

    SliceLocationSetType             m_LocationSet;
    NameToIndexMapType               m_FilenameToIndexMap;
    SliceLocationToNamesMultiMapType m_LocationToFilenamesMap;

    StringVectorType           m_EmptyVector;
};

} // end of namespace
