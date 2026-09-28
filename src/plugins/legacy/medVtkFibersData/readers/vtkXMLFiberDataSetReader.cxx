/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkXMLFiberDataSetReader.h"

#include "vtkCompositeDataPipeline.h"
#include "vtkDataSet.h"
#include "vtkInformation.h"
#include "vtkFiberDataSet.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkXMLDataElement.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include <vtkVector.h>
#include <vtkCellData.h>
#include <vtkUnsignedCharArray.h>

vtkStandardNewMacro(vtkXMLFiberDataSetReader);

//----------------------------------------------------------------------------
vtkXMLFiberDataSetReader::vtkXMLFiberDataSetReader()
{
}

//----------------------------------------------------------------------------
vtkXMLFiberDataSetReader::~vtkXMLFiberDataSetReader()
{
}

//----------------------------------------------------------------------------
void vtkXMLFiberDataSetReader::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
int vtkXMLFiberDataSetReader::FillOutputPortInformation(
        int vtkNotUsed(port), vtkInformation* info)
{
    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkFiberDataSet");
    return 1;
}

//----------------------------------------------------------------------------
const char* vtkXMLFiberDataSetReader::GetDataSetName()
{
    return "vtkFiberDataSet";
}

//----------------------------------------------------------------------------
vtkFiberDataSet *vtkXMLFiberDataSetReader::GetOutput()
{
    return this->GetOutput (0);
}

//----------------------------------------------------------------------------
vtkFiberDataSet *vtkXMLFiberDataSetReader::GetOutput(int port)
{
    return vtkFiberDataSet::SafeDownCast(this->GetOutputDataObject(port));
}

//----------------------------------------------------------------------------
void vtkXMLFiberDataSetReader::ReadComposite(vtkXMLDataElement* element,
                                             vtkCompositeDataSet* composite, const char* filePath,
                                             unsigned int &dataSetIndex)
{
    vtkFiberDataSet* fiberDataSet = vtkFiberDataSet::SafeDownCast(composite);
    if (!fiberDataSet)
    {
        vtkErrorMacro("Unsuported composite dataset.");
        return;
    }

    if (this->GetFileMajorVersion() < 1)
    {
        vtkErrorMacro("Deprecated file format, unsupported.");
        return;
    }

    unsigned int maxElems = element->GetNumberOfNestedElements();
    for (unsigned int cc=0; cc < maxElems; ++cc)
    {
        vtkXMLDataElement* childXML = element->GetNestedElement(cc);
        if (!childXML || !childXML->GetName())
        {
            continue;
        }

        int index = 0;
        if (!childXML->GetScalarAttribute("index", index))
            // if index not in the structure file, then
            // set up to add at the end
        {
            index = fiberDataSet->GetNumberOfBlocks();
        }

        // child is a leaf node, read and insert.
        const char* tagName = childXML->GetName();
        if (strcmp(tagName, "Fibers") == 0)
        {
            vtkSmartPointer<vtkDataSet> childDS;
            // if (this->ShouldReadDataSet(dataSetIndex)) // CountLeaves return erroneous values
            {
                // Read
                childDS.TakeReference(this->ReadDataset(childXML, filePath));
            }

            // Check vtk data and re-add local and global colors if not there
            vtkPolyData *childData = vtkPolyData::SafeDownCast (childDS);
            if (!childData->GetPointData()->GetScalars())
                this->GenerateLocalColors(childData);

            if (!childData->GetCellData()->GetScalars())
                this->GenerateGlobalColors(childData);

            // insert
            fiberDataSet->SetFibers(childData);
        }
        else if (strcmp(tagName, "Bundle") == 0)
        {
            vtkSmartPointer<vtkDataSet> childDS;
            const char *bundleName = childXML->GetAttribute ("name");
            double color[3] = {0.0, 0.0, 0.0};
            childXML->GetScalarAttribute ("red",   color[0]);
            childXML->GetScalarAttribute ("green", color[1]);
            childXML->GetScalarAttribute ("blue",  color[2]);
            // if (this->ShouldReadDataSet(dataSetIndex)) // CountLeaves return erroneous values
            {
                // Read
                childDS.TakeReference(this->ReadDataset(childXML, filePath));
            }
            // insert
            vtkPolyData *bundle = vtkPolyData::SafeDownCast (childDS);
            if (fiberDataSet->GetFibers())
            {
                bundle->SetPoints(fiberDataSet->GetFibers()->GetPoints());
                bundle->GetPointData()->SetScalars(fiberDataSet->GetFibers()->GetPointData()->GetScalars());
                for( int i=0; i<fiberDataSet->GetFibers()->GetPointData()->GetNumberOfArrays(); i++)
                {
                    bundle->GetPointData()->AddArray ( fiberDataSet->GetFibers()->GetPointData()->GetArray (i) );
                }
            }
            fiberDataSet->AddBundle(bundleName, bundle, color);
        }
        else
        {
            vtkErrorMacro("Syntax error in file.");
            return;
        }
    }
}

void vtkXMLFiberDataSetReader::GenerateLocalColors(vtkPolyData *data)
{
    // Regenerate colors by local direction
    vtkSmartPointer <vtkUnsignedCharArray> myColors = vtkUnsignedCharArray::New();
    vtkIdType nbTotalPts = data->GetNumberOfPoints();
    myColors->SetNumberOfComponents (3);
    myColors->SetNumberOfValues(3*nbTotalPts);
    vtkIdType nbCells = data->GetNumberOfCells();

    vtkVector <double, 3> diff;
    for (int j = 0;j < nbCells;++j)
    {
        vtkCell* cell = data->GetCell(j);
        vtkPoints* cellPts = cell->GetPoints();
        vtkIdType nbPts = cellPts->GetNumberOfPoints();

        if (nbPts > 0)
        {
            double endPtVals[3];
            double firstPtVals[3];

            if (nbPts > 1)
                cellPts->GetPoint(1,endPtVals);
            else
                cellPts->GetPoint(0,endPtVals);

            cellPts->GetPoint(0,firstPtVals);
            int ptId = cell->GetPointId(0);

            for( unsigned int i=0; i<3; i++)
                diff[i] = endPtVals[i] - firstPtVals[i];

            if (diff.Norm() != 0)
                diff.Normalize();

            for( unsigned int i=0; i<3; i++)
            {
                double c = fabs (diff[i])*255.0;
                myColors->SetValue(3*ptId + i, (unsigned char)( c>255.0?255.0:c ) );
            }

            for( int i=1; i<nbPts-1; i++)
            {
                cellPts->GetPoint(i+1,endPtVals);
                cellPts->GetPoint(i-1,firstPtVals);
                ptId = cell->GetPointId(i);

                for( unsigned int a=0; a<3; a++)
                    diff[a] = endPtVals[a] - firstPtVals[a];

                if (diff.Norm() != 0)
                    diff.Normalize();

                for( unsigned int a=0; a<3; a++)
                {
                    double c = fabs (diff[a])*255.0;
                    myColors->SetValue(3*ptId + a, (unsigned char)( c>255.0?255.0:c ) );
                }
            }

            cellPts->GetPoint(nbPts-1,endPtVals);
            cellPts->GetPoint(nbPts-2,firstPtVals);
            ptId = cell->GetPointId(nbPts - 1);

            for( unsigned int a=0; a<3; a++)
                diff[a] = endPtVals[a] - firstPtVals[a];

            if (diff.Norm() != 0)
                diff.Normalize();

            for( unsigned int a=0; a<3; a++)
            {
                double c = fabs (diff[a])*255.0;
                myColors->SetValue(3*ptId + a, (unsigned char)( c>255.0?255.0:c ) );
            }
        }
    }

    data->GetPointData()->SetScalars(myColors);
}

void vtkXMLFiberDataSetReader::GenerateGlobalColors(vtkPolyData *data)
{
    // Regenerate colors by global tract direction
    vtkSmartPointer <vtkUnsignedCharArray> myCellColors = vtkUnsignedCharArray::New();
    myCellColors->SetNumberOfComponents (3);
    vtkIdType nbCells = data->GetNumberOfCells();

    vtkVector <double, 3> diff;
    for (int j = 0;j < nbCells;++j)
    {
        vtkCell* cell = data->GetCell(j);
        vtkPoints* cellPts = cell->GetPoints();
        vtkIdType nbPts = cellPts->GetNumberOfPoints();

        if (nbPts > 0)
        {
            double endPtVals[3];
            double firstPtVals[3];
            cellPts->GetPoint(nbPts-1,endPtVals);
            cellPts->GetPoint(0,firstPtVals);

            for( unsigned int i=0; i<3; i++)
                diff[i] = endPtVals[i] - firstPtVals[i];

            if (diff.Norm() != 0)
                diff.Normalize();

            for( unsigned int i=0; i<3; i++)
            {
                double c = fabs (diff[i])*255.0;
                myCellColors->InsertNextValue( (unsigned char)( c>255.0?255.0:c ) );
            }
        }
    }

    data->GetCellData()->SetScalars(myCellColors);
}

//----------------------------------------------------------------------------
int vtkXMLFiberDataSetReader::RequestDataObject(vtkInformation* vtkNotUsed(request),
                                                vtkInformationVector** vtkNotUsed(inputVector),
                                                vtkInformationVector* outputVector )
{
    //RequestDataObject (RDO) is an earlier pipeline pass.
    //During RDO, each filter is supposed to produce an empty data object of the proper type

    vtkInformation* outInfo = outputVector->GetInformationObject(0);
    vtkFiberDataSet* output = vtkFiberDataSet::SafeDownCast(
                outInfo->Get( vtkDataObject::DATA_OBJECT() ) );

    if ( ! output )
    {
        output = vtkFiberDataSet::New();
        outInfo->Set( vtkDataObject::DATA_OBJECT(), output );
        output->FastDelete();

        this->GetOutputPortInformation(0)->Set(
                    vtkDataObject::DATA_EXTENT_TYPE(), output->GetExtentType() );
    }
    return 1;
}
