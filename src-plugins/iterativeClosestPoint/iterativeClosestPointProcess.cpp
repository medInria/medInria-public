/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "iterativeClosestPointProcess.h"

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <dtkCore/dtkAbstractProcess.h>

#include <vtkMetaDataSet.h>
#include <vtkMetaSurfaceMesh.h>

#include <vtkSmartPointer.h>
#include <vtkAlgorithmOutput.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

#include <medDataManager.h>
#include <medMetaDataKeys.h>
#include <medAbstractDataFactory.h>
#include <medViewContainer.h>
#include <medTabbedViewContainers.h>

#include <vtkICPFilter.h>

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
    int bRigidBody;
    int bCheckMeanDistance;

    double ScaleFactor;
    int MaxNumIterations;
    int MaxNumLandmarks;
    double MaxMeanDistance;
};

// /////////////////////////////////////////////////////////////////
// iterativeClosestPointProcess
// /////////////////////////////////////////////////////////////////

iterativeClosestPointProcess::iterativeClosestPointProcess() : dtkAbstractProcess(), d(new iterativeClosestPointProcessPrivate)
{
    d->inputSource = 0;
    d->inputTarget = 0;
    d->output = 0;

	d->bStartByMatchingCentroids=1;
	d->bRigidBody=0;
	d->bCheckMeanDistance=0;
	
	d->ScaleFactor=1;
	d->MaxNumIterations=100;
    d->MaxNumLandmarks=1000;
	d->MaxMeanDistance=1;
}

iterativeClosestPointProcess::~iterativeClosestPointProcess()
{
    delete d;
    d = NULL;
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
    if ( !data  || data->identifier()!="vtkDataMesh")
        return;
    
    if (channel==0)
    {
        d->inputSource = data;
    }
    
    if (channel==1)
    {
        d->inputTarget = data;
    }
    
    if (!d->output)
        d->output = medAbstractDataFactory::instance()->createSmartPointer ( "vtkDataMesh" );
}    


void iterativeClosestPointProcess::setParameter ( double data, int channel )
{
    switch (channel) {
        case 0:
            d->bStartByMatchingCentroids = (data>0) ? true : false;
            break;
        case 1:
            d->bRigidBody = (data>0) ? true : false;
            break;
        case 2:
            d->bCheckMeanDistance = (data>0) ? true : false;
            break;
        case 3:
            d->MaxMeanDistance = data;
            break;
        case 4:
            d->MaxNumIterations = data;
            break;
        case 5:
            d->MaxNumLandmarks = data;
            break;
        case 6:
            d->ScaleFactor = data;
            break;
    }
}

int iterativeClosestPointProcess::update()
{
    if ( !d->inputSource || !d->inputTarget )
        return -1;
    
    // Wait Cursor
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    vtkSmartPointer<vtkICPFilter> ICPFilter = vtkICPFilter::New();
    
    vtkMetaDataSet * source_dataset = static_cast<vtkMetaDataSet*>(d->inputSource->data());
    vtkMetaDataSet * target_dataset = static_cast<vtkMetaDataSet*>(d->inputTarget->data());
    
    ICPFilter->SetSource(static_cast<vtkPolyData*>(source_dataset->GetDataSet()));
    ICPFilter->SetTarget(static_cast<vtkPolyData*>(target_dataset->GetDataSet()));

    ICPFilter->SetbStartByMatchingCentroids(d->bStartByMatchingCentroids);
    ICPFilter->SetbRigidBody(d->bRigidBody);
    ICPFilter->SetbCheckMeanDistance(d->bCheckMeanDistance);
    ICPFilter->SetScaleFactor(d->ScaleFactor);
    ICPFilter->SetMaxNumIterations(d->MaxNumIterations);
    ICPFilter->SetMaxNumLandmarks(d->MaxNumLandmarks);
    ICPFilter->SetMaxMeanDistance(d->MaxMeanDistance);

    ICPFilter->Update();

    vtkPolyData * output_polyData = ICPFilter->GetOutput();

    vtkMetaSurfaceMesh * output_mesh = vtkMetaSurfaceMesh::New();
    output_mesh->SetDataSet(output_polyData);
    
    QString patientName = d->inputSource->metadata ( medMetaDataKeys::PatientName.key() );
    d->output->addMetaData ( medMetaDataKeys::PatientName.key(), patientName );

    d->output->setData(output_mesh);
    
    // Arrow Cursor
    QApplication::restoreOverrideCursor();
    QApplication::processEvents();

    return EXIT_SUCCESS;
}


medAbstractData * iterativeClosestPointProcess::output()
{
    return ( d->output );
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createiterativeClosestPointProcess()
{
    return new iterativeClosestPointProcess;
}
