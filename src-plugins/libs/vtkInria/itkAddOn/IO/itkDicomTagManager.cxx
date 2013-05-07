/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkDicomTagManager.h"
#include <itksys/SystemTools.hxx>


#include <sstream>


namespace itk
{
  DicomTagManager::DicomTagManager()
  {
    //itk::DicomTag tag = (*itk::DicomDictionary)[325];
  }
  
  DicomTagManager::~DicomTagManager()
  {
    this->ClearTags();
    
  }


  
  void DicomTagManager::ImportTagList (std::vector<itk::DicomTag> taglist)
  {
    this->ClearTags();
    
    for (unsigned int i=0; i<taglist.size(); i++)
    {
      itk::DicomTag tag = taglist[i];
      this->AddTag (tag);
    }
  }
  
  void DicomTagManager::ImportTagList (gdcm::File* file)
  {
    this->ClearTags();

    for (unsigned int i=0; i<itk::DicomTagManager::GetDictionarySize(); i++)
    {
      itk::DicomTag dictionarytag = itk::DicomTagManager::GetDictionaryTag (i); 
      std::string found = file->GetEntryValue (dictionarytag.group,dictionarytag.element);	
      if (found == gdcm::GDCM_UNFOUND || !found.size())
	continue;
      
      itk::DicomTag tag = dictionarytag;
      tag.value = found.c_str();
      
      this->AddTag (tag);
    }
  }

  
  void DicomTagManager::AddTag (itk::DicomTag tag)
  {
    itk::DicomTag toinsert;
    
    toinsert.group = tag.group;
    toinsert.element = tag.element;
    toinsert.NotUsed = tag.NotUsed;
    toinsert.name = tag.name;
    toinsert.value = tag.value;
    toinsert.publicstate = tag.publicstate;
    toinsert.identifier = tag.identifier;
    
    
    this->TagList.push_back(toinsert);
  }

  void DicomTagManager::InsertTag (unsigned int tagid, itk::DicomTag tag)
  {
    if (tagid < this->TagList.size()-1)
    {
      std::vector<itk::DicomTag>::iterator it = this->TagList.begin();
      for (unsigned int i=0; i<tagid; i++)
      {
	it++;
      }
      this->TagList.insert(it, tag);
    }
    else
      this->TagList.push_back(tag);
  }
  
    
  
  std::vector<unsigned int> DicomTagManager::FindString (const char* string)
  {

    std::vector<unsigned int> ret;
    
    std::string tosearch = itksys::SystemTools::LowerCase (string);


    for (unsigned int i=0; i<this->GetTagList().size(); i++)
    {
      itk::DicomTag tag = this->GetTagList()[i];
      
      std::string name = itksys::SystemTools::LowerCase (tag.name);
      std::string type = itksys::SystemTools::LowerCase (tag.NotUsed);
      std::string value = itksys::SystemTools::LowerCase (tag.value);
      
      std::string::size_type pos1, pos2, pos3;
      pos1 = name.find (tosearch);
      pos2 = type.find (tosearch);
      pos3 = value.find (tosearch);
      if (pos1 != std::string::npos ||
	  pos2 != std::string::npos ||
	  pos3 != std::string::npos)
      {
	ret.push_back(i);
	
      } 
    }    
    return ret;
  }


  itk::DicomTag DicomTagManager::FindTagByIdentifier (const char* identifier)
  {
    for (unsigned int i=0; i<DicomTagManager::GetDictionarySize(); i++)
    {
      int firstid = 0;
      int secondid = i;
      if (i >= DicomDictionarySize_1)
      {	
	firstid = 1;
	secondid = i - DicomDictionarySize_1;
      }
      itk::DicomTag dictionarytag = itk::DicomDictionary[firstid][secondid];

      if (strcmp(identifier, dictionarytag.identifier.c_str()) == 0)
	return dictionarytag;
    }
    return itk::DicomDictionary[0][0];
    
  }
  
  
  itk::DicomTag DicomTagManager::GetTag (unsigned int tagid)
  {
//     if (tagid >= this->TagList.size())
//       return &NULL;
    return this->TagList[tagid];
    
  }

  
  
  const char* DicomTagManager::GetTagValue(unsigned int tagid)
  {
    std::string ret = "";
    itk::DicomTag tag = this->GetTag (tagid);
//     if (tag)
//     {
      ret = tag.value;
//     }
      return ret.c_str();
  }

  bool DicomTagManager::HasTag(uint16_t group, uint16_t element, unsigned int &tagid)
  {
    tagid = 0;
    for (unsigned int i=0; i<this->TagList.size(); i++)
    {
      if ( (group == this->GetTag (i).group) &&
	   (element == this->GetTag (i).element) )
      {
	tagid = i;
	return true;
      }
    }
    return false;
    
      


  }
  
    
  

  
  void DicomTagManager::SetTagValue(unsigned int tagid, const char* value)
  {
    itk::DicomTag tag = this->GetTag (tagid);
//     if (tag && value)
//     {
      tag.value = value;
//     }
  }
  
  
  std::vector<itk::DicomTag> DicomTagManager::GetTagList()
  {
    return this->TagList;
  }
    
  void DicomTagManager::ClearTags()
  {
//     this->TagList.clear();
  }
  
}


  

