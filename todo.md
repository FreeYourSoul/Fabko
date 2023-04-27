# TODO

## BUG

## To Think about

* Currently, the blackboard use classic type erasure via inheritance. Its usage is to hide the type of the CommunicationProtocol which will actually be static as we could only have a "local" / "remote" communication. It would fit better into a tuple + visitor pattern for implementation usage than a heavy inheritance type erasure.

* Strong typing for the different values:
  * literal_value (currently unsigned) (value of a literal associated with an assignment)
  * variable      (currently unsigned) (value of the variable :: without any association to an assignment)