/* itkDcmtkReaderTest.cxx --- 
 * 
 * Author: Pierre Fillard
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Oct  2 21:46:08 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Jul 29 00:50:04 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 132
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include <iostream>

#include <itkCommand.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include <vtkViewImage2D.h>
#include <vtkViewImage3D.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageReslice.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkImageMapToColors.h>
#include <vtkImageActor.h>
#include <vtkCallbackCommand.h>

#include <time.h>

#include <QtCore>
#include <QtGui>
#include <QVTKWidget.h>

namespace itk
{

    class SliceReadCommand : public Command
    {
        
    public:
        typedef SliceReadCommand               Self;
        typedef Command                        Superclass;
        typedef itk::SmartPointer<Self>        Pointer;
        typedef itk::SmartPointer<const Self>  ConstPointer;

        itkTypeMacro( SliceReadCommand, Command );
        itkNewMacro(Self);
        
        typedef itk::Image<float, 3> ImageType;
        
        void Execute(Object *caller, const EventObject &event);
        void Execute(const Object *caller, const EventObject &event);
        
        itkSetObjectMacro (Image, ImageType);
        itkGetObjectMacro (Image, ImageType);

        void SetView (vtkViewImage2D* view)
        { m_View = view; }
    
    protected:
        SliceReadCommand(){ this->ImageNotSet = 1; };
        virtual ~SliceReadCommand(){};

    private:
        ImageType::Pointer m_Image;
        vtkViewImage2D*    m_View;
        int ImageNotSet;
    
    };


    void SliceReadCommand::Execute (Object *caller, const EventObject &event)
    {
        static int slice = 0;

        std::cout << event.GetEventName() << " - Slice read: " << slice++ << std::endl;
        
            //qDebug() << "Slice read";
            // if( this->ImageNotSet ){
        m_View->SetITKImage(m_Image);

        qApp->processEvents();
        
            //     m_View->ResetCurrentPoint();
            //     m_View->ResetZoom();
            //     m_View->SetWindow (2320);
            //     m_View->SetLevel (1138);
            //     this->ImageNotSet = 0;
            // }
            // //m_View->GetImageReslice()->Modified();
            // m_View->GetWindowLevel()->Modified();
            // m_View->GetRenderWindow()->InvokeEvent ( m_View->GetRenderWindow()->GetEventPending() );
            // m_View->GetRenderWindowInteractor()->Render();
       
            //qDebug() << "Rendered!";
    }


    void SliceReadCommand::Execute (const Object *caller, const EventObject &event)
    {
            //qDebug() << "Slice read";
            // if( this->ImageNotSet ){
            //     m_View->SetITKImage ( m_Image );
            //     m_View->ResetCurrentPoint();
            //     m_View->ResetZoom();
            //     m_View->SetWindow (2320);
            //     m_View->SetLevel (1138);
            //     this->ImageNotSet = 0;
            // }
            // m_View->GetWindowLevel()->Modified();
            // m_View->GetRenderWindow()->InvokeEvent ( m_View->GetRenderWindow()->GetEventPending() );
            // m_View->GetRenderWindow()->Render();
  
    }
  
}


/*
typedef itk::Image<short, 3>   ImageType;
typedef itk::ImageFileReader<ImageType> ImageReaderType;
static ImageReaderType::Pointer reader    = 0;
static int                      readImage = 0;
*/


#include "itkMetaImageIO.h"
#include "itkStreamingImageFilter.h"

int main (int narg, char* arg[])
{
    QApplication app(narg, arg);
    
    typedef float         InputPixelType;
    typedef float         OutputPixelType;
    const   unsigned int  Dimension = 3;
    
    typedef itk::Image<InputPixelType,  Dimension>    InputImageType;
    typedef itk::Image<OutputPixelType, Dimension>    OutputImageType;    
    typedef itk::ImageFileReader<InputImageType>      ReaderType;
    typedef itk::StreamingImageFilter< OutputImageType, OutputImageType > StreamingFilterType;
    
    StreamingFilterType::Pointer streamer = StreamingFilterType::New();
    
    ReaderType::Pointer reader = ReaderType::New();
    
    const std::string inputFilename  = arg[1];
    
    reader->SetFileName(inputFilename );
    reader->SetUseStreaming(true);
    
    streamer->SetInput(reader->GetOutput());
    streamer->SetNumberOfStreamDivisions(100);
    // streamer->Update();    
    
// -----

    // if( narg<2 ) {
    //     std::cerr << "Usage: " << arg[0] << " file.mhd\n";
    //     return -1;
    // }



    // const char* filename = arg[1];

    // typedef itk::MetaImageIO ImageIOType;
    // ImageIOType::Pointer imageio = ImageIOType::New();
    // imageio->SetDimensions(0, 512);
    // imageio->SetDimensions(1, 512);
    // imageio->SetSpacing(0, 0.664062);
    // imageio->SetSpacing(1, 0.664062);
    // // imageio->SetHeaderSize(512*512*0);

    // typedef itk::Image<float, 3>            ImageType;
    // typedef itk::ImageFileReader<ImageType> ImageReaderType;

    // // typedef ImageType::RegionType     RegionType;
    // // typedef ImageType::IndexType      IndexType;
    // // typedef ImageType::SizeType       SizeType;


    // // IndexType index = {{0,0,128}};
    // // SizeType size = {{512,512,1}};
    
    // // RegionType region;
    // // region.SetIndex(index);
    // // region.SetSize(size);

    // ImageReaderType::Pointer reader = ImageReaderType::New();
    // reader->SetFileName(filename);
    // // reader->SetImageIO(imageio);
    // // reader->GetOutput()->SetRequestedRegion ( region );

    // itk::StreamingImageFilter<ImageType, ImageType> *streamer = itk::StreamingImageFilter<ImageType, ImageType>::New();
    // streamer->SetInput(reader->GetOutput());
    // streamer->SetNumberOfStreamDivisions(100);

    // try {
    //   streamer->Update();
    // }
    // catch (itk::ExceptionObject &e)
    // {
    //   std::cerr << e;
    //   return -1;
    // }
    
    OutputImageType::Pointer image = streamer->GetOutput();
    // image->DisconnectPipeline();
    
    std::cout << image << std::endl;
    
    vtkViewImage2D* view = vtkViewImage2D::New();
    // vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
    // vtkRenderWindow* rwin = vtkRenderWindow::New();
    vtkRenderer* renderer = vtkRenderer::New();

    QVTKWidget widget;
    widget.SetRenderWindow(view->GetRenderWindow());
    widget.resize(256,256);
    widget.show();


    // iren->SetRenderWindow (rwin);
    // rwin->AddRenderer (renderer);
    view->SetRenderWindowInteractor(widget.GetRenderWindow()->GetInteractor());
    view->SetRenderWindow (widget.GetRenderWindow());
    // view->UnInstallInteractor();

    view->SetRenderer ( renderer );
    view->SetBackgroundColor (0.0, 0.0, 0.0);
    view->Show2DAxisOff();
    view->CursorFollowMouseOn();
    view->SetInterpolationMode(0);

    view->SetLeftButtonInteractionStyle   (vtkViewImage2D::SELECT_INTERACTION);
    view->SetMiddleButtonInteractionStyle (vtkViewImage2D::SELECT_INTERACTION);
    view->SetWheelInteractionStyle        (vtkViewImage2D::SELECT_INTERACTION);
    view->SetRightButtonInteractionStyle  (vtkViewImage2D::WINDOW_LEVEL_INTERACTION);

    itk::SliceReadCommand::Pointer command = itk::SliceReadCommand::New();
    command->SetImage (image);
    command->SetView (view);
    streamer->AddObserver(itk::ProgressEvent(), command);
    streamer->AddObserver(itk::ModifiedEvent(), command);   

    /*
      itk::SliceReadCommand::Pointer command = itk::SliceReadCommand::New();
      command->SetImage ( reader->GetOutput() );
      command->SetView ( view );
      io->AddObserver (itk::SliceReadEvent(), command);      
    */
  

    view->ResetCurrentPoint();
    view->ResetZoom();
    view->SetWindow (2320);
    view->SetLevel (1138);

    // iren->Initialize();
    // iren->Start();

    // view->SetITKImage (image);

    // if (view->GetImage())
    //   std::cout << *view->GetImage() << std::endl;

    streamer->Update();

    int status = app.exec();

    /*
      clock_t t1 = clock();
      try
      {
      reader->Update();
      }
      catch (itk::ExceptionObject &e)
      {
      std::cerr << e;
      view->Delete();
      iren->Delete();
      renderer->Delete();
      rwin->Delete();
      return -1;
      }
      clock_t t2 = clock();
  
  
      std::cout << "Threads: " << io->GetNumberOfThreads() << " Elapsed time:" << (double)(t2-t1)/(double)(CLOCKS_PER_SEC) << std::endl;

      image->DisconnectPipeline();
      std::cout << image << std::endl;
    */

    //iren->Start();


    view->Delete();
    // iren->Delete();
    renderer->Delete();
    // rwin->Delete();
  
    /*  
 
        typedef itk::ImageFileWriter<ImageType> WriterType;
        WriterType::Pointer writer = WriterType::New();
        writer->SetFileName ("test.nii.gz");
        writer->SetInput ( image );
        writer->Update();

    */
  
    return 0;  
}
