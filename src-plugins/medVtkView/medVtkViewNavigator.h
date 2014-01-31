///*=========================================================================

//medInria

//Copyright (c) INRIA 2013. All rights reserved.
//See LICENSE.txt for details.

//This software is distributed WITHOUT ANY WARRANTY; without even
//the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//PURPOSE.

//=========================================================================*/

//#pragma once

//#include <medAbstractImageViewNavigator.h>
//#include <medImageViewOrientation.h>

//#include <medVtkViewPluginExport.h>

//class medAbstractImageView;

//class medVtkViewNavigatorPrivate;
//class MEDVTKVIEWPLUGIN_EXPORT medVtkViewNavigator : public medAbstractImageViewNavigator
//{
//    Q_OBJECT

//public:
//             medVtkViewNavigator(medAbstractImageView* parent);
//    virtual ~medVtkViewNavigator();


//    virtual QString description() const = 0;


//public:
//    virtual medImageView::Orientation orientaion() const = 0;
//    virtual void camera(QVector3D &position,
//                        QVector3D &viewup,
//                        QVector3D &focal,
//                        double &parallelScale) const = 0;
//    virtual QVector3D positionBeingViewed() const = 0;


//    virtual void setOrientation(medImageView::Orientation orientation) = 0;
//    virtual void setCamera(const QVector3D &position,
//                           const QVector3D &viewup,
//                           const QVector3D &focal,
//                           double parallelScale) = 0;
//private:
//    medVtkViewNavigatorPrivate *d;
//};
