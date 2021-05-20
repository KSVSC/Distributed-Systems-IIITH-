import java.util.*;
import java.lang.*;
import java.io.*;
import java.rmi.registry.Registry; 
import java.rmi.registry.LocateRegistry; 
import java.rmi.RemoteException; 
import java.rmi.server.UnicastRemoteObject; 

public class Server extends Kruskal{
    public Server() {}
    public static void main (String args[]){
        try{
            // Instantiating the implementation class
            if(args.length!=1){
				System.out.println("Usage: java Server <port>"); 
                System.exit(0);
			}

			Integer port = Integer.parseInt(args[0]);

            Kruskal mst = new Kruskal();
            Mst stub = (Mst) UnicastRemoteObject.exportObject(mst, 0);  
         
            // Binding the remote object (stub) in the registry 
            Registry registry = LocateRegistry.createRegistry(port); 
         
            registry.bind("rmi://localhost:"+ port.toString() + "/mst", stub);  
            System.err.println("Server ready");
        }
        catch (Exception e) { 
         System.err.println("Server exception: " + e.toString()); 
         e.printStackTrace(); 
      } 
    }
}