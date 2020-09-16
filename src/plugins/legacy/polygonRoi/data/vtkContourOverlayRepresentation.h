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
#include <vtkOrientedGlyphContourRepresentation.h>
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
    void UpdateContourWorldPositionsBasedOnDisplayPositions() override;

    vtkSetMacro(needToSaveState, bool)
    vtkGetMacro(needToSaveState, bool)

    void Initialize(vtkPolyData* polyData) override
    {
        Superclass::Initialize(polyData);
    }
    void WidgetInteraction(double eventPos[2]) override;

    /**
     * @brief ComputeInteractionState is called when the mouse hovers a slice with a vtkContourRepresentation.
     * Change the cursor shape at hovering of a point.
     * @param X
     * @param Y
     * @param vtkNotUsed
     * @return Interaction state: cursor is nearby a contour point or not (outside)
     */
    int ComputeInteractionState(int X, int Y, int vtkNotUsed(modified)) override;
    int DeleteLastNode() override;
    int DeleteActiveNode() override;
    int DeleteNthNode( int n ) override;
    void ClearAllNodes() override;
    int AddNodeOnContour( int X, int Y ) override;
    int AddNodeAtDisplayPosition( int X, int Y ) override;
    int AddNodeAtDisplayPosition( double displayPos[2] ) override;

    int GetNthNodeWorldPosition(int n, double worldPos[3]) override;
    int GetIntermediatePointWorldPosition(int n, int idx, double point[3]) override;
    int FindClosestPointOnContour( int X, int Y,
                                   double worldPos[3],
                                   int *idx ) override;

protected:
    vtkContourOverlayRepresentation();
    ~vtkContourOverlayRepresentation();

private:
    vtkContourOverlayRepresentation(const vtkContourOverlayRepresentation&);  //Not implemented
    void operator=(const vtkContourOverlayRepresentation&);  //Not implemented

    bool needToSaveState;
};





