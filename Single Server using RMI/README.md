## Usage 
Run the following commands:
-  `javac *. java`
-  `java Server  \<port>`
-  `java Client  \<server_ip>  \<server_port>` 

## Architecture Details

- RMI(Remote Method Invocation) was used to implement a simple single architecture.
- The server maintains a list of graphs each associated with a distinct identifier.
- Clients can request to add a new graph, update an existing graph and query for the total weight of the minimum weight spanning tree of a given graph.
- Clients request for graph creation using `add_graph 〈graph identifier〉 n`.
- Clients can request to add a new edge in a graph using `add_edge 〈graph identifier〉〈u〉〈v〉〈w〉`.
- Clients can request for the total weight of the minimum weight spanning tree in a graph from the server using `get mst 〈graph identifier〉`.
- It can support multiple agents. Runs on localhost.

#### MST Algorithm
- Kruskal's algorithm is used.
