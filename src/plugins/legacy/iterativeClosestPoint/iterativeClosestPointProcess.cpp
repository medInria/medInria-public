/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "iterativeClosestPointProcess.h"

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>
#include <dtkCoreSupport/dtkSmartPointer.h>

#include <medAbstractDataFactory.h>
#include <medUtilities.h>
#include <medUtilitiesVTK.h>

#include <vtkICPFilter.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkLandmarkTransform.h>
#include <vtkLinearTransform.h>
#include <vtkMetaDataSet.h>
#include <vtkMetaSurfaceMesh.h>
#include <vtkSmartPointer.h>

// /////////////////////////////////////////////////////////////////
// iterativeClosestPointProcessPrivate
// /////////////////////////////////////////////////////////////////

class iterativeClosestPointProcessPrivate
{
public:
    dtkSmartPointer <medAbstractData> inputSource;
    dtkSmartPointer <medAbstractData> inputTarget;
    dtkSmartPointer <medAbstractData> output;
    
    int bStartByMatchingCentroids;
    int bTransformation;
    int bCheckMeanDistance;

    double ScaleFactor;
    int MaxNumIterations;
    int MaxNumLandmarks;
    double MaxMeanDistance;
};

// /////////////////////////////////////////////////////////////////
// iterativeClosestPointProcess
// /////////////////////////////////////////////////////////////////

iterativeClosestPointProcess::iterativeClosestPointProcess()
    : medAbstractProcessLegacy(), d(new iterativeClosestPointProcessPrivate)
{
    d->inputSource = nullptr;
    d->inputTarget = nullptr;
    d->output = nullptr;

    d->bStartByMatchingCentroids = 1;
    d->bTransformation           = 0;
    d->bCheckMeanDistance        = 0;
    d->ScaleFactor               = 1;
    d->MaxNumIterations          = 100;
    d->MaxNumLandmarks           = 1000;
    d->MaxMeanDistance           = 1;
}

iterativeClosestPointProcess::~iterativeClosestPointProcess()
{
    delete d;
    d = nullptr;
}

bool iterativeClosestPointProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("iterativeClosestPointProcess", createiterativeClosestPointProcess);
}

QString iterativeClosestPointProcess::description() const
{
    return "iterativeClosestPointProcess";
}

void iterativeClosestPointProcess::setInput(medAbstractData *data, int channel)
{
    if (data && (data->identifier().contains("vtkDataMesh") ||
                 data->identifier().contains("EPMap")))
    {
        if (channel == 0)
        {
            d->inputSource = data;
        }

        if (channel == 1)
        {
            d->inputTarget = data;
        }
    }
}    

void iterativeClosestPointProcess::setParameter(double data, int channel)
{
    switch (channel)
    {
        case 3:
            d->MaxMeanDistance = data;
            break;
        case 6:
            d->ScaleFactor = data;
            break;
    }
}

void iterativeClosestPointProcess::setParameter(int data, int channel)
{
    switch (channel)
    {
        case 0:
            d->bStartByMatchingCentroids = data;
            break;
        case 1:
            d->bTransformation = data;
            break;
        case 2:
            d->bCheckMeanDistance = data;
            break;
        case 4:
            d->MaxNumIterations = data;
            break;
        case 5:
            d->MaxNumLandmarks = data;
            break;
    }
}

int iterativeClosestPointProcess::update()
{
    if ( !d->inputSource || !d->inputTarget )
    {
        return medAbstractProcessLegacy::FAILURE;
    }

    vtkSmartPointer<vtkICPFilter> ICPFilter = vtkSmartPointer<vtkICPFilter>::New();
    
    vtkMetaDataSet *source_dataset = static_cast<vtkMetaDataSet*>(d->inputSource->data());
    vtkMetaDataSet *target_dataset = static_cast<vtkMetaDataSet*>(d->inputTarget->data());
    
    ICPFilter->SetSource(static_cast<vtkPolyData*>(source_dataset->GetDataSet()));
    ICPFilter->SetTarget(static_cast<vtkPolyData*>(target_dataset->GetDataSet()));

    ICPFilter->SetbStartByMatchingCentroids(d->bStartByMatchingCentroids);
    ICPFilter->SetbTransformation(d->bTransformation);
    ICPFilter->SetbCheckMeanDistance(d->bCheckMeanDistance);
    ICPFilter->SetScaleFactor(d->ScaleFactor);
    ICPFilter->SetMaxNumIterations(d->MaxNumIterations);
    ICPFilter->SetMaxNumLandmarks(d->MaxNumLandmarks);
    ICPFilter->SetMaxMeanDistance(d->MaxMeanDistance);

    ICPFilter->Update();

    vtkPolyData *output_polyData = ICPFilter->GetOutput();

    vtkMetaSurfaceMesh *output_mesh = vtkMetaSurfaceMesh::New();
    output_mesh->SetDataSet(output_polyData);

    d->output = medAbstractDataFactory::instance()->createSmartPointer(d->inputSource->identifier());
    d->output->setData(output_mesh);
    medUtilities::setDerivedMetaData(d->output, d->inputSource, "ICP");
    output_mesh->Delete();

    // manually transform catheter coordinates;
    if (d->output->identifier().contains("EPMap"))
    {
        QStringList arrayNames;
        arrayNames << "KT_Coordinates" << "_catheter_electrode_positions";
        vtkLinearTransform* transform = ICPFilter->GetLinearTransform();
        medUtilitiesVTK::transformCoordinates(d->output, arrayNames, transform);
    }

    return medAbstractProcessLegacy::SUCCESS;
}

medAbstractData * iterativeClosestPointProcess::output()
{
    return d->output;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createiterativeClosestPointProcess()
{
    return new iterativeClosestPointProcess;
}
