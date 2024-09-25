# Class Hierarchy: #

## Graph (Graph Class) ##

### Role: Represents a graph with a number of vertices and edges. ###
# Main Functions:
# addEdge: Adds an edge between two vertices.
# getEdges: Retrieves a list of edges in the graph.
# buildSpanningTree: Builds a spanning tree from the graph.
# getPath: Retrieves the path from the root to a specified vertex.
# getAdjacentEdges: Retrieves the edges adjacent to a specified vertex.

### MST (Abstract Class for Minimum Spanning Tree Calculation) ###
# Role: Defines an interface for computing a minimum spanning tree.

Main Function:
# computeMST: A pure virtual function for computing the MST, which derived classes must implement.

### KruskalMST and PrimMST (MST Implementations) ###
# Role: Implement Kruskal’s and Prim’s algorithms respectively for computing the minimum spanning tree.

Main Function:
# computeMST: Implements the specific algorithm for computing the MST for a given graph.

### StrategyFactory (Abstract Class for Creating Strategies) ###
# Role: Defines an interface for creating MST objects based on the user's selection.

### ConcreteStrategyFactory (StrategyFactory Implementation) ###
# Role: Creates MST objects based on a string representing the strategy name (e.g., "kruskal" or "prim").

Main Function:
# createStrategy: Returns a unique pointer (unique_ptr) to an MST object based on the strategy name.

### Relationships Between Classes: ###
Graph - is the core class upon which all operations are performed.
MST - is the abstract class that defines the interface for computing the minimum spanning tree.
KruskalMST and PrimMST - are the classes that implement this interface using different algorithms.
StrategyFactory - is the class that provides an interface for selecting the MST algorithm.
ConcreteStrategyFactory - implements the StrategyFactory and creates the appropriate algorithm based on the user's request.