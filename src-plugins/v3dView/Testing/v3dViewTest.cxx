/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QtGui>

#include "dtkCore/dtkPluginManager.h"
#include "dtkCore/dtkAbstractDataFactory.h"
#include <dtkCore/dtkAbstractDataReader.h>
#include "dtkCore/dtkAbstractData.h"
#include "dtkCore/dtkAbstractViewFactory.h"
#include "dtkCore/dtkAbstractView.h"
#include "dtkCore/dtkSmartPointer.h"

#include <medViewPool.h>
#include <medAbstractView.h>

#include <vtkImageEllipsoidSource.h>
#include <vtkImageView2D.h>
#include <vtkImageViewCornerAnnotation.h>
#include <vtkImageView2DCommand.h>
#include <vtkMath.h>

#include <time.h>

int v3dViewTest(int argc, char *argv[])
{
    if (argc<2) {
        qDebug() << "Not enough arguments";
        return EXIT_FAILURE;
    }

    QApplication application(argc, argv);

    dtkPluginManager::instance()->setPath (argv[1]);
    dtkPluginManager::instance()->initialize();

    dtkSmartPointer<dtkAbstractData> data;
    data = dtkAbstractDataFactory::instance()->createSmartPointer ("v3dDataImage");

    if (!data) {
        qDebug() << "Cannot create data object from plugin";
        return EXIT_FAILURE;
    }

    vtkImageEllipsoidSource* imageSource = vtkImageEllipsoidSource::New();
    imageSource->SetWholeExtent (0, 127, 0, 127, 0, 127);
    imageSource->SetCenter (64,64,64);
    imageSource->SetRadius (64,32,32);
    imageSource->Update();

    vtkImageData* image = imageSource->GetOutput();

    data->setData(image);

    medViewPool *pool = new medViewPool;

    dtkSmartPointer<medAbstractView> view = qobject_cast<medAbstractView*>(dtkAbstractViewFactory::instance()->createSmartPointer("v3dView"));
    if (!view) {
        qDebug() << "Cannot create view object from plugin";
        return EXIT_FAILURE;
    }

    dtkSmartPointer<medAbstractView> view2 =
            qobject_cast<medAbstractView*>(dtkAbstractViewFactory::instance()->createSmartPointer ("v3dView"));
    dtkSmartPointer<medAbstractView> view3 =
            qobject_cast<medAbstractView*>(dtkAbstractViewFactory::instance()->createSmartPointer ("v3dView"));
    dtkSmartPointer<medAbstractView> view4 =
            qobject_cast<medAbstractView*>(dtkAbstractViewFactory::instance()->createSmartPointer ("v3dView"));


    view->widget()->move(20,20);
    view->widget()->resize(316, 316);

    view2->widget()->move(336,20);
    view2->widget()->resize(316, 316);

    view3->widget()->move(20,336);
    view3->widget()->resize(316, 316);

    view4->widget()->move(336,336);
    view4->widget()->resize(316, 316);


    view->setProperty("ShowAnnotations", "false");
    view2->setProperty("ShowAnnotations", "false");
    view3->setProperty("ShowAnnotations", "false");
    view4->setProperty("ShowAnnotations", "false");

    view->setData ( data );
    view2->setData ( data );
    view3->setData ( data );
    view4->setData ( data );

    pool->appendView(view);
    pool->appendView(view2);
    pool->appendView(view3);
    pool->appendView(view4);

    view->setLinkCamera(true);
    view2->setLinkCamera(true);
    view3->setLinkCamera(true);
    view4->setLinkCamera(true);

    view->setLinkPosition(true);
    view2->setLinkPosition(true);
    view3->setLinkPosition(true);
    view4->setLinkPosition(true);

    view->setLinkWindowing(true);
    view2->setLinkWindowing(true);
    view3->setLinkWindowing(true);
    view4->setLinkWindowing(true);

    view->widget()->show();
    view2->widget()->show();
    view3->widget()->show();
    view4->widget()->show();

    //QMainWindow window;
    //window.setCentralWidget ( view->widget() );
    //window.show();
    //window.resize (256,256);

    view->setProperty ("Orientation", "Axial");
    view2->setProperty ("Orientation", "Sagittal");
    view3->setProperty ("Orientation", "Coronal");
    view4->setProperty ("Orientation", "3D");
    view4->setProperty ("3DMode", "MPR");
    view->setProperty("ShowRuler", "false");
    view2->setProperty("ShowRuler", "false");
    view3->setProperty("ShowRuler", "false");
    view4->setProperty("ShowRuler", "false");
    view->setProperty("ShowAxis", "true");
    view2->setProperty("ShowAxis", "true");
    view3->setProperty("ShowAxis", "true");
    view4->setProperty("ShowAxis", "true");

    /*
    view->setProperty("ScalarBarVisibility", "true");
    view2->setProperty("ScalarBarVisibility", "true");
    view3->setProperty("ScalarBarVisibility", "true");
    view4->setProperty("ScalarBarVisibility", "true");
*/

    vtkImageView2D* imageView = vtkImageView2D::SafeDownCast( (vtkObject*)view->view());
    if (!imageView)
        return EXIT_FAILURE;

    vtkImageView* imageView2 = vtkImageView::SafeDownCast( (vtkObject*)view2->view());
    vtkImageView* imageView3 = vtkImageView::SafeDownCast( (vtkObject*)view3->view());
    vtkImageView* imageView4 = vtkImageView::SafeDownCast( (vtkObject*)view4->view());

    view->reset();
    view2->reset();
    view3->reset();
    view4->reset();

    view->update();
    view2->update();
    view3->update();
    view4->update();

    const unsigned int N = 500;

    double positions[N][3];
    srand( time(NULL) );

    for (unsigned int i=0; i<N; i++)
    {
        const float scale1 = rand()/float(RAND_MAX);
        const float scale2 = rand()/float(RAND_MAX);
        const float scale3 = rand()/float(RAND_MAX);

        positions[i][0] = scale1 * 128;
        positions[i][1] = scale2 * 128;
        positions[i][2] = scale3 * 128;
    }

    clock_t t1 = clock();

    for( unsigned int i=0; i<N; i++)
    {
        imageView->SetCurrentPoint ( positions[i] );
        imageView->GetInteractorStyle()->InvokeEvent(vtkImageView2DCommand::SliceMoveEvent);

        view->update();
        view2->update();
        view3->update();
        view4->update();
    }

    double zfactor = 0.95;

    imageView->SetColorWindow (178);
    imageView->SetColorLevel (51);
    imageView2->SetColorWindow (178);
    imageView2->SetColorLevel (51);
    imageView3->SetColorWindow (178);
    imageView3->SetColorLevel (51);
    imageView4->SetColorWindow (178);
    imageView4->SetColorLevel (51);
    view->update();
    view2->update();
    view3->update();
    view4->update();


    unsigned int n_2 = vtkMath::Round ((double)(N) / 2.0);

    imageView->GetCornerAnnotation()->SetText (3, "SyncSetWindow (decreasing)");
    for( unsigned int i=0; i<n_2; i++){
        imageView2->SetColorWindow ( imageView->GetColorWindow() * zfactor );
        imageView3->SetColorWindow ( imageView->GetColorWindow() * zfactor );
        imageView4->SetColorWindow ( imageView->GetColorWindow() * zfactor );
        imageView->SetColorWindow ( imageView->GetColorWindow() * zfactor );

        view->update();
        view2->update();
        view3->update();
        view4->update();
    }

    imageView->SetColorWindow (178);
    imageView->SetColorLevel (51);
    imageView2->SetColorWindow (178);
    imageView2->SetColorLevel (51);
    imageView3->SetColorWindow (178);
    imageView3->SetColorLevel (51);
    imageView4->SetColorWindow (178);
    imageView4->SetColorLevel (51);
    view->update();
    view2->update();
    view3->update();
    view4->update();

    imageView->GetCornerAnnotation()->SetText (3, "SyncSetWindow (increasing)");
    for( unsigned int i=0; i<n_2; i++){
        imageView2->SetColorWindow ( imageView->GetColorWindow() / zfactor );
        imageView3->SetColorWindow ( imageView->GetColorWindow() / zfactor );
        imageView4->SetColorWindow ( imageView->GetColorWindow() / zfactor );
        imageView->SetColorWindow ( imageView->GetColorWindow() / zfactor );

        view->update();
        view2->update();
        view3->update();
        view4->update();
    }

    imageView->SetColorWindow (178);
    imageView->SetColorLevel (51);
    imageView2->SetColorWindow (178);
    imageView2->SetColorLevel (51);
    imageView3->SetColorWindow (178);
    imageView3->SetColorLevel (51);
    imageView4->SetColorWindow (178);
    imageView4->SetColorLevel (51);
    view->update();
    view2->update();
    view3->update();
    view4->update();


    imageView->GetCornerAnnotation()->SetText (3, "SyncSetLevel (decreasing)");
    for( unsigned int i=0; i<n_2; i++){
        imageView2->SetColorLevel ( imageView->GetColorLevel() * zfactor );
        imageView3->SetColorLevel ( imageView->GetColorLevel() * zfactor );
        imageView4->SetColorLevel ( imageView->GetColorLevel() * zfactor );
        imageView->SetColorLevel ( imageView->GetColorLevel() * zfactor );

        view->update();
        view2->update();
        view3->update();
        view4->update();
    }

    imageView->SetColorWindow (178);
    imageView->SetColorLevel (51);
    imageView2->SetColorWindow (178);
    imageView2->SetColorLevel (51);
    imageView3->SetColorWindow (178);
    imageView3->SetColorLevel (51);
    imageView4->SetColorWindow (178);
    imageView4->SetColorLevel (51);
    view->update();
    view2->update();
    view3->update();
    view4->update();

    imageView->GetCornerAnnotation()->SetText (3, "SyncSetLevel (increasing)");
    for( unsigned int i=0; i<n_2; i++){
        imageView2->SetColorLevel ( imageView->GetColorLevel() / zfactor );
        imageView3->SetColorLevel ( imageView->GetColorLevel() / zfactor );
        imageView4->SetColorLevel ( imageView->GetColorLevel() / zfactor );
        imageView->SetColorLevel ( imageView->GetColorLevel() / zfactor );

        view->update();
        view2->update();
        view3->update();
        view4->update();
    }

    imageView->GetCornerAnnotation()->SetText (3, "SyncReset ()");
    for( unsigned int i=0; i<N; i++){
        imageView->Reset();
        imageView->GetInteractorStyle()->InvokeEvent(vtkImageView2DCommand::ResetViewerEvent);
        imageView->Render();
    }



    /*
  view->setProperty ("Mode", "VR"); view->update();
  view->setProperty ("Cropping", "true"); view->update();
  view->setProperty ("Cropping", "false"); view->update();
  view->setProperty ("Mode", "MPR"); view->update();
  view->setProperty ("Mode", "MIP - Maximum"); view->update();
  view->setProperty ("Mode", "MIP - Minimum"); view->update();
  view->setProperty ("Orientation", "Axial"); view->update();
  view->setProperty ("ScalarBarVisibility", "true"); view->update();
  view->setProperty ("ScalarBarVisibility", "false"); view->update();
  view->setProperty ("LookupTable", "Black&White"); view->update();
  view->setProperty ("LookupTable", "Black&WhiteInversed"); view->update();
  view->setProperty ("LookupTable", "Spectrum"); view->update();
  view->setProperty ("LookupTable", "HotMetal"); view->update();
  view->setProperty ("LookupTable", "GE"); view->update();
  view->setProperty ("LookupTable", "Flow"); view->update();
  view->setProperty ("LookupTable", "Loni"); view->update();
  view->setProperty ("LookupTable", "Loni2"); view->update();
  view->setProperty ("LookupTable", "Asymmetry"); view->update();
  view->setProperty ("LookupTable", "PValue"); view->update();
  view->setProperty ("LookupTable", "blueBlackAlpha"); view->update();
  view->setProperty ("LookupTable", "greenBlackAlpha"); view->update();
  view->setProperty ("LookupTable", "redBlackAlpha"); view->update();
  view->setProperty ("LookupTable", "Muscles&Bones"); view->update();
  view->setProperty ("LookupTable", "Red Vessels"); view->update();
  view->setProperty ("LookupTable", "Bones"); view->update();
  view->setProperty ("LookupTable", "Stern"); view->update();
  view->setProperty ("Preset", "VR Muscles&Bones"); view->update();
  view->setProperty ("Preset", "Vascular I"); view->update();
  view->setProperty ("Preset", "Vascular II"); view->update();
  view->setProperty ("Preset", "Vascular III"); view->update();
  view->setProperty ("Preset", "Vascular IV"); view->update();
  view->setProperty ("Preset", "Standard"); view->update();
  view->setProperty ("Preset", "Soft"); view->update();
  view->setProperty ("Preset", "Soft on White"); view->update();
  view->setProperty ("Preset", "Soft on Blue"); view->update();
  view->setProperty ("Preset", "Red on White"); view->update();
  view->setProperty ("Preset", "Glossy"); view->update();
  view->setProperty ("ShowAxis" ,"true"); view->update();
  view->setProperty ("ShowAxis" ,"false"); view->update();
 */

    view->close();
    view2->close();
    view3->close();
    view4->close();

    clock_t t2 = clock();

    qDebug() << "Time: " << (double)(t2-t1)/(double)CLOCKS_PER_SEC;

    return EXIT_SUCCESS; //application.exec();

}
