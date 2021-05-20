import java.util.*;
import java.rmi.registry.LocateRegistry; 
import java.rmi.registry.Registry;  


public class Client { 
	private Client() {}
	public static void main(String[] args) 
	{ 
		try { 
            if(args.length!=2)
			{
				System.out.println("Usage: java ShareFile <server-ip> <server-port>");
				System.exit(0);
			}

			String ip = args[0];            
			Integer run_port = Integer.parseInt(args[1]);
			Registry registry = LocateRegistry.getRegistry(run_port);
			
			Mst stub = (Mst) registry.lookup("rmi://" + ip + ":" + run_port + "/mst");

            String str;
			int mst_weight;
            String delims = " ";
            Scanner scanner = new Scanner(System.in);

            while(true) {
                if(scanner.hasNextLine()){
                    str = scanner.nextLine();
                    String[] tokens = str.split(delims);
                    
                    if(tokens[0].equals("add_graph")){
                        stub.add_graph(tokens[1], Integer.parseInt(tokens[2]));
                    }
                    else if(tokens[0].equals("add_edge")){
                        stub.add_edge(tokens[1],Integer.parseInt(tokens[2]),Integer.parseInt(tokens[3]),Integer.parseInt(tokens[4]));
                    }
                    else if(tokens[0].equals("get_mst")){
                        mst_weight = stub.get_mst(tokens[1]);
                        System.out.println(mst_weight);
                    }
                }
                else{
                    System.exit(1);
                }
            }
 
		} 
		catch (Exception e) {
				System.err.println("Client exception: " + e.toString()); 
				e.printStackTrace(); 
			} 
	} 
}