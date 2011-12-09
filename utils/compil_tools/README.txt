***************************************************************************
*
*    Short Documentation for the compilation/installation scripts
*
***************************************************************************

***************************************************************************
1- Overview
***************************************************************************

The script installer.py aims at downloading sources, installing binary dependencies, configuring, compiling, installing and creating packages of a list of projects. A project is a self contained directory to be compiled and potentially packaged.

It works with several package managing systems for linux (apt, yum). download sources using either wget, git or svn. It configures using autoconf and cmake. The compilation is possible using Visual studio and gcc. Package creation is done using cpack and its generators (rpm, deb, nsis, packagemaker).


***************************************************************************
2- Command line:
***************************************************************************

The script is run through this command:
./installer.py

The configuration is based on config files, (.cfg). They are parsed by a python ConfigParser module. That module is awesome: it allows inheritance between different config files. You end up using always the file called default_param.cfg which needs to be in the same directory you are running the script from. And then you overwrite the values you need with additionnal files. a typical usage being:

./installer.py -frun_pipol_winxp.cfg -frun.cfg

"-f" is the short name for the option to append another config file to the hierarchy of configuration values.

In our example here, first the installer.py populates its values with default_param.cfg, then overwrites them with run_pipol_winxp.cfg, which has a few windows specific parameters, and run_example.cfg contains only a sublist of packages the user is interested in at the moment.

To get a complete list of command line options, use the usual help argument:

./installer.py --help

All the switches present on the command line are computed in conjunction with the configuration file values (a logical "and"). This way you can choose whether to enable downloading the sources globally or to delegate that responsibility to the configuratiion files, which can fine tune this behaviour at the project level.

***************************************************************************
3- Configuration Files
***************************************************************************

The file to always keep in mind is "default_param.cfg". It contains a documented list of all the parameters recognised by the installer.py script.

The syntax and organisation of the config files follow the ConfigParser python module rules. In short the file follows the .ini syntax. Sections contain key-value pairs. 

There is a special section [DEFAULT] that contains values that will exist in every other section, as default values. If a key is present in both a section and the default the section's value will prevail.

There is a way to use variables in the file:
[section]
var=value
var2=%(var)s_2

means that var2=value_2
The replaced keyword must be a key present in the section (also true for keys in DEFAULT).

***************************************************************************
4- Configure steps
***************************************************************************

This section will explain how to use cmake to configure a project.


First define a project as a new section:
[vtk]
version_major=5
version_minor=6
version_patch=0
#source host, does not include file for wget obtained files
source_host=http://www.vtk.org/files/release/%(version_major)s.%(version_minor)s
source_file=vtk-%(version)s.tar.gz
scm=wget
destination_dir=vtk-%(version)s
#move up one directory the content of the VTK uncompressed directory 
extra_create_dirs_cmd=mv %(destination_dir)s/VTK/* %(destination_dir)s/ && rmdir %(destination_dir)s/VTK
#do not install nor package it
install=0
package=0

Then define the cmake values for it, as they would appear on a CMakeCache file or as a command line cmake option, all in a section called project_name-cmake:

[vtk-cmake]
BUILD_DOXYGEN:BOOL=OFF
BUILD_EXAMPLES:BOOL=OFF
BUILD_SHARED_LIBS:BOOL=ON
BUILD_TESTING:BOOL=OFF

Be aware that all values in [cmake-common] will also be added to your project.

***************************************************************************
5- scm
***************************************************************************

Many projects are not downloaded using wget.
3 supported methods so far:
* wget
* git
* svn

git has several "parameters"
username=somebody
git_protocol=git or ssh
branch=master

username only matters if ssh has been chosen. In the case of gitorious hosted projects, use git as a username.

source_host is always the server host, and source_file is the path on the server to the git directory. Note that installer.py will add the protocol substring itself. Example:

To check out medinria-asclepios-private, you need authentication on the server and can't use the git protocol.
 "git@dev-med.inria.fr:medinria-asclepios/medinria.git"
 is the path to the repository.

 you must then use the following syntax in your section:

[medinria]
source_host=dev-med.inria.fr
source_file=medinria-asclepios/medinria.git
username=git
scm=git
scm_protocol=ssh

For a repository that does not need authentication, you may drop the scm_protocol and username tag, they default respectively to "" and "git"

For svn, there are 2 scm_protocols: ssh or any value.
if ssh, you will need a username. if the value is anything else, the program will just concatenate source_host+/+source_file
Your source_host will need the protocol substring in it. If using ssh, don't put any such string, installer.py does it for you, because of the username.

[ttk-public]
source_host= svn://scm.gforge.inria.fr/svn
source_file=ttk/trunk
scm=svn
scm_protocol=svn

or 
[any-private-proj]
source_host=scm.gforge.inria.fr/svn
source_file=any-private-proj/trunk/my selected_directory
scm=svn
scm_protocol=ssh
username=someone




