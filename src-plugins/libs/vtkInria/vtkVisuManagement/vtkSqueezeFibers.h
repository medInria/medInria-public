/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkSqueezeFibers.h 608 2008-01-14 08:21:23Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2008-01-14 09:21:23 +0100 (Mon, 14 Jan 2008) $
Version:   $Revision: 608 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#pragma once

#include "vtkVisuManagementExport.h"

#include <vtkPolyDataAlgorithm.h>

class VTK_VISUMANAGEMENT_EXPORT vtkSqueezeFibers: public vtkPolyDataAlgorithm
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


