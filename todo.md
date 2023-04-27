# TODO

## BUG

## To Think about

* Currently the blackboard use classic type erasure via inheritance. It's usage is to hide the type of the CommunicationProtocol which will actually be static as we could only have a "local" / "remote" communication. It would fit better into a tuple + visitor pattern for implementation usage than a heavy inheritance type erasure.