import tables
import readdyviewer as rv
import numpy as np
import h5py
from readdy.util.trajectory_utils import TrajectoryReader
import readdy.util.io_utils as ioutils


def test_random_frames():
    n_frames = 5
    n_particles_per_frame = np.array([5, 3, 7, 8, 2])
    positions = np.random.rand(n_frames, 8, 3).astype(np.float64, order="C")
    types = np.ones((n_frames, 8)).astype(np.uint, order="C")
    ids = np.zeros((n_frames, 8)).astype(np.ulonglong, order="C")
    conf = rv.Configuration()
    conf.colors[0] = rv.Color(0, 1, 0)
    conf.colors[1] = rv.Color(1, 0, 0)
    rv.watch_npy(positions, types, ids, n_particles_per_frame, conf)


def test_polymer_chain():
    fname = '/home/mho/Development/readdy/readdy/wrappers/python/src/python/readdy/examples/out.h5'
    config = rv.Configuration()
    p_types = ioutils.get_particle_types(fname)
    config.colors[p_types['Topology A']] = rv.Color(255./255., 153./255., 0.)
    config.radii[p_types['Topology A']] = .5
    config.colors[p_types['B']] = rv.Color(51./255., 153./255., 255./255.)
    config.radii[p_types['B']] = .5
    config.stride = 1
    r = TrajectoryReader(fname)
    items = r[:]
    n_frames = len(items)
    print("n_frames = %s" % n_frames)
    n_particles_per_frame = np.zeros((n_frames))
    for idx, item in enumerate(items):
        n_particles_per_frame[idx] = len(item)
    max_n_particles = int(np.amax(n_particles_per_frame, 0))
    positions = np.zeros((n_frames, max_n_particles, 3), dtype=np.float64)
    types = np.zeros((n_frames, max_n_particles), dtype=np.uint)
    ids = np.zeros((n_frames, max_n_particles), dtype=np.ulonglong)
    chaintype = -1
    for t, frame in enumerate(items):
        for p, entry in enumerate(frame):
            positions[t, p, 0:3] = entry.position
            types[t, p] = entry.type_id
            ids[t, p] = entry.id
            chaintype = entry.type_id
    rv.watch_npy(positions, types, ids, n_particles_per_frame, config)


if __name__ == '__main__':
    test_polymer_chain()
