# medBoutiques plugin

A GUI to execute boutiques tools in medInria.

## Requirements

 - Docker or Singularity

## Directory structure

The required data and executables lie in the `BoutiquesGUI-Data` which must be located beside the medInria executable:

 - `boutiques/`: contains boutiques for python2.7 and python3, and the command line tool bosh,
 - `python/`: contains the python executable for windows (python2.7 is installed by default on Ubuntu and OS X),
 - `boutiques-gui-settings.json`: the boutiques gui settings,
 - `vc_redist.x86.exe`: the Visual C++ Redistributable for Visual Studio 2015, 2017 and 2019 installer ; required to run python.exe (python3) on windows. 

## Running medBoutiques for the first time

On Windows, BoutiquesGUI will try to execute python3 (`BoutiquesGUI-Data/python/python.exe`) ; if it fails it will try to silently install Visual C++ Redistributable for Visual Studio 2015, 2017 and 2019 (with `BoutiquesGUI-Data/vc_redist.x86.exe`). After the installation, BoutiquesGUI will retry to run python3 ; a message will ask the user to install python3 (in `BoutiquesGUI-Data/python/python.exe`) it fails again.

On Linux, BoutiquesGUI checks that the `python` command works ; if not, a message asks the user to install it manually.

Then, on all platforms, BoutiquesGUI checks that Docker is installed (by checking the return result of the `docker` command). If not, a message asks the user to install it manually. 

If everything works properly (both `python` and `docker` can be executed), BoutiquesGUI writes the property `installed: true` to the settings (`BoutiquesGUI-Data/boutiques-gui-settings.json`) ; the install checks will be ignored for the next times.

BoutiquesGUI will run `boutiques/bosh.py` (the boutiques command line tool), which will in turn run `boutiques` for python3 if launched with python3 (`boutiques/boutiques-python3`), or python2 otherwise.

## Database

BoutiquesGUI downloads the entire tool database form zenodo (search the 1000 first boutiques tools on Zenodo) at each startup.

The database is stored and updated in the file "~ /.cache/boutiques/all-descriptors.json".
This is useful to keep BoutiquesGUI working when the zenodo database is down (we can then rely on the data stored in "all-descriptors.json"), and to get instantaneous search result.
This process is executed on the background, and updates the database once all descriptors are pulled from Zenodo.

## Settings

The BoutiquesGUI settings are managed automatically, although it could also be set manually.

 - `dataTypeToFormatAndExtension`: describes which format to associate with each data type. When converting an input data to a temporary file (BoutiquesGUI saves medInria input data / images to temporary files to feed them to boutiques) the file format will be given by this settings, depending on the data type. When the user sets a parameter as input (by drag-and-dropping data, or with the corresponding `Set input` button), BoutiquesGUI opens a dialog to choose a format from a list of formats compatible with the data type (only if no compatible format is found in `dataTypeToFormatAndExtension` or `preferredFormatsAndExtensions`). This setting is updated when the user chooses a format to associate with the data type. 

 - `preferredFormatsAndExtensions`: also used to determine the format of temporary files, but instead of associating each data type to a format, it just enumerates the preferred formats and extensions. If the data type is not found in the `dataTypeToFormatAndExtension` map, BoutiquesGUI will use the first compatible format in the `preferredFormatsAndExtensions` list.

 - `outputExtensions`: define the extensions of the boutiques tools outputs which can then be opened in medInria. If an output parameter has an extension listed in `outputExtensions`, a button will enable to set the output as *current output* ; it will automatically be opened in medInria once the process is finished.

  - `installed`: a simple boolean which is set to true when BoutiquesGUI successfully run `python` and `docker` commands for the first time. The install check will not be executed if `installed` is true.

```
    "dataTypeToFormatAndExtension": {
    },
    "installed": true,
    "outputExtensions": [
        ".nii.gz",
        ".vtk"
    ],
    "preferredFormatsAndExtensions": [
        [
            "itkNiftiDataImageWriter",
            ".nii.gz"
        ]
    ]
```

## Input

Input data can be drag-and-dropped on the input parameters. The medBoutiques plugin will write the input data to a temporary file (the format is defined by the `dataTypeToFormatAndExtension` or `preferredFormatsAndExtensions` settings ; if nothing corresponds to the data type in those settings, a dialog enables the user to choose a format compatible with the data type, which can optionally be saved in the settings) which will be used by the boutiques tool.
Another way to set the current input data displayed in medInria (in the *input panel*) to a parameter of a boutiques tool is to click the correponding `Set input` button.

## Output

The `Set output` button following the boutiques tools parameters is used to automatically open the file in medInria *output panel* once the boutiques tool execution is finished. 
