#pragma once
#include <vtkAlgorithmOutput.h>
#include <vtkLookupTable.h>
#include <vtkSetGet.h>
#include "medVtkImageInfo.h"

#include <vtkImageData.h>

class vtkImage3DDisplay : public vtkObject
{
public:
    static vtkImage3DDisplay*New();

    vtkTypeMacro (vtkImage3DDisplay, vtkObject);

    //vtkSetObjectMacro(InputConnection, vtkAlgorithmOutput);

    virtual void SetInputConnection(vtkAlgorithmOutput*  pi_poVtkAlgoPort);
    virtual medVtkImageInfo* GetVtkImageInfo();

    virtual bool isInputSet();

    vtkSetMacro(Opacity, double);
    vtkGetMacro(Opacity, double);

    vtkSetMacro(Visibility, int);
    vtkGetMacro(Visibility, int);

    vtkSetMacro(UseLookupTable, bool);
    vtkGetMacro(UseLookupTable, bool);

    vtkSetObjectMacro(LookupTable, vtkLookupTable);
    virtual vtkLookupTable*GetLookupTable();

    vtkSetMacro(ColorWindow, double);
    vtkGetMacro(ColorWindow, double);

    vtkSetMacro(ColorLevel, double);
    vtkGetMacro(ColorLevel, double);

    vtkAlgorithmOutput* GetOutputPort();

protected:
    vtkImage3DDisplay();
    ~vtkImage3DDisplay();

private:
    //vtkSmartPointer<vtkImageData>               InputImageOld;
    vtkSmartPointer<vtkAlgorithmOutput>         InputConnection;
    double Opacity;
    int Visibility;
    double ColorWindow;
    double ColorLevel;
    bool UseLookupTable;
    vtkSmartPointer<vtkLookupTable>             LookupTable;
    medVtkImageInfo                             m_sVtkImageInfo;

    vtkImage3DDisplay(const vtkImage3DDisplay&);
    void operator=(const vtkImage3DDisplay&);
};
