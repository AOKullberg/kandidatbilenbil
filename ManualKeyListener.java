/*
 * ManualKeyListener.java
 * 
 * Created: 2017-03-26
 * Author: Erik Karvonen
 * Version: 1.0
 */


import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

import javax.swing.JRadioButton;
import javax.swing.JTextArea;

import jssc.SerialPortException;

public class ManualKeyListener implements KeyListener {

	public InitSerialPort sp;
	public JTextArea drivingDecisions;

	public boolean breakCar = false;
	public Map karta;
	private byte writeAccelerate, writeAngle, writeCommand = 0;
public JRadioButton rdbtnManuell;

	public ManualKeyListener(InitSerialPort sp, JTextArea drivingDecisions, JRadioButton rdbtnManuell, Map karta) {
		this.sp = sp;
		this.drivingDecisions = drivingDecisions;
		this.rdbtnManuell = rdbtnManuell;
		this.karta = karta;
	}

	@Override
	public void keyTyped(KeyEvent e) {

	}

	@Override
	public void keyPressed(KeyEvent e) {
		if (rdbtnManuell.isSelected()) {
			int keyCode = e.getKeyCode();
			switch (keyCode) {
			case KeyEvent.VK_UP:
				// Send forward to bluetooth
				writeAccelerate = 1;
				drivingDecisions.setText(drivingDecisions.getText() + "Framåt\n");
				break;
			case KeyEvent.VK_DOWN:
				// Send backward to bluetooth
				writeAccelerate = 2;
				drivingDecisions.setText(drivingDecisions.getText() + "Bakåt\n");
				break;
			case KeyEvent.VK_LEFT:
				// Send left to bluetooth
//			
				writeAngle = 4;
				drivingDecisions.setText(drivingDecisions.getText() + "Vänster\n");
				break;
			case KeyEvent.VK_RIGHT:
				// Send right to bluetooth
				writeAngle = 8;
				drivingDecisions.setText(drivingDecisions.getText() + "Höger\n");
				break;
			case KeyEvent.VK_2:
				writeAngle = 0;
				writeAccelerate = 0;
				breakCar=true;
				System.out.println("S pressed");
				drivingDecisions.setText(drivingDecisions.getText() + "Stopp\n");
				break;
			}
			// Skicka ut nya körkommandon till bilen
			writeCommand = (byte) (writeAccelerate + writeAngle);
			sendCommand();
		}

	}

	@Override
	public void keyReleased(KeyEvent e) {
		// TODO Auto-generated method stub
		if (rdbtnManuell.isSelected()) {
			int keyCode = e.getKeyCode();
			switch (keyCode) {
			case KeyEvent.VK_UP:
				// Send forward to bluetooth
				writeAccelerate = 0;
				drivingDecisions.setText(drivingDecisions.getText() + "Framåt släppt\n");
				break;
			case KeyEvent.VK_DOWN:
				// Send backward to bluetooth
				writeAccelerate = 0;
				drivingDecisions.setText(drivingDecisions.getText() + "Bakåt släppt\n");
				break;
			case KeyEvent.VK_LEFT:
				// Send left to bluetooth
				writeAngle = 0;
				drivingDecisions.setText(drivingDecisions.getText() + "Vänster släppt\n");
				break;
			case KeyEvent.VK_RIGHT:
				// Send right to bluetooth
				writeAngle = 0;
				drivingDecisions.setText(drivingDecisions.getText() + "Höger släppt\n");
				break;
			case KeyEvent.VK_2:
				System.out.println("S released");
				// Send right to bluetooth
				writeAngle = 0;
				writeAccelerate = 0;
				breakCar = false;
				break;
			}
			

			// Skicka ut körkommando till bilen
			
			writeCommand = (byte) (writeAccelerate + writeAngle);
			sendCommand();
	
		}

	}
	public void sendCommand(){
		if(breakCar){
			try{
				sp.chosenPort.writeInt((int) 0x0B);	
			}
			catch(SerialPortException e1){
				e1.printStackTrace();
			}
		}
		else{
			try{
				sp.chosenPort.writeInt((int) writeCommand);	
			}
			catch(SerialPortException e1){
				e1.printStackTrace();
			}
		}

		writeCommand = 0;

	}

}
