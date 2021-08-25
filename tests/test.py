import numpy as np
import readdy
import os
from readdy.api.utils import load_trajectory_to_npy
import sys
sys.path.append('/home/mho/Development/readdyviewer/cmake-build-release/src')
import readdyviewer_binding as readdyviewer

readdyviewer.set_resource_dir('/home/mho/Development/readdyviewer/readdyviewer/resources')


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
    config.smoothing = 10
    config.bond_radius = .2
    config.cutoff = 3
    config.edge_color = readdyviewer.Color(.1, .1, .1)
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

def bilayer_lipid():
    import matplotlib.pyplot as plt
    outfile = "/home/mho/Downloads/new-self-ass.h5"
    n_particles_per_frame, positions, types, ids = load_trajectory_to_npy(outfile, stride=10)
    config = readdyviewer.Configuration()
    t = readdy.Trajectory(outfile)

    config.colors[t.particle_types['Head']] = readdyviewer.Color(plt.cm.jet(0)[0],
                                                                 plt.cm.jet(0)[1],
                                                                 plt.cm.jet(0)[2])
    config.radii[t.particle_types['Head']] = .7
    config.colors[t.particle_types['Tail']] = readdyviewer.Color(plt.cm.jet(1.)[0],
                                                                 plt.cm.jet(1.)[1],
                                                                 plt.cm.jet(1.)[2])
    config.radii[t.particle_types['Tail']] = .7

    config.stride = 1
    config.smoothing = 5
    config.cutoff = 5
    config.bond_radius = .02
    config.wait=3
    config.draw_periodic = True
    config.set_box_size(25, 25, 25)
    readdyviewer.watch_npy(positions, types, ids, n_particles_per_frame, config) #

def showsim():
    # more_topologies_sim()
    # logo()
    # edges()
    # test_cylinders()
    bilayer_lipid()


def logo():
    outfile = "/home/mho/Dropbox/backup2/tmp/readdylogo_out2.h5"

    config = get_config_solarized_light(outfile)

    config.stride = 1
    config.smoothing = 1
    config.draw_periodic = True
    config.set_box_size(200, 200, 200)
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

def create_and_show_sim():
    try:
        import os
        import readdy.util.io_utils as ioutils
        import matplotlib.pyplot as plt

        if os.path.exists('out.h5'):
            os.unlink('out.h5')

        system = readdy.ReactionDiffusionSystem([25.,25.,25.], temperature=300.*readdy.units.kelvin)
        system.add_species("A", 1.0)
        system.reactions.add("myfusion: A +(2) A -> A", rate=10.)
        system.reactions.add("myfission: A -> A +(2) A", rate=3.)
        system.potentials.add_harmonic_repulsion("A", "A", force_constant=10.,
                                                 interaction_distance=2.)
        simulation = system.simulation(kernel="CPU")

        simulation.output_file = "out.h5"
        simulation.reaction_handler = "UncontrolledApproximation"

        simulation.add_particle("A", [0.,0.,0.])

        simulation.record_trajectory(stride=1)
        simulation.observe.number_of_particles(stride=100)
        simulation.run(n_steps=3000, timestep=1e-2)

        n_particles_per_frame, positions, types, ids = load_trajectory_to_npy(simulation.output_file, stride=10)
        config = readdyviewer.Configuration()
        t = readdy.Trajectory(simulation.output_file)

        config.colors[t.particle_types['A']] = readdyviewer.Color(plt.cm.jet(0)[0],
                                                                     plt.cm.jet(0)[1],
                                                                     plt.cm.jet(0)[2])
        config.radii[t.particle_types['A']] = 7

        config.stride = 1
        config.smoothing = 3
        config.cutoff = 5
        config.bond_radius = .02
        config.wait = 5
        config.clearcolor = readdyviewer.Color(.5, .5, .5)
        config.draw_periodic = False
        config.set_box_size(25, 25, 25)
        readdyviewer.watch_npy(positions, types, ids, n_particles_per_frame, config)
    finally:
        if os.path.exists('out.h5'):
            os.unlink('out.h5')


if __name__ == '__main__':
    create_and_show_sim()
