/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkImageFromBoundsSource.h 724 2008-03-03 14:55:36Z jgarcia $
Language:  C++
Author:    $Author: jgarcia $
Date:      $Date: 2012-02-19 15:55:36 +0100 (Mon, 19 Jan 2012) $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


/**
   \class vtkImageFromBoundsSource vtkImageFromBoundsSource.h "vtkImageFromBoundsSource.h"
   \brief This class is a convenient image source that creates an image from user specified bounds

   This image source takes as input the desired world coordinates image bounds, and outputs
   an *empty* bounded image. The output image spacing (or size) can be set by the user by
   calling SetOutputImageSpacing() or SetOutputImageSize().

   By default Origin is set to the origin of the specified bounds, but you can set it manually
   with SetOutputImageOrigin().

   \author: Jaime Garcia, INRIA

   \see
   vtkImageFuse vtkImageAlgorithm
*/

#pragma once

#include "vtkVisuManagementExport.h"

#include "vtkImageAlgorithm.h"

class VTK_VISUMANAGEMENT_EXPORT vtkImageFromBoundsSource : public vtkImageAlgorithm
{
public:
  static vtkImageFromBoundsSource *New();
  vtkTypeMacro(vtkImageFromBoundsSource,vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);   
  
  // Description:
  // Set/Get the extent of the whole output image.
  void SetOutputImageBounds(const double Bounds[6]);
  void SetOutputImageBounds(double minX, double maxX, double minY, double maxY,
                            double minZ, double maxZ);
  void GetOutputImageBounds(double extent[6]);
  double *GetOutputImageBounds() {return this->OutputImageBounds;}


  void SetOutputImageSize (const unsigned int* size)
  {
    for (unsigned int i=0; i<3; i++)
      this->OutputImageSize[i] = size[i];
  }

  const unsigned int* GetOutputImageSize() const
  {
    return this->OutputImageSize;
  }
  void SetOutputImageSpacing (const double* spacing)
  {
    for (unsigned int i=0; i<3; i++)
      this->OutputImageSpacing[i] = spacing[i];
    this->SpacingSet = 1;
  }
  const double* GetOutputImageSpacing() const
  {
    return this->OutputImageSpacing;
  }
  void SetOutputImageOrigin (const double* origin)
  {
    for (unsigned int i=0; i<3; i++)
      this->OutputImageOrigin[i] = origin[i];
    this->OriginSet = 1;
  }
  const double* GetOutputImageOrigin() const
  {
    return this->OutputImageOrigin;
  }

  // Description:
  // Set/Get the outside pixel values.
  void SetOutValue (const double out)
  {
      this->OutValue = out;
      this->OutValueSet = 1;
  }

  vtkGetMacro(OutValue,double);
  
  // Description:
  // Set what type of scalar data this source should generate.
  vtkSetMacro(OutputScalarType,int);
  vtkGetMacro(OutputScalarType,int);
  void SetOutputScalarTypeTodouble()
    {this->SetOutputScalarType(VTK_FLOAT);}
  void SetOutputScalarTypeToDouble()
    {this->SetOutputScalarType(VTK_DOUBLE);}
  void SetOutputScalarTypeToLong()
    {this->SetOutputScalarType(VTK_LONG);}
  void SetOutputScalarTypeToUnsignedLong()
    {this->SetOutputScalarType(VTK_UNSIGNED_LONG);};
  void SetOutputScalarTypeToInt()
    {this->SetOutputScalarType(VTK_INT);}
  void SetOutputScalarTypeToUnsignedInt()
    {this->SetOutputScalarType(VTK_UNSIGNED_INT);}
  void SetOutputScalarTypeToShort()
    {this->SetOutputScalarType(VTK_SHORT);}
  void SetOutputScalarTypeToUnsignedShort()
    {this->SetOutputScalarType(VTK_UNSIGNED_SHORT);}
  void SetOutputScalarTypeToChar()
    {this->SetOutputScalarType(VTK_CHAR);}
  void SetOutputScalarTypeToUnsignedChar()
    {this->SetOutputScalarType(VTK_UNSIGNED_CHAR);}
  
protected:
  vtkImageFromBoundsSource();
  ~vtkImageFromBoundsSource();

  double OutputImageSpacing[3];
  bool SpacingSet;

  double OutputImageOrigin[3];
  int OriginSet;

  double OutputImageBounds[6];
  bool ImageBoundsSet;

  unsigned int OutputImageSize[3];
  bool SizeSet;

  double OutValue;
  bool OutValueSet;

  int OutputScalarType;
  
  virtual int RequestInformation (vtkInformation *, 
                                  vtkInformationVector **, 
                                  vtkInformationVector *);

  virtual int RequestData(vtkInformation *, 
                          vtkInformationVector **, vtkInformationVector *);

private:
  vtkImageFromBoundsSource(const vtkImageFromBoundsSource&);  // Not implemented.
  void operator=(const vtkImageFromBoundsSource&);  // Not implemented.
};





