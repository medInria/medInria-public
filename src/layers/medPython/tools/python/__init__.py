import sys
from . import qt_bindings
from .medInria_bindings import *
from .console import Console

sys.modules['qt'] = qt_bindings
sys.modules['__main__'].__dict__['qt'] = qt_bindings
sys.modules['__main__'].__dict__['med'] = sys.modules[__name__]

loaded_plugins = dict()
failed_plugins = dict()

def plugins():
    if loaded_plugins:
        print("Successfully loaded plugins:\n\t")
        for plugin in loaded_plugins.items():
            print(f"{plugin} ");
        print("\n")
    if failed_plugins:
        print(f"{len(failed_plugins)} plugins(s) failed to load:\n")
        for plugin, message in failed_plugins.items():
            print(f"{plugin}:\n{message}\n")
