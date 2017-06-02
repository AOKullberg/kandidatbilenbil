/*
 * InitSerialPort.java
 * 
 * Created: 2017-04-01
 * Author: Erik Karvonen
 * Version: 1.0
 */

import javax.swing.JFrame;
import javax.swing.JOptionPane;

import jssc.SerialPort;
import jssc.SerialPortEvent;
import jssc.SerialPortEventListener;
import jssc.SerialPortException;


public class InitSerialPort {

	private JFrame frame;
	public SerialPort chosenPort;
	public SensorData sensors;
	public Map map;
	public byte[] data;
	public byte buffer[];
	
	int bytePosition;
	public int[] dataBuffer = new int[9];
	public int[] recievedData = new int[9];
	public int mapSize, roadSize;
	public int[] mapBuffer, roadBuffer;
	
	public boolean recievingRoad = false;
	public boolean recievingFirstSensorData = true;
	
	public InitSerialPort(SerialPort cP, JFrame frame, SensorData sensors, Map map){
		this.chosenPort=cP;
		this.frame = frame;
		this.sensors=sensors;
		this.map=map;
	}
	
	public void choosePort(String portName){
		chosenPort = new SerialPort(portName); 
	}
	@SuppressWarnings("static-access")
	public void initPort () throws SerialPortException{
		
				//mac osx - /dev/tty.FireFly_D47F-SPP
//		Bluetooth-Incoming-Port
				// Windows - någon (COM1,COM2,COM3)
		
		
		try{
			chosenPort.openPort();
			chosenPort.setParams(chosenPort.BAUDRATE_115200, 8, 1, 0); // Set parameters, baudrate, databits, stopbits, paritybits
			int mask = SerialPort.MASK_RXCHAR + SerialPort.MASK_CTS + SerialPort.MASK_DSR;//Prepare mask
			chosenPort.setEventsMask(mask);//Set mask
			
			chosenPort.addEventListener(new SerialPortReader());//Add SerialPortEventListener
			
						
		}
		catch(SerialPortException ex){
			JOptionPane.showMessageDialog(frame, "Bluetooth är ej kopplad!");
			throw ex;
		}
		
		
	}
	
	class SerialPortReader implements SerialPortEventListener{

		boolean recievingMessage, recievingMap, recievedMapSize, 
				 recievedRoadSize = false;
		

		@Override
		public void serialEvent(SerialPortEvent event) {

			if (event.isRXCHAR() && event.getEventValue() > 0){
				//connected
				//read data... 
				try{
					//Read ints to get 0 to 255 values
					int[] buffer = chosenPort.readIntArray();
					

			        System.out.print("BUFFER SIZE = " + buffer.length + ", BUFFER DATA = ");

			        for(int i = 0; i<buffer.length; i++){
			        	System.out.println(buffer[i]);
			        }
			        
//			        System.out.println();

			        for(int b : buffer){

			        	//Sensordata
			        	
			        	if(b == 'a' && recievingFirstSensorData){
			        		recievingMessage=true;	
			        		recievingFirstSensorData = false;
			        	}
				    	
				      //Ta in datat
			        	else if(recievingMessage){
			        		//Stop efter ett visst antal byte (data) 4 st
			        		if(bytePosition==7){
			        			//Add last byte of data
			        			dataBuffer[bytePosition] = b;
			        			recievingMessage=false;
			        			recievingFirstSensorData=true;
			        			recievedData = dataBuffer;
			        			bytePosition=0;

			        			 //Update sensordata
			        			 updateAllSensorData(sensors);
			        			 
			        			 //Print on GUI
			        			 sensors.printSensorData();
			        		}
			        		else {
			        			int data = b;
			        			dataBuffer[bytePosition] = data;			        			
			        			bytePosition++;
			        		}				    	  
			        	}
//			        	Vägval
			        	else if(b==114 && recievingRoad){
			        		System.out.println("Recieving road");
			        	}
			        	else if(recievingRoad){
			        		recievedRoadSize=true;
			        		roadSize = b;
			        		roadBuffer = new int[roadSize];
			        		System.out.println("Recieving roadSize :" + roadSize);
			        		recievingRoad=false;
			        	}
			        	else if(recievedRoadSize&&!recievingRoad){
			        		roadBuffer[bytePosition]=b;
			        		bytePosition++;
			        		System.out.println("Recieving road nod: " + b);
			        		if(bytePosition==roadSize){
			        			recievedRoadSize=false;
			        			int roadBuffer2 [] = new int[roadSize];
				        		for(int i=0; i<roadBuffer.length; i++){
				    				roadBuffer2[roadBuffer.length-i-1]=roadBuffer[i];
				    			}
				        		map.road=roadBuffer2;
				        		System.out.println("roadBuffer2[i] , ");
				        		for(int i=0; i<roadBuffer2.length; i++){
				        			System.out.println(roadBuffer2[i] + ", ");
				        		}
				        		map.repaintRoad();
				        		recievingRoad=false;
				        		recievedRoadSize=false;	
				        		bytePosition=0;
				        		recievingFirstSensorData=true;
			        		}
			        	}				      
				     }					
				}
				catch (SerialPortException ex) {
		            System.out.println(ex);
		            System.out.println("serialEvent");
		        }  
		   }	
		}	
	}
	
	
	public void updateAllSensorData(SensorData sensors){

		
		sensors.updateSensorData(0, (float)(recievedData[0]&0xFF));
		sensors.updateSensorData(1, (float)(recievedData[1]&0xFF));
		sensors.updateSensorData(2, (float)(recievedData[2]&0xFF));
		sensors.updateSensorData(3, (float)(recievedData[3]&0xFF));
		sensors.updateSensorData(4, (float)(recievedData[4]&0xFF));
		sensors.updateSensorData(5, (float)(recievedData[5]&0xFF));
		sensors.updateSensorData(6, (float)(recievedData[6]&0xFF));
		sensors.updateSensorData(7, (float)(recievedData[7]&0xFF));
		sensors.updateSensorData(8, (float)(recievedData[8]&0xFF));
		
	}
}
