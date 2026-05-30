[![Build Linux](https://github.com/FreeYourSoul/Fabko/actions/workflows/ci-linux.yml/badge.svg)](https://github.com/FreeYourSoul/Fabko/actions/workflows/ci-linux.yml)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/7d6160f5dc1246888f0fa94ab48ddfaa)](https://app.codacy.com/gh/FreeYourSoul/Fabko/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)
![](https://tokei.rs/b1/github/FreeYourSoul/Fabko?category=lines)

# FabKo

Free Agent Board operative is an agent / blackboard library that enables distributed resolution of a problem among
Agents.

# Introduction

Fabko is a library that some to make it easy to create agent that interoperates via customizable communication medium (
peer-to-peer, centralized server based, distributed, etc...).

Those agents will be able to register their capability to an agent node called a "peerboard" node.
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

The language is split into different types: `agent`, `capabilities` and `resources`.

Here is the FABL language :

```fabl
////////////////
/// fabl language
////////////////

// Define locations
// builtins types are : bool, int, str

type vec2  : { x: int, y: int; };
type location  : vec2;
type rectangle : vec2;

// Define resources
// every agent (not environment) implicitly has 0 of every defined resources. This is useful to provides the ability
// for agent to receive resources they did not existed before the apparition of another agent in their landscape.
resource bike : int;
resource money : int;
resource time_unit: int;
resource position: location;

// agent A - Transportation service
agent A {
  has 1 position;

  can capability transport {
    pre: {
        self.time == 0;
        user.time == 0;

        user.money > 100;
        user.position.x == 42 ;
        user.position.y >= 1337;
    }
    post: {
        // takes 3 instance unit to sell the bike
        self.time + 3;
        user.time + 3;

        user.money --100--> self.money;

        user.position.x = 100;
        user.position.y = 100;
    }
  };

}

capability provide_bike(quantity: int, price: int) {
  pre: {
      self.bike >= quantity;
      user.money > price;
  }
  post: {
    self.bike --quantity--> user.bikes_owned;
    user.money   --price--> self.money;
  }
}

// agent C - Resource provider
agent C {
  has 10 bike;

  can capability provide_bike {
    pre: {
      user.money > 10;
    }
    post: {
      self.bike   --1--> user.bike;
      user.money --10--> self.money;
    }
  };
  // same as
  // can provide_bike(1, 100);
}

// agent B - The one who wants to move
agent B {
  has 1 position;
}

// it is possible to extends the definition of an agent outside of the declaration scope
extend agent B {
  can provide_bike(1, 110);
};

// environment is exactly like an agent, except all its capabilities and resources are constant by default
environment X {
}

// Request: Make B reach Z

var RESULT_POS: location {x: 100, y: 100} // constant global

request reach_destination {
  actor: B;
  goal: {
    B.position == RESULT_POS
  };

  constraints {
    time_limit: 2hours;
    prefer: fastest;
    allow_resource_waste: true;
  }
}
```

actor part of FABL

Here is an example of the 9 Queen problems using the actor syntax

Compilation would be the following steps :

```terminaloutput
ENCODING (loses information):
┌─────────────────────────────────┐
│  FABL: High-level semantics     │
│  - Natural time (minutes)       │
│  - Implicit durations           │
│  - Complex constraints          │
└──────────────┬──────────────────┘
               │ Discretize, abstract, encode
               ↓
┌─────────────────────────────────┐
│  CNF: Boolean logic             │
│  - Slot-level time              │
│  - Explicit all variables       │
│  - Pure boolean clauses         │
└──────────────┬──────────────────┘
               │ SAT solve
               ↓
┌─────────────────────────────────┐
│  Assignment: Variable values    │
│  - 1000s of true/false values   │
│  - Lost high-level structure    │
└──────────────┬──────────────────┘
               │ Reconstruct (with variable registry)
               ↓
┌─────────────────────────────────┐
│  Execution Plan: Lower-level    │
│  - Slot-level time              │
│  - Actions with parameters      │
│  - Lost sub-slot precision      │
└─────────────────────────────────┘
```

## Compile

Fabko is an agent binary that can be compiled using nix. The binary is a nix flake and thus can be compiled using
`nix build` command.
It depends on FiL, a C++ library developed as a nix flake as well.