import sys
from importlib.machinery import *

def installCustomImportHook():
    """
    Customize Python's import mechanism with additional library extensions.

    By default Python does not import dynamic libraries with .dylib or .pyd
    suffixes. This function adds a custom path hook that accepts these names.
    """

    extensions = (ExtensionFileLoader, EXTENSION_SUFFIXES + [".dylib", ".pyd"])
    source = (SourceFileLoader, SOURCE_SUFFIXES)
    bytecode = (SourcelessFileLoader, BYTECODE_SUFFIXES)
    sys.path_hooks.insert(0, (FileFinder.path_hook(extensions, source, bytecode)))
    sys.path_importer_cache.clear()
