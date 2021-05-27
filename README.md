# <img align="left" height="30" src="media/TH-logo-2.png">&nbsp;&nbsp;Treasure Hunt Framework

Treasure Hunt (TH) is an open source, multilevel framework capable of distributing independent search algorithms to a large number of processing nodes.
TH is not tied to any specific search algorithm because, instead of a distributed optimization mechanism, TH is a mechanism to distribute optimizers.

Treasure Hunt boosts the inherent characteristics of these search algorithms, making poor performer algorithms to become comparable to good ones, and good performer algorithms to extend their limits to larger problems.
TH's robust scaling also reduces the importance of using complex search algorithms on complex problems by just distributing additional TH instances.

TH ensures quick propagation of information and explores a driving mechanism that results in a joint convergences between the working nodes.
The sampling mechanism provided by Treasure Hunt also reduces the need for large population sizes, increasing cooperation between multiple evolving populations, what is specially important on problems with time-consuming fitness functions, due to superior quality of optimization obtained per time frame.


Treasure Hunt Framework can be summarized by the following advantages:
- Independence of the optimization method.
- Hierarchical search space matching a fast tree topology. 
- Decentralized and asynchronous communication. 
- Simple growth to a massive number of nodes. 
- Fast propagation of information. 
- Very low network usage. 
- Simultaneous exploration/intensification on several levels of granularity. 
- Good scalability. 
- Controlled work overlap. 
- Search stagnation management. 
- Decentralized joint optimization. 
- Large sampling of the search space. 


# ![TH logo](media/TH-logo-favicon-1.png) TH API Implementation

This open source project is a highly flexible TH implementation that currently supports 7 types of interfaces to fine control the behavior of each TH instances.

Each type of interface allows tight control over a different aspect of the optimization, such that a large variety of types of problems, optimizers and running environments can be take advantage of TH.

Default implementations are provided for 6 out of the 7 types of interfaces currently supported (except for the fitness/cost function itself), covering most of standard usages for simpler problems.




# ![TH logo](media/TH-logo-favicon-1.png) Examples

Three complete examples are provided, all using the Rosenbrock fitness function:
- `examples/TH_example_1TH_1alg.cpp`: optimization using 1 single TH instance and the PSO optimization algorithm;
- `examples/TH_example_1TH.cpp`: optimization using 1 single TH instance and two distinct configurations of PSO optimization algorithm;
- `examples/TH_example_7TH.cpp`: optimization using 7 TH instances and multiple configurations of PSO and Hill Climbing optimization algorithms.

To build the examples, just enter the folder `examples` and run the command `make`.

To run the examples, you must have the MPI installed:
- For `examples/TH_example_1TH_1alg.cpp`: run the command `mpirun -n 1 TH_example_1TH_1alg`;
- For `examples/TH_example_1TH.cpp`: run the command `mpirun -n 1 TH_example_1TH`;
- For `examples/TH_example_7TH.cpp`: run the command `mpirun -n 7 TH_example_7TH`.

To see more or less trace outputs, switch the global compilation parameter `DEBUG` to one of these values: `DEBUG_NONE`, `DEBUG_BASIC` or `DEBUG_DETAILED`.

For a reasonably deterministic behavior, change the global compilation parameter `RANDBEHAVIOR` to `RANDRANDBEHAVIOR_DETERMINISTIC`. However, be aware that deterministic behavior also depends on external factors, like the optimization algorithms and execution configurations (wall clock time, number of evaluations, etc).


## ![TH logo](media/TH-logo-favicon-1.png) References

For Treasure Hunt Framework, please refer (and cite) to the following:

> _PERRONI, Peter Frank; DELGADO, Myriam Regattieri; WEINGAERTNER, Daniel. Treasure Hunt Framework: Distributing Metaheuristics on High Performance Computing Systems. Swarm and Evolutionary Computation, p. 100906, 2021_

 
For the CSMOn (formerly C'MOn) method, please refer (and cite) to the following paper:
> _PERRONI, Peter Frank; WEINGAERTNER, Daniel; DELGADO, Myriam Regattieri. Estimating stop conditions of swarm based stochastic metaheuristic algorithms. In: Proceedings of the Genetic and Evolutionary Computation Conference. 2017._

For the Iterative Partitioning method used by TH, please refer (and cite) to the following paper:

> _PERRONI, Peter Frank; WEINGAERTNER, Daniel; DELGADO, Myriam Regattieri. Automated iterative partitioning for cooperatively coevolving particle swarms in large scale optimization. In: 2015 Brazilian Conference on Intelligent Systems (BRACIS). IEEE, 2015. p. 19-24._

For a more throughout explanation of the method, please refer to the original thesis:
> _PERRONI, Peter Frank (2019). Treasure hunt: a framework for cooperative, distributed parallel optimization (Doctoral thesis, UFPR). Institutional repository at UFPR: https://acervodigital.ufpr.br/handle/1884/62517_
