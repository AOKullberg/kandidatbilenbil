import java.util.ArrayList;
import java.util.Arrays;

import javax.swing.JTextField;

public class SensorData {
	private float[] sensorData;
	private ArrayList<JTextField> sensorDataText = new ArrayList<>();
	
	public SensorData( JTextField hastighet, JTextField avståndFram, JTextField avståndBak, JTextField avståndHöger,
			JTextField avståndVänster){
		this.sensorData = new float[8];
		Arrays.fill(sensorData, 0);
		this.sensorDataText.add(hastighet);
		this.sensorDataText.add(avståndFram);
		this.sensorDataText.add(avståndBak);
		this.sensorDataText.add(avståndHöger);
		this.sensorDataText.add(avståndVänster);
	
	}

	
	/* 0 - hastighet
	 * 1 - avstånd fram
	 * 2 - avstånd bak
	 * 3 - avstånd höger
	 * 4 - avstånd vänster
	 * 5 - Styrbeslut
	 * 6 - Ledigt
	 * 7 - Ledigt
	 */
	
	public void updateSensorData(int dataType, float data){
		
		//Om konvertering behövs
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
		sensorData[dataType]=data;
	}
	
	public void printSensorData(){
		for(int i=0; i<sensorDataText.size(); i++){
			sensorDataText.get(i).setText(String.valueOf(sensorData[i]));
		}
	}
}
