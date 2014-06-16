/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkFibersData.h>

#include <medAbstractDataFactory.h>

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkFiberDataSet.h>

#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

#include <QVTKWidget.h>

// /////////////////////////////////////////////////////////////////
// medVtkFibersDataPrivate
// /////////////////////////////////////////////////////////////////

class medVtkFibersDataPrivate
{
public:
    vtkSmartPointer<vtkFiberDataSet> data;
};

// /////////////////////////////////////////////////////////////////
// medVtkFibersData
// /////////////////////////////////////////////////////////////////

medVtkFibersData::medVtkFibersData() : medAbstractFibersData(), d(new medVtkFibersDataPrivate)
{
    d->data = 0;
}

medVtkFibersData::~medVtkFibersData()
{
    delete d;
    d = 0;
}

bool medVtkFibersData::registered()
{
    return medAbstractDataFactory::instance()->registerDataType("medVtkFibersData", create_medVtkFibersData);
}

QString medVtkFibersData::description() const
{
    return tr("VTK fibers data") ;
}

QString medVtkFibersData::identifier() const
{
    return "medVtkFibersData";
}

void medVtkFibersData::setData(void *data)
{
  if (vtkFiberDataSet *dataset = vtkFiberDataSet::SafeDownCast (static_cast<vtkObject*>(data)))
      d->data = dataset;
}

void *medVtkFibersData::data()
{
    return d->data;
}


// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

medAbstractData *create_medVtkFibersData()
{
    return new medVtkFibersData;
}

