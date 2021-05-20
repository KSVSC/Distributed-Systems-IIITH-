import java.util.*;
import java.lang.*;
import java.io.*;

// Implementing the remote interface 
public class Kruskal implements Mst {  
   
    // Implementing the interface method 
    private ArrayList <Graph> Graph_List = new ArrayList <>();
    
    public class Graph{
        public String identifier;
        public int V;
        public int E;
        public ArrayList <Edge> edges = new ArrayList <>();

        public Graph(String id, int v){
            this.V = v;
            this.E = 0;
            this.identifier = id;
        }
    }

    public void add_graph(String id, int V){
        Graph new_graph= new Graph(id, V);
        this.Graph_List.add(new_graph);
    };

    class Edge {
            int src, dest, val;
            public Edge(int src, int dest, int val){
                this.src = src;
                this.dest = dest;
                this.val = val;
            }
    };

    public void add_edge(String id, int src, int dest, int val){
        int idx = -1;
        for(int i=0;i<this.Graph_List.size();i++){
            if(this.Graph_List.get(i).identifier.equals(id)){
                idx = i;
            }
        }
        if(idx != -1) {
            this.Graph_List.get(idx).E += 1;
            Edge edge = new Edge(src - 1, dest - 1, val);
            this.Graph_List.get(idx).edges.add(edge);
        }

    }

    class Subtree{
        int root, rank;
    };

    public int parent(Subtree tree[], int i) { 
        if (tree[i].root != i) 
            tree[i].root = parent(tree, tree[i].root); 

        return tree[i].root; 
    }

    public void Join(Subtree tree[], int x, int y) { 
        int par1 = parent(tree, x); 
        int par2 = parent(tree, y);

        if (tree[par1].rank < tree[par2].rank){
            tree[par1].root = par2; 
        } 
        else if (tree[par1].rank > tree[par2].rank){
            tree[par2].root = par1; 
        } 
        else
        { 
            tree[par2].root = par1; 
            tree[par1].rank++; 
        } 
    }

    public int get_mst(String id){
        int idx = -1;
        int mst_weight = 0;

        for(int i=0;i<this.Graph_List.size();i++){
            if(this.Graph_List.get(i).identifier.equals(id)){
                idx = i;
            }
        }

        if(idx != -1) {
            if(this.Graph_List.get(idx).E == 0){
                return -1;
            }

            Edge mst[] = new Edge[this.Graph_List.get(idx).V];
            for(int i=0;i<this.Graph_List.get(idx).V;i++){
                mst[i] = new Edge(0,0,0); 
            }
            Collections.sort(this.Graph_List.get(idx).edges, new Comparator<Edge>(){
                public int compare(Edge a, Edge b) 
                { 
                    return a.val-b.val; 
                }
            });

            Subtree tree[] = new Subtree[this.Graph_List.get(idx).V];
            for(int i=0; i<this.Graph_List.get(idx).V; i++){
                tree[i] = new Subtree();
                tree[i].root = i;
                tree[i].rank = 0;
            }
            int cnt = 0, e = 0;
            while(cnt<this.Graph_List.get(idx).V-1){
                if(e >= this.Graph_List.get(idx).E){
                    return -1;
                }
                Edge temp = this.Graph_List.get(idx).edges.get(e++);

                int par1 = parent(tree,temp.src);
                int par2 = parent(tree,temp.dest);

                if(par1 != par2){
                    mst[cnt++] = temp;
                    mst_weight += temp.val;
                    Join(tree,par1,par2);
                }

            }
        }
        return mst_weight;
    }  
} 