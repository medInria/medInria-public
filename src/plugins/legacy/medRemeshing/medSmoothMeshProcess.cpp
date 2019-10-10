/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <dtkCoreSupport/dtkAbstractProcessFactory.h>
#include <dtkCoreSupport/dtkSmartPointer.h>

#include <medAbstractDataFactory.h>
#include <medSmoothMeshProcess.h>
#include <medUtilities.h>

#include <vtkMetaDataSetSequence.h>
#include <vtkMetaSurfaceMesh.h>
#include <vtkSmoothPolyDataFilter.h>

// /////////////////////////////////////////////////////////////////
// medSmoothMeshProcessPrivate
// /////////////////////////////////////////////////////////////////

class medSmoothMeshProcessPrivate
{
public:
    dtkSmartPointer <medAbstractData> input;
    dtkSmartPointer <medAbstractData> output;
    int iterations;
    double relaxationFactor;
};

// /////////////////////////////////////////////////////////////////
// medSmoothMeshProcess
// /////////////////////////////////////////////////////////////////

medSmoothMeshProcess::medSmoothMeshProcess() : d(new medSmoothMeshProcessPrivate)
{
    d->output = nullptr;
}

medSmoothMeshProcess::~medSmoothMeshProcess()
{
    delete d;
    d = nullptr;
}

bool medSmoothMeshProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("medSmoothMeshProcess", createmedSmoothMeshProcess);
}

QString medSmoothMeshProcess::description() const
{
    return "medSmoothMeshProcess";
}

void medSmoothMeshProcess::setInput(medAbstractData *data , int channel)
{
    if (data)
    {
        d->input = data;
    }
}    

void medSmoothMeshProcess::setParameter(double data, int channel)
{
    if (channel > 1)
    {
        return;
    }

    switch (channel)
    {
        case 0:
            d->iterations = data;
            break;
        case 1:
            d->relaxationFactor = data;
            break;
    }
}

vtkMetaDataSet* medSmoothMeshProcess::smoothOneMetaDataSet(vtkMetaDataSet *inputMetaDaset)
{
    vtkPolyData *polyData = dynamic_cast<vtkPolyData*>(inputMetaDaset->GetDataSet());

    if (!polyData)
    {
        emit polyDataCastFailure();
        return nullptr;
    }
    else
    {
        vtkSmoothPolyDataFilter *contourSmoothed = vtkSmoothPolyDataFilter::New();
        contourSmoothed->SetInputData(polyData);
        contourSmoothed->SetNumberOfIterations(d->iterations);
        contourSmoothed->SetRelaxationFactor(d->relaxationFactor);
        contourSmoothed->Update();

        vtkMetaSurfaceMesh *smesh = vtkMetaSurfaceMesh::New();
        smesh->SetDataSet(contourSmoothed->GetOutput());
        contourSmoothed->Delete();

        return smesh;
    }
}

int medSmoothMeshProcess::update()
{
    progressed(0);

    if(!d->input->identifier().contains("vtkDataMesh"))
    {
        return medAbstractProcessLegacy::FAILURE;
    }

    d->output = medAbstractDataFactory::instance()->createSmartPointer(d->input->identifier());

    if (d->input->identifier() == "vtkDataMesh4D")
    {
        vtkMetaDataSetSequence *inputSequence = static_cast<vtkMetaDataSetSequence*>(d->input->data());
        vtkMetaDataSetSequence *outputSequence = vtkMetaDataSetSequence::New();

        foreach(vtkMetaDataSet *inputMetaDataSet, inputSequence->GetMetaDataSetList())
        {
            vtkMetaDataSet *outputMetaDataSet = smoothOneMetaDataSet(inputMetaDataSet);
            outputMetaDataSet->SetTime(inputMetaDataSet->GetTime());
            if (outputMetaDataSet == nullptr)
            {
                return medAbstractProcessLegacy::FAILURE;
            }
            outputSequence->AddMetaDataSet(outputMetaDataSet);
            outputMetaDataSet->Delete();
        }
        d->output->setData(outputSequence);
        outputSequence->Delete();
    }
    else
    {
        vtkMetaDataSet *inputMetaDataSet = static_cast<vtkMetaDataSet*>(d->input->data());
        vtkMetaDataSet *outputMetaDataSet = smoothOneMetaDataSet(inputMetaDataSet);
        if (outputMetaDataSet == nullptr)
        {
            return medAbstractProcessLegacy::FAILURE;
        }
        d->output->setData(outputMetaDataSet);
        outputMetaDataSet->Delete();
    }

    progressed(100);

    medUtilities::setDerivedMetaData(d->output, d->input, "smoothed");

    return medAbstractProcessLegacy::SUCCESS;
}

medAbstractData * medSmoothMeshProcess::output()
{
    return ( d->output );
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createmedSmoothMeshProcess()
{
    return new medSmoothMeshProcess;
}
