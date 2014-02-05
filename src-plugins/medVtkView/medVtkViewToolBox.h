/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medGuiExport.h>
#include <medToolBox.h>

class medVtkView;
class medVtkViewToolBoxPrivate;

class MEDGUI_EXPORT medVtkViewToolBox : public medToolBox
{
    Q_OBJECT

    enum viewMode
    {
        display2d,
        display3d
    };

public:
    explicit medVtkViewToolBox(QWidget *parent = 0);
    ~medVtkViewToolBox();

public:
    void update(dtkAbstractView * view);
    static bool registered();

//    QList<medAbstractParameter*> getViewParameters(medVtkView* view);
//    void setViewMode(viewMode mode);
//    void clearParams();
//    void addView(medVtkView*);

//public slots:
//    void updateViews();
//    void updateLayerListWidget(QList<medVtkView*>);
//    void resetList();
//    void updateLayerParameters();
//    void updateViewPool(QList<medVtkView*>);
//    void updateLayerParameters(QMultiMap<medVtkView*, int>);
//    void removeView();
//    void removeLayer();

//signals:
//    void viewSelected();

//private slots:
//    void setAxial(bool checked);
//    void setSagittal(bool checked);
//    void setCoronal(bool checked);
//    void setView3D(bool checked);

//protected:
//    void setupOrientationParamters(dtkAbstractView * view);

private:
    medVtkViewToolBoxPrivate *d;

};
