/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAnnIntImageMaskHelper.h>

#include <medImageMaskAnnotationData.h>

#include <medAnnotationInteractor.h>

#include <vtkImageView2D.h>
#include <vtkImageView3D.h>

#include <vtkCommand.h>
#include <vtkImageMapToColors.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkScalarsToColors.h>
#include <vtkSmartPointer.h>

#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkLookupTable.h>

#include <medAbstractImageView.h>
#include <medVtkViewBackend.h>


class medAnnIntImageMaskHelperPrivate {
public:
};

medAnnIntImageMaskHelper::medAnnIntImageMaskHelper(medAnnotationInteractor * annInt)
    : msegAnnIntHelper(annInt) ,
    d(new medAnnIntImageMaskHelperPrivate)
{
}

medAnnIntImageMaskHelper::~medAnnIntImageMaskHelper()
{
    delete d;
    d = NULL;
}

bool medAnnIntImageMaskHelper::addAnnotation( medAnnotationData * annData )
{
    medImageMaskAnnotationData * imad = qobject_cast<medImageMaskAnnotationData*>(annData);
    if ( !imad )
        return false;

    medAbstractImageView * view = this->getView();
    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(view->backend());

    int oldLayer = view->currentLayer();
    int maskLayer = view->layersCount()-1;

    QList<double> scalars;
    QList<QColor> colors;
    for( medImageMaskAnnotationData::ColorMapType::const_iterator it(imad->colorMap().begin()), end(imad->colorMap().end());
        it != end; ++it )
    {
        scalars.push_back(it->first);
        colors.push_back(it->second);
    }

    view->setCurrentLayer(maskLayer);
    backend->view2D->SetCurrentLayer(maskLayer);

    setColorLookupTable(scalars,colors);

    backend->view2D->GetWindowLevel(maskLayer)->GetLookupTable()->SetRange(
        scalars.first() - 1, // mseg pads the data by one.
        scalars.back()  + 1);

    view->setCurrentLayer(oldLayer);
    backend->view2D->SetCurrentLayer(oldLayer);

    this->annotationModified(annData);
    return true;
}


void medAnnIntImageMaskHelper::setColorLookupTable ( QList<double> scalars, QList<QColor> colors )
{
    medAbstractImageView * view = this->getView();
    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(view->backend());

    int size= qMin ( scalars.count(),colors.count() );
    vtkColorTransferFunction * ctf = vtkColorTransferFunction::New();
    vtkPiecewiseFunction * pf = vtkPiecewiseFunction::New();
    for ( int i=0;i<size;i++ )
    {
        ctf->AddRGBPoint ( scalars.at ( i ),
                           colors.at ( i ).redF(),
                           colors.at ( i ).greenF(),
                           colors.at ( i ).blueF() );
        pf->AddPoint ( scalars.at ( i ),colors.at ( i ).alphaF() );
    }

    double min = scalars.first();
    double max = scalars.last();
    int n = static_cast< int > ( max - min ) + 1;
    n = std::max(n, size);
    double * table = new double[3*n];
    double * alphaTable = new double[n];
    ctf->GetTable ( min, max, n, table );
    ctf->Delete();
    pf->GetTable ( min,max,n,alphaTable );
    pf->Delete();

    vtkLookupTable * lut = vtkLookupTable::New();
    lut->SetNumberOfTableValues ( n + 2 );
    lut->SetTableRange ( min - 1.0, max + 1.0 );
    // lut->Build();

    lut->SetTableValue ( 0, 0.0, 0.0, 0.0, 0.0 );
    for ( int i = 0, j = 0; i < n; ++i, j += 3 )
        lut->SetTableValue ( i+1, table[j], table[j+1], table[j+2], alphaTable[i] );
    lut->SetTableValue ( n + 1, 0.0, 0.0, 0.0, 0.0 );

    if ( view->is2D() ) {
        backend->view2D->SetLookupTable( lut , view->currentLayer() );
        backend->view2D->Render();
    } else {
        backend->view3D->SetLookupTable ( lut, view->currentLayer() );
        backend->view3D->Render();
    }

    lut->Delete();
    delete [] table;
    delete [] alphaTable;
}

void medAnnIntImageMaskHelper::removeAnnotation( medAnnotationData * annData )
{
    medAbstractImageView * view = this->getView();
    view->removeData(annData);
}

void medAnnIntImageMaskHelper::annotationModified( medAnnotationData * annData )
{
    medImageMaskAnnotationData * imad = qobject_cast<medImageMaskAnnotationData*>(annData);
    medAbstractImageView * view = this->getView();
    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(view->backend());


    if ( !imad )
        return;

    if( view->contains(imad) )
    {
        backend->view2D->Modified();
        backend->view3D->Modified();

        view->viewWidget()->update();
    }
}

