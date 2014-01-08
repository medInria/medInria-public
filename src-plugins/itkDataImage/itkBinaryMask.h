/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "itkDataImage.h"
#include "itkDataImagePluginExport.h"

extern const char itkBinaryMaskName[];
class ITKDATAIMAGEPLUGIN_EXPORT itkBinaryMask : public itkDataImage<3,unsigned char,itkBinaryMaskName>{
public:
    itkBinaryMask(){ itkDataImage<3,unsigned char,itkBinaryMaskName>() ;
        qDebug()<<"Creating binary mask !"<<endl;
    }
    ~itkBinaryMask(){}

    QString description() const {return "itk binary mask";}

    static bool registered();
};


dtkAbstractData *createITKBinaryMask();