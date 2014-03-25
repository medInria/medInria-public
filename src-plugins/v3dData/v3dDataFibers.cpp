/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <v3dDataFibers.h>

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
// v3dDataFibersPrivate
// /////////////////////////////////////////////////////////////////

class v3dDataFibersPrivate
{
public:
    vtkSmartPointer<vtkFiberDataSet> data;
    QList<QImage>          thumbnails;
    QImage thumbnail;
};

// /////////////////////////////////////////////////////////////////
// v3dDataFibers
// /////////////////////////////////////////////////////////////////

v3dDataFibers::v3dDataFibers() : medAbstractFibersData(), d(new v3dDataFibersPrivate)
{
    d->data = 0;
    d->thumbnail = QImage(":v3dData/icons/fibers.png");
    d->thumbnails.push_back (d->thumbnail);
}

v3dDataFibers::~v3dDataFibers()
{
    delete d;
    d = 0;
}

bool v3dDataFibers::registered()
{
    return medAbstractDataFactory::instance()->registerDataType("v3dDataFibers", createV3dDataFibers);
}

QString v3dDataFibers::description() const
{
    return tr("v3d fibers data") ;
}

QString v3dDataFibers::identifier() const
{
    return "v3dDataFibers";
}

void v3dDataFibers::setData(void *data)
{
  if (vtkFiberDataSet *dataset = vtkFiberDataSet::SafeDownCast (static_cast<vtkObject*>(data)))
      d->data = dataset;
}

void *v3dDataFibers::data()
{
    return d->data;
}


// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

medAbstractData *createV3dDataFibers()
{
    return new v3dDataFibers;
}

