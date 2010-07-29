// itkStreamedReaderTest2.cxx --- 
// 
// Author: Julien Wintz
// Copyright (C) 2008 - Julien Wintz, Inria.
// Created: Thu Jul 29 14:42:36 2010 (+0200)
// Version: $Id$
// Last-Updated: Thu Jul 29 14:43:00 2010 (+0200)
//           By: Julien Wintz
//     Update #: 1
// 

// Commentary: 
// 
// 

// Change log:
// 
// 

#include <QtCore>
#include <QtGui>
#include <QtDebug>

#include "vtkImageViewer2.h"
#include "vtkInteractorStyleImage.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageToVTKImageFilter.h"
#include "itkStreamingImageFilter.h"

#include "vtkImageView2D.h"

#include "QVTKWidget.h"

// /////////////////////////////////////////////////////////////////
// main
// /////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    
    // --- Read using itk

    typedef float InputPixelType;
    typedef float OutputPixelType;
    const unsigned int  Dimension = 3;
    typedef itk::Image<InputPixelType, Dimension> InputImageType;
    typedef itk::Image<OutputPixelType, Dimension> OutputImageType;    
    typedef itk::ImageFileReader<InputImageType> ReaderType;
    typedef itk::StreamingImageFilter<OutputImageType, OutputImageType> StreamingFilterType;
    typedef itk::ImageToVTKImageFilter<OutputImageType> ConverterType;
        
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(argv[1]);
    reader->SetUseStreaming(true);

    // -- The following is actually not needed anymore since itk 3.12
    //
    // StreamingFilterType::Pointer streamer = StreamingFilterType::New();
    // streamer->SetInput(reader->GetOutput());
    // streamer->SetNumberOfStreamDivisions(100);
    // streamer->Update();
    
    ConverterType::Pointer converter = ConverterType::New();
    converter->SetInput(reader->GetOutput());

    // // --- View using vtk
    
    vtkImageViewer2 *view = vtkImageViewer2::New();
    view->SetInput(converter->GetOutput());

    QVTKWidget widget;
    widget.resize(800, 800);    
    widget.SetRenderWindow(view->GetRenderWindow());

    view->SetupInteractor(widget.GetInteractor());
    view->SetColorLevel(1300);
    view->SetColorWindow(2300);

    widget.show();

    for(int slice = 3900; slice > 0; slice-=10) {
        view->SetSlice(slice);
        view->Render();
    }

    for(int slice = 0; slice < 3900; slice+=10) {
        view->SetSlice(slice);
        view->Render();
    }

    for(int slice = 3900; slice > 0; slice-=10) {
        view->SetSlice(slice);
        view->Render();
    }

    for(int slice = 0; slice < 3900; slice+=10) {
        view->SetSlice(slice);
        view->Render();
    }

    for(int slice = 3900; slice > 0; slice-=10) {
        view->SetSlice(slice);
        view->Render();
    }

    for(int slice = 0; slice < 3900; slice+=10) {
        view->SetSlice(slice);
        view->Render();
    }
    
    // -- View using v3d

    // vtkRenderWindow *rwin = vtkRenderWindow::New();

    // vtkImageView2D *view = vtkImageView2D::New();
    // view->SetRenderWindow(rwin);
    // view->SetInput(converter->GetOutput());

    // QVTKWidget widget;
    // widget.resize(800, 800);    
    // widget.SetRenderWindow(view->GetRenderWindow());

    // view->SetupInteractor(widget.GetInteractor());
    // view->SetColorLevel(1300);
    // view->SetColorWindow(2300);

    // widget.show(); qApp->processEvents();

    // for(int slice = 3900; slice > 0; slice-=10) {
    //     view->SetSlice(slice);
    //     view->Render();
    // }

    // for(int slice = 0; slice < 3900; slice+=10) {
    //     view->SetSlice(slice);
    //     view->Render();
    // }

    // for(int slice = 3900; slice > 0; slice-=10) {
    //     view->SetSlice(slice);
    //     view->Render();
    // }

    // for(int slice = 0; slice < 3900; slice+=10) {
    //     view->SetSlice(slice);
    //     view->Render();
    // }

    // for(int slice = 3900; slice > 0; slice-=10) {
    //     view->SetSlice(slice);
    //     view->Render();
    // }

    // for(int slice = 0; slice < 3900; slice+=10) {
    //     view->SetSlice(slice);
    //     view->Render();
    // }

    // --

    int status = app.exec();

    view->Delete();
    
    return status;
}
