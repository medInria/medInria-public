/*============================================================================

The Hardware Shading (HWShading) module is protected by the
following copyright:

Copyright (c) 2007 Biomedical Image Analysis (BMIA) - Department of
Biomedical Engineering - Eindhoven University of Technology.
All rights reserved. See Copyright.txt for details.

The HWShading implementation was originally written by Tim Peeters (BMIA - TUe)
and published at the "11th International Fall Workshop on Vision, Modeling,
and Visualization 2006" (VMV'06):
"Visualization of the Fibrous Structure of the Heart", by T. Peeters et al.
See http://timpeeters.com/research/vmv2006 for more information.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

============================================================================*/
/**
 * vtkInterpolatedImageData.cxx
 * by Tim Peeters
 *
 * 2005-01-25	Tim Peeters
 * - First version
 *
 * 2005-01-26	Tim Peeters
 * - No longer a subclass of vtkImageData, so this-> is replaced by
 *   this->ImageData->, and some other changes.
 *
 * 2005-03-09	Tim Peeters
 * - Added GetInterpolatedTensorAt_Nearest()
 */

#include "vtkImageDataInterpolator.h"

#include <vtkObjectFactory.h>
#include <vtkCell.h>
#include <vtkDataArray.h>
#include <vtkPointData.h>
#include <vtkImageData.h>

#include <assert.h>

#define vtkIDIGetMacro( name ) \
double* vtkImageDataInterpolator::GetInterpolated##name##At(double x, double y, double z) \
{ \
  double tmp[3]; \
  tmp[0] = x; tmp[1] = y; tmp[2] = z; \
  return this->GetInterpolated##name##At(tmp); \
}

vtkStandardNewMacro(vtkImageDataInterpolator);

vtkImageDataInterpolator::vtkImageDataInterpolator()
{
  this->Tuple = NULL;
  this->TupleSize = 0;
  this->ImageData = NULL;
  this->InterpolationType = VTK_NEAREST_INTERPOLATION;
}

vtkImageDataInterpolator::~vtkImageDataInterpolator()
{
  this->SetImageData(NULL);
  if (this->TupleSize == 0)
    {
    assert( this->Tuple == NULL );
    }
  else
    {
    assert( this->TupleSize > 0 );
    assert( this->Tuple != NULL );
    delete [] this->Tuple;
    this->Tuple = NULL;
    }
}

void vtkImageDataInterpolator::SetImageData(vtkImageData* imageData)
{
  if (this->ImageData != imageData)
    {
    if (this->ImageData != NULL)
      {
      this->ImageData->UnRegister(this);
      }
    this->ImageData = imageData;
    if (this->ImageData != NULL)
      {
      this->ImageData->Register(this);

      // make sure the data arrays are initialized.
      // XXX: should this be here, or in the GetInterPolated...() functions?
      this->ImageData->Update();
      }
    this->Modified();
    }
}

double* vtkImageDataInterpolator::GetInterpolatedTupleAt(double x, double y, double z, vtkDataArray* dataArray)
{
  double tmp[3];
  tmp[0] = x;
  tmp[1] = y;
  tmp[2] = z;
  return this->GetInterpolatedTupleAt(tmp, dataArray);
}

double* vtkImageDataInterpolator::GetInterpolatedTupleAt(double x[3], vtkDataArray* dataArray)
{
  if ( this->ImageData == NULL )
    {
    vtkErrorMacro(<<"No image data!");
    return NULL;
    }
  
  if ( dataArray == NULL )
    {
    return NULL;
    }

/* XXX: doesn't work??
  double bounds[6];
  this->ImageData->GetBounds(bounds);
  if (	(x[0] < bounds[0]) || (x[0] > bounds[1]) ||
	(x[1] < bounds[2]) || (x[1] > bounds[3]) ||
	(x[2] < bounds[4]) || (x[2] > bounds[5]) )
    {
    // outside of image range.
    return NULL;
    // continuing here would not introduce bugs, but it could take longer.
    }
*/

  switch(this->InterpolationType)
    {
    case VTK_NEAREST_INTERPOLATION:
      return this->GetInterpolatedTupleAt_Nearest(x, dataArray);
      break; // not really useful after return
    case VTK_LINEAR_INTERPOLATION:
      return this->GetInterpolatedTupleAt_Linear(x, dataArray);
      break;
    default:
      vtkErrorMacro ( "Only nearest and linear interpolation are a allowed" );
      return NULL;
    } // switch
}

double* vtkImageDataInterpolator::GetInterpolatedTupleAt_Nearest(double x[3], vtkDataArray* dataArray)
{
  if ( this->ImageData == NULL )
    {
    vtkErrorMacro(<<"No image data!");
    return NULL;
    }
  
  assert( this->ImageData->CheckAttributes() == 0 );
  assert( dataArray != NULL );

  // TODO: check extent and return NULL if out-of-range.
  //	FindPoint may still find a nearest point if x is out-of-range
  //	(check that in the FindPoint implementation).

//cout<<"vtkImageDataInterpolator::GetInterpolatedTupleAt_Nearest("<<x[0]<<", "<<x[1]<<", "<<x[2]<<")."<<endl;
//  double* spacing = this->ImageData->GetSpacing();
//  double y[3];
//cout<<"spacing = "<<spacing[0]<<", "<<spacing[1]<<", "<<spacing[2]<<"."<<endl;
//  for (int i=0; i < 3; i++) y[i] = x[i]*spacing[i];

//  vtkIdType pointId = this->ImageData->FindPoint(y);

  vtkIdType pointId = this->ImageData->FindPoint(x); 
  
  if ( pointId < 0 )
    {
    vtkDebugMacro(<<"No point found at coordinates ("<<x[0]<<", "<<x[1]<<", "<<x[2]<<")");
    // no point was found
    return NULL;
    }

  // Get the number of components per tuple.
  int numberOfComponents = dataArray->GetNumberOfComponents();
  assert( numberOfComponents > 0 );
  // now before storing the result in this->Tuple,
  // make sure this->Tuple is a large enough array.
  this->SetMinTupleSize(numberOfComponents);

  dataArray->GetTuple(pointId, this->Tuple);
  return this->Tuple;
}

// Optimized for vtkTensorRayCastMapper. Use with caution. Checks were removed.
double* vtkImageDataInterpolator::GetInterpolatedTensorAt_Nearest(double x[3])
{
//cout<<"vtkImageDataInterpolator::GetInterpolatedTensorAt_Nearest("<<x[0]<<", "<<x[1]<<", "<<x[2]<<")."<<endl;
//  double* spacing = this->ImageData->GetSpacing();
//  double y[3];
//cout<<"spacing = "<<spacing[0]<<", "<<spacing[1]<<", "<<spacing[2]<<"."<<endl;
//  for (int i=0; i < 3; i++) y[i] = x[i]*spacing[i];

//  vtkIdType pointId = this->ImageData->FindPoint(y);
  vtkIdType pointId = this->ImageData->FindPoint(x);

  if ( pointId < 0 )
    {
    // no point was found
    return NULL;
    }

  this->ImageData->GetPointData()->GetTensors()->GetTuple(pointId, this->Tuple);
  return this->Tuple;
}

double* vtkImageDataInterpolator::GetInterpolatedTupleAt_Linear(double x[3], vtkDataArray* dataArray)
{
  if (this->ImageData == NULL)
    {
    return NULL;
    }

  assert( this->ImageData->CheckAttributes() == 0 );
    // 0 means everything is ok; // 1 indicates a mismatch. See vtkDataSet description.

  assert( dataArray != NULL );

  int subId; // not used, but FindCell() requires it.
		// in vtkImageData, FindCell always sets this to 0.

  double pcoords[3]; // used to calculate weights.
  double* weights; // = NULL; // = double[8];

  weights = new double[VTK_CELL_SIZE]; // 512; may be a bit large, since only
		// 8 are needed for a voxel. But there are cells that have more
		// than 8 points. And cells with more than VTK_CELL_SIZE(512)
		// may also be constructed. In that case, THERE IS A BUG HERE!!

  //XXX: submitting subId is wrong here?
  // goes ok because our cells are vtkVoxels, but other types of cells may really
  // need an address?

  // get the cell which contains the specified coordinates:
 // vtkCell* cell = this->ImageData->FindAndGetCell(x, NULL, 0, 0, subId, pcoords, weights);
  vtkIdType cellId = this->ImageData->FindCell(x, NULL, 0, 0, subId, pcoords, weights);
  vtkCell* cell = this->ImageData->GetCell(cellId);

  if (cell == NULL)
    {
    // no appropriate cell was found. x was out-of-range.
    delete[] weights; weights = NULL;
    return NULL;
    }

    // vtk documentation: findcell Returns cellId >= 0 if inside, < 0 otherwise
    //  assert( subId == 0 ); // not really used, but if the cell has no subcells then it is 0.

  double closestPoint[3];
  double dist2;

    // Given a point x[3] return inside(=1) or outside(=0) cell;
    // On rare occasions a -1 is returned from the method. This means that numerical 
    // error has occurred and all data returned from this method should be ignored.
    //if (!cell->EvaluatePosition(x, closestPoint, subId, pcoords, dist2, weights))
    if (cell->EvaluatePosition(x, closestPoint, subId, pcoords, dist2, weights) < 0)
    {
        vtkErrorMacro(<<"(-1) I don't get it anymore!");
        return NULL;
    }
    /*else if (cell->EvaluatePosition(x, closestPoint, subId, pcoords, dist2, weights) == 0)
    {
        vtkErrorMacro(<<"(0) I don't get it anymore!");
        return NULL;
    }*/
    else
    {
        for (int i=0; i < 3; i++) x[i] = closestPoint[i];
    }

    // cout<<"x = ("<<x[0]<<", "<<x[1]<<", "<<x[2]<<"), cp = ("<<closestPoint[0]<<", "<<closestPoint[1]<<", "<<closestPoint[2]<<"), "
    //     <<"edistance = "<<x[0]-closestPoint[0] + x[1]-closestPoint[1] + x[2]-closestPoint[2]<<endl;

  cell->EvaluateLocation(subId, pcoords, x, weights);

  // create cellTuples for storing the tuples in the current cell:
  vtkDataArray* cellTuples = vtkDataArray::CreateDataArray(dataArray->GetDataType());
  cellTuples->SetNumberOfComponents( dataArray->GetNumberOfComponents() );
  assert(cell->PointIds != NULL );
  cellTuples->SetNumberOfTuples(cell->PointIds->GetNumberOfIds());

  // get the tuples from scalars which are in the cell and store them in cellTuples
  dataArray->GetTuples(cell->PointIds, cellTuples);

  // Get the number of components per tuple.
  int numberOfComponents = dataArray->GetNumberOfComponents();
  assert( numberOfComponents > 0 );

  // now before storing the result in this->Tuple,
  // make sure this->Tuple is a large enough array.
  this->SetMinTupleSize(numberOfComponents);

  // set all values of this->Tuple to 0.
  for (int i = 0; i < this->TupleSize; i++) this->Tuple[i] = 0.0;

  // calculate the result
  double* currentTuple = NULL; double currentTupleWeight = 0;
  // for each edge of the cell:
  for (int i = 0; i < cellTuples->GetNumberOfTuples(); i++)
    {
    currentTuple = cellTuples->GetTuple(i);
    // assert weights[i] is valid.
    currentTupleWeight = weights[i];
    for (int compnr = 0; compnr < numberOfComponents; compnr++)
      {
      //cout<<"this->Tuple["<<compnr<<"] += "<<currentTuple[compnr]<<" * "<<currentTupleWeight<<";"<<endl;
      if (currentTupleWeight < 1e-10)
        { // XXX: ugly fix for rounding errors?
//        currentTupleWeight = 0;
	}
      this->Tuple[compnr] += currentTuple[compnr] * currentTupleWeight;
      }
    }
  currentTuple = NULL; currentTupleWeight = 0;
  cellTuples->Delete(); cellTuples = NULL;
  delete [] weights; weights = NULL;

  return this->Tuple;
}

double* vtkImageDataInterpolator::GetInterpolatedTupleNAt(double x, double y, double z, vtkDataArray* dataArray, int n)
{
  double tmp[3];
  tmp[0] = x; tmp[1] = y; tmp[2] = z;
  return this->GetInterpolatedTupleNAt(tmp, dataArray, n);
}

double* vtkImageDataInterpolator::GetInterpolatedTupleNAt(double x[3], vtkDataArray* dataArray, int n)
{
  if ( dataArray == NULL )
    {
    vtkErrorMacro(<<"Data array is NULL!");
    return NULL;
    }

  int numComp = dataArray->GetNumberOfComponents();
  if (numComp != n)
    {
    vtkErrorMacro("The number of scalar components do not match the number requested: "
                  << numComp << " != " << n);
    return NULL;
    }
  return this->GetInterpolatedTupleAt(x, dataArray);
}

double* vtkImageDataInterpolator::GetInterpolatedScalarAt(double x[3])
{
  if (this->ImageData == NULL )
    {
    return NULL;
    }
  assert( this->ImageData->GetPointData() != NULL );
  vtkDataArray* scalars = this->ImageData->GetPointData()->GetScalars();
  if ( scalars != NULL )
    {
    assert( this->ImageData->GetNumberOfScalarComponents() ==
	    scalars->GetNumberOfComponents() );
    }
  return this->GetInterpolatedTupleAt(x, scalars);
}

double vtkImageDataInterpolator::GetInterpolatedScalar1At(double x[3])
{
  if ( this->ImageData == NULL )
    {
    vtkErrorMacro(<<"Input image data is NULL.");
    return 0;
    }
  if ( this->ImageData->GetNumberOfScalarComponents() != 1 )
    {
    vtkErrorMacro("The number of scalar components is not 1");
    return 0;
    }
  assert( this->ImageData->GetPointData() != NULL );

  vtkDataArray* scalars = this->ImageData->GetPointData()->GetScalars();
  if ( scalars != NULL ) 
    {
    assert( this->ImageData->GetNumberOfScalarComponents() ==
	    scalars->GetNumberOfComponents() );
    }

  double* result = this->GetInterpolatedTupleNAt(x, scalars, 1);
  if (result == NULL)
    {
    return 0;
    }
  return result[0];
}

double* vtkImageDataInterpolator::GetInterpolatedVectorAt(double x[3])
{
  if ( this->ImageData == NULL )
    {
    return NULL;
    }
  assert( this->ImageData->GetPointData() != NULL );
  vtkDataArray* vectors = this->ImageData->GetPointData()->GetVectors();
  return this->GetInterpolatedTupleNAt(x, vectors, 3);
}

double* vtkImageDataInterpolator::GetInterpolatedNormalAt(double x[3])
{
  if ( this->ImageData == NULL )
    {
    return NULL;
    }
  assert( this->ImageData->GetPointData() != NULL );
  vtkDataArray* normals = this->ImageData->GetPointData()->GetNormals();
  return this->GetInterpolatedTupleNAt(x, normals, 3);
}

double* vtkImageDataInterpolator::GetInterpolatedTCoordAt(double x[3])
{
  if ( this->ImageData == NULL )
    {
    return NULL;
    }
  assert( this->ImageData->GetPointData() != NULL );
  vtkDataArray* tcoords = this->ImageData->GetPointData()->GetNormals();

  if ( tcoords == NULL )
    {
    return NULL;
    }

  int numComp = tcoords->GetNumberOfComponents();
  if (numComp > 3)
    {
    vtkErrorMacro("TCoords can have at most 3 components: "
                  << numComp << " > 3");
    return NULL;
    }

  return this->GetInterpolatedTupleAt(x, tcoords);
}

double* vtkImageDataInterpolator::GetInterpolatedTensorAt(double x[3])
{
  if ( this->ImageData == NULL )
    {
    return NULL;
    }
  assert( this->ImageData->GetPointData() != NULL );
  vtkDataArray* tensors = this->ImageData->GetPointData()->GetTensors();
  return this->GetInterpolatedTupleNAt(x, tensors, 9);
}

void vtkImageDataInterpolator::SetMinTupleSize(int minTupleSize)
{
  // before storing interpolation results in this->Tuple,
  // make sure this->Tuple is a large enough array.
  if ( this->TupleSize < minTupleSize )
    {
    this->TupleSize = minTupleSize;
    if ( this->Tuple != NULL )
      {
      delete [] this->Tuple;
      this->Tuple = NULL;
      }
    this->Tuple = new double[this->TupleSize];
    }
}

vtkIDIGetMacro(Scalar);
double vtkImageDataInterpolator::GetInterpolatedScalar1At(double x, double y, double z)
{
  double tmp[3];
  tmp[0] = x; tmp[1] = y; tmp[2] = z;
  return this->GetInterpolatedScalar1At(tmp);
}
vtkIDIGetMacro(Vector);
vtkIDIGetMacro(Normal);
vtkIDIGetMacro(TCoord);
vtkIDIGetMacro(Tensor);

