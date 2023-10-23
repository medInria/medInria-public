/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkFibersDataWriter.h>

#include <dtkCoreSupport/dtkAbstractData.h>
#include <medAbstractDataFactory.h>

#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkSmartPointer.h>

#include <vtkXMLFiberDataSetWriter.h>
#include <vtkFiberDataSet.h>

#include <QFile>
#include <QXmlStreamWriter>

medVtkFibersDataWriter::medVtkFibersDataWriter()
{
}

medVtkFibersDataWriter::~medVtkFibersDataWriter()
{
}

QStringList medVtkFibersDataWriter::handled() const
{
    return QStringList() << "medVtkFibersData";
}

bool medVtkFibersDataWriter::canWrite(const QString& path)
{
  return true;
}

bool medVtkFibersDataWriter::write(const QString& path)
{  
  if (!this->data())
      return false;

  if (this->data()->identifier()!="medVtkFibersData")
      return false;

  vtkFiberDataSet *dataset = static_cast<vtkFiberDataSet*>(this->data()->data());
  if (!dataset)
      return false;
  
  vtkXMLFiberDataSetWriter *writer = vtkXMLFiberDataSetWriter::New();
  writer->SetFileName ( path.toUtf8().constData() );
  writer->SetInputData ( dataset );
  writer->SetDataModeToBinary();
  writer->Write();

  writer->Delete();
  
  return true;
}

QString medVtkFibersDataWriter::identifier() const {
    return "medVtkFibersDataWriter";
}

QString medVtkFibersDataWriter::description() const {
    return "medVtkFibersDataWriter";
}

bool medVtkFibersDataWriter::registered()
{
  return medAbstractDataFactory::instance()->registerDataWriterType("medVtkFibersDataWriter",
                                    QStringList() << "medVtkFibersData",
                                    create_medVtkFibersDataWriter);
}

QStringList medVtkFibersDataWriter::supportedFileExtensions() const
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

dtkAbstractDataWriter *create_medVtkFibersDataWriter()
{
  return new medVtkFibersDataWriter;
}
