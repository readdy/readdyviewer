import readdyviewer as rv
import numpy as np

if __name__ == '__main__':
    n_frames = 5
    n_particles_per_frame = np.array([5, 3, 7, 8, 2])
    positions = np.random.rand(n_frames, 8, 3).astype(np.float64, order="C")
    types = np.ones((n_frames, 8)).astype(np.uint, order="C")
    ids = np.zeros((n_frames, 8)).astype(np.ulonglong, order="C")
    conf = rv.Configuration()
    conf.colors[0] = rv.Color(0,1,0)
    conf.colors[1] = rv.Color(1,0,0)
    rv.watch_npy(positions, types, ids, n_particles_per_frame, conf)
    print("positions: %s" % positions)
