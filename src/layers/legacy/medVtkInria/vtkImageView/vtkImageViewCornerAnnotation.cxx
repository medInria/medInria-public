/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkImageViewCornerAnnotation.h"

#include "vtkImageView2D.h"
#include "medVtkImageInfo.h"

#include <vtkCamera.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkRenderer.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>
#include <vtkWindow.h>

#include <cmath>

vtkStandardNewMacro(vtkImageViewCornerAnnotation)

vtkImageViewCornerAnnotation::vtkImageViewCornerAnnotation()
{
    this->ImageView = nullptr;
    this->view2d = nullptr;
}

vtkImageViewCornerAnnotation::~vtkImageViewCornerAnnotation() = default;

void vtkImageViewCornerAnnotation::SetImageView(vtkImageView* arg)
{
    this->ImageView = arg;
}

void vtkImageViewCornerAnnotation::TextReplace(vtkImageActor *imageActor,
                                               vtkImageMapToWindowLevelColors */*wl*/)
{
    int slice = 0, slice_max = 0;
    double window = 0, level = 0;
    long int windowi = 0, leveli = 0;
    vtkImageData *ia_input = nullptr;
    medVtkImageInfo *wl_input = nullptr;
    vtkCamera *cam = nullptr;
    bool input_type_is_float = 0;
    int size_x = -1, size_y = -1;
    double spacing_x = 0.0, spacing_y = 0.0;
    double pos[3] = {0.0, 0.0, 0.0};
    double pos_x = 0.0, pos_y = 0.0, pos_z = 0.0;
    int coord[3] = {0, 0, 0};
    int coord_x = 0, coord_y = 0;
    double value = 0.0, zoom = 100.0;

    if (!view2d)
    {
        this->view2d = vtkImageView2D::SafeDownCast (this->ImageView);
    }

    if (this->ImageView)
    {
        this->ImageView->GetCursorPosition (pos);
        this->ImageView->GetImageCoordinatesFromWorldCoordinates (pos, coord);
        value = this->ImageView->GetValueAtPosition(pos);
        zoom  = this->ImageView->GetZoom()*100.0;

        window = this->ImageView->GetColorWindow();
        window *= this->LevelScale;
        level = this->ImageView->GetColorLevel();
        level = level * this->LevelScale + this->LevelShift;
        windowi = static_cast<long int>(window);
        leveli = static_cast<long int>(level);

        wl_input = this->ImageView->GetMedVtkImageInfo();
        cam = this->ImageView->GetRenderer() ? this->ImageView->GetRenderer()->GetActiveCamera() : nullptr;

        if (wl_input && wl_input->initialized && cam)
        {
            double *viewup = cam->GetViewUp();
            double *normal = cam->GetViewPlaneNormal();
            double rightvector[3];
            vtkMath::Cross (normal, viewup, rightvector);

            // naively the X and Y axes of the current view
            // correspond to the rightvector and the viewup respectively.
            // But in fact we have to put those vectors back in the image
            // coordinates and see to which xyz image axis they correspond.

            double Xaxis[4] = {0, 0, 0, 0};
            double Yaxis[4] = {0, 0, 0, 0};
            for (unsigned int i=0; i<3; i++)
            {
                Xaxis[i] = rightvector[i];
                Yaxis[i] = viewup[i];
            }

            this->ImageView->GetInvertOrientationMatrix()->MultiplyPoint (Xaxis, Xaxis);
            this->ImageView->GetInvertOrientationMatrix()->MultiplyPoint (Yaxis, Yaxis);

            double dotX = 0;
            double dotY = 0;
            int idX, idY;
            idX = idY = 0;

            for (unsigned int i=0; i<3; i++)
            {
                if (dotX <= std::fabs (Xaxis[i]))
                {
                    dotX = std::fabs (Xaxis[i]);
                    idX = i;
                }
                if (dotY <= std::fabs (Yaxis[i]))
                {
                    dotY = std::fabs (Yaxis[i]);
                    idY = i;
                }
            }

            int sliceOrientation = 0;
            if (view2d)
            {
                sliceOrientation = view2d->GetSliceOrientation();
            }

            input_type_is_float = (wl_input->scalarType == VTK_FLOAT ||
                                   wl_input->scalarType == VTK_DOUBLE);

            size_x = wl_input->dimensions[idX];
            size_y = wl_input->dimensions[idY];

            spacing_x = wl_input->spacing[idX];
            spacing_y = wl_input->spacing[idY];

            pos_x = pos[idX];
            pos_y = pos[idY];
            pos_z = pos[sliceOrientation];

            coord_x = coord[idX];
            coord_y = coord[idY];
        }
    }

    if (view2d && imageActor)
    {
        // Current slice and Slice Max
        int min = 0, max = 0;
        view2d->GetSliceRange(min, max);
        slice = view2d->GetSlice() - min +1;
        slice_max = max - min + 1;

        // Slice Orientation & Position
        int sliceOrientation = view2d->GetSliceOrientation();
        coord[sliceOrientation]=slice-1;

        double position[3]={0.0, 0.0, 0.0};
        this->ImageView->GetWorldCoordinatesFromImageCoordinates(coord, position);
        pos_z = position[sliceOrientation];

        ia_input = imageActor->GetInput();
        if (!wl_input && ia_input)
        {
            input_type_is_float = (ia_input->GetScalarType() == VTK_FLOAT ||
                                   ia_input->GetScalarType() == VTK_DOUBLE);
        }
    }

    int isKnownAnnotationStyle = -1;
    if (view2d)
    {
        isKnownAnnotationStyle = static_cast<int>(view2d->GetAnnotationStyle2D());
    }

    // Search for tokens, replace and then assign to TextMappers
    QString replaceText = "";
    QString textQ = "";

    if (imageActor && this->ShowSliceAndImage && this->ImageView)
    {
        //---- osSW
        if (this->CornerText[0] && strlen(this->CornerText[0]))
        {
            textQ = this->CornerText[0];

            // Zoom
            replaceText = " Zoom: " + QString::number(zoom);
            textQ.replace("<zoom>", replaceText);

            // Slice current and max
            replaceText = " Slice: " + QString::number(slice) + "/" + QString::number(slice_max);
            textQ.replace("<slice_and_max>", replaceText);

            if (isKnownAnnotationStyle == -1
                    || isKnownAnnotationStyle == vtkImageView2D::AnnotationStyle1)
            {
                // Location
                replaceText = " Location: " + QString::number(pos_z) + " mm";
                textQ.replace("<pos_z>", replaceText);
            }

            this->TextMapper[0]->SetInput(textQ.toStdString().c_str());
        }

        //---- osSE
        // No South Est change, it's done in vtkImageView2D::SetAnnotationsFromOrientation()
        if (this->CornerText[1] && strlen(this->CornerText[1]))
        {
            textQ = this->CornerText[1];
            this->TextMapper[1]->SetInput(textQ.toStdString().c_str());
        }

        //---- osNW
        if (this->CornerText[2] && strlen(this->CornerText[2]))
        {
            textQ = this->CornerText[2];

            // AnnotationStyle1
            if (isKnownAnnotationStyle == -1
                    || isKnownAnnotationStyle == vtkImageView2D::AnnotationStyle1)
            {
                // Tag from VTK
                // Image
                replaceText = " Image: " + QString::number(slice);
                textQ.replace("<image>", replaceText);

                // From this app
                // Image size x/y
                replaceText = " Image size: " + QString::number(size_x) + " x " + QString::number(size_y);
                textQ.replace("<image_size>", replaceText);

                // Voxel size x/y
                replaceText = " Voxel size: " + QString::number(spacing_x) + QString(" mm") + " x "
                        + QString::number(spacing_y) + QString(" mm");
                textQ.replace("<voxel_size>", replaceText);

                // Coordinates x/y
                replaceText = " X: " + QString::number(coord_x) + QString(" px Y: ")
                        + QString::number(coord_y) + QString(" px")
                        +  QString(" Value: ") + QString::number(value);
                textQ.replace("<coord_xy_and_value>", replaceText);

                // Position x/y
                replaceText = " X: " + QString::number(pos_x) + QString(" mm Y: ") + QString::number(pos_y) + QString(" mm");
                textQ.replace("<pos_xy>", replaceText);

                // WW/WL
                if (input_type_is_float)
                {
                    replaceText = " WW/WL: " + QString::number(window) + QString("/") + QString::number(level);
                }
                else
                {
                    replaceText = " WW/WL: " + QString::number(windowi) + QString("/") + QString::number(leveli);
                }
                textQ.replace("<window_level>", replaceText);
            }

            // AnnotationStyle2
            if (isKnownAnnotationStyle == -1
                    || isKnownAnnotationStyle == vtkImageView2D::AnnotationStyle2)
            {
                // Image size x/y, almost equal to <image_size> for AnnotationStyle1
                replaceText = " image size: " + QString::number(size_x) + " x " + QString::number(size_y);
                textQ.replace("<size>", replaceText);

                // Spacing x/y, almost equal to <voxel_size> for AnnotationStyle1
                replaceText = " spacing: " + QString::number(spacing_x) + " x " + QString::number(spacing_y) + QString(" mm");
                textQ.replace("<spacing>", replaceText);

                // Location x/y/z
                replaceText = " xyz: " +
                        QString::number(pos[0]) + "," +
                        QString::number(pos[1]) + "," +
                        QString::number(pos[2]);
                textQ.replace("<xyz>", replaceText);

                // Value
                replaceText = " value: " + QString::number(value);
                textQ.replace("<value>", replaceText);
            }

            this->TextMapper[2]->SetInput(textQ.toStdString().c_str());
        }

        //---- osNE
        if (this->CornerText[3] && strlen(this->CornerText[3]))
        {
            textQ = this->CornerText[3];

            // AnnotationStyle1
            if (isKnownAnnotationStyle == -1
                    || isKnownAnnotationStyle == vtkImageView2D::AnnotationStyle1)
            {
                // Patient Name
                textQ.replace("<patient>", this->ImageView->GetPatientName());

                // Study Name
                textQ.replace("<study>", this->ImageView->GetStudyName());

                // Series Name
                textQ.replace("<series>", this->ImageView->GetSeriesName());

                // Slice: current and max for AnnotationStyle2
                replaceText = " Slice: " + QString::number(slice) + "/" + QString::number(slice_max);
                textQ.replace("<slice_and_max>", replaceText);
            }

            // AnnotationStyle2
            if (isKnownAnnotationStyle == -1
                    || isKnownAnnotationStyle == vtkImageView2D::AnnotationStyle2)
            {
                // Window
                if (input_type_is_float)
                {
                    replaceText = QString::number(window);
                }
                else
                {
                    replaceText = QString::number(windowi);
                }
                textQ.replace("<window>", replaceText);

                // Level
                if (input_type_is_float)
                {
                    replaceText = QString::number(level);
                }
                else
                {
                    replaceText = QString::number(leveli);
                }
                textQ.replace("<level>", replaceText);
            }

            this->TextMapper[3]->SetInput(textQ.toStdString().c_str());
        }
    }
    else
    {
        this->TextMapper[0]->SetInput("");
        this->TextMapper[1]->SetInput("");
        this->TextMapper[2]->SetInput("");
        this->TextMapper[3]->SetInput("");
    }
}

namespace
{
// Ported from old vtkTextMapper implementation
    int GetNumberOfLines(const char *str)
    {
        if (str == nullptr || *str == '\0')
        {
            return 0;
        }

        int result = 1;
        while (str != nullptr)
        {
            if ((str = strstr(str, "\n")) != nullptr)
            {
                result++;
                str++; // Skip '\n'
            }
        }
        return result;
    }
}


//----------------------------------------------------------------------------
int vtkImageViewCornerAnnotation::RenderOpaqueGeometry(vtkViewport *viewport)
{
    int fontSize;
    int i;

    // Check to see whether we have to rebuild everything
    // If the viewport has changed we may - or may not need
    // to rebuild, it depends on if the projected coords chage
    int viewport_size_has_changed = 0;
    if (viewport->GetMTime() > this->BuildTime ||
        (viewport->GetVTKWindow() &&
         viewport->GetVTKWindow()->GetMTime() > this->BuildTime))
    {
        int *vSize = viewport->GetSize();
        if (this->LastSize[0] != vSize[0] || this->LastSize[1] != vSize[1])
        {
            viewport_size_has_changed = 1;
        }
    }

    // Is there an image actor ?
    vtkImageMapToWindowLevelColors *wl = this->WindowLevel;
    vtkImageActor *ia = nullptr;
    if (this->ImageActor)
    {
        ia = this->ImageActor;
    }
    else
    {
        vtkPropCollection *pc = viewport->GetViewProps();
        int numProps = pc->GetNumberOfItems();
        for (i = 0; i < numProps; i++)
        {
            ia = vtkImageActor::SafeDownCast(pc->GetItemAsObject(i));
            if (ia)
            {
                break;
            }
        }
    }
    vtkImageView *iv = this->ImageView;

    int tprop_has_changed = (this->TextProperty &&
                             this->TextProperty->GetMTime() > this->BuildTime);

    // Check to see whether we have to rebuild everything
    if (viewport_size_has_changed ||
        tprop_has_changed ||
        (this->GetMTime() > this->BuildTime) ||
        (ia && (ia != this->LastImageActor ||
                ia->GetMTime() > this->BuildTime)) ||
        (iv && iv->GetMTime() > this->BuildTime) ||
        (wl && wl->GetMTime() > this->BuildTime))
    {
        int *vSize = viewport->GetSize();

        vtkDebugMacro(<<"Rebuilding text");

        // Replace text
        this->TextReplace(ia, wl);

        // Get the viewport size in display coordinates
        this->LastSize[0] = vSize[0];
        this->LastSize[1] = vSize[1];

        // Only adjust size then the text changes due to non w/l slice reasons
        if (viewport_size_has_changed ||
            tprop_has_changed ||
            this->GetMTime() > this->BuildTime)
        {
            // Rebuid text props.
            // Perform shallow copy here since each individual corner has a
            // different aligment/size but they share the other this->TextProperty
            // attributes.
            fontSize = this->TextMapper[0]->GetTextProperty()->GetFontSize();

            if (tprop_has_changed)
            {
                vtkTextProperty *tprop = this->TextMapper[0]->GetTextProperty();
                tprop->ShallowCopy(this->TextProperty);
                tprop->SetFontSize(fontSize);

                tprop = this->TextMapper[1]->GetTextProperty();
                tprop->ShallowCopy(this->TextProperty);
                tprop->SetFontSize(fontSize);

                tprop = this->TextMapper[2]->GetTextProperty();
                tprop->ShallowCopy(this->TextProperty);
                tprop->SetFontSize(fontSize);

                tprop = this->TextMapper[3]->GetTextProperty();
                tprop->ShallowCopy(this->TextProperty);
                tprop->SetFontSize(fontSize);

                this->SetTextActorsJustification();
            }

            // Update all the composing objects to find the best size for the font
            // use the last size as a first guess

            /*
                +--------+
                |2      3|
                |        |
                |        |
                |0      1|
                +--------+
            */

            int tempi[8];
            int allZeros = 1;
            for (i = 0; i < 4; i++)
            {
                this->TextMapper[i]->GetSize(viewport, tempi + i * 2);
                if (tempi[2*i] > 0 || tempi[2*i+1] > 0)
                {
                    allZeros = 0;
                }
            }

            if (allZeros)
            {
                return 0;
            }

            int height_02 = tempi[1] + tempi[5];
            int height_13 = tempi[3] + tempi[7];

            int width_01 = tempi[0] + tempi[2];
            int width_23 = tempi[4] + tempi[6];

            int max_width = (width_01 > width_23) ? width_01 : width_23;


            int num_lines_02 = GetNumberOfLines(this->TextMapper[0]->GetInput())
                               + GetNumberOfLines(this->TextMapper[2]->GetInput());

            int num_lines_13 = GetNumberOfLines(this->TextMapper[1]->GetInput())
                               + GetNumberOfLines(this->TextMapper[3]->GetInput());

            int line_max_02 = (int)(vSize[1] * this->MaximumLineHeight) *
                              (num_lines_02 ? num_lines_02 : 1);

            int line_max_13 = (int)(vSize[1] * this->MaximumLineHeight) *
                              (num_lines_13 ? num_lines_13 : 1);

            // Target size is to use 90% of x and y

            int tSize[2];
            tSize[0] = (int)(0.9*vSize[0]);
            tSize[1] = (int)(0.9*vSize[1]);

            // While the size is too small increase it

            while (height_02 < tSize[1] &&
                   height_13 < tSize[1] &&
                   max_width < tSize[0] &&
                   height_02 < line_max_02 &&
                   height_13 < line_max_13 &&
                   fontSize < 100)
            {
                fontSize++;
                for (i = 0; i < 4; i++)
                {
                    this->TextMapper[i]->GetTextProperty()->SetFontSize(fontSize);
                    this->TextMapper[i]->GetSize(viewport, tempi + i * 2);
                }
                height_02 = tempi[1] + tempi[5];
                height_13 = tempi[3] + tempi[7];
                width_01 = tempi[0] + tempi[2];
                width_23 = tempi[4] + tempi[6];
                max_width = (width_01 > width_23) ? width_01 : width_23;
            }

            // While the size is too large decrease it

            while ((height_02 > tSize[1] ||
                    height_13 > tSize[1] ||
                    max_width > tSize[0] ||
                    height_02 > line_max_02 ||
                    height_13 > line_max_13) &&
                   fontSize > 0)
            {
                fontSize--;
                for (i = 0; i < 4; i++)
                {
                    this->TextMapper[i]->GetTextProperty()->SetFontSize(fontSize);
                    this->TextMapper[i]->GetSize(viewport, tempi + i * 2);
                }
                height_02 = tempi[1] + tempi[5];
                height_13 = tempi[3] + tempi[7];
                width_01 = tempi[0] + tempi[2];
                width_23 = tempi[4] + tempi[6];
                max_width = (width_01 > width_23) ? width_01 : width_23;
            }

            fontSize = static_cast<int>(pow((double)fontSize,
                                            NonlinearFontScaleFactor)*LinearFontScaleFactor);
            if (fontSize > this->MaximumFontSize)
            {
                fontSize = this->MaximumFontSize;
            }
            this->FontSize = fontSize;
            for (i = 0; i < 4; i++)
            {
                this->TextMapper[i]->GetTextProperty()->SetFontSize(fontSize);
                this->TextMapper[i]->GetTextProperty()->ShadowOn();
            }

            // Now set the position of the TextActors

            this->SetTextActorsPosition(vSize);

            for (i = 0; i < 4; i++)
            {
                this->TextActor[i]->SetProperty(this->GetProperty());
            }
        }
        this->BuildTime.Modified();
        this->LastImageActor = ia;
    }

    // Everything is built, just have to render

    if (this->FontSize >= this->MinimumFontSize)
    {
        for (i = 0; i < 4; i++)
        {
            this->TextActor[i]->RenderOpaqueGeometry(viewport);
        }
    }

    return 1;
}
