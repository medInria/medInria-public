import sys
from importlib.machinery import *

def installCustomImportHook():
    extensions = (ExtensionFileLoader, EXTENSION_SUFFIXES + [".dylib"])
    source = (SourceFileLoader, SOURCE_SUFFIXES)
    bytecode = (SourcelessFileLoader, BYTECODE_SUFFIXES)
    sys.path_hooks.insert(0, (FileFinder.path_hook(extensions, source, bytecode)))
    sys.path_importer_cache.clear()
