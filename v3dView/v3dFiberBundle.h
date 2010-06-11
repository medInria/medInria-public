#ifndef _V3D_FIBERBUNDLE_H_
#define _V3D_FIBERBUNDLE_H_

// vtk include
#include <vtkFibersManager.h>
#include <vtkStructuredPointsReader.h>
#include <vtkStructuredPoints.h>
#include <vtkPolyDataWriter.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkSqueezeFibers.h>
#include <vtkTubeFilter.h>
#include <vtkRibbonFilter.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkCellArray.h>

#include <vector>
#include <string>

#ifdef vtkINRIA3D_USE_HWSHADING
#include <vtkFiberMapper.h>
#endif

#include "v3dViewPluginExport.h"

#ifdef WIN32
  #ifdef VTK_BUILD_SHARED
    #define V3DFIBERBUNDLE_EXPORT V3DVIEWPLUGIN_EXPORT
  #else
    #define V3DFIBERBUNDLE_EXPORT
  #endif
#else
  #define V3DFIBERBUNDLE_EXPORT
#endif

class V3DFIBERBUNDLE_EXPORT v3dFiberBundle : public vtkObject
{
 public:

  static v3dFiberBundle* New();
  vtkTypeRevisionMacro(v3dFiberBundle, vtkObject);  

  typedef vtkFibersManager::vtkFiberRenderingMode vtkFiberRenderingMode;
  
  // get set macro
  
  void SetName (const std::string& name)
  { this->Name = name; }
  std::string GetName(void) const
  { return this->Name; }
  
  
  void SetColor (float color[3])
  {
    this->Color[0] = color[0];
    this->Color[1] = color[1];
    this->Color[2] = color[2];
    this->Actor->GetProperty()->SetColor((double)color[0], (double)color[1], (double)color[2]);
  }
  const float* GetColor(void) const
  { return this->Color; }  

  void SetColorByDirection();
  void SetColorByFixedColor();
  bool isColorFixed();

  vtkSetObjectMacro (Points, vtkPoints);
  vtkGetObjectMacro (Points, vtkPoints);
  vtkGetObjectMacro (Bundle, vtkPolyData);
  vtkSetObjectMacro (Cells, vtkCellArray);
  vtkGetObjectMacro (Cells, vtkCellArray);
  vtkGetObjectMacro (Actor, vtkActor);
  vtkSetObjectMacro (FiberColors, vtkDataArray);
  vtkGetObjectMacro (FiberColors, vtkDataArray);

  vtkGetObjectMacro (PolyData, vtkPolyData);
  
  void Create (void);


  void SetRenderingMode (int i);
    
  
  void SetRenderingModeToPolyLines (void);
  
  
  void SetRenderingModeToTubes (void);
  

  void SetRenderingModeToRibbons (void);
  

  int GetRenderingMode(void) const
  { return this->RenderingMode; }


  void UseHardwareShadersOn(void);

  void UseHardwareShadersOff(void);

  void SetUseHardwareShaders(int);
  
  
  
  void SetRadius (double r)
  {
    this->TubeFilter->SetRadius (r);
    this->RibbonFilter->SetWidth (r);
  }
  
 protected:
  v3dFiberBundle();
  ~v3dFiberBundle();
    
 private:
  
  std::string        Name;
  float              Color[3];
  vtkPoints*         Points;
  vtkDataArray*      FiberColors;
  vtkPolyData*       Bundle;
  vtkPolyData*       PolyData;
  vtkSqueezeFibers*  Squeezer;
  vtkRibbonFilter*   RibbonFilter;
  vtkTubeFilter*     TubeFilter;
  vtkPolyDataMapper* Mapper;
  vtkActor*          Actor;
  int                ColorMethod;
  vtkCellArray*      Cells;
  
  vtkFiberRenderingMode RenderingMode;
  
  enum
  {
    COLOR_BY_DIRECTION,
    COLOR_BY_FIXEDCOLOR
  };
};


#endif
