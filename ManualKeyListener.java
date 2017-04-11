import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

import javax.swing.JFrame;
import javax.swing.JTextArea;
import javax.swing.JToggleButton;

import jssc.SerialPort;
import jssc.SerialPortException;

public class ManualKeyListener implements KeyListener {
	
	public initSerialPort sp;
	public JTextArea drivingDecisions;
	private byte writeByte, prevWriteByte = 0;
	private byte[] readByte = {0, 0, 0, 0, 0, 0, 0, 0};
	public JToggleButton tglbtnManuellautonomKrning;
	
	
	
	
	public ManualKeyListener(initSerialPort sp, JTextArea drivingDecisions, JToggleButton tglbtnManuellautonomKrning){
		this.sp = sp;
		this.drivingDecisions=drivingDecisions;
		this.tglbtnManuellautonomKrning = tglbtnManuellautonomKrning;
	}
	
	
	@Override
	public void keyTyped(KeyEvent e) {

	}

	@Override
	public void keyPressed(KeyEvent e) {
		if(tglbtnManuellautonomKrning.isSelected()){
			//setModeBit - nollställ allt! 

			int keyCode = e.getKeyCode();
			switch(keyCode){
			case KeyEvent.VK_UP:
				//Send forward to bluetooth
				//setDirectionBit 00 10 00 00
				try {
					sp.chosenPort.writeByte((byte)('w'));
				} catch (SerialPortException e2) {
					// TODO Auto-generated catch block
					e2.printStackTrace();
				}
				readByte[2]=1;
				drivingDecisions.setText(drivingDecisions.getText() + "Framåt\n" );
				System.out.println("Framåt\n");
				break;
			case KeyEvent.VK_DOWN:
				//Send backward to bluetooth
				//setDirectionBit
				readByte[3]=1;
				drivingDecisions.setText(drivingDecisions.getText() + "Bakåt\n" );
				System.out.println("Bakåt\n");
				break;
			case KeyEvent.VK_LEFT:
				//Send left to bluetooth
				//setDirectionBit
				readByte[4]=1;
				drivingDecisions.setText(drivingDecisions.getText() + "Vänster\n" );
				System.out.println("Vänster\n");
				break;
			case KeyEvent.VK_RIGHT:
				//Send right to bluetooth
				//setDirectionBit
				readByte[5]=1;
				drivingDecisions.setText(drivingDecisions.getText() + "Höger\n" );
				System.out.println("Höger\n");
				break;
			}
			writeByte=0;
			for(int i=0; i<readByte.length; i++){
				if(readByte[i]==1){
					writeByte=(byte) (writeByte+Math.pow(2, 7-i));
				}
			}
			for(int k=0; k<readByte.length; k++){
				System.out.print(readByte[k]);
			}
			System.out.println("");
			System.out.println(writeByte);
			
		
			
			
			//Skicka ut nya körkommandon till bilen
			if(!(prevWriteByte==writeByte)){
				try {
					sp.chosenPort.writeByte(writeByte);
				} catch (SerialPortException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
			}
			prevWriteByte=writeByte;
		}

	}

	@Override
	public void keyReleased(KeyEvent e) {
		// TODO Auto-generated method stub
		if(tglbtnManuellautonomKrning.isSelected()){				
			int keyCode = e.getKeyCode();
			switch(keyCode){
			case KeyEvent.VK_UP:
				//Send forward to bluetooth
				//setDirectionBit & with 11 01 11 11 = 223
				readByte[2]=0;
				
				drivingDecisions.setText(drivingDecisions.getText() + "Framåt släppt\n" );
				break;
			case KeyEvent.VK_DOWN:
				//Send backward to bluetooth
				//setDirectionBit & with 11 10 11 11 = 223
				readByte[3]=0;
				drivingDecisions.setText(drivingDecisions.getText() + "Bakåt släppt\n" );
				break;
			case KeyEvent.VK_LEFT:
				//Send left to bluetooth
				//setDirectionBit
				readByte[4]=0;
				drivingDecisions.setText(drivingDecisions.getText() + "Vänster släppt\n" );
				break;
			case KeyEvent.VK_RIGHT:
				//Send right to bluetooth
				//setDirectionBit
				readByte[5]=0;
				drivingDecisions.setText(drivingDecisions.getText() + "Höger släppt\n" );
				break;
			}
			writeByte=0;
			for(int i=0; i<readByte.length; i++){
				if(readByte[i]==1){
					writeByte=(byte) (writeByte+Math.pow(2, 7-i));
				}
			}
			for(int k=0; k<readByte.length; k++){
				System.out.print(readByte[k]);
			}
			System.out.println("");
			System.out.println(writeByte);
			//Skicka ut körkommando till bilen
			if(!(prevWriteByte==writeByte)){
				try {
					sp.chosenPort.writeByte((byte) writeByte);
				} catch (SerialPortException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
		}
			prevWriteByte=writeByte;
		}
	}

}
