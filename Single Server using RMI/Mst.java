import java.util.*;
import java.lang.*;
import java.io.*;
import java.rmi.Remote; 
import java.rmi.RemoteException;  

// Creating Remote interface for our application 
public interface Mst extends Remote {
    ArrayList <Graph> Graph_List = new ArrayList <>();
    public class Graph{
        public String identifier;
        public int V;
        public int E;
        public ArrayList <Edge> edges;
    };
    public void add_graph(String id, int V) throws RemoteException;
    class Edge {
        int src, dest, val;
    };
    public void add_edge(String id, int src, int dest, int val) throws RemoteException;
    public int get_mst(String id) throws RemoteException; 
} 