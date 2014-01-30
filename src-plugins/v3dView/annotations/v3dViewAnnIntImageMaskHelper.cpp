/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <v3dViewAnnIntImageMaskHelper.h>

#include <medImageMaskAnnotationData.h>

#include <v3dView.h>
#include <v3dViewAnnotationInteractor.h>

#include <vtkImageView2D.h>

#include <vtkCommand.h>
#include <vtkImageMapToColors.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkScalarsToColors.h>
#include <vtkSmartPointer.h>


class v3dViewAnnIntImageMaskHelperPrivate {
public:
};

v3dViewAnnIntImageMaskHelper::v3dViewAnnIntImageMaskHelper(v3dViewAnnotationInteractor * annInt)
    : v3dViewAnnIntHelper(annInt) ,
    d(new v3dViewAnnIntImageMaskHelperPrivate)
{
}

v3dViewAnnIntImageMaskHelper::~v3dViewAnnIntImageMaskHelper()
{
    delete d; d = NULL;
}

bool v3dViewAnnIntImageMaskHelper::addAnnotation( medAnnotationData * annData )
{
    medImageMaskAnnotationData * imad = qobject_cast<medImageMaskAnnotationData*>(annData);
    if ( !imad )
        return false;

    medAbstractDataImage * dataImage = imad->maskData();
    v3dView * view = this->getV3dView();
    int oldLayer = view->currentLayer();
    int maskLayer = view->layersCount();

    view->addLayer(dataImage);

    QList<double> scalars;
    QList<QColor> colors;
    for( medImageMaskAnnotationData::ColorMapType::const_iterator it(imad->colorMap().begin()), end(imad->colorMap().end());
        it != end; ++it )
    {
        scalars.push_back(it->first);
        colors.push_back(it->second);
    }


    view->setCurrentLayer(maskLayer);
    view->setColorLookupTable(scalars,colors);
//    qDebug() << "windowLevel" << view->view2d()->GetWindowLevel(maskLayer);
//    qDebug() << "lookuptable"<< view->view2d()->GetWindowLevel(maskLayer)->GetLookupTable();
    view->view2d()->GetWindowLevel(maskLayer)->GetLookupTable()->SetRange(
        scalars.first() - 1, // v3dView pads the data by one.
        scalars.back()  + 1);
    view->setCurrentLayer(oldLayer);

    this->annotationModified(annData);
    return true;
}


void v3dViewAnnIntImageMaskHelper::removeAnnotation( medAnnotationData * annData )
{
    v3dView * view = this->getV3dView();
    view->removeLayer(annData);
}

void v3dViewAnnIntImageMaskHelper::annotationModified( medAnnotationData * annData )
{
    medImageMaskAnnotationData * imad = qobject_cast<medImageMaskAnnotationData*>(annData);
    v3dView * view = this->getV3dView();

    if ( !imad )
        return;

    if( view->contains(imad) )
    {
        view->currentView()->Modified();
        view->update();
    }
}

