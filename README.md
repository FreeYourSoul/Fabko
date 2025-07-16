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

```rust

define MAX_BOARD = 8; // defines are string to value (replaced in the same way than a pre-processor #define)
define MIN_BOARD = 0; // the difference that the copies occurs for the whole scope of the define until a ';' is found (\n here is taken as well)
pub define NAME = "9QueenProblems"; // here the define is published and thus public to the code aggregating this 

Position :: struct {
  x: int@4; // adding @4 indicates that the integer is represented on 4 bits
  y: int;   // the bit_size function is a function to retrieve the minimum number of bytes required for the provided number (here 8, which would be 4 bits)
};

Queen :: struct {
  position: Position;
};
  
queens: Queen[9];
queen1 alias Queen[0]; 

Reine alias Queen; // aliasing a type name

// (var, var) -> { /*predicates;*/ }  : this represent a lambda predicate 
all_of(queens, (i: int, q: Reine) -> { // all_of is a foreach loop where every condition inside must be truee for the predicate
    q.position.x <= MAX_BOARD;
    q.position.x >= MIN_BOARD;
   
    all_of(queens, 
      (j: int, qq: Queen) -> { i != j; }, // optional conditions on all_of, if predicate is filled, then the generation of constraint occurs
      (j: int, qq: Queen) -> {
        { q.position.x != qq.position.x;  q.position.y != qq.position.y; };           // positions cannot be equal
        { q.position.x + j != qq.position.x;  q.position.y == qq.position.y; };       // position cannot be on the same line
        { q.position.y + j != qq.position.y;  q.position.x == qq.position.x; };       // position cannot be on the same collumn
        { abs(q.position.x - qq.position.x) != abs(q.position.y - qq.position.y); };  // no other queen on the diaogonals
    });
});

```

actor part of FABL

```rust

Item :: struct {
  id: label@key; // @key defines the member as beeing the one defining the search capability 
}
// Item alias label;  // equivalent ?

pub cap_sell :: cap (item: Item, price: int) { // pub make it so that cap_sell can be used accross compilation unit

  constraint {                // only constraint actions accepted
    this.items[item.id] > 0;  // seller must have the item to sell
    that.money >= price;      // buyer must have enough money
  },

  side_effect {               // effect on actor (here actors (this and that) are mutable) : only mutable actions accepted
    this.money += price;      // seller earn the money of the object sold

    that.items[item.id] += 1; // buyer get  one of the item sold (item is retrieved by key)
    that.money -= price;      // buyer lose the money from the object bought
  }

};

move_from_to :: cap (from: Position, to: Position) {
  constraint {
    this.position == from;
    that.position == from;
  },
  side_effect {
    this.position = to;
    that.position = to;
  }
}; 

P1 = Position(x=10, y=20);
P2 = Position(x=20, y=30);
P3 = Position(x=20, y=40);
P4 = Position(x=50, y=50);

CharacterA :: actor {

  money     : int;
  position  : Position;
  items     : quantity<Item>;

  capabilities {
    cap_sell(this, that, Item(label="object1"));
    cap_sell(this, that, Item(label="object2"));  
    move_from_to(P1, P2);  
  };

};

CharacterB :: actor {
  position  : Position;

  capabilities {
    move_from_to(P2, P3);  
  };

};

CharacterB :: actor {

  position  : Position;

  capabilities {
    move_from_to(P2, P4);  
  };

};

```

Here is an example of the 9 Queen problems using the actor syntax