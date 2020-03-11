/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
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
    d->data = nullptr;
}

medVtkFibersData::~medVtkFibersData()
{
    delete d;
    d = nullptr;
}

bool medVtkFibersData::registered()
{
    return medAbstractDataFactory::instance()->registerDataType<medVtkFibersData>();
}

void medVtkFibersData::setData(void *data)
{
    if (vtkFiberDataSet *dataset = vtkFiberDataSet::SafeDownCast (static_cast<vtkObject*>(data)))
    {
        d->data = dataset;
        return;
    }

    vtkPolyData *inputData = vtkPolyData::SafeDownCast(static_cast <vtkObject *> (data));
    if (!inputData)
        return;

    if (!d->data)
        d->data = vtkFiberDataSet::New();

    d->data->SetFibers(inputData);
    this->setMetaData("BundleList", QStringList());
}

void *medVtkFibersData::data()
{
    return d->data;
}
