# TODO

## BUG

* introduced on logging implementation: sat solver gets incorrect results (sometimes? likely the second usage!?)

## To Think about

* Currently, the blackboard use classic type erasure via inheritance. Its usage is to hide the type of the CommunicationProtocol which will actually be static as we could only have a "local" / "remote" communication. It would fit better into a tuple + visitor pattern for implementation usage than a heavy inheritance type erasure.

* Strong typing for the different values:
  * ~~literal_value (currently unsigned) (value of a literal associated with an assignment)~~
  * variable (currently unsigned) (value of the variable :: without any association to an assignment)

* ~~Improve tests : check variable id also~~

* Improving SAT debug log when clause are contradicted

* ~~create a logging utility (using spdlog)~~

* logger should by default retrieve the logging level in an environment variable

* ~~adapt current fmt::print into proper logging~~
 
