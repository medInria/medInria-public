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

#ifdef WIN32
#define snprintf sprintf_s
#endif

vtkStandardNewMacro(vtkImageViewCornerAnnotation);

vtkImageViewCornerAnnotation::vtkImageViewCornerAnnotation()
{
    this->ImageView = nullptr;
}

vtkImageViewCornerAnnotation::~vtkImageViewCornerAnnotation() = default;

void vtkImageViewCornerAnnotation::TextReplace(vtkImageActor *ia,
                                               vtkImageMapToWindowLevelColors *wl)
{
    int i;
    char *text = nullptr, *text2 = nullptr;
    int slice = 0, slice_max = 0;
    char *rpos = nullptr, *tmp = nullptr;
    double window = 0, level = 0;
    long int windowi = 0, leveli = 0;
    vtkImageData *ia_input = nullptr;
    medVtkImageInfo *wl_input = nullptr;
    vtkCamera *cam = nullptr;

    int input_type_is_float = 0;
    int size_x = -1, size_y = -1, size_z = -1;
    double spacing_x = 0.0, spacing_y = 0.0, spacing_z = 0.0;
    double pos[3]={0.0, 0.0, 0.0};
    double pos_x = 0.0, pos_y = 0.0, pos_z = 0.0;
    int coord[3] = { 0, 0, 0 };
    int coord_x = 0, coord_y = 0, coord_z = 0;
    double value = 0.0, zoom = 100.0;
    std::string patient_name, study_name, series_name;

    vtkImageView2D *view2d = vtkImageView2D::SafeDownCast (this->ImageView);

    if (this->ImageView)
    {
        this->ImageView->GetCursorPosition (pos);
        this->ImageView->GetImageCoordinatesFromWorldCoordinates (pos, coord);
        value = this->ImageView->GetValueAtPosition(pos);
        zoom  = this->ImageView->GetZoom()*100.0;
        patient_name = this->ImageView->GetPatientName();
        study_name   = this->ImageView->GetStudyName();
        series_name  = this->ImageView->GetSeriesName();
        window = this->ImageView->GetColorWindow();
        window *= this->LevelScale;
        level = this->ImageView->GetColorLevel();
        level = level * this->LevelScale + this->LevelShift;
        windowi = (long int)window;
        leveli = (long int)level;

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
            int idX, idY, idZ;
            idX = idY = idZ = 0;

            for (unsigned int i=0; i<3; i++)
            {
                if (dotX <= std::fabs (Xaxis[i]))
                { dotX = std::fabs (Xaxis[i]);
                  idX = i; }
                if (dotY <= std::fabs (Yaxis[i]))
                { dotY = std::fabs (Yaxis[i]);
                  idY = i; }
            }
            if (view2d)
                idZ = view2d->GetSliceOrientation();

            input_type_is_float = (wl_input->scalarType == VTK_FLOAT ||
                                   wl_input->scalarType == VTK_DOUBLE);

            size_x = wl_input->dimensions[idX];
            size_y = wl_input->dimensions[idY];
            size_z = wl_input->dimensions[idZ];

            spacing_x = wl_input->spacing[idX];
            spacing_y = wl_input->spacing[idY];
            spacing_z = wl_input->spacing[idZ];

            pos_x = pos[idX];
            pos_y = pos[idY];
            pos_z = pos[idZ];

            coord_x = coord[idX];
            coord_y = coord[idY];
            coord_z = coord[idZ];
        }
    }

    if (view2d && ia)
    {
        int min, max, orientation;
        double position[3]={0.0, 0.0, 0.0};

        view2d->GetSliceRange(min,max);
        slice = view2d->GetSlice() - min +1;
        slice_max = max - min + 1;

        orientation = view2d->GetSliceOrientation();
        coord[orientation]=slice-1;

        this->ImageView->GetWorldCoordinatesFromImageCoordinates(coord, position);
        pos_z = position[orientation];

        ia_input = ia->GetInput();
        if (!wl_input && ia_input)
        {
            input_type_is_float = (ia_input->GetScalarType() == VTK_FLOAT ||
                                   ia_input->GetScalarType() == VTK_DOUBLE);
        }
    }

    // search for tokens, replace and then assign to TextMappers
    for (i = 0; i < 4; i++)
    {
        if (this->CornerText[i] && strlen(this->CornerText[i]))
        {
            size_t len = strlen(this->CornerText[i]) + 1000;
            text  = new char[len];
            text2 = new char[len];
            strncpy(text, this->CornerText[i], strlen(this->CornerText[i])+1);

            // now do the replacements

            rpos = strstr(text, "<image>");
            while (rpos)
            {
                *rpos = '\0';
                if (ia && this->ShowSliceAndImage)
                {
                    snprintf(text2, len, "%sImage: %i%s", text, slice, rpos+7);
                }
                else
                {
                    snprintf(text2, len, "%s%s", text, rpos+7);
                }
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<image>");
            }

            rpos = strstr(text, "<image_and_max>");
            while (rpos)
            {
                *rpos = '\0';
                if (ia && this->ShowSliceAndImage)
                {
                    snprintf(text2, len, "%sImage: %i / %i%s", text, slice, slice_max, rpos+15);
                }
                else
                {
                    snprintf(text2, len, "%s%s", text, rpos+15);
                }
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<image_and_max>");
            }

            rpos = strstr(text, "<slice>");
            while (rpos)
            {
                *rpos = '\0';
                if (ia && this->ShowSliceAndImage)
                {
                    snprintf(text2, len, "%sSlice: %i%s", text, slice, rpos+7);
                }
                else
                {
                    snprintf(text2, len, "%s%s", text, rpos+7);
                }
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<slice>");
            }

            rpos = strstr(text, "<slice_and_max>");
            while (rpos)
            {
                *rpos = '\0';
                if (ia && this->ShowSliceAndImage)
                {
                    snprintf(text2, len, "%sSlice: %i / %i%s", text, slice, slice_max, rpos+15);
                }
                else
                {
                    snprintf(text2, len, "%s%s", text, rpos+15);
                }
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<slice_and_max>");
            }

            rpos = strstr(text, "<slice_pos>");
            while (rpos)
            {
                *rpos = '\0';
                if (ia && this->ShowSliceAndImage)
                {
                    double *dbounds = ia->GetDisplayBounds();
                    int *dext = ia->GetDisplayExtent();
                    double pos;
                    if (dext[0] == dext[1])
                    {
                        pos = dbounds[0];
                    }
                    else if (dext[2] == dext[3])
                    {
                        pos = dbounds[2];
                    }
                    else
                    {
                        pos = dbounds[4];
                    }
                    snprintf(text2, len, "%s%g%s", text, pos, rpos+11);
                }
                else
                {
                    snprintf(text2, len, "%s%s", text, rpos+11);
                }
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<slice_pos>");
            }

            rpos = strstr(text, "<window>");
            while (rpos)
            {
                *rpos = '\0';
                if (this->ImageView)
                {
                    if (input_type_is_float)
                    {
                        snprintf(text2, len, "%sWindow: %g%s", text, window, rpos+8);
                    }
                    else
                    {
                        snprintf(text2, len, "%sWindow: %li%s", text, windowi, rpos+8);
                    }
                }
                else
                {
                    snprintf(text2, len, "%s%s", text, rpos+8);
                }
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<window>");
            }

            rpos = strstr(text, "<level>");
            while (rpos)
            {
                *rpos = '\0';
                if (this->ImageView)
                {
                    if (input_type_is_float)
                    {
                        snprintf(text2, len, "%sLevel: %g%s", text, level, rpos+7);
                    }
                    else
                    {
                        snprintf(text2, len, "%sLevel: %li%s", text, leveli, rpos+7);
                    }
                }
                else
                {
                    snprintf(text2, len, "%s%s", text, rpos+7);
                }
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<level>");
            }

            rpos = strstr(text, "<window_level>");
            while (rpos)
            {
                *rpos = '\0';
                if (this->ImageView)
                {
                    if (input_type_is_float)
                    {
                        snprintf(text2, len, "%sWW/WL: %g / %g%s", text, window, level, rpos+14);
                    }
                    else
                    {
                        snprintf(text2, len, "%sWW/WL: %li / %li%s", text, windowi, leveli, rpos+14);
                    }
                }
                else
                {
                    snprintf(text2, len, "%s%s", text, rpos+14);
                }
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<window_level>");
            }

            rpos = strstr(text, "<size_x>");
            while (rpos)
            {
                *rpos = '\0';
                snprintf(text2, len, "%s%i%s", text, size_x, rpos+8);
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<size_x>");
            }

            rpos = strstr(text, "<size_y>");
            while (rpos)
            {
                *rpos = '\0';
                snprintf(text2, len, "%s%i%s", text, size_y, rpos+8);
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<size_y>");
            }

            rpos = strstr(text, "<size_z>");
            while (rpos)
            {
                *rpos = '\0';
                snprintf(text2, len, "%s%i%s", text, size_z, rpos+8);
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<size_z>");
            }

            rpos = strstr(text, "<size>");
            while (rpos)
            {
                *rpos = '\0';
                snprintf(text2, len, "%simage size: %i x %i%s", text, size_x, size_y, rpos+6);
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<size>");
            }

            rpos = strstr(text, "<spacing_x>");
            while (rpos)
            {
                *rpos = '\0';
                snprintf(text2, len, "%s%3.3g%s", text, spacing_x, rpos+11);
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<spacing_x>");
            }

            rpos = strstr(text, "<spacing_y>");
            while (rpos)
            {
                *rpos = '\0';
                snprintf(text2, len, "%s%3.3g%s", text, spacing_y, rpos+11);
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<spacing_y>");
            }

            rpos = strstr(text, "<spacing_z>");
            while (rpos)
            {
                *rpos = '\0';
                snprintf(text2, len, "%s%3.3g%s", text, spacing_z, rpos+11);
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<spacing_z>");
            }

            rpos = strstr(text, "<spacing>");
            while (rpos)
            {
                *rpos = '\0';
                snprintf(text2, len, "%sspacing: %3.3g x %3.3g mm%s", text, spacing_x, spacing_y, rpos+9);
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<spacing>");
            }

            rpos = strstr(text, "<pos_x>");
            while (rpos)
            {
                *rpos = '\0';
                snprintf(text2, len, "%s%g%s", text, pos_x, rpos+7);
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<pos_x>");
            }

            rpos = strstr(text, "<pos_y>");
            while (rpos)
            {
                *rpos = '\0';
                snprintf(text2, len, "%s%g%s", text, pos_y, rpos+7);
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<pos_y>");
            }

            rpos = strstr(text, "<pos_z>");
            while (rpos)
            {
                *rpos = '\0';
                snprintf(text2, len, "%s%g%s", text, pos_z, rpos+7);
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<pos_z>");
            }

            rpos = strstr(text, "<coord_x>");
            while (rpos)
            {
                *rpos = '\0';
                snprintf(text2, len, "%s%i%s", text, coord_x, rpos+9);
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<coord_x>");
            }

            rpos = strstr(text, "<coord_y>");
            while (rpos)
            {
                *rpos = '\0';
                snprintf(text2, len, "%s%i%s", text, coord_y, rpos+9);
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<coord_y>");
            }

            rpos = strstr(text, "<coord_z>");
            while (rpos)
            {
                *rpos = '\0';
                snprintf(text2, len, "%s%i%s", text, coord_z, rpos+9);
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<coord_z>");
            }

            rpos = strstr(text, "<xyz>");
            while (rpos)
            {
                *rpos = '\0';
                snprintf(text2, len, "%sxyz: %4.4g, %4.4g, %4.4g mm%s", text, pos[0], pos[1], pos[2], rpos+5);
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<xyz>");
            }

            rpos = strstr(text, "<value>");
            while (rpos)
            {
                *rpos = '\0';
                snprintf(text2, len, "%svalue: %4.4g%s", text, value, rpos+7);
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<value>");
            }

            rpos = strstr(text, "<zoom>");
            while (rpos)
            {
                *rpos = '\0';
                snprintf(text2, len, "%szoom: %4.4g%s", text, zoom, rpos+6);
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<zoom>");
            }

            rpos = strstr(text, "<patient>");
            while (rpos)
            {
                *rpos = '\0';
                snprintf(text2, len, "%s%s%s", text, patient_name.c_str(), rpos+9);
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<patient>");
            }

            rpos = strstr(text, "<study>");
            while (rpos)
            {
                *rpos = '\0';
                snprintf(text2, len, "%s%s%s", text, study_name.c_str(), rpos+7);
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<study>");
            }

            rpos = strstr(text, "<series>");
            while (rpos)
            {
                *rpos = '\0';
                snprintf(text2, len, "%s%s%s", text, series_name.c_str(), rpos+8);
                tmp = text;
                text = text2;
                text2 = tmp;
                rpos = strstr(text, "<series>");
            }

            this->TextMapper[i]->SetInput(text);
            delete [] text;
            delete [] text2;
        }
        else
        {
            this->TextMapper[i]->SetInput("");
        }
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

