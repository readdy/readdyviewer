from .readdyviewer_binding import Color, Configuration, TrajectoryEntry, set_resource_dir, watch_npy
import os as _os
from typing import Mapping

_ROOT = _os.path.abspath(_os.path.dirname(__file__))

set_resource_dir(_os.path.join(_ROOT, 'resources'))


def plot_colormap(config: Configuration, particle_type_map: Mapping[str, int], size=42):
    import matplotlib.pyplot as plt
    import matplotlib.patches as patches
    fig = plt.figure(figsize=(3, 2))
    ax = fig.add_subplot(111)
    ax.axis('off')
    handles = []
    for t in particle_type_map.keys():
        ix = particle_type_map[t]
        color = config.colors[ix]
        handles.append(patches.Patch(color=(color.r, color.g, color.b), label=t))
    fig.legend(handles=handles, prop={'size': size})
    return fig, ax
