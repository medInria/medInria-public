/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include <itkMultiThreadedImageIOBase.h>
#include <itkMetaDataObject.h>

#include <medImageIOExport.h>

#include <map>
#include <vector>
#include <set>
#include <functional>


class DcmElement;

class double_fuzzy_less : public std::binary_function<double,double,bool>
{
public:
    double_fuzzy_less( double arg_ = 1e-7 ) : epsilon(arg_) {}
    bool operator()( const double &left, const double &right  ) const
    {
        return (fabs(left - right) > epsilon) && (left < right);
    }
    double epsilon;
};


namespace itk //TODO no namespace please ! (and no class name prifixed with other librarie pefix)
{

class MEDIMAGEIO_EXPORT DCMTKImageIO : public MultiThreadedImageIOBase
{

public:
    typedef DCMTKImageIO             Self;
    typedef MultiThreadedImageIOBase Superclass;
    typedef SmartPointer<Self>       Pointer;
    typedef SmartPointer<const Self> ConstPointer;

    itkNewMacro (Self);
    itkTypeMacro(DCMTKImageIO, MultiThreadedImageIOBase);

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
    
    bool CanReadFile(const char*);
    void ReadImageInformation();

    bool CanWriteFile(const char*);
    void WriteImageInformation();
    void Write(const void* buffer);

    // DICOM related stuff
    std::string GetPatientName() const;
    std::string GetPatientID() const;
    std::string GetPatientSex() const;
    std::string GetPatientAge() const;
    std::string GetStudyID() const;
    std::string GetPatientDOB() const;
    std::string GetStudyDescription() const;
    std::string GetSeriesDescription() const;
    std::string GetBodyPart() const;
    std::string GetNumberOfSeriesInStudy() const;
    std::string GetNumberOfStudyRelatedSeries() const;
    std::string GetStudyDate() const;
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
    std::string GetSeriesID() const;
    std::string GetOrientation() const;
    std::string GetSeriesNumber() const;
    std::string GetSequenceName() const;
    std::string GetSliceThickness() const;
    std::string GetRows() const;
    std::string GetColumns() const;
    std::string GetFrameTime() const;
    
    // US Images (VP2HF Project)
    std::string GetNumberOfFrames() const;
    std::string GetRegionLocation() const;
    std::string GetPhysicalDelta() const;
    
    
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
    void PrintSelf(std::ostream& os, Indent indent) const;

    void ThreadedRead (void* buffer, RegionType region, int threadId);
    void InternalRead (void* buffer, int slice, unsigned long pixelCount);

    void SwapBytesIfNecessary(void* buffer, unsigned long numberOfPixels);

    void DetermineNumberOfPixelComponents();
    void DeterminePixelType();
    void DetermineSpacing();
    void DetermineDimensions();
    void DetermineOrigin();
    void DetermineOrientation();

    double GetZPositionForImage (int);
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
