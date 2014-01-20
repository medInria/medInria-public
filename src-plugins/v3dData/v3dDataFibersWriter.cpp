/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "v3dDataFibersWriter.h"

#include "dtkCore/dtkAbstractData.h"
#include "dtkCore/dtkAbstractDataFactory.h"

#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkSmartPointer.h>

#include "vtkXMLFiberDataSetWriter.h"
#include "vtkFiberDataSet.h"

#include <QFile>
#include <QXmlStreamWriter>

class v3dDataFibersWriterPrivate
{
public:
};

v3dDataFibersWriter::v3dDataFibersWriter(): d (new v3dDataFibersWriterPrivate)
{
}

v3dDataFibersWriter::~v3dDataFibersWriter()
{
   delete d;
   d = NULL;
}

QStringList v3dDataFibersWriter::handled() const
{
    return QStringList() << "v3dDataFibers";
}

bool v3dDataFibersWriter::canWrite(const QString& path)
{
  return true;
}

bool v3dDataFibersWriter::write(const QString& path)
{  
  if (!this->data())
      return false;

  if (this->data()->identifier()!="v3dDataFibers")
      return false;

  vtkFiberDataSet *dataset = static_cast<vtkFiberDataSet*>(this->data()->data());
  if (!dataset)
      return false;

  dataset->Update();
  
  vtkXMLFiberDataSetWriter *writer = vtkXMLFiberDataSetWriter::New();
  writer->SetFileName ( path.toAscii().constData() );
  writer->SetInput ( dataset );
  writer->SetDataModeToBinary();
  writer->Write();

  writer->Delete();
  
  return true;
}

QString v3dDataFibersWriter::identifier() const {
    return "v3dDataFibersWriter";
}

QString v3dDataFibersWriter::description() const {
    return "v3dDataFibersWriter";
}

bool v3dDataFibersWriter::registered()
{
  return dtkAbstractDataFactory::instance()->registerDataWriterType("v3dDataFibersWriter",
								    QStringList() << "v3dDataFibers",
								    createV3dDataFibersWriter);
}

QStringList v3dDataFibersWriter::supportedFileExtensions() const
{
    QStringList ret;
    vtkSmartPointer<vtkXMLFiberDataSetWriter> writer = vtkSmartPointer<vtkXMLFiberDataSetWriter>::New();
    QString extensionWithDot = QString(".%1").arg(writer->GetDefaultFileExtension()); 
    ret << extensionWithDot;
    return ret;
}


// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createV3dDataFibersWriter()
{
  return new v3dDataFibersWriter;
}
