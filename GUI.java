import java.awt.EventQueue;

import javax.swing.JFrame;
import javax.swing.JTextArea;
import java.awt.BorderLayout;
import javax.swing.JButton;
import javax.swing.BoxLayout;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import javax.swing.JLabel;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import javax.swing.JScrollBar;
import javax.swing.JScrollPane;
import javax.swing.ScrollPaneConstants;

import jssc.SerialPort;
import jssc.SerialPortException;


import javax.swing.JToggleButton;
import java.awt.Font;
import javax.swing.JComboBox;
import javax.swing.JTextField;
import javax.swing.SwingConstants;

public class GUI {

	private JFrame frame;
	private initSerialPort serialPort;
	static SerialPort chosenPort; //Firefly_D47F
	
	
	private byte[] writeByte = {0, 0, 0, 0, 0, 0, 0, 0};
	private byte[] readByte = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77};
	private byte[] startByte = {0};
	private JTextField txtAvstndFram;
	private JTextField txtAvstndBak;
	private JTextField txtAvstndHger;
	private JTextField txtAvstndVnster;
	private JTextField txtHastighet;
	private JTextField txtGyro;
	private JLabel lblAvstndFram;
	private JLabel lblNewLabel_1;
	private JLabel lblAvstndHger;
	private JLabel lblAvstndVnster;
	private JLabel lblHastighet;
	private JLabel lblKamerabild;
	private JLabel lblKarta;
	private Map karta;
	private Kamerabild kamera;

	public static void main(String[] args) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					GUI window = new GUI();
					window.frame.setVisible(true);
					
					
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}

	public GUI() {
		initialize();
	}

	private void initialize() {
		
		
		//the actual window
		frame = new JFrame();
		frame.setBounds(100, 100, 900, 600);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.getContentPane().setLayout(null);
	

		JLabel lblSensordata = new JLabel("SensorData");
		lblSensordata.setFont(new Font("YuGothic", Font.BOLD, 16));
		lblSensordata.setBounds(596, 355, 182, 49);
		frame.getContentPane().add(lblSensordata);
		
		txtHastighet = new JTextField();
		txtHastighet.setEditable(false);
		txtHastighet.setHorizontalAlignment(SwingConstants.RIGHT);
		txtHastighet.setText("Hastighet");
		txtHastighet.setBounds(661, 389, 130, 26);
		frame.getContentPane().add(txtHastighet);
		txtHastighet.setColumns(10);
		
		txtAvstndFram = new JTextField();
		txtAvstndFram.setHorizontalAlignment(SwingConstants.RIGHT);
		txtAvstndFram.setToolTipText("Avstånd Fram");
		txtAvstndFram.setEditable(false);
		txtAvstndFram.setText("Avstånd fram");
		txtAvstndFram.setBounds(661, 416, 130, 26);
		frame.getContentPane().add(txtAvstndFram);
		txtAvstndFram.setColumns(10);
		
		txtAvstndBak = new JTextField();
		txtAvstndBak.setEditable(false);
		txtAvstndBak.setHorizontalAlignment(SwingConstants.RIGHT);
		txtAvstndBak.setText("Avstånd bak");
		txtAvstndBak.setBounds(661, 442, 130, 26);
		frame.getContentPane().add(txtAvstndBak);
		txtAvstndBak.setColumns(10);
		
		txtAvstndHger = new JTextField();
		txtAvstndHger.setEditable(false);
		txtAvstndHger.setHorizontalAlignment(SwingConstants.RIGHT);
		txtAvstndHger.setText("Avstånd höger");
		txtAvstndHger.setBounds(661, 468, 130, 26);
		frame.getContentPane().add(txtAvstndHger);
		txtAvstndHger.setColumns(10);
		
		
		txtAvstndVnster = new JTextField();
		txtAvstndVnster.setHorizontalAlignment(SwingConstants.RIGHT);
		txtAvstndVnster.setText("Avstånd vänster");
		txtAvstndVnster.setBounds(661, 495, 130, 26);
		frame.getContentPane().add(txtAvstndVnster);
		txtAvstndVnster.setColumns(10);
		
		
			
		SensorData sensors = new SensorData(txtHastighet, txtAvstndFram, txtAvstndBak, txtAvstndHger, txtAvstndVnster);
		
		//Bluetooth connection 
		//
		serialPort = new initSerialPort(chosenPort, frame, sensors);
		try {
			serialPort.initPort();
		} catch (SerialPortException e2) {
			// TODO Auto-generated catch block
			e2.printStackTrace();
		}
		
		
		
		//Titel	
		
		JLabel lblKandidatbilBilen = new JLabel("Kandidat-bil Bilen");
		lblKandidatbilBilen.setFont(new Font("YuGothic", Font.BOLD, 20));
		lblKandidatbilBilen.setBounds(358, 19, 196, 36);
		frame.getContentPane().add(lblKandidatbilBilen);
		
		
		//Styrbeslut
		JLabel lblNewLabel = new JLabel("Styrbeslut");
		lblNewLabel.setFont(new Font("YuGothic", Font.BOLD, 15));
		lblNewLabel.setBounds(23, 367, 108, 29);
		frame.getContentPane().add(lblNewLabel);
		
		JScrollPane scrollPane = new JScrollPane();	
		scrollPane.setHorizontalScrollBarPolicy(ScrollPaneConstants.HORIZONTAL_SCROLLBAR_NEVER);
		scrollPane.setBounds(23, 408, 262, 153);
		frame.getContentPane().add(scrollPane);
		
		
		
		JTextArea drivingDecisions = new JTextArea();
		drivingDecisions.setEditable(false);
		scrollPane.setViewportView(drivingDecisions);
		drivingDecisions.setRows(7);
		drivingDecisions.setColumns(1);
		
		
		
		//Manuell körning/autonom med listener på knappen så att man får in kommandon
		JToggleButton tglbtnManuellautonomKrning = new JToggleButton("Manuell/Autonom körning");
		tglbtnManuellautonomKrning.setToolTipText("Blå - Manuell\nVit - Autonom");
		tglbtnManuellautonomKrning.setFont(new Font("Lucida Grande", Font.PLAIN, 10));
		tglbtnManuellautonomKrning.setSelected(false);
		tglbtnManuellautonomKrning.setBounds(23, 326, 262, 29);
		tglbtnManuellautonomKrning.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent e) {
				
				//När knappen trycks från manuell körning till autonom skickas noll till bilen. 
				if(!tglbtnManuellautonomKrning.isSelected()){
					try {
						serialPort.chosenPort.writeByte((byte) 0x80); //100000000
					} catch (SerialPortException e1) {
						// TODO Auto-generated catch block
						e1.printStackTrace();
					}
				}
				
			}
		});
		//Lyssnare på piltangenterna när manuell körning är aktiverad
		ManualKeyListener drivingListener = new ManualKeyListener(serialPort, drivingDecisions, tglbtnManuellautonomKrning);
		tglbtnManuellautonomKrning.addKeyListener(drivingListener);
		frame.getContentPane().add(tglbtnManuellautonomKrning);
		
		lblHastighet = new JLabel("Hastighet (cm/s)");
		lblHastighet.setBounds(521, 394, 126, 16);
		frame.getContentPane().add(lblHastighet);
		
		lblAvstndFram = new JLabel("Avstånd fram (cm)");
		lblAvstndFram.setBounds(521, 421, 128, 16);
		frame.getContentPane().add(lblAvstndFram);
		
		lblNewLabel_1 = new JLabel("Avstånd bak (cm)");
		lblNewLabel_1.setBounds(521, 447, 126, 16);
		frame.getContentPane().add(lblNewLabel_1);
		
		lblAvstndHger = new JLabel("Avstånd höger (cm)");
		lblAvstndHger.setBounds(521, 473, 140, 16);
		frame.getContentPane().add(lblAvstndHger);
		
		lblAvstndVnster = new JLabel("Avstånd vänster (cm)");
		lblAvstndVnster.setBounds(521, 500, 128, 16);
		frame.getContentPane().add(lblAvstndVnster);
		
		
		lblKamerabild = new JLabel("Kamerabild");
		lblKamerabild.setFont(new Font("YuGothic", Font.BOLD, 15));
		lblKamerabild.setBounds(717, 39, 140, 36);
		frame.getContentPane().add(lblKamerabild);
		
		lblKarta = new JLabel("Karta");
		lblKarta.setFont(new Font("YuGothic", Font.BOLD, 15));
		lblKarta.setBounds(142, 39, 122, 36);
		frame.getContentPane().add(lblKarta);
		
//		karta = new Map();
//		karta.setBounds(142, 50, 122, 36);
//		frame.getContentPane().add(karta);
		
		
//		kamera = new Kamerabild();
//		if(kamera.initSocket()){
//			System.out.println("Connected");
//		}
//		
	}
}
