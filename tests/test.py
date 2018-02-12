import numpy as np
import readdy
from readdy.api.utils import load_trajectory_to_npy
import readdyviewer as readdyviewer


def runsim():
    system = readdy.ReactionDiffusionSystem(box_size=[150, 150, 150])
    system.topologies.add_type("Topology")
    system.add_topology_species("T")
    system.topologies.configure_harmonic_bond("T", "T", force_constant=20., length=2.)
    sim = system.simulation()
    n_topology_particles = 70
    positions = [[0, 0, 0], np.random.normal(size=3)]
    for i in range(n_topology_particles - 2):
        delta = positions[-1] - positions[-2]
        offset = np.random.normal(size=3) + delta
        offset = offset / np.linalg.norm(offset)
        positions.append(positions[-1] + 2. * offset)
    topology = sim.add_topology(topology_type="Topology", particle_types="T", positions=np.array(positions))

    graph = topology.get_graph()
    for i in range(len(graph.get_vertices()) - 1):
        graph.add_edge(i, i + 1)

    sim.observe.topologies(1)
    sim.record_trajectory(1)
    sim.output_file = "yay.h5"

    sim.run(n_steps=1000, timestep=1e-2)


def edges():
    print(readdyviewer.__file__)
    trajfile = "/home/mho/Development/readdyviewer/tests/topology_simulation.h5"
    n_particles_per_frame, positions, types, ids = load_trajectory_to_npy(trajfile)
    config = readdyviewer.Configuration()
    t = readdy.Trajectory(trajfile)
    entries = t.read()
    time, topology_records = t.read_observable_topologies()
    print(len(topology_records))
    print(len(topology_records[0][0].edges))

    edges = []
    for timestep, tops in zip(time, topology_records):
        current_edges = []
        for top in tops:
            for e1, e2 in top.edges:
                if e1 <= e2:
                    ix1 = top.particles[e1]
                    ix2 = top.particles[e2]
                    current_edges.append((ix1, ix2))
                    p1 = entries[timestep][ix1]
                    p2 = entries[timestep][ix2]
                    assert p1.type == 'T' or p1.type == 'unstable T', "expected topology type but got {} -- {}".format(
                        p1, p2)
                    assert p2.type == 'T' or p2.type == 'unstable T', "expected topology type but got {} -- {}".format(
                        p1, p2)
        edges.append(current_edges)
    config.colors[t.particle_types['unstable T']] = readdyviewer.Color(153. / 255., 255. / 255., 0.)
    config.radii[t.particle_types['unstable T']] = .5
    config.colors[t.particle_types['T']] = readdyviewer.Color(255. / 255., 153. / 255., 0.)
    config.radii[t.particle_types['T']] = .5
    config.colors[t.particle_types['Ligand']] = readdyviewer.Color(.5, .5, .5)
    config.radii[t.particle_types['Ligand']] = 1.
    config.colors[t.particle_types['Decay']] = readdyviewer.Color(.1, .2, .3)
    config.radii[t.particle_types['Decay']] = .5

    config.stride = 1
    readdyviewer.watch_npy(positions, types, ids, n_particles_per_frame, config, edges)


def test_cylinders():
    n_particles_per_frame = np.array([2], dtype=int)
    positions = np.array([[[2., -4., 1.], [0, 0., 0.]]])
    types = np.array([[0, 0]], dtype=int)
    ids = np.array([[0, 1]], dtype=int)
    config = readdyviewer.Configuration()
    config.colors[0] = readdyviewer.Color(153. / 255., 255. / 255., 0.)
    config.radii[0] = .1
    config.stride = 1
    edges = [[(0, 1)]]
    readdyviewer.watch_npy(positions, types, ids, n_particles_per_frame, config, edges)


def more_topologies_sim():
    trajfile = "/home/mho/Development/readdyviewer/tests/more_topologies_simulation.h5"
    n_particles_per_frame, positions, types, ids = load_trajectory_to_npy(trajfile)
    config = readdyviewer.Configuration()
    t = readdy.Trajectory(trajfile)
    entries = t.read()
    time, topology_records = t.read_observable_topologies()

    edges = []
    for timestep, tops in enumerate(topology_records):
        current_edges = []
        for top in tops:
            for e1, e2 in top.edges:
                ix1 = top.particles[e1]
                ix2 = top.particles[e2]
                current_edges.append((ix1, ix2))
                p1 = entries[timestep][ix1]
                p2 = entries[timestep][ix2]
                assert p1.type == 'T' or p1.type == 'center T', "expected topology type but got {} -- {}".format(p1, p2)
                assert p2.type == 'T' or p2.type == 'center T', "expected topology type but got {} -- {}".format(p1, p2)
        edges.append(current_edges)
    config.colors[t.particle_types['center T']] = readdyviewer.Color(153. / 255., 255. / 255., 0.)
    config.radii[t.particle_types['center T']] = .5
    config.colors[t.particle_types['T']] = readdyviewer.Color(255. / 255., 153. / 255., 0.)
    config.radii[t.particle_types['T']] = .5
    config.clearcolor = readdyviewer.Color(155. / 255., 155. / 255., 155. / 255.)

    config.stride = 1
    print(positions.shape)
    print("go!")
    readdyviewer.watch_npy(positions, types, ids, n_particles_per_frame, config, edges)


def showsim():
    # more_topologies_sim()
    # logo()
    edges()
    # test_cylinders()


def logo():
    outfile = "/home/mho/tmp/readdylogo_out2.h5"

    config = get_config_solarized_light(outfile)

    config.stride = 1
    from readdy.api.utils import load_trajectory_to_npy as to_npy
    n_particles_per_frame, positions, types, ids = to_npy(outfile)
    readdyviewer.watch_npy(positions, types, ids, n_particles_per_frame, config)


def get_config_original(outfile):
    import readdy.util.io_utils as ioutils
    config = readdyviewer.Configuration()
    p_types = ioutils.get_particle_types(outfile)
    config.colors[p_types['ReaDDy_inactive']] = readdyviewer.Color(0. / 255., 0. / 255., 0. / 255.)
    config.radii[p_types['ReaDDy_inactive']] = 1.
    config.colors[p_types['ReaDDy_active']] = readdyviewer.Color(200. / 255., 50. / 255., 0. / 255.)
    config.radii[p_types['ReaDDy_active']] = 1.
    config.colors[p_types['Activator']] = readdyviewer.Color(155. / 255., 155. / 255., 255. / 255.)
    config.radii[p_types['Activator']] = .7
    config.colors[p_types['Activator_predator']] = readdyviewer.Color(130. / 255., 170. / 255., 155. / 255.)
    config.radii[p_types['Activator_predator']] = .3
    config.stride = 1
    config.clearcolor = readdyviewer.Color(0. / 255., 0. / 255.,
                                           0. / 255.)  # readdyviewer.Color(253. / 255., 246. / 255., 227. / 255.)
    return config


def get_config_rgb_black(outfile):
    import readdy.util.io_utils as ioutils
    config = readdyviewer.Configuration()
    p_types = ioutils.get_particle_types(outfile)
    config.colors[p_types['ReaDDy_inactive']] = readdyviewer.Color(0. / 255., 0. / 255., 0. / 255.)
    config.radii[p_types['ReaDDy_inactive']] = 1.
    config.colors[p_types['ReaDDy_active']] = readdyviewer.Color(230. / 255., 9. / 255., 9. / 255.)
    config.radii[p_types['ReaDDy_active']] = 1.2
    config.colors[p_types['Activator']] = readdyviewer.Color(9. / 255., 230. / 255., 230. / 255.)
    config.radii[p_types['Activator']] = .9
    config.colors[p_types['Activator_predator']] = readdyviewer.Color(120. / 255., 230. / 255., 9. / 255.)
    config.radii[p_types['Activator_predator']] = .4
    config.stride = 1
    config.clearcolor = readdyviewer.Color(0. / 255., 0. / 255.,
                                           0. / 255.)  # readdyviewer.Color(253. / 255., 246. / 255., 227. / 255.)
    return config


def get_config_rgb_light_gray(outfile):
    import readdy.util.io_utils as ioutils
    config = readdyviewer.Configuration()
    p_types = ioutils.get_particle_types(outfile)
    config.colors[p_types['ReaDDy_inactive']] = readdyviewer.Color(128. / 255., 0. / 255., 0. / 255.)
    config.radii[p_types['ReaDDy_inactive']] = 1.
    config.colors[p_types['ReaDDy_active']] = readdyviewer.Color(230. / 255., 9. / 255., 9. / 255.)
    config.radii[p_types['ReaDDy_active']] = 1.2
    config.colors[p_types['Activator']] = readdyviewer.Color(9. / 255., 230. / 255., 230. / 255.)
    config.radii[p_types['Activator']] = .9
    config.colors[p_types['Activator_predator']] = readdyviewer.Color(120. / 255., 230. / 255., 9. / 255.)
    config.radii[p_types['Activator_predator']] = .4
    config.stride = 1
    config.clearcolor = readdyviewer.Color(240. / 255., 240. / 255.,
                                           240. / 255.)  # readdyviewer.Color(253. / 255., 246. / 255., 227. / 255.)
    return config


def get_config_solarized_light(outfile):
    import readdy.util.io_utils as ioutils
    config = readdyviewer.Configuration()
    p_types = ioutils.get_particle_types(outfile)
    config.colors[p_types['ReaDDy_inactive']] = readdyviewer.Color(128. / 255., 0. / 255., 0. / 255.)
    config.radii[p_types['ReaDDy_inactive']] = 1.
    config.colors[p_types['ReaDDy_active']] = readdyviewer.Color(255. / 255., 50. / 255., 47. / 255.)
    config.radii[p_types['ReaDDy_active']] = 1.2
    config.colors[p_types['Activator']] = readdyviewer.Color(139. / 255., 210. / 255., 255. / 255.)
    config.radii[p_types['Activator']] = .9
    config.colors[p_types['Activator_predator']] = readdyviewer.Color(181. / 255., 137. / 255., 137. / 255.)
    config.radii[p_types['Activator_predator']] = .4
    config.stride = 1
    config.clearcolor = readdyviewer.Color(227. / 255., 227. / 255.,
                                           227. / 255.)  # readdyviewer.Color(253. / 255., 246. / 255., 227. / 255.)
    return config


def get_config_brighter(outfile):
    import readdy.util.io_utils as ioutils
    config = readdyviewer.Configuration()
    p_types = ioutils.get_particle_types(outfile)
    config.colors[p_types['ReaDDy_inactive']] = readdyviewer.Color(0. / 255., 0. / 255., 0. / 255.)
    config.radii[p_types['ReaDDy_inactive']] = 1.
    config.colors[p_types['ReaDDy_active']] = readdyviewer.Color(255. / 255., 50. / 255., 0. / 255.)
    config.radii[p_types['ReaDDy_active']] = 1.2
    config.colors[p_types['Activator']] = readdyviewer.Color(200. / 255., 200. / 255., 255. / 255.)
    config.radii[p_types['Activator']] = .9
    config.colors[p_types['Activator_predator']] = readdyviewer.Color(130. / 255., 170. / 255., 155. / 255.)
    config.radii[p_types['Activator_predator']] = .4
    config.stride = 1
    config.clearcolor = readdyviewer.Color(15. / 255., 15. / 255., 15. / 255.)
    return config


if __name__ == '__main__':
    showsim()
