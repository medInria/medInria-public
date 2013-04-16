/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <cstddef> // For ITK 3.20 that does not define correctly ptrdiff_t

#include <itkProcessObject.h>
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageSeriesReader.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkFlipImageFilter.h>
#include <gdcmFile.h>

#include "itkDicomDictionary.h"

#include <string>
#include <vector>

namespace itk
{


  
  /**
     
     \class DicomTagManager
     \brief This class can be used to manage DICOM flags, recover information from
     these flags (such as their description).
     It can contain a vector of flags, and their associated values.
     
     \author Nicolas Toussaint
     
     \see DicomDictionary GDCMSeriesFileNames ImageSeriesReader
     \ingroup   ProcessObject
     
  */
 class ITK_EXPORT DicomTagManager : public ProcessObject
  {
    
  public:
    typedef DicomTagManager Self;
    typedef ProcessObject            Superclass;
    typedef SmartPointer<Self>       Pointer;
    typedef SmartPointer<const Self> ConstPointer;

    
     
    /** generic constructors */
    itkNewMacro (Self);
    itkTypeMacro (DicomTagManager, ProcessObject);
    
    /**
       Import a flag list from \arg taglist and copy it to the manager
       Careful : this overlap the previous list if any
     */
    void ImportTagList (std::vector<itk::DicomTag> taglist);
    void ImportTagList (gdcm::File* file);
    /**
       Append a tag at the end of the current list
    */
    void AddTag (itk::DicomTag tag);
    /**
       Insert a tag at a specific location in the list
    */
    void InsertTag (unsigned int tagid, itk::DicomTag tag);
    /**
       Find a specific string (not case sensitive) in the tag list
       returns a vector containing the ids of matching tags
    */
    std::vector<unsigned int> FindString (const char* string);
    /**
       Directly access to a specific tag
    */
    itk::DicomTag GetTag (unsigned int tagid);
    /**
       Returns a tag value
    */
    const char* GetTagValue(unsigned int tagid);
    /**
       Returns true if the tag <group|element> is in the list,
       the matching tag id is returned in \arg i
    */
    bool HasTag(uint16_t group, uint16_t element, unsigned int &i);
    /**
       Set a tag value
    */
    void SetTagValue(unsigned int tagid, const char* value);
    /**
       Directly access to the entire tag list
       To be used with care
    */
    std::vector<itk::DicomTag> GetTagList();
    /**
       Initialize list, clear all tags
    */
    void ClearTags();
    /**
       Static method to access a tag from the dictionary
       \arg i has to be lower than GetDictionarySize()
    */
    static itk::DicomTag GetDictionaryTag(unsigned int i)
    {
      int firstid = 0;
      int secondid = i;
      if (i >= DicomDictionarySize_1)
      {	
	firstid = 1;
	secondid = i - DicomDictionarySize_1;
      }
      itk::DicomTag dictionarytag = itk::DicomDictionary[firstid][secondid];
      return dictionarytag;
    }
    /**
       Static methode : returns the size of the dictionary : the amount of known DICOM flags
    */
    static unsigned int GetDictionarySize()
    {
      return DicomDictionarySize;
    }
    /**
       Static methode : finds a tag in the dictionary by its identifier
       i.e. FindTagByIdentifier("0002|0012");
    */
    static itk::DicomTag FindTagByIdentifier (const char* identifier);
    
  protected:

    DicomTagManager();
    ~DicomTagManager();
    
  
    
  private :
    DicomTagManager(const Self&);
    void operator=(const Self&);


    std::vector<itk::DicomTag> TagList;
    
    

    
  };
  
}// end of namespace itk





