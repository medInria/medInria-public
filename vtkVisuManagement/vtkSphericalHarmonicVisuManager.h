#ifndef _vtk_SPHERICALHARMONICVisuManager_h_
#define _vtk_SPHERICALHARMONICVisuManager_h_

#include <vtkSphericalHarmonicSource.h>

class vtkSphericalHarmonicSource;
class vtkSphericalHarmonicGlyph;
class vtkPolyDataNormals;
class vtkExtractVOI;
class vtkPolyDataMapper;
class vtkActor;
class vtkStructuredPoints;

class vtkSphericalHarmonicVisuManager : public vtkObject
{

 public:
  static vtkSphericalHarmonicVisuManager *New();
  vtkTypeRevisionMacro(vtkSphericalHarmonicVisuManager, vtkObject);


  void SetGlyphScale (const float& scale);

  /** Get the Polyhedron type to be tesselated */
  void SetTesselationType (const int& type);

  /** Set the Polyhedron type to be tesselated */
  int GetTesselationType (void) const
  { return this->SHSource->GetTesselationType(); }

//  void SetTesselation (const int& type)
//  { this->SHSource->SetTesselation (type); }
  
//  int GetTesselation (void) const
//  { return this->SHSource->GetTesselation(); }

  void SetGlyphResolution (const int&);


  void SetSampleRate (const int&,const int&, const int&);

  void FlipX (const int&);
  void FlipY (const int&);
  void FlipZ (const int&);
  void MaxThesisFunc (const int&);
  
  /** Set the Volume Of Interest (VOI). Consists in
      6 integers: xmin, xmax, ymin, ymax, zmin, zmax.*/
  void SetVOI(const int&,const int&,const int&,const int&,const int&,const int&);

  void SetInput (vtkStructuredPoints*);
  
  vtkGetObjectMacro (SHSource, vtkSphericalHarmonicSource);
  vtkGetObjectMacro (SHGlyph,  vtkSphericalHarmonicGlyph);
  vtkGetObjectMacro (Normals,  vtkPolyDataNormals);
  vtkGetObjectMacro (VOI,      vtkExtractVOI);
  vtkGetObjectMacro (Mapper,   vtkPolyDataMapper);
  vtkGetObjectMacro (Actor,    vtkActor);
  
  
 protected:
  vtkSphericalHarmonicVisuManager();
  ~vtkSphericalHarmonicVisuManager();

  
 private:

  vtkSphericalHarmonicSource* SHSource;
  vtkSphericalHarmonicGlyph*  SHGlyph;
  vtkPolyDataNormals *        Normals;
  vtkExtractVOI*              VOI;
  vtkPolyDataMapper*          Mapper;
  vtkActor*                   Actor;
  
  
};


#endif
