/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medVtkInriaExport.h>

#include <vtkImageAlgorithm.h>

/**
   \class vtkDatasetToImageGenerator vtkDatasetToImageGenerator.h "vtkDatasetToImageGenerator.h"
   \brief This class is a convenient image process that creates an image which bounds match those from the input dataset

   This algorithm takes any DataSet as input and outputs an *empty* image, which bounds
   match the input DataSet bounds. The output image spacing (or size) can be set by the user by calling SetOutputImageSpacing()
   or SetOutputImageSize().

   By default Origin is set to the origin of the input bounds, but you can set it manually with
   SetOutputImageOrigin().
   
   \author: Nicolas Toussaint, INRIA

   \see
   vtkImageFuse vtkImageAlgorithm
*/


class vtkDataSet;
class vtkImageData;


class MEDVTKINRIA_EXPORT vtkDatasetToImageGenerator: public vtkImageAlgorithm
{
 public:

  static vtkDatasetToImageGenerator *New();
  vtkTypeRevisionMacro(vtkDatasetToImageGenerator, vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  void SetOutputImageSize (unsigned int* size)
  {
    for (unsigned int i=0; i<3; i++)
      this->OutputImageSize[i] = size[i];
  }
  
  const unsigned int* GetOutputImageSize() const
  {
    return this->OutputImageSize;
  }
  void SetOutputImageSpacing (double* spacing)
  {
    for (unsigned int i=0; i<3; i++)
      this->OutputImageSpacing[i] = spacing[i];
    this->SpacingSet = 1;
  }
  const double* GetOutputImageSpacing() const
  {
    return this->OutputImageSpacing;
  }
  void SetOutputImageOrigin (double* origin)
  {
    for (unsigned int i=0; i<3; i++)
      this->OutputImageOrigin[i] = origin[i];
    this->OriginSet = 1;
  }
  const double* GetOutputImageOrigin() const
  {
    return this->OutputImageOrigin;
  }

 protected:
  
  vtkDatasetToImageGenerator();
  ~vtkDatasetToImageGenerator();

  virtual int FillInputPortInformation(int port, vtkInformation* info);
  virtual int RequestInformation (vtkInformation *, 
                                  vtkInformationVector **, 
                                  vtkInformationVector *);
  virtual int RequestData(vtkInformation *, 
                          vtkInformationVector **, 
                          vtkInformationVector *);

  unsigned int OutputImageSize[3];
  bool SizeSet;

  double OutputImageSpacing[3];
  bool SpacingSet;
  
  double OutputImageOrigin[3];
  int OriginSet;

 private:
  vtkDatasetToImageGenerator(const vtkDatasetToImageGenerator&);  // Not implemented.
  void operator=(const vtkDatasetToImageGenerator&);  // Not implemented.
  
};



