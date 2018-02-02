import numpy as np
import readdy
from readdy.api.utils import load_trajectory_to_npy
import readdyviewer.readdyviewer as readdyviewer


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


def showsim():
    n_particles_per_frame, positions, types, ids = load_trajectory_to_npy("yay.h5")
    config = readdyviewer.Configuration()
    t = readdy.Trajectory("yay.h5")
    time, topology_records = t.read_observable_topologies()
    print(len(topology_records))
    print(len(topology_records[0][0].edges))

    for tops in topology_records:
        for top in tops:
            top.edges
            top.particles
    config.colors[t.particle_types['T']] = readdyviewer.Color(255./255., 153./255., 0.)
    config.radii[t.particle_types['T']] = .5

    config.stride = 1
    readdyviewer.watch_npy(positions, types, ids, n_particles_per_frame, config)



if __name__ == '__main__':
    showsim()
