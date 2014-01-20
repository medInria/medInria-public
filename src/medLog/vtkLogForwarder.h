/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medLogExport.h"
#include <vtkOutputWindow.h>

/**
* @class vtkLogForwarder
* @brief This class forwards all vtk messages (Warning, Error, Debug) to the dtkLogger class
* You only need to instantiate this class once at the very beginning of you code (that uses itk)
*/
class MEDLOG_EXPORT vtkLogForwarder : public vtkOutputWindow 
{
public:
    static vtkLogForwarder *New();
    
    vtkTypeRevisionMacro(vtkLogForwarder,vtkOutputWindow);

    void PrintSelf(ostream& os, vtkIndent indent);

    /**
    * overrides error logging method and redirects it
    * @param txt – error log text
    */
    void DisplayErrorText(const char *txt);
    
    /**
    * overrides warning logging method and redirects it
    * @param txt – error log text
    */
    void DisplayWarningText(const char *txt);
    
    /**
    * overrides debug logging method and redirects it
    * @param txt – error log text
    */
    void DisplayDebugText(const char *txt);

protected:
     vtkLogForwarder();
    ~vtkLogForwarder();
};


