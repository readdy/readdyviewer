from . import readdyviewer
import os as _os

_ROOT = _os.path.abspath(_os.path.dirname(__file__))

readdyviewer.set_resource_dir(_os.path.join(_ROOT, 'resources'))
