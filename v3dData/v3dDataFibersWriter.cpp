#include "v3dDataFibersWriter.h"

#include "dtkCore/dtkAbstractData.h"
#include "dtkCore/dtkAbstractDataFactory.h"

#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkXMLMultiBlockDataWriter.h>
#include "vtkFiberDataSet.h"

#include <QFile>
#include <QXmlStreamWriter>

class v3dDataFibersWriterPrivate
{
public:
  QXmlStreamWriter xml;

  void writeBundles (vtkFiberDataSet *dataset);
};

void v3dDataFibersWriterPrivate::writeBundles (vtkFiberDataSet *dataset)
{
  vtkFiberDataSet::vtkFiberBundleListType bundles = dataset->GetBundleList();

  int bundleCount = dataset->GetNumberOfBundles();

  xml.writeStartElement("BundleList");
  xml.writeAttribute ("size", QString::number (bundleCount));

  vtkFiberDataSet::vtkFiberBundleListType::iterator it = bundles.begin();
  while (it!=bundles.end()) {
    vtkPolyData *bundle = (*it).second.Bundle;
    
    xml.writeStartElement("Bundle");
    xml.writeAttribute ("name", (*it).first.c_str());
    QString color;
    color.sprintf("%.4f %.4f %.4f", (*it).second.Red,
		  (*it).second.Green,
		  (*it).second.Blue);    
    xml.writeTextElement ("color", color);
    xml.writeAttribute ("size", QString::number (bundle->GetNumberOfLines()) );
    
    vtkCellArray *lines = bundle->GetLines();
    lines->InitTraversal();
    vtkIdType npt, *pt;
    
    while (lines->GetNextCell (npt, pt)) {
      xml.writeStartElement("line");
      xml.writeAttribute ("size", QString::number (npt));
      QString indices;
      for (int i=0; i<npt; i++)
      {
	indices += QString::number (pt[i]);
	indices += " ";
      }
      xml.writeTextElement ("indices", indices);
      xml.writeEndElement();
    }
    xml.writeEndElement();
  }
  xml.writeEndElement();
}


v3dDataFibersWriter::v3dDataFibersWriter(): d (new v3dDataFibersWriterPrivate)
{
}

v3dDataFibersWriter::~v3dDataFibersWriter()
{
   delete d;
   d = NULL;
}

QStringList v3dDataFibersWriter::handled(void) const
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

  if (this->data()->description()!="v3dDataFibers")
      return false;

  vtkFiberDataSet *dataset = static_cast<vtkFiberDataSet*>(this->data()->data());
  if (!dataset)
      return false;

#ifdef _MSC_VER
  qDebug() << __FUNCSIG__;
#elif defined __GNUG__
  qDebug() << __PRETTY_FUNCTION__;
#else
  qDebug() << __func__;
#endif
  
  /*
  QFile file (path);
  d->xml.setDevice(&file);
  d->xml.writeStartDocument();
  d->xml.writeDTD("<!DOCTYPE v3dDataFibers>");
  d->xml.writeStartElement("Fibers");
  d->xml.writeAttribute("version", "1.0");

  d->xml.writeEndElement();
  
  d->writeBundles ( dataset );
  
  d->xml.writeEndDocument();
  */

  dataset->Update();
  
  qDebug() << "Number of blocks: " << dataset->GetNumberOfBlocks();

  vtkXMLMultiBlockDataWriter *writer = vtkXMLMultiBlockDataWriter::New();
  writer->SetFileName ( path.toAscii().constData() );
  writer->SetWriteMetaFile (1);
  writer->SetInput ( dataset );
  writer->Write();

  writer->Delete();
  
  return true;
}

QString v3dDataFibersWriter::description(void) const
{
    return "v3dDataFibersWriter";
}


bool v3dDataFibersWriter::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataWriterType("v3dDataFibersWriter",
								    QStringList() << "v3dDataFibers",
								    createV3dDataFibersWriter);
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createV3dDataFibersWriter(void)
{
  return new v3dDataFibersWriter;
}
