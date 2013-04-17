/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <cstddef> // For ITK 3.20 that does not define correctly ptrdiff_t

#include "itkCommand.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageSeriesReshapeFilter.h"

#include <itkAnalyzeImageIOFactory.h>
#include <itkGDCMImageIOFactory.h>

#ifndef WIN32
#include <getopt.h>
#else
#include "getoptcompat/getopt.h"
#endif //WIN32
#include <iostream>


struct arguments
{
   std::string  InputImageFileList;  /* -i option */
   std::string  OutputImageFilePrefix; /* -o option */

   arguments () :
      InputImageFileList(""),
      OutputImageFilePrefix("")
   {
   }

   friend std::ostream& operator<< (std::ostream& o, const arguments& args)
   {
      std::ostringstream osstr;
      std::string iterstr = "[ " + osstr.str() + "]";

         
      return o
         <<"Arguments structure:"<<std::endl
	 <<"  Input list: "<<args.InputImageFileList<<std::endl	
         <<"  Output prefix: "<<args.OutputImageFilePrefix;
   }
};

static const char *optString = "i:p:h";

static const struct option longOpts[] = {
   { "input-image-list", required_argument, NULL, 'i' },
   { "output-image-prefix", required_argument, NULL, 'p' },
   { NULL, no_argument, NULL, 0 }
};

/*
  Display program usage, and exit.
*/
void display_usage( const std::string progname )
{   
   struct arguments defargs = arguments();

   std::ostringstream osstr;
   std::string iterstr = "[ " + osstr.str() + "]";
   
   std::cout<<std::endl;
   std::cout<<progname<<" - reshape a list of images into another"<<std::endl;
   std::cout<<"Usage: "<<progname<<" [OPTION...]"<<std::endl;
   
   std::cout<<"  -i/--input-image-list=STRING    input image list - mandatory"<<std::endl;
   std::cout<<"  -p/--output-image-prefix=STRING output image prefix - mandatory"<<std::endl;

   std::cout<<std::endl;
   std::cout<<"Copyright (c) 2010 INRIA."<<std::endl;
   std::cout<<"Code: Nicolas Toussaint."<<std::endl;
   std::cout<<"Report bugs to <nicolas.toussaint@kcl.ac.uk>."<<std::endl;
   
   exit( EXIT_FAILURE );
}


void parseOpts (int argc, char **argv, struct arguments & args)
{
   const std::string progname( "itkReshaper" );

   // Default values.
   args = arguments();

   if (argc == 1)
   {
     display_usage(progname);
   }
   
   int opt = 0; /* it's actually going to hold a char */
   int longIndex = 0;

   while ( (opt = getopt_long(argc, argv, optString, longOpts, &longIndex)) != -1 )
   {
      switch( opt )
      {
	  case 'i':
	    if (! optarg) display_usage(progname);
	    args.InputImageFileList = optarg;
	    break;
	    
	  case 'p':
	    std::cout<<"in p before "<<std::endl;
	    if (! optarg) display_usage(progname);
	    args.OutputImageFilePrefix = optarg;
	    std::cout<<"in p "<<args.OutputImageFilePrefix<<std::endl;
	    break;
	    
	  case 'h':	/* fall-through is intentional */
	  case '?':   /* fall-through is intentional */
	  default:
	    display_usage(progname);
	    break;
      }
   }
   
}



int main( int argc, char *argv[] )
{
   struct arguments args;
   parseOpts (argc, argv, args);
   std::cout<<"parsed"<<std::endl;
   
   itk::MultiThreader::SetGlobalDefaultNumberOfThreads(2);
   typedef itk::Image<short, 3> ImageType;
   typedef itk::ImageFileReader<ImageType> ReaderType;
   typedef itk::ImageFileWriter<ImageType> WriterType;
   typedef itk::ImageSeriesReshapeFilter<ImageType, ImageType> ReshaperType;
   typedef ReshaperType::InputImageListType ImageListType;
   
   std::cout<<"Starting reshaping with the following arguments:"<<std::endl;
   std::cout<<args<<std::endl<<std::endl;

   // open the file:
   std::ifstream filelist (args.InputImageFileList.c_str());
   
   if(filelist.fail())
   {
     std::cerr << "Unable to open file: " << args.InputImageFileList.c_str() << std::endl;
     exit (EXIT_FAILURE);
   }
   
   
   unsigned int NumberOfImages = 0;
   filelist >> NumberOfImages;
   std::string sline = "";
   itksys::SystemTools::GetLineFromStream(filelist, sline);


   ImageListType inputs;
   
   for (unsigned int i=0; i<NumberOfImages; i++)
   {
     ReaderType::Pointer reader = ReaderType::New();
     std::string filename;
     itksys::SystemTools::GetLineFromStream(filelist, filename);
     std::cout << "Reading file: " << filename.c_str() << std::endl;
     reader->SetFileName (filename.c_str() );
     try
     {
       reader->Update();
     }
     catch(itk::ExceptionObject &e)
     {
       std::cerr << e;
       exit( EXIT_FAILURE );
     }

     inputs.push_back (reader->GetOutput());
   }

   ReshaperType::Pointer reshaper = ReshaperType::New();
   reshaper->SetInputImageList (inputs);
   reshaper->SetInput (inputs[0]);
   
   try
   {
     reshaper->Update();
   }
   catch(itk::ExceptionObject &e)
   {
     std::cerr << e;
     exit( EXIT_FAILURE );
   }

   ImageListType outputs = reshaper->GetOutputImageList();
   
   for (unsigned int i=0; i<outputs.size(); i++)
   {
     WriterType::Pointer writer = WriterType::New();
     itksys_ios::ostringstream filename;
     filename << args.OutputImageFilePrefix;
     if (i < 10)
       filename << "0";
     filename << i + 1 << ".mha";

     std::cout << "Writing file: " << filename.str().c_str() << std::endl;
     writer->SetFileName (filename.str().c_str());
     writer->SetInput (outputs[i]);
     
     try
     {
       writer->Update();
     }
     catch(itk::ExceptionObject &e)
     {
       std::cerr << e;
       exit( EXIT_FAILURE );
     }
   }
   
   return EXIT_SUCCESS;
}

