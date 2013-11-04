/*=========================================================================

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkGDCMImporter.h>

#ifndef WIN32
#include <getopt.h>
#else
#include "getoptcompat/getopt.h"
#endif //WIN32

#include <iostream>
#include <cstdlib>
#include <fstream>

struct arguments
{
  std::string  InputDirectory;                   /* -i option */
  std::string  OutputDirectory;                  /* -o option */
  std::string  FileExtension;                    /* -e option */
  unsigned int WriteDictionaries;                /* -d option */
  unsigned int UsePhilipsPrivateTagRestrictions; /* -p option */
  unsigned int Verbose;                          /* -v option */
  
  arguments () :
    InputDirectory(""),
    OutputDirectory("./"),
    FileExtension(".mha"),
    WriteDictionaries(0),
    UsePhilipsPrivateTagRestrictions(0),
    Verbose(0)
  {
  }

  friend std::ostream& operator<< (std::ostream& o, const arguments& args)
  {
    std::ostringstream osstr;
    std::string iterstr = "[ " + osstr.str() + "]";

    return o
      <<"Arguments structure:"<<std::endl
      <<"  InputDirectory: "<<args.InputDirectory<<std::endl
      <<"  OutputDirectory: "<<args.OutputDirectory<<std::endl
      <<"  FileExtension: "<<args.FileExtension<<std::endl
      <<"  WriteDictionaries: "<<args.WriteDictionaries<<std::endl
      <<"  UsePhilipsPrivateTagRestrictions: "<<args.UsePhilipsPrivateTagRestrictions<<std::endl
      <<"  Verbose: "<<args.Verbose<<std::endl;
    
  }
};


static const char *optString = "i:o:e:d:p:v:h?";

static const struct option longOpts[] = {
  { "input-directory", required_argument, NULL, 'i' },
  { "output-directory", optional_argument, NULL, 'o' },
  { "file-extension", optional_argument, NULL, 'e' },
  { "write-dictionaries", optional_argument, NULL, 'd' },
  { "use-philips-restrictions", optional_argument, NULL, 'p' },
  { "verbose", optional_argument, NULL, 'v' },
  { "help", no_argument, NULL, 'h' },
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
  std::cout<<progname<<" - Reconstruct volume images from DICOM exam"<<std::endl;
  std::cout<<"Usage: "<<progname<<" [OPTION...]"<<std::endl;

  std::cout<<"  -i\t Input directory, root path to the DICOM exam - mandatory"<<std::endl;
  std::cout<<"  -o\t Output directory, where volume images shall be written (default: current directory)"<<std::endl;
  std::cout<<"  -e\t Output image file extension (default: '.mha')"<<std::endl;
  std::cout<<"  -d\t Write dicom dictionary associated with each volume (default: 0)"<<std::endl;
  std::cout<<"  -p\t Use Philips Restrictions for Cardiac Diffusion Interlaced (default: 0)"<<std::endl;
  std::cout<<"  -v\t Display more detailed messages during DICOM extraction (default: 0)"<<std::endl;
  std::cout<<"  -h\t Display this message and exit"<<std::endl;

  std::cout<<std::endl;
  std::cout<<"Copyright (c) 2013 UCL."<<std::endl;
  std::cout<<"Code: Nicolas Toussaint."<<std::endl;
  std::cout<<"Report bugs to <n.toussaint@ucl.ac.uk>."<<std::endl<<std::endl<<std::endl;

  exit( EXIT_FAILURE );
}


void parseOpts (int argc, char **argv, struct arguments & args)
{
  const std::string progname( "gdcmimport" );

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
    switch( opt ) {
	case 'i':
	  if (! optarg) display_usage(progname);
	  args.InputDirectory = optarg;
	  break;
	case 'o':
	  if (! optarg) display_usage(progname);
	  args.OutputDirectory = optarg;
	  break;
	case 'e':
	  if (! optarg) display_usage(progname);
	  args.FileExtension = optarg;
	  break;
	case 'd':
	  if (! optarg) display_usage(progname);
	  args.WriteDictionaries = std::atoi (optarg);
	  break;
	case 'p':
	  if (! optarg) display_usage(progname);
	  args.UsePhilipsPrivateTagRestrictions = std::atoi (optarg);
	  break;
	case 'v':
	  if (! optarg) display_usage(progname);
	  args.Verbose = std::atoi (optarg);
	  break;
	case 'h':	/* fall-through is intentional */
	case '?':       /* fall-through is intentional */
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

  if (args.Verbose)
  {
    itk::MultiThreader::SetGlobalDefaultNumberOfThreads(1);
    itk::Object::GlobalWarningDisplayOn();
  }
  
  std::cout<<"Starting GDCM DICOM import with the following arguments:"<<std::endl;
  std::cout<<args<<std::endl<<std::endl;
  
  typedef itk::GDCMImporter<short> ImporterType;
  ImporterType::Pointer importer = ImporterType::New();
  
  try
  {
    importer->SetDebug (args.Verbose);
    importer->SetUsePhilipsPrivateTagRestrictions(args.UsePhilipsPrivateTagRestrictions);
    importer->SetPreferredExtension (args.FileExtension);
    importer->SetWriteDictionaries (args.WriteDictionaries);
    importer->SetInputDirectory (args.InputDirectory);
    importer->Scan();
    importer->Update();

    std::string directory = itksys::SystemTools::GetCurrentWorkingDirectory();
    if (args.OutputDirectory.size())
      directory = args.OutputDirectory;    

    char last = directory[directory.size()-1];
#if defined(_WIN32) && !defined(__CYGWIN__)
    if (last != '\\')
      directory+="\\";
#else
    if (last != '/')
      directory+="/";
#endif
    
    importer->WriteOutputsInDirectory (directory.c_str());
  }
  catch(itk::ExceptionObject &e)
  {
    std::cerr << e;
    exit( EXIT_FAILURE );
  }


  return EXIT_SUCCESS;
}

