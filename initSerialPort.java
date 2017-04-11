


import javax.swing.JFrame;

import jssc.SerialPort;
import jssc.SerialPortEvent;
import jssc.SerialPortEventListener;
import jssc.SerialPortException;


public class initSerialPort {

	private JFrame frame;
	public SerialPort chosenPort;
	public SensorData sensors;
	public byte[] data;
	public byte buffer[];
	
	int bytePosition;
	public int[] dataBuffer = new int[5];
	public int[] recievedData = new int[5];
	
	public initSerialPort(SerialPort cP, JFrame frame, SensorData sensors){
		this.chosenPort=cP;
		this.frame = frame;
		this.sensors=sensors;
	}
	
	
	public void initPort () throws SerialPortException{
		
				//mac osx - /dev/tty.FireFly_D47F-SPP
				// Windows - någon (COM1,COM2,COM3)
		chosenPort = new SerialPort("/dev/tty.FireFly_D47F-SPP"); 
		
		
		
		try{
			chosenPort.openPort();
			chosenPort.setParams(chosenPort.BAUDRATE_115200, 8, 1, 0); // Set parameters, baudrate, databits, stopbits, paritybits
			int mask = SerialPort.MASK_RXCHAR + SerialPort.MASK_CTS + SerialPort.MASK_DSR;//Prepare mask
			chosenPort.setEventsMask(mask);//Set mask
			chosenPort.addEventListener(new SerialPortReader());//Add SerialPortEventListener
						
		}
		catch(SerialPortException ex){
			throw ex;
		}
		
		
	}
	
	class SerialPortReader implements SerialPortEventListener{

		boolean recievingMessage = false;
		

		@Override
		public void serialEvent(SerialPortEvent event) {
			// TODO Auto-generated method stub
			//If data is available, //Check bytes count in the input buffer larger than 0, can check = fixed amount of databytes
			if (event.isRXCHAR() && event.getEventValue() > 0){
				//connected
				//read data... 
				try{
					//Read ints to get 0 to 255 values
					int[] buffer = chosenPort.readIntArray();
					
					//System.out.println();
					//Print how many bytes recieved
			       // System.out.print("BUFFER SIZE = " + buffer.length + ", BUFFER DATA = ");
			        //Print all the bytes
//			        for(int k = 0; k < buffer.length; ++k) {
//			        	   System.out.print(k + " ");
//			        }
//			        System.out.println();
				    
			        for(int b : buffer){
//			        	System.out.println("Current byte: " + (int)(b&0xFF) + " ");
//			        	System.out.println("BytePostition " + bytePosition + " ");
			        	
			        	//Startbyte när b==0 t.ex.
			        	if(b == 'a' && !recievingMessage){
				    	  recievingMessage=true;				    		  
//				    	  System.out.println("STARTBYTE\n");
			        	}
				    	
				      //Ta in datat
			        	else if(recievingMessage){
			        		//Stop efter ett visst antal byte (data) 4 st
			        		if(bytePosition==4){
			        			//Add last byte of data
			        			dataBuffer[bytePosition] = b;
			        			//System.out.println("Stop Byte\n");
			        			//End of databytes, time to Update SensorData!
			        			recievingMessage=false;
			        			recievedData = dataBuffer;
			        			bytePosition=0;
//			        			 for(int c: recievedData){
//			        				 System.out.print(c + " ");
//			        			 }
			        			 //Update sensordata
			        			 updateAllSensorData(sensors);
			        			 
			        			 //Print on GUI
			        			 sensors.printSensorData();
			        		}
			        		else {
			        			//(int)(b & 0xFF)?
			        			int data = b;
//			        			System.out.println("b = " + b);
//			        			System.out.println("BytePosition = " + bytePosition);
			        			dataBuffer[bytePosition] = data;
			        			
			        			bytePosition++;
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
		//1 - avståndfram
		sensors.updateSensorData(0, (float)(recievedData[0]&0xFF));
		sensors.updateSensorData(1, (float)(recievedData[1]&0xFF));
		sensors.updateSensorData(2, (float)(recievedData[2]&0xFF));
		sensors.updateSensorData(3, (float)(recievedData[3]&0xFF));
		sensors.updateSensorData(4, (float)(recievedData[4]&0xFF));
		
	}
}
