from .readdyviewer_binding import Color, Configuration, TrajectoryEntry, set_resource_dir, watch_npy
import os as _os

_ROOT = _os.path.abspath(_os.path.dirname(__file__))

set_resource_dir(_os.path.join(_ROOT, 'resources'))
