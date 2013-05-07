/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkSmartPointer.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractData.h>

#include "medCoreExport.h"

struct MEDCORE_EXPORT medDataReaderWriter {

    typedef dtkSmartPointer<dtkAbstractDataReader> Reader;
    typedef dtkSmartPointer<dtkAbstractDataWriter> Writer;
    typedef dtkSmartPointer<dtkAbstractData>       Data;

    static Reader reader(const QString& path);
    static Writer writer(const QString& path,const dtkAbstractData* data);

    static Data read(const QString& path);
    static bool write(const QString& path,dtkAbstractData* data);
    };


