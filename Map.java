/*
 * Map.java
 * 
 * Created: 2017-04-27
 * Author: Erik Karvonen
 * Version: 1.0
 */


import java.util.*;
import java.util.concurrent.TimeUnit;
import java.awt.*;
import java.awt.event.*;
import java.awt.geom.AffineTransform;

import javax.swing.*;


import jssc.SerialPortException;

@SuppressWarnings("serial")
public class Map extends JPanel {
	int width;
	int height;
	static int nrOfNodes=0;
	ArrayList<Node> nodes;
	ArrayList<Edge> edges;
	public int karta[][];
	public JButton skapa, finished;
	public JPanel mapAreaGUI, drawMapArea;
	public SensorData sensors;
	public InitSerialPort sp;
	public boolean mapFinished, isEnabled, reverseWay = false;
	public int road[];
	public int[][] savedMap, savedTypes;
	
	private final int ARR_SIZE = 4;
	public int start,slut;

	String[] values = { "1 - T-korsning vänster", "2 - T-korsning höger",
			"3 - T-korsning rakt fram", "4 - Stopplats", "5 - Parkeringsficka" };
	//public SerialPort chosenPort = new SerialPort("/dev/tty.FireFly_D47F-SPP");

	public Map() { //Constructor
		nodes = new ArrayList<Node>();
		edges = new ArrayList<Edge>();
		width = 30;
		height = 30;
		setPreferredSize(new Dimension(553, 391));
		setBorder(BorderFactory.createLineBorder(Color.black));
	}



	static class car {
		static int x;
		static int y;
		static int currentNode = 0;
		static int destinationNode = 0;

	}
	class Node {
		int x, y;
		String name;
		int nodeNr;

		public Node(String myName, int myX, int myY, int nodeNr) {
			x = myX;
			y = myY;
			name = myName;
			this.nodeNr= nodeNr;

		}
	}

	class Edge {
		int i,j;
		char[] k;
		boolean road, reverseWay = false;


		public Edge(int ii, int jj, String k, boolean reverseWay) {
			i = ii;
			j = jj;	    
			this.k=k.toCharArray();
			this.reverseWay=reverseWay;

		}
	}
	public void addNode(String name, int x, int y, int nodeNr) { 
		//add a node at pixel (x,y)
		nodes.add(new Node(name,x,y,nodeNr));
		this.repaint();
	}
	public void addEdge(int i, int j, String k, boolean reverseWay) {
		//add an edge between nodes i and j
		edges.add(new Edge(i,j,k,reverseWay));
		this.repaint();
	}

	public void paint(Graphics g) { // draw the nodes and edges
		g.setColor(Color.white);
		super.paint(g);
		FontMetrics f = g.getFontMetrics();
		int nodeHeight = Math.max(height, f.getHeight());
		for (Edge e : edges) {
			if(e.road){
				g.setColor(Color.red);
				if(!e.reverseWay){
					drawArrow(g, nodes.get(e.i).x, nodes.get(e.i).y, nodes.get(e.j).x, nodes.get(e.j).y);
				}
				else{
					drawArrow(g, nodes.get(e.i).x, nodes.get(e.i).y, nodes.get(e.j).x, nodes.get(e.j).y);
				}
			}


			//				g.drawLine(nodes.get(e.i).x, nodes.get(e.i).y,
			//						nodes.get(e.j).x, nodes.get(e.j).y);
			if(!e.reverseWay){
				g.setColor(Color.DARK_GRAY);
				drawArrow(g, nodes.get(e.i).x, nodes.get(e.i).y-4, nodes.get(e.j).x, nodes.get(e.j).y-4);
			}
			else if (e.reverseWay){
				g.setColor(Color.GRAY);
				drawArrow(g, nodes.get(e.i).x, nodes.get(e.i).y+4, nodes.get(e.j).x, nodes.get(e.j).y+4);
			}


			int charX = (nodes.get(e.i).x+((nodes.get(e.j).x-nodes.get(e.i).x)/2));
			int charY = (nodes.get(e.i).y+((nodes.get(e.j).y-nodes.get(e.i).y)/2));
			if(!e.reverseWay){
				g.setColor(Color.DARK_GRAY);
				g.drawChars(e.k, 0, e.k.length, charX+20, charY-7);
			}
			else if(e.reverseWay){
				g.setColor(Color.GRAY);
				g.drawChars(e.k, 0, e.k.length, charX-20, charY+20);
			}
		}
		//(nodes.get(e.i).x-nodes.get(e.j).x)
		for (Node n : nodes) {
			int nodeWidth = Math.max(width, f.stringWidth(n.name)+width/2);
			g.setColor(Color.white);
			g.fillOval(n.x-nodeWidth/2, n.y-nodeHeight/2, 
					nodeWidth, nodeHeight);
			g.setColor(Color.black);
			g.drawOval(n.x-nodeWidth/2, n.y-nodeHeight/2, 
					nodeWidth, nodeHeight);

			g.drawString(n.name, n.x-f.stringWidth(n.name)/2,
					n.y+f.getHeight()/2);
		}


		g.setColor(Color.DARK_GRAY);
		g.fillRect(car.x-5, car.y-nodeHeight, 10, 10);


		
	}

	void drawArrow(Graphics g1, int x1, int y1, int x2, int y2) {
		Graphics2D g = (Graphics2D) g1.create();

		double dx = x2 - x1, dy = y2 - y1;
		double angle = Math.atan2(dy, dx);
		int len = (int) Math.sqrt(dx*dx + dy*dy);
		AffineTransform at = AffineTransform.getTranslateInstance(x1, y1);
		at.concatenate(AffineTransform.getRotateInstance(angle));
		g.transform(at);

		// Draw horizontal arrow starting in (0, 0)
		g.drawLine(0, 0, len, 0);
		g.fillPolygon(new int[] {len/2, len/2-ARR_SIZE, len/2-ARR_SIZE, len/2},
				new int[] {0, -ARR_SIZE, ARR_SIZE, 0}, 4);
	}

	public void createMap(InitSerialPort sp, JButton skapa, JPanel mapAreaGUI, SensorData sensors) {
		this.sp=sp;
		this.mapAreaGUI=mapAreaGUI;
		this.sensors=sensors;
		this.skapa=skapa;
		skapa.setVisible(false);
		isEnabled=true;
		drawMapArea = new JPanel();
		add(drawMapArea);


		if((this.getMouseListeners().length==0)){
			addMouseListener(new MouseListener() {
				@Override
				public void mouseReleased(MouseEvent e) {
					// TODO Auto-generated method stub
				}
				@Override
				public void mousePressed(MouseEvent e) {
					// TODO Auto-generated method stub
					if(isEnabled){
						//Integer.parseInt(JOptionPane.showInputDialog("Vad är det för typ av nod? ",JOptionPane.INFORMATION_MESSAGE));
						addNode("Node " + nrOfNodes, e.getX(),e.getY(), nrOfNodes);
						nrOfNodes++;

					}
				}
				@Override
				public void mouseExited(MouseEvent e) {
					// TODO Auto-generated method stub
				}
				@Override
				public void mouseEntered(MouseEvent e) {
					// TODO Auto-generated method stub
				}
				@Override
				public void mouseClicked(MouseEvent e) {
					// TODO Auto-generated method stub
				}

			});
		}


		JPanel buttonPanel = new JPanel();
		finished = new JButton("Klar med noder");
		finished.setBounds(10, 10, 10, 5);
		buttonPanel.add(finished);
		drawMapArea.add(buttonPanel);
		finished.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				// TODO Auto-generated method stub
				finished.setVisible(false);

				
				int[][] karta = new int[nrOfNodes][nrOfNodes];
				int[][] nodeTypes = new int[nrOfNodes][nrOfNodes];
				for(int i=0; i<nrOfNodes ; i++){
					for(int j=0; j<nrOfNodes ; j++){
						//Samma nod
						if(i==j){
							karta[i][j] = 99;
						}
						//
						else{
							karta[i][j]= Integer.parseInt(JOptionPane.showInputDialog("Avstånd mellan nod " + i + " och " + j + ".",0));

							if(karta[i][j] == 0){
								karta[i][j] = 99;

							}
							else{
								if(j<i){
									reverseWay=true;
								}
								else if (j>i){
									reverseWay=false;
								}
								
								addEdge(i, j,  Integer.toString(karta[i][j]), reverseWay);
							}
						}

					}

				}
				
				for(int i=0; i<nrOfNodes ; i++){
					for(int j=0; j<nrOfNodes ; j++){
						if(i==j){
							nodeTypes[i][j]=99;
						}
						else{
							if(!(karta[i][j]==99)){
								Object selected = JOptionPane.showInputDialog(null, "Vilken typ av nod är det när man kör från nod " + i + " till " + j + "?", "Selection", 
										JOptionPane.DEFAULT_OPTION, null, values, "0");
								nodeTypes[i][j] = Character.getNumericValue(selected.toString().charAt(0));
							}
							else{
								nodeTypes[i][j]=99;
							}
						}
					}
				}
				start =Integer.parseInt(JOptionPane.showInputDialog("Välj startnod" ,JOptionPane.INFORMATION_MESSAGE));

				for(Node n : nodes){
					if(n.nodeNr==start){
						n.name="Start" + "\n" + n.name;
						car.x=n.x;
						car.y=n.y;
						car.currentNode=n.nodeNr;
						repaint();
					}
				}

				slut =Integer.parseInt(JOptionPane.showInputDialog("Välj sluttnod" ,JOptionPane.INFORMATION_MESSAGE));
				for(Node n : nodes){
					if(n.nodeNr==slut){
						n.name="Slut"+ "\n" + n.name;
						mapFinished = true;
						repaint();

					}
				}


		
				sensors.createdMap=karta;
								
								try {
									sendMap(karta, nodeTypes);
								} catch (SerialPortException | InterruptedException e1) {
									// TODO Auto-generated catch block
									e1.printStackTrace();
								}
								saveMap(karta,nodeTypes);
				isEnabled=false;
			}
		});

	}

	public void repaintRoad(){
		for(int k=0; k<road.length-1; k++){
			for(Edge edge : edges){			
				if(road[k]==edge.i){
					if(road[k+1] == edge.j){
						System.out.println("Vanlig väg mellan " + road[k] + " och " + road[k+1]);
						edge.road=true;
						repaint();
					}
				}
			}
		}
	}
	public void createRoad(int length){
		for(int k=0; k<length; k++){
			road[length]=0;
		}
	}

	public void updateCarPosition(int newPosition, int destination){
		for(Node n : nodes){
			if(newPosition==n.nodeNr){
				car.x=n.x;
				car.y=n.y;
				car.destinationNode=destination;
				repaint();
			}
		}
	}

	public void sendMap(int[][] karta, int[][] nodeTypes) throws SerialPortException, InterruptedException{

		System.out.println("Send Map");
		sp.chosenPort.writeByte((byte) karta.length);
		System.out.println(karta.length);
		TimeUnit.SECONDS.sleep(1);
		
		System.out.println("");
		for(int i=0; i<karta.length ; i++){
			for(int j=0; j<karta.length ; j++){
				sp.chosenPort.writeByte((byte) karta[i][j]);
				System.out.print(karta[i][j] + ", ");
			}
			System.out.println("");
		}
		TimeUnit.SECONDS.sleep(1);
		
		System.out.println("");

		for(int i=0; i<nodeTypes.length ; i++){
			for(int j=0; j<nodeTypes.length ; j++){
				sp.chosenPort.writeByte((byte) nodeTypes[i][j]);
				System.out.print(nodeTypes[i][j] + ", ");
			}
			System.out.println("");
		}
		TimeUnit.SECONDS.sleep(1);
		
		sp.chosenPort.writeByte((byte) start);
		System.out.println(start);
		TimeUnit.SECONDS.sleep(1);
		
		sp.chosenPort.writeByte((byte) slut);
		System.out.println(slut);
		sp.recievingRoad=true;
		sp.recievingFirstSensorData=false;
	}

	public void clearMap(){
		nodes.clear();
		edges.clear();
		skapa.setVisible(true);
		skapa.setEnabled(true);
		finished.setVisible(false);
		nrOfNodes=0;
		car.x=0;
		car.y=0;
		isEnabled=false;
		drawMapArea.setVisible(false);
		System.out.println("Cleared: " + nrOfNodes);
	}
	public void saveMap(int[][] karta, int[][] nodeTypes){
		savedMap=karta;
		savedTypes=nodeTypes;
	}
	public void newDestination(int newStart, int newStop) throws SerialPortException, InterruptedException{
		System.out.println("Nystart: "+newStart);
		System.out.println("Nystart: "+newStop);
		for(Node n : nodes){
			if(n.nodeNr==start){
				n.name="Node " + n.nodeNr;
				repaint();
//				System.out.println("Bilen ligger på:" + car.x + " x, " + car.y + " y.");
			}
		}
		for(Node n : nodes){
			if(n.nodeNr==slut){
				n.name="Node " + n.nodeNr;
				repaint();
			}
		}
		for(Edge e : edges){
			if(e.road){
				e.road=false;
				repaint();
			}
		}
		
		start=newStart;
		slut=newStop;
		sp.chosenPort.writeByte((byte) 0xEE);
		TimeUnit.SECONDS.sleep(1);
		
		System.out.println("Start: " + start);
		sp.chosenPort.writeByte((byte)  start);
		
		for(Node n : nodes){
			if(n.nodeNr==start){
				n.name="Start" + "\n" + "Node " + n.nodeNr;
				car.x=n.x;
				car.y=n.y;
				car.currentNode=n.nodeNr;
				repaint();
//				System.out.println("Bilen ligger på:" + car.x + " x, " + car.y + " y.");
			}
		}
		
		
		System.out.println("Slut: " + slut);
		sp.chosenPort.writeByte((byte) slut);
		for(Node n : nodes){
			if(n.nodeNr==slut){
				n.name="Slut" + "\n" + "Node " + n.nodeNr;
				repaint();
//				System.out.println("Bilen ligger på:" + car.x + " x, " + car.y + " y.");
			}
		}
		
	}
	


}