/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include <polygonRoiPluginExport.h>

#include <vtkOrientedGlyphFocalPlaneContourRepresentation.h>
#include <vtkSmartPointer.h>

class vtkProperty2D;
class vtkActor2D;
class vtkPolyDataMapper2D;
class vtkPolyData;
class vtkGlyph2D;
class vtkPoints;
class vtkPolyData;

class POLYGONROIPLUGIN_EXPORT vtkContourOverlayRepresentation: public vtkOrientedGlyphFocalPlaneContourRepresentation
{

public:
    // Description:
    // Instantiate this class.
    static vtkContourOverlayRepresentation *New();

    // Description:
    // Standard methods for instances of this class.
    vtkTypeMacro(vtkContourOverlayRepresentation, vtkOrientedGlyphFocalPlaneContourRepresentation)

    // Description:
    // The class maintains its true contour locations based on display co-ords
    // This method syncs the world co-ords data structure with the display co-ords.
    virtual void UpdateContourWorldPositionsBasedOnDisplayPositions();

    vtkSetMacro(needToSaveState, bool)
    vtkGetMacro(needToSaveState, bool)

    int CanUndo(); 
    int CanRedo(); 
    int SaveState();
    void Undo();
    void Redo();
    virtual void Initialize(vtkPolyData* polyData){Superclass::Initialize(polyData);}
    virtual void WidgetInteraction(double eventPos[2]);

    /**
     * @brief ComputeInteractionState is called when the mouse hovers a slice with a vtkContourRepresentation.
     * Change the cursor shape at hovering of a point.
     * @param X
     * @param Y
     * @param vtkNotUsed
     * @return Interaction state: cursor is nearby a contour point or not (outside)
     */
    virtual int ComputeInteractionState(int X, int Y, int vtkNotUsed(modified));
    virtual int DeleteLastNode();
    virtual int DeleteActiveNode();
    virtual int DeleteNthNode( int n );
    virtual void ClearAllNodes();
    virtual int AddNodeOnContour( int X, int Y );
    virtual int AddNodeOnContourAtIndex(int X, int Y, int idx);
    virtual int AddNodeAtDisplayPosition( int X, int Y );

protected:
    vtkContourOverlayRepresentation();
    ~vtkContourOverlayRepresentation();

private:
    vtkContourOverlayRepresentation(const vtkContourOverlayRepresentation&);  //Not implemented
    void operator=(const vtkContourOverlayRepresentation&);  //Not implemented

    std::vector<vtkSmartPointer<vtkPolyData> > undo_stack;
    std::vector<vtkSmartPointer<vtkPolyData> > redo_stack;
    bool needToSaveState;
};





