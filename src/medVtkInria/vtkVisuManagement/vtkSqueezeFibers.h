/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medVtkInriaExport.h>

#include <vtkPolyDataAlgorithm.h>

class MEDVTKINRIA_EXPORT vtkSqueezeFibers: public vtkPolyDataAlgorithm
{

public:
	static vtkSqueezeFibers *New();
	vtkTypeRevisionMacro(vtkSqueezeFibers, vtkPolyDataAlgorithm);


	void PrintSelf(ostream &os, vtkIndent indent);

	vtkSetMacro(MaximumNumberOfFibers, double);
	vtkGetMacro(MaximumNumberOfFibers, double);


protected:
	vtkSqueezeFibers();
	~vtkSqueezeFibers();

	// Usual data generation method
	virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

private:
	vtkSqueezeFibers(const vtkSqueezeFibers&);
	void operator=(const vtkSqueezeFibers&);

	double MaximumNumberOfFibers;

};


