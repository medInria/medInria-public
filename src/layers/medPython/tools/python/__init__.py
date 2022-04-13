import importlib, sys
from .customImport import installCustomImportHook as _installCustomImportHook


_installCustomImportHook()


from .bindings import registerSettingsWidget as _registerSettingsWidget
from .pluginManager import loadPlugins as _loadPlugins, pluginsInfo
from .console import initialize as _initializeConsole


def _initializeOnce():
    _registerSettingsWidget()
    _loadPlugins()
    _initializeConsole()
    yield
    while True:
        print(f'{__name__} is already initialized')
        yield

_initialize = _initializeOnce()

def initialize():
    next(_initialize)

def finalize():
    _initialize = _initializeOnce()
