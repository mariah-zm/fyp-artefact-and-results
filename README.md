# Artefact accompying the FYP titled: _A Framework for Modelling Believable Behaviour of Ambient Non-Player Characters_

This repository contains the source code for the case study discussed in the FYP report. This case study serves as a base implementation of the proposed framework. It implements a university setting and models the behaviour of a population of students, professors and admin staff. The requirements and instructions explaining how to set up and run the simulation locally are found below. 

The repository also includes the Jupyter Notebook scripts used to evaluate the framework in the directory [evaluation](https://github.com/mariah-zm/fyp-artefact-and-results/evaluation). These scripts process the behavioural traces generated from a simulation run and analyse the traces for believable behaviour. The directory [results](https://github.com/mariah-zm/fyp-artefact-and-results/results) contains the results of one simulation, which was run with the configuration file that comes with the case study, [case-study/config.json](https://github.com/mariah-zm/fyp-artefact-and-results/case-study/config.json).

## Requirements and Third-Party Libraries and Tools used  

_Note that this code was developed with the intention of being compiled on a Linux system._

The framework implementation uses the following third-party libraries and tools:
* [Flecs](https://www.flecs.dev/flecs/): used to develop the framework as an ECS implementation.
* [FuzzyLite](https://www.fuzzylite.com/): used to define the fuzzy logic rule engine. 
* [Boost C++ Libraries](https://www.boost.org/): used for the different distributions required during population setup.
* [Lohmann’s JSON library](https://json.nlohmann.me/): used to read configuration files in JSON format during the simulation setup.

The instructions given in the next section assume that the user has the following installed on their Linux system:
* CMake - version 3.16 or higher
* GNU C/C++ compiler - version with C++20 support

## Instructions to Compile and Run

1. Clone this repository locally. 
2. Install Boost Libraries using the command `sudo apt-get install libboost-all-dev`.
3. Navigate to the directory [case-study/fuzzylite](https://github.com/mariah-zm/fyp-artefact-and-results/case-study/fuzzylite), and run `./build.sh` to build the FuzzyLite library. 
4. Navigate to the [case-study](https://github.com/mariah-zm/fyp-artefact-and-results/case-study) directory, and run the following commands to compile the project with CMake:
```
cmake .
make
```
5. CMake should have now generated an executable called `university`. To run the simulation, execute the command:
```
./university path/to/config path/to/results
```
in which the first argument should be the path to the configuration file, and the second argument is optional which should specify where to output the results of the simulation if these are desired. This artefact comes with a [config](https://github.com/mariah-zm/fyp-artefact-and-results/case-study/config.json) file that can be used to run the simulation.
