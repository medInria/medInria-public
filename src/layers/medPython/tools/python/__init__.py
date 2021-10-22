import sys
from . import qt_bindings as qt
from . import medInria_bindings as med
from .console import Console

sys.modules['medInria'] = med
sys.modules['qt'] = qt
