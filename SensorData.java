/*
 * SensorData.java
 * 
 * Created: 2017-03-27
 * Author: Erik Karvonen
 * Version: 1.0
 */


//Import relevant libraries
import java.util.ArrayList;
import java.util.Arrays;

import javax.swing.JScrollBar;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;


/*
 * The SensorData class is handling the sensordata that we are recieving from the Bluetooth
 * It saves the data in a buffer and then prints that buffer to corresponding textfields in 
 * the GUI.
 */

public class SensorData {
	//Useful variables
	//Data
	private float[] sensorData;
	//Text
	private ArrayList<JTextField> sensorDataText = new ArrayList<>();
	public JTextArea drivingDecisions;
	public String styrning = "Väntar på instruktion";
	public String prevStyrning = "";
	//Misc
	public Map karta;
	public int[][] mapNodes,createdMap;
	public JScrollBar vertical;

	
	//Constructor of the class	
	public SensorData( JTextField hastighet, JTextField avståndFram, 
			JTextField avståndKameraFram, JTextField styrBeslut,
			JTextField position, JTextField avståndKameraHöger, 
			JTextField avståndKameraVänster, JTextArea drivingDecisions,
			Map karta, JScrollPane scrollPane){
		this.sensorData = new float[9];
		Arrays.fill(sensorData, 0);
		this.sensorDataText.add(hastighet);
		this.sensorDataText.add(avståndFram);
		this.sensorDataText.add(avståndKameraHöger);
		this.sensorDataText.add(avståndKameraVänster);
		this.sensorDataText.add(avståndKameraFram);
		this.sensorDataText.add(styrBeslut);
		this.sensorDataText.add(position);
		this.drivingDecisions=drivingDecisions;
		this.karta=karta;
		this.vertical = scrollPane.getVerticalScrollBar();
		
	}

	/* 0 - Velocity
	 * 1 - Distance front laser
	 * 2 - Distance camera right
	 * 3 - Distance camera left
	 * 4 - Distance camera front
	 * 5 - Steering decision
	 * 6 - Position
	 * 7 - Free
	 */

	/*
	 * Function updateSensorData, used in InitSerialPort for recieving
	 * data from bluetooth and saving it in the SensorData class.
	 * Converts the data if needed
	*/
	public void updateSensorData(int dataType, float data){
		//Converts the data if needed
		if (dataType == 0) {
		} 
		else if (dataType == 1) {
		} 
		else if (dataType == 2) {
		} 
		else if (dataType == 3) {
		} 
		else if (dataType == 4) {
		} 
		else if (dataType == 5) {
		}
		else if (dataType == 6) {
			//Send the cars' new position to the Map-class 
			int position = (int) data;
			karta.updateCarPosition(position, karta.slut);
		}
		else if (dataType == 7) {
		}
		//Save the data in a buffer for sensor data
		sensorData[dataType]=data;
	}

	
	/*
	 * Function printSensorData takes the saved buffer and prints correct data 
	 * to the corresponding place in the GUI
	 */
	public void printSensorData(){
		for(int i=0; i<sensorDataText.size(); i++){
			//Steering decision for the car comes in hexadecimal numbers
			//Has to be converted to strings with a switch/case function
			if(i==5){
				int styrBeslut = (int) sensorData[i];
				switch (styrBeslut) {
				case 192:
					styrning = "Autonom körning";
					break;
				case 208:
					styrning = "Korsning vänster";
					break;
				case 224:
					styrning = "Korsning höger";
					break;
				case 129:
					styrning = "Korsning rakt fram";
					break;
				case 130:
					styrning = "Stopplinje";
					break;
				case 131:
					styrning = "Parkeringsficka";
					break;
				case 240:
					styrning = "Stannar vid stopplinje";
					break;
				case 221:
					styrning = "Stanna för hinder";
					break;
				case 255:
					styrning = "Nod klar";
					break;
				}
				/*Because of that the steering 
				* decision is continuously received
				* the program checks what it previously 
				* had and if it hasn't changed, it won't print it.
				*/
				if(!styrning.equals(prevStyrning)){
					drivingDecisions.setText( 
							drivingDecisions.getText() + 
							styrning + "\n");
				}
				prevStyrning = styrning;
			
				vertical.setValue(
						vertical.getMaximum());
			}
			else{
				//Every sensor data except 
				//steering decision is printed out in the
				//corresponding text field 
				sensorDataText.get(i).setText(
						String.valueOf(sensorData[i]));
			}
		}
	}	
}
