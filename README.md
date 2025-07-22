[![Build Linux](https://github.com/FreeYourSoul/Fabko/actions/workflows/ci-linux.yml/badge.svg)](https://github.com/FreeYourSoul/Fabko/actions/workflows/ci-linux.yml)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/7d6160f5dc1246888f0fa94ab48ddfaa)](https://app.codacy.com/gh/FreeYourSoul/Fabko/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)
![](https://tokei.rs/b1/github/FreeYourSoul/Fabko?category=lines)

# FabKo

Free Agent Board operative is an agent / blackboard library that enables distributed resolution of a problem among
Agents.

# Introduction

Fabko is a library that some to make it easy to create agent that interoperates via customizable communication medium (
peer-to-peer, centralised server based, distributed etc...).

Those agent will be able to register their capability to an agent node called a "peerboard" node.
The agent will then be able to communicate through a blackboard generated each time an agent attempts to achieve a new
action. And the agent is going to cooperate to find a solution to resolve that problem.

## FABL (Free Agent Board Language)

Free Agent Board Language is the base of the FabKo library as it is a declarative language that makes it possible for an
agent to describe the resources and
the capabilities that it provides. Those can be aggregated among multiple agents to produce complex graphs. It is then
possible to produce a request over this
compiled graph to define if the agent can do an action or not.
The compiled graph represents the blackboard on which each node can add their own descriptions of actions. The agent
that holds the peerboard is the agent that aggregates
all the capabilities of each agent to provide solutions.

The language is split into different sections: `agent`, `capabilities`.

```mnz

board_size : int @parameter;

Coord :: struct {
    int : x
    int : y
};

alias Size = Coord;

Queen :: actor {
    pos : Coord
};

queens : Queen[board_size];

# minizinc style constraint
constraint all_different([q.x for q in queens]);
constraint all_different([q.y for q in queens]); 
constraint all_different([queens[i].y + 1 for i in 1 .. board_size]);
constraint all_different([queens[i].x + 1 for i in 1 .. board_size]); 

```

actor part of FABL

```rust


```

Here is an example of the 9 Queen problems using the actor syntax