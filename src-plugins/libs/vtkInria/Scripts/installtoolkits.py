#!/usr/bin/python

#####################################################################################
#
#                         Cross-Platform ToolKit-Install
#   creating all source and build architecture for ToolKits needed for vtkINRIA3D
#                   configure cmake variables and make toolkits
#
#                             09/2008 N. Toussaint
#####################################################################################



# READ ME FIRST :

# 1. Script will build a tree architecture /src /build for each toolkit (exept GMM which only consists on include files)
# 2. If /src exists it will not extract sources from archives
# 3. On the other hand it will do the makefile configuration anyway
# 4. If your python version is strictly lower than 2.5, 
# the script will not be able to extract sources from archives
# Hence you will have to manually extract sources and move them in the /src 
# directory, following the extact same synthax as the script does.
# 5. On windows systems, don't bother downloading neither 
# vtkmpegencode nor ffmpeg, it will not be used
# 6. On windows systems, you will have to download visual express, cmake, 
# python client, and svn client prior to use this script


# 7. IMPORTANT : on mac systems, please provide the option --macmode
# (=carbon or =X11). If using carbon, please don't provide KWWidgets
# On the other hand, if using X11, please don't provide wxWidgets




# here are useful urls :

# VTK : http://www.vtk.org/get-software.php (download the source code:	vtk-X.X.X.tar.gz)
# ITK : http://www.itk.org/HTML/Download.htm (download InsightToolkit-X.X.X.tar.gz)
# KWWidgets : http://www.kwwidgets.org/Wiki/KWWidgets/Download (download KWWidgetsNightlyCVS.tar.gz)
# wxWidgets : http://www.wxwidgets.org/downloads/ (download, for Windows, the source archives in ZIP format: wxMSW, and for Linux, the source archives wxGTK)
# GMM : http://home.gna.org/getfem/download.html (download gmm-X.X.tar.gz)
# tcl tk : http://www.tcl.tk/
# python : http://www.python.org/download/
# svn : http://subversion.tigris.org/
# svn client for windows : http://www.sliksvn.com/en/home/download/
# cmake : http://www.cmake.org/cmake/resources/software.html
# Visual C++ Express Edition for windows : http://www.microsoft.com/express/vc/
# mac xcode : http://developer.apple.com/tools/xcode/
# mac X11 : http://www.apple.com/support/downloads/x11formacosx.html











import os
import os.path
import sys
import platform
import tarfile
import shutil
import zipfile
def _extract_all(self, destdir):
    namelist = self.namelist()
    namelist.sort()
    for name in namelist:
        if name.endswith('/'):
            print name
            os.mkdir(os.path.join(destdir, name))
        else:
            outfile = open(os.path.join(destdir, name), 'wb')
            outfile.write(self.read(name))
            outfile.close()
zipfile.ZipFile.extract_all = _extract_all
def unzip(src, dest):
    zipo = zipfile.ZipFile(src, 'r')
    zipo.extract_all(dest)


from optparse import OptionParser, SUPPRESS_HELP


ARCHITECTURE = ''

if sys.platform.find('darwin') != -1:
  ARCHITECTURE='mac'
else:
  if sys.platform.find('win') != -1:
    ARCHITECTURE='win'
  else:
    ARCHITECTURE='linux'
  if platform.architecture()[0].find('64') != -1:
    ARCHITECTURE=ARCHITECTURE+'64'
  else:
    ARCHITECTURE=ARCHITECTURE+'32'


usage=  '%prog [options]\n'\
        '\n'\
        'This script can be use to install toolkits such as ITK or VTK on your systems\n'\
        'It will create a subdirectory called "ToolKits"\n'\
        'For each ToolKit archive provided, it will create a directory containing sources and build trees, \n'\
        'with the correct cmake varialbes, and make them\n'\
        '\n'\
        'Example : \n'\
        '%prog --archives=<ArchivesPath> -s -r -n 2 -d <DestinationPath>\n'\
        'will take every archive (*.tar.gz) found in <ArchivesPath> and build the corresponding \n'\
	'ToolKit trees in <DestinationPath> folder, configure and make them all in a shared_lib and release mode, taking 2 processors \n'\
        '\n'\
	'Please type -h or --help for details\n'\
	
version = '%prog 1.0'
parser = OptionParser(usage=usage, version = version)


parser.add_option("-a","--archives", dest="archives",
                  help="provide path to the the directory containing toolokit source archives (default is current directory)")
parser.add_option("-d","--destination", dest="destination",
                  help="MANDATORY : provide the destination path : path from where a directory 'ToolKits' will be created  ")
parser.add_option("-i","--itkarchive", dest="itkarchive",
                  help="provide path to the ITK archive file (e.g. InsightToolKitX.X.X.tar.gz). If provided, this file will be taken even if a similar one is found in --archives")
parser.add_option("-v","--vtkarchive", dest="vtkarchive",
                  help="provide path to the VTK archive file")
parser.add_option("-k","--kwwidgetsarchive", dest="kwwidgetsarchive",
                  help="provide path to the KWWidgets archive file")
parser.add_option("-w","--wxwidgetsarchive", dest="wxwidgetsarchive",
                  help="provide path to the wxWidgets archive file")
parser.add_option("-g","--gmmarchive", dest="gmmarchive",
                  help="provide path to the GMM include dir")
parser.add_option("-f","--ffmpegarchive", dest="ffmpegarchive",
                  help="provide path to the L-GPL ffmpeg archive (used under unix systems in VTK to write movie files) -- not necessary on win32")
parser.add_option("-p","--mpegarchive", dest="mpegarchive",
                  help="provide path to the vtkmpeg2encode archive (used under unix systems in VTK to write movie files) -- not necessary on win32")
if ARCHITECTURE == 'linux32' or ARCHITECTURE == 'linux64':
  parser.add_option("--checkoutffmpeg", dest="checkoutffmpeg", action="store_true", default=False,
                    help="force the svn checkout of FFMPEG L-GPL library code. default is off")
if ARCHITECTURE == 'mac':
  parser.add_option("--macmode", dest="macmode", default='carbon',
                    help="Indicate build mode for MacOSX (X11/carbon) -- default is carbon")
  parser.add_option("--enable-universal-binaries", dest="universalbinaries", action="store_true", default=False,
                    help="Build all in universal binaries (default is True)")
  parser.add_option("--disable-universal-binaries", dest="universalbinaries", action="store_false", default=True,
                    help="Disable the universal binaries")

parser.add_option("-s","--sharedlibs", dest="sharedlibs", action="store_true", default=False,
                  help="Build everything in shared libraries (default in false)")
parser.add_option("-r","--release", dest="release", action="store_true", default=False,
                  help="Build everything in release mode (default in false)")
parser.add_option("-n","--cpus", dest="cpus", default='1',
                  help="Indicate the amount of cpu in the computer (default is 1)")
parser.add_option("-q","--quiet", dest="configure", action="store_true", default=False,
                  help="If set then only configure (and generate) toolkits without making them")
if ARCHITECTURE == 'linux32' or ARCHITECTURE == 'linux64':
  parser.add_option("--keepgoing", dest="keepgoing", default=True,
                    help="If set to \"0\" then stop building when an error occurs (default is 1)")

if ARCHITECTURE == 'win32' or ARCHITECTURE == 'win64':
  parser.add_option("--make", dest="make",
                    help="provide path to make compiler executable file(default : vcbuild)")
  parser.add_option("--cmake", dest="cmake",
                    help="provide path to cmake executable file")
  parser.add_option("--generator", dest="generator",
                    help="provide the makefile generator type : \"NMake Makefiles\", \"Visual Studio 8 2005\" (default), etc")

(options, args) = parser.parse_args()

if options.destination == None:
  parser.error('Program requires a destination (-d) argument')

# common definitions

CMAKE_COMMAND = 'cmake'
MAKE_COMMAND = 'make'
SHARED_LIBS = '0'
RELEASE = ''
SVN_COMMAND='svn'


    
print 'found the architecture : ' + ARCHITECTURE


# For windows systems, nmake is the compiler included in Visual C++

if ARCHITECTURE == 'win32' or ARCHITECTURE == 'win64':
  GENERATOR_TYPE = "Visual Studio 8 2005"
  MAKE_COMMAND = 'nmake'
  VISUAL_BUILD_COMMAND = 'vcbuild /M'+options.cpus
  WIN32_CONFIGURATION = ''


# If provided, we have to use them :

  if options.cmake != None:
    CMAKE_COMMAND = options.cmake
  if options.make != None:
    MAKE_COMMAND = options.make
  if options.generator != None:
    GENERATOR_TYPE = options.generator


# If provided, we have to use them :

if options.sharedlibs == True:
  SHARED_LIBS = '1'
if options.release == True:
  RELEASE = 'Release'
  if ARCHITECTURE == 'win32' or ARCHITECTURE == 'win64':
    WIN32_CONFIGURATION = '"Release|Win32"'


# These values will be set to default if not provided

CPUS = options.cpus
MACMODE = 'carbon'
if ARCHITECTURE == 'mac':
  MACMODE = options.macmode
  
# For some reasons under windows systems, 
# if the user-provided paths contain spaces, 
# the commands will not recognize them,
# Hence we user 'call "<path>" instead

# noreover, we want to generate nmake makefiles
# to be able to call them from a command line

if ARCHITECTURE == 'win32' or ARCHITECTURE == 'win64':
  CMAKE_COMMAND = 'call "'+CMAKE_COMMAND+'" -G '+'"' + GENERATOR_TYPE + '"'
  SVN_COMMAND = 'call "'+SVN_COMMAND+'"'

# commoncmakeflags will include all cmake flags that are 
# common for every project but platform-specific (e.g. -mlong-branch for mac systems)

commoncmakeflags=[['CMAKE_BUILD_TYPE', 'STRING', RELEASE]]

if ARCHITECTURE == 'mac':
  if options.universalbinaries == True:
    commoncmakeflags.append(['CMAKE_OSX_ARCHITECTURES', 'STRING', '"i386;ppc"'])
  commoncmakeflags.append(['CMAKE_EXE_LINKER_FLAGS', 'STRING', '-bind_at_load'])
  if SHARED_LIBS == '0':
    commoncmakeflags.append(['CMAKE_C_FLAGS', 'STRING', '-mlong-branch'])
    commoncmakeflags.append(['CMAKE_CXX_FLAGS', 'STRING', '-mlong-branch'])

if ARCHITECTURE == 'linux64':
  commoncmakeflags.append(['CMAKE_C_FLAGS', 'STRING', '-fPIC'])
  commoncmakeflags.append(['CMAKE_CXX_FLAGS', 'STRING', '-fPIC'])




#----------------- ToolKit specific code -------------------- #

# Arbitrarily, we do not build tests and examples, basically for time reasons

BUILD_EXAMPLES = '0'
BUILD_TESTING = '0'

ARCHIVES_DIR = os.getcwd()
if options.archives != None:
  ARCHIVES_DIR = options.archives


def Find_Archive_In_Directory( string, directory, archive_extension='.tar.gz' ):
  for file in os.listdir(directory):
    if file.find(string) != -1:
      if file.find (archive_extension) != -1:
        return os.path.abspath(os.path.join(directory, file))
  return ''



ITK_ARCHIVE = ''

if options.itkarchive != None:
  ITK_ARCHIVE = os.path.abspath(options.itkarchive)
else:
  ITK_ARCHIVE = Find_Archive_In_Directory('Insight', ARCHIVES_DIR)

VTK_ARCHIVE = ''

if options.vtkarchive != None:
  VTK_ARCHIVE = os.path.abspath(options.vtkarchive)
else:
  VTK_ARCHIVE = Find_Archive_In_Directory('vtk-', ARCHIVES_DIR)

KW_ARCHIVE = ''

if options.kwwidgetsarchive != None:
  print 'archive given as ' + os.path.abspath(options.kwwidgetsarchive)
  KW_ARCHIVE = os.path.abspath(options.kwwidgetsarchive)
else:
  KW_ARCHIVE = Find_Archive_In_Directory('KWWidgets', ARCHIVES_DIR)

GMM_ARCHIVE = ''

if options.gmmarchive != None:
  GMM_ARCHIVE = os.path.abspath(options.gmmarchive)
else:
  GMM_ARCHIVE = Find_Archive_In_Directory('gmm', ARCHIVES_DIR)

WX_ARCHIVE = ''

if options.wxwidgetsarchive != None:
  WX_ARCHIVE = os.path.abspath(options.wxwidgetsarchive)
else:
  extension = '.tar.gz'
  if ARCHITECTURE == 'win32' or ARCHITECTURE == 'win64':
    extension = '.zip'
  WX_ARCHIVE = Find_Archive_In_Directory('wx', ARCHIVES_DIR, extension)

MPEG_ARCHIVE = ''

if options.mpegarchive != None:
  MPEG_ARCHIVE = os.path.abspath(options.vtkmpegarchive)
else:
  MPEG_ARCHIVE = Find_Archive_In_Directory('vtkmpeg', ARCHIVES_DIR)

FFMPEG_ARCHIVE = ''

if options.ffmpegarchive != None:
  FFMPEG_ARCHIVE = os.path.abspath(options.ffmpegarchive)
else:
  FFMPEG_ARCHIVE = Find_Archive_In_Directory('ffmpeg', ARCHIVES_DIR)




#----------------- Insight ToolKit -------------------------- #




# Copying common flags to itk-flags

itkflags=[['CMAKE_BUILD_TYPE', 'STRING', RELEASE]]
for i in range(1, len(commoncmakeflags)):
  itkflags.append(commoncmakeflags[i])

itkflags.append(['BUILD_EXAMPLES', 'BOOL', BUILD_EXAMPLES])
itkflags.append(['BUILD_TESTING', 'BOOL', BUILD_TESTING])
itkflags.append(['BUILD_SHARED_LIBS', 'BOOL', SHARED_LIBS])

# We have some trouble if CONCEPT_CHECKING is set to ON (which is the default)
# mostly because some of our code is not completely 'concept-clean'

itkflags.append(['ITK_USE_CONCEPT_CHECKING', 'BOOL', '0'])

# Since ITK-3.8, the DiffeomorphicDemons have been added to the review 
# subdirectory of ITK. So, if we want to have access to it, we need to 
# enable reviews.

itkflags.append(['ITK_USE_REVIEW', 'BOOL', '1'])
itkflags.append(['ITK_USE_ORIENTED_IMAGE_DIRECTION', 'BOOL', '0'])
itkflags.append(['ITK__IMAGE_BEHAVES_AS_ORIENTED_IMAGE', 'BOOL', '0'])

# Since ITK-3.10, ITK changed the image behaviour into the behaviour
# of the oriented images, taking into account the orientation information 
# for pixel value access.
# This will be critical in our work... 

#itkflags.append(['ITK_IMAGE_BEHAVES_AS_ORIENTED_IMAGE', 'BOOL', '0'])

# There is a strange lack in the linux64 machine installation
# making ITK doing a mistake in choosing the UUID library
# it should take the 64 version of it

if ARCHITECTURE == 'linux64':
  uuidlib = '/usr/lib64/libuuid.so'
  if os.path.exists('/usr/lib64/libuuid.so'):
    uuidlib = '/usr/lib64/libuuid.so'
  elif os.path.exists('/lib64/libuuid.so'):
    uuidlib = '/lib64/libuuid.so'
  elif os.path.exists('/lib64/libuuid.so.1'):
    uuidlib = '/lib64/libuuid.so.1'
  elif os.path.exists('/usr/lib/libuuid.so'):
    uuidlib = '/usr/lib/libuuid.so'

  itkflags.append(['UUID_LIBRARY', 'FILEPATH', uuidlib])







#----------------- Visualization ToolKit ------------------- #


# Copying common flags to vtk-flags

vtkflags=[['CMAKE_BUILD_TYPE', 'STRING', RELEASE]]
for i in range(1, len(commoncmakeflags)):
  vtkflags.append(commoncmakeflags[i])

vtkflags.append(['BUILD_EXAMPLES', 'BOOL', BUILD_EXAMPLES])
vtkflags.append(['BUILD_TESTING', 'BOOL', BUILD_TESTING])
vtkflags.append(['BUILD_SHARED_LIBS', 'BOOL', SHARED_LIBS])

# If KWWidgets has been required, then enable tcl-wrapping

if KW_ARCHIVE != '':
  if ARCHITECTURE != 'mac' or  MACMODE != 'carbon':
    vtkflags.append(['VTK_WRAP_TCL', 'BOOL', '1'])

# We arbitrarily set the VTK_USE_VIDEO_FOR_WINDOWS to ON.
# It does not impact to the computation time, neither on the licensing
# and it makes the vtkAVIWriter class available

if ARCHITECTURE == 'win32' or ARCHITECTURE == 'win64':
  vtkflags.append(['VTK_USE_VIDEO_FOR_WINDOWS', 'BOOL', '1'])

  
# Now we enter the underground of cmake flags
# On mac systems, we basically have two options :
# 1. We want to build a VTK carbon version, (for instance to get wx-examples running)
# 2. We want to build a VTK X11 version, (for instance to get the KW-examples running)
# unfortunately these versions are not compatible one to the other
# the user will need to separate builds

if ARCHITECTURE == 'mac':
  if MACMODE == 'carbon':
    vtkflags.append(['VTK_USE_CARBON', 'BOOL', '1'])
    vtkflags.append(['VTK_USE_COCOA', 'BOOL', '0'])
    vtkflags.append(['VTK_USE_X', 'BOOL', '0'])
  else:
    vtkflags.append(['VTK_USE_CARBON', 'BOOL', '0'])
    vtkflags.append(['VTK_USE_COCOA', 'BOOL', '0'])
    vtkflags.append(['VTK_USE_X', 'BOOL', '1'])
    vtkflags.append(['X11_X11_LIB','FILEPATH','/usr/X11R6/lib/libX11.dylib'])
    vtkflags.append(['X11_Xext_LIB','FILEPATH','/usr/X11R6/lib/libXext.dylib'])
    vtkflags.append(['X11_Xft_LIB','FILEPATH','/usr/X11R6/lib/libXft.dylib'])
    vtkflags.append(['X11_Xt_LIB','FILEPATH','/usr/X11R6/lib/libXt.dylib'])
    vtkflags.append(['X11_Xscreensaver_LIB','FILEPATH','/usr/X11R6/lib/libXss.dylib'])
    vtkflags.append(['X11_fontconfig_LIB','FILEPATH','/usr/X11R6/lib/libfontconfig.dylib'])
    vtkflags.append(['X11_X11_INCLUDE_PATH','PATH','/usr/include'])
    vtkflags.append(['X11_Xutil_INCLUDE_PATH','PATH','/usr/include'])

# Apparently we need to re-set the openGL flags for VTK, 
# default ones are not OK
    
    vtkflags.append(['OPENGL_INCLUDE_DIR', 'PATH', '/usr/X11R6/include'])
    vtkflags.append(['OPENGL_gl_LIBRARY', 'PATH', '/usr/X11R6/lib/libGL.a'])
    vtkflags.append(['OPENGL_glu_LIBRARY', 'PATH', '/usr/X11R6/lib/libGLU.a'])
  










#----------------- KitWare Widgets --------------------------- #



kwwidgetsflags=[['CMAKE_BUILD_TYPE', 'STRING', RELEASE]]
for i in range(1, len(commoncmakeflags)):
  kwwidgetsflags.append(commoncmakeflags[i])

kwwidgetsflags.append(['BUILD_EXAMPLES', 'BOOL', BUILD_EXAMPLES])
kwwidgetsflags.append(['BUILD_TESTING', 'BOOL', BUILD_TESTING])

# KWWidgets will find any other cmake variables in the VTK cmake configuration









#----------------- vtk-mpeg-encode --------------------------- #



mpegflags=[['CMAKE_BUILD_TYPE', 'STRING', RELEASE]]
for i in range(1, len(commoncmakeflags)):
  mpegflags.append(commoncmakeflags[i])








# --------------- extracting toolkits, configuring and generating makefiles... ---- #



# this little function will do automatically all the work

def Configure_ToolKit (TOOLKIT_ARCHIVE, TOOLKIT_NAME, TOOLKIT_FLAGS, IS_EXTRACTED_DIR = True):

  if TOOLKIT_ARCHIVE == '':
    return ''
  else:
    print TOOLKIT_NAME+' archive was found : '+TOOLKIT_ARCHIVE
    if not os.path.exists(os.path.abspath(TOOLKIT_NAME)):
      os.mkdir(TOOLKIT_NAME)
    os.chdir(TOOLKIT_NAME)
    mytarfile = tarfile.open(TOOLKIT_ARCHIVE, 'r:gz')
    if os.path.exists(os.path.abspath('src')):
      print 'The directory ' + os.path.abspath('src') + ' already exists, we don\'t extract anything'
    else:
      if IS_EXTRACTED_DIR:
        mytarfile.extractall(os.getcwd())
        os.rename(os.listdir(os.getcwd())[0], 'src')
      else:
        os.mkdir('src')
        mytarfile.extractall(os.path.abspath('src'))
        
    if not os.path.exists(os.path.abspath('build')):
      os.mkdir('build')
    
    os.chdir('build')
    command=CMAKE_COMMAND+' '
    for i in range(len(TOOLKIT_FLAGS)):
      flag=TOOLKIT_FLAGS[i]
      command=command+'-D'+flag[0]+':'+flag[1]+'='+flag[2]+' '
    command=command+'../src'
    os.system(command)
    if ARCHITECTURE == 'win32' or ARCHITECTURE == 'win64':
      if options.configure == False:
        if GENERATOR_TYPE == 'NMake Makefiles':     
          os.system(MAKE_COMMAND + ' -f Makefile' )
        else:
          os.system(VISUAL_BUILD_COMMAND + ' ' + TOOLKIT_NAME + '.sln ' + WIN32_CONFIGURATION)
      else:
        print 'not making yet'
    else:
      if options.configure == False:
        os.system(MAKE_COMMAND+' -j'+options.cpus + ' -k')
      else:
        print 'not making yet'
  
    builddir = os.getcwd()
    os.chdir('..')
    os.chdir('..')
    return builddir













#----------------- Projects configuration --------------------------- #


# We create subdirectory called 'ToolKits'

if options.destination != None:
  os.chdir(os.path.abspath(options.destination))

if not os.path.exists(os.path.abspath('ToolKits')):
  os.mkdir('ToolKits')
os.chdir('ToolKits')


# we will begin with stand-alone toolkits that require
# special attention.



#----------------- GMM (getfem) --------------------------- #


# GMM ToolKit is only an include diractory,
# we only have to extract the include directory from the archive

gmmincludedir = ''
if GMM_ARCHIVE != '':
  if not os.path.exists(os.path.abspath('GMM')):
    os.mkdir('GMM')
  os.chdir('GMM')
  if os.path.exists(os.path.abspath('src')):
    print 'The directory ' + os.path.abspath('src') + ' already exists, we don\'t extract anything'
  else:  
    gmmtarfile = tarfile.open(GMM_ARCHIVE, 'r:gz')
    gmmtarfile.extractall(os.getcwd())
    extractedname = os.listdir(os.getcwd())[0]
    shutil.copytree(extractedname+'/include', 'include')
    os.rename(extractedname, 'src')
    os.chdir('include')
    os.remove('Makefile.in')
    os.remove('Makefile.am')
    gmmincludedir = os.getcwd()
    os.chdir('..')
  os.chdir('..')


if gmmincludedir == '':
  if os.path.exists(os.path.join(os.getcwd(), os.path.join('GMM', 'include'))):
    gmmincludedir = os.path.join(os.getcwd(), os.path.join('GMM', 'include'))


#----------------- wxWidgets --------------------------- #


# wxWidgets configuration is very platform-dependant...

wxwidgetsbuilddir=''
if WX_ARCHIVE != '':
  print 'wxWidgets archive was found : '+WX_ARCHIVE
  
  if ARCHITECTURE == 'win32' or ARCHITECTURE == 'win64':
      
# on win32 systems, the archive is a zip file (MSW)
# we need to unzip it, and deal with the pre-formatted makefile 'makefile.vc'
    if os.path.exists(os.path.abspath('wxWidgets')):
      print 'The directory ' + os.path.abspath('wxWidgets') + ' already exists, we don\'t extract anything'
    else:
      unzip(WX_ARCHIVE, os.getcwd())
      outputdir = ''
      
      for file in os.listdir(os.getcwd()):
        if file.find('wx') != -1:
          outputdir = file
      os.rename(outputdir, 'wxWidgets')
    os.chdir('wxWidgets')
    
    if not os.path.exists(os.path.abspath('build')):
      print 'The directory ' + os.path.abspath('build') + ' does not exists, please mmake wxWidgets manually'
    else:
      os.chdir('build')
      os.chdir('msw')
    
      wxmakecommand = '"'+MAKE_COMMAND+'"'+' -f makefile.vc '

# I am not sure of this syntax, but we definitely have to be carefull here...
#    if ARCHITECTURE == 'win64':
#      wxmakecommand = wxmakecommand+' TARGET_CPU=AMD64'

      if RELEASE == 'Release':
        wxmakecommand = wxmakecommand+'BUILD=release '
      wxmakecommand = wxmakecommand+'SHARED='+SHARED_LIBS+' USE_OPENGL=1'
      print 'wx - make - command is : ' + wxmakecommand
    
      if options.configure == False:
        os.system(wxmakecommand)
      else:
        print 'not making yet'
      
# still have to figure out what is the result of the configuration on win32

    wxwidgetsbuilddir=os.getcwd()
    os.chdir('..')
    os.chdir('..')
    os.chdir('..')

  else: # ARCHITECTURE == 'win32' or ARCHITECTURE == 'win64':

    if not os.path.exists(os.path.abspath('wxWidgets')):
      os.mkdir('wxWidgets')
    os.chdir('wxWidgets')
    if os.path.exists(os.path.abspath('src')):
      print 'The directory ' + os.path.abspath('src') + ' already exists, we don\'t extract anything'
      wxwidgetssourcedir = os.path.abspath('src')
    else:
      mytarfile = tarfile.open(WX_ARCHIVE, 'r:gz')
      mytarfile.extractall(os.getcwd())
      os.rename(os.listdir(os.getcwd())[0], 'src')
      wxwidgetssourcedir = os.path.join(os.getcwd(), 'src')

    if not os.path.exists(os.path.abspath('build')):
      os.mkdir('build')
    os.chdir('build')
    wxconfigurecommand = ''
    if ARCHITECTURE == 'mac':

      wxconfigurecommand =  wxwidgetssourcedir+ '/configure --with-opengl --enable-streams --enable-std_iostreams --with-libpng=builtin '

# CAUTION : next line obviously will not work on macOS-10.5 or higher
# still need to find a way to get this version information somewhere
      wxconfigurecommand = wxconfigurecommand + ' --with-macosx-sdk=/Developer/SDKs/MacOSX10.4u.sdk/'

      if options.universalbinaries == True:
        wxconfigurecommand = wxconfigurecommand + ' --enable-universal_binary'      
    else:
      wxconfigurecommand = wxwidgetssourcedir + '/configure --with-opengl --enable-std_iostreams --enable-streams '
    
    if SHARED_LIBS == '1':
      wxconfigurecommand = wxconfigurecommand + '--enable-shared'
    os.system( wxconfigurecommand )
    if options.configure == False:
      os.system(MAKE_COMMAND+' -j'+options.cpus + ' -k')
    else:
      print 'not making yet'  
    wxwidgetsbuilddir=os.getcwd()
    os.chdir("..")
    os.chdir("..")
  
if wxwidgetsbuilddir == '':
  if os.path.exists(os.path.join(os.getcwd(), os.path.join('wxWidgets', 'build'))):
    wxwidgetsbuilddir = os.path.join(os.getcwd(), os.path.join('wxWidgets', 'build'))




#----------------- L-GPL ffmpeg library --------------------------- #


# On linux and mac systems, writing a movie file with VTK requires external
# library to be usable.
# we can use ffmpeg (L-GPL) or mpeg-encode (not L-GPL) to do so
# Unfortunately we did not find a simple way to get ffmpeg 
# libraries for universal-binaries on macOSX systems
# This is the reason why internally we use mpeg to encode movies

# you can download a mpeg encode tar.gz archive at this url http://www.vtk.org/files/support/vtkmpeg2encode.tar.gz
# you can download a ffmpeg tar.gz archive at this url http://www.vtk.org/files/support/ffmpeg_source.tar.gz

# IMPORTANT : You can use ffmpeg and/OR mpegencode internally. For release versions, use ffmpeg
# because of license issues



ffmpegbuilddir = ''
if FFMPEG_ARCHIVE != '':
  if ARCHITECTURE != 'win32' and ARCHITECTURE != 'win64' and ARCHITECTURE != 'mac':
    if not os.path.exists(os.path.abspath('FFMPEG')):
      mytarfile = tarfile.open(FFMPEG_ARCHIVE, 'r:gz')
      mytarfile.extractall(os.getcwd())
      os.rename('ffmpeg_source', 'FFMPEG')
    
    os.chdir('FFMPEG')
    
    if os.path.exists(os.path.abspath('src')):
      print 'The directory ' + os.path.abspath('src') + ' already exists, we don\'t extract anything'
    else:
      mytarfile2 = tarfile.open('ffmpeg.tar.gz', 'r:gz')
      mytarfile2.extractall(os.getcwd())
      os.rename('ffmpeg', 'src')

    if not os.path.exists(os.path.abspath('build')):
      os.mkdir('build')

    os.chdir('build')
    ffmpegconfigurecommand = '../src/configure --disable-network --disable-zlib --disable-ffserver --disable-ffplay --disable-decoders '
# if FFMPEG is compiled in static, then VTK is not able to compile,
# because there is a link problem (library dependance sort...)
# Hence we compile it in shared mode  
    ffmpegconfigurecommand = ffmpegconfigurecommand + '--enable-shared --disable-static '
    if ARCHITECTURE == 'mac':
      ffmpegconfigurecommand = ffmpegconfigurecommand + '--enable-pthreads --disable-vhook --enable-memalign-hack '
    
# CAUTION : There will be a problem on MAC systems as we cannot set universal_binaries 
# for ffmpeg configuration...
# not available unfortunately for ffmpeg
#    if ARCHITECTURE == 'mac' and options.universalbinaries == True:
#      ffmpegconfigurecommand =  ffmpegconfigurecommand + '--enable-universal_binary=/Developer/SDKs/MacOSX10.4u.sdk/'

    os.system( ffmpegconfigurecommand )
    if options.configure == False:
      os.system(MAKE_COMMAND+' -j'+options.cpus + ' -k')
    else:
      print 'not making yet'  
    ffmpegbuilddir=os.getcwd() 
    os.chdir("..")
  

    os.chdir("..")

if ffmpegbuilddir == '':
  if os.path.exists(os.path.join(os.getcwd(), os.path.join('FFMPEG', 'build'))):
    ffmpegbuilddir = os.path.join(os.getcwd(), os.path.join('FFMPEG', 'build'))

# Under linux systems, if ffmpeg archive was not found, then this next step will
# actually downloaded the svn version of it, and compile it
# I am not sure this is something we want though...

# this was not a good idea 

if ARCHITECTURE != 'win32' and ARCHITECTURE != 'win64' and ARCHITECTURE != 'mac':
  if ffmpegbuilddir == '' and options.checkoutffmpeg == True:
    if not os.path.exists(os.path.abspath('FFMPEG')):
      os.mkdir('FFMPEG')
    os.chdir('FFMPEG')
    os.system('svn checkout -r 12665 svn://svn.mplayerhq.hu/ffmpeg/trunk src')
    if not os.path.exists(os.path.abspath('FFMPEG')):
      os.mkdir('build')
    os.chdir('build')
    os.system('../src/configure --disable-mmx --disable-vhook --enable-shared --disable-static')
    if options.configure == False:
      os.system(MAKE_COMMAND+' -j'+options.cpus + ' -k')
    else:
      print 'not making yet'  
    ffmpegbuilddir = os.getcwd()
    os.chdir('..')
    os.chdir('..')



if ffmpegbuilddir != "":
  os.chdir(ffmpegbuilddir)
  os.chdir('..')

  # this is bad... really bad...

  os.chdir("src")

  if not os.path.exists(os.path.abspath('ffmpeg')):
    os.mkdir('ffmpeg')
  os.system('cp -f libavcodec/*.h ffmpeg')
  os.system('cp -f libavformat/*.h ffmpeg')
  os.system('cp -f libavutil/*.h ffmpeg')
  os.chdir('..')
  os.chdir('build')
  os.chdir('libavformat')
  if os.path.exists(os.path.abspath('libavformat.so')):
    os.system('ln -f -s libavformat.so libavformat.so.50')
  os.chdir("..")
  os.chdir("..")
  
  # end of bad stuff...

  os.chdir('..')

    
	  

  
if ffmpegbuilddir != '':
  vtkflags.append(['VTK_USE_FFMPEG_ENCODER', 'BOOL', '1'])
  vtkflags.append(['FFMPEG_INCLUDE_DIR', 'PATH', ffmpegbuilddir+'/../src/'])

  libext = '.so'
  if ARCHITECTURE == 'mac':
    libext = '.dylib'

  vtkflags.append(['FFMPEG_avcodec_LIBRARY', 'FILEPATH', ffmpegbuilddir+'/libavcodec/libavcodec' + libext])
  vtkflags.append(['FFMPEG_avformat_LIBRARY', 'FILEPATH', ffmpegbuilddir+'/libavformat/libavformat' + libext])
  vtkflags.append(['FFMPEG_avutil_LIBRARY', 'FILEPATH', ffmpegbuilddir+'/libavutil/libavutil' + libext])





#----------------- vtk-mpeg-encode --------------------------- #


# On linux and mac systems, writing a movie file with VTK requires external
# library to be usable.
# we can use ffmpeg (L-GPL) or mpeg-encode (not L-GPL) to do so
# Unfortunately we did not find a simple way to get ffmpeg 
# libraries for universal-binaries on macOSX systems
# This is the reason why internally we use mpeg to encode movies

# you can download a mpeg encode tar.gz archive at this url http://www.vtk.org/files/support/vtkmpeg2encode.tar.gz
# you can download a ffmpeg tar.gz archive at this url http://www.vtk.org/files/support/ffmpeg_source.tar.gz

# IMPORTANT : You can use ffmpeg and/OR mpegencode internally. For release versions, use ffmpeg
# because of license issues


mpegbuilddir = ''

if MPEG_ARCHIVE != '':
  if ARCHITECTURE != 'win32' and ARCHITECTURE != 'win64':
    mpegbuilddir = Configure_ToolKit(MPEG_ARCHIVE, 'MPEG', mpegflags)
  
if mpegbuilddir == '':
  if os.path.exists(os.path.join(os.getcwd(), os.path.join('MPEG', 'build'))):
    mpegbuilddir = os.path.join(os.getcwd(), os.path.join('MPEG', 'build'))


if mpegbuilddir != '' and ffmpegbuilddir == '':
  shutil.copy(os.path.join(mpegbuilddir,'mpeg2encDllConfig.h'), mpegbuilddir+'/../src/')
  vtkflags.append(['VTK_USE_MPEG2_ENCODER', 'BOOL', '1'])
  vtkflags.append(['vtkMPEG2Encode_INCLUDE_PATH', 'PATH', mpegbuilddir+'/../src/'])
  vtkflags.append(['vtkMPEG2Encode_LIBRARIES', 'STRING', os.path.join(mpegbuilddir,'libvtkMPEG2Encode.a')])




#----------------- KitWare Projects --------------------------- #


itkbuilddir = Configure_ToolKit(ITK_ARCHIVE, 'ITK', itkflags)

if itkbuilddir == '':
  if os.path.exists(os.path.join(os.getcwd(), os.path.join('ITK', 'build'))):
    itkbuilddir = os.path.join(os.getcwd(), os.path.join('ITK', 'build'))



vtkbuilddir = Configure_ToolKit(VTK_ARCHIVE, 'VTK', vtkflags)

if vtkbuilddir == '':
  if os.path.exists(os.path.join(os.getcwd(), os.path.join('VTK', 'build'))):
    vtkbuilddir = os.path.join(os.getcwd(), os.path.join('VTK', 'build'))


kwwidgetsbuilddir = ''
# We build KWWidgets only if VTK is found

if vtkbuilddir != '':
  if ARCHITECTURE != 'mac' or  MACMODE != 'carbon':
    kwwidgetsflags.append(['VTK_DIR', 'PATH', vtkbuilddir])
    kwwidgetsbuilddir = Configure_ToolKit(KW_ARCHIVE, 'KWWidgets', kwwidgetsflags)



os.chdir("..")

