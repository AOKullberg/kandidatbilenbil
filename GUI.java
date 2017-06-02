/*
 * GUI.java
 * 
 * Created: 2017-03-15
 * Author: Erik Karvonen
 * Version: 1.0
 */


import java.awt.EventQueue;

import javax.swing.JFrame;
import javax.swing.JTextArea;
import javax.swing.JButton;
import javax.swing.BorderFactory;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;

import javax.swing.JScrollPane;
import javax.swing.ScrollPaneConstants;

import jssc.SerialPort;
import jssc.SerialPortException;

import java.awt.Font;
import javax.swing.JTextField;
import javax.swing.SwingConstants;
import java.awt.Color;
import javax.swing.JRadioButton;

public class GUI {

	private JFrame frame;
	private InitSerialPort serialPort;
	static SerialPort chosenPort; // Firefly_D47F

	
	private JTextField txtAvståndFram;
	private JTextField txtAvståndBak;
	private JTextField txtAvståndHöger;
	private JTextField txtAvståndVänster;
	private JTextField txtHastighet;
	private JTextField txtAvståndCameraLeft;
	private JTextField txtAvståndCameraRight;
	private JLabel lblAvståndFram;
	private JLabel lblStoppFram;
	private JLabel lblAvståndHöger;
	private JLabel lblAvståndVänster;
	private JLabel lblAvståndKameraHöger;
	private JLabel lblAvståndKameraVänster;
	private JLabel lblHastighet;
	private JLabel lblKarta;
	private JPanel mapArea;
	private Map karta;
	protected JLabel lblCamera;
	
	public int newPosition,destination;
	private JButton btnSendMap;
	private JButton btnNewDestination;
	

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

		// the actual window
		frame = new JFrame();
		frame.setBounds(100, 100, 1000, 600);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.getContentPane().setLayout(null);

		JLabel lblSensordata = new JLabel("SensorData");
		lblSensordata.setFont(new Font("YuGothic", Font.BOLD, 16));
		lblSensordata.setBounds(608, 231, 182, 49);
		frame.getContentPane().add(lblSensordata);

		txtHastighet = new JTextField();
		txtHastighet.setEditable(false);
		txtHastighet.setHorizontalAlignment(SwingConstants.RIGHT);
		txtHastighet.setText("Hastighet");
		txtHastighet.setBounds(781, 276, 207, 26);
		frame.getContentPane().add(txtHastighet);
		txtHastighet.setColumns(10);

		txtAvståndFram = new JTextField();
		txtAvståndFram.setHorizontalAlignment(SwingConstants.RIGHT);
		txtAvståndFram.setToolTipText("Avstånd Fram");
		txtAvståndFram.setEditable(false);
		txtAvståndFram.setText("Avstånd fram");
		txtAvståndFram.setBounds(781, 303, 207, 26);
		frame.getContentPane().add(txtAvståndFram);
		txtAvståndFram.setColumns(10);

		txtAvståndBak = new JTextField();
		txtAvståndBak.setEditable(false);
		txtAvståndBak.setHorizontalAlignment(SwingConstants.RIGHT);
		txtAvståndBak.setText("Avstånd bak");
		txtAvståndBak.setBounds(781, 329, 207, 26);
		frame.getContentPane().add(txtAvståndBak);
		txtAvståndBak.setColumns(10);

		txtAvståndHöger = new JTextField();
		txtAvståndHöger.setEditable(false);
		txtAvståndHöger.setHorizontalAlignment(SwingConstants.RIGHT);
		txtAvståndHöger.setText("Styrbeslut");
		txtAvståndHöger.setBounds(781, 355, 207, 26);
		frame.getContentPane().add(txtAvståndHöger);
		txtAvståndHöger.setColumns(10);

		txtAvståndVänster = new JTextField();
		txtAvståndVänster.setHorizontalAlignment(SwingConstants.RIGHT);
		txtAvståndVänster.setText("Position nod");
		txtAvståndVänster.setBounds(781, 382, 207, 26);
		frame.getContentPane().add(txtAvståndVänster);
		txtAvståndVänster.setColumns(10);
		
		txtAvståndCameraRight = new JTextField();
		txtAvståndCameraRight.setHorizontalAlignment(SwingConstants.RIGHT);
		txtAvståndCameraRight.setText("Avstånd kamera höger");
		txtAvståndCameraRight.setBounds(806, 419, 169, 26);
		frame.getContentPane().add(txtAvståndCameraRight);
		txtAvståndCameraRight.setColumns(10);
		
		txtAvståndCameraLeft = new JTextField();
		txtAvståndCameraLeft.setHorizontalAlignment(SwingConstants.RIGHT);
		txtAvståndCameraLeft.setText("Avstånd kamera vänster");
		txtAvståndCameraLeft.setBounds(806, 446, 169, 26);
		frame.getContentPane().add(txtAvståndCameraLeft);
		txtAvståndCameraLeft.setColumns(10);
		
		JScrollPane scrollPane = new JScrollPane();
		scrollPane.setHorizontalScrollBarPolicy(ScrollPaneConstants.HORIZONTAL_SCROLLBAR_NEVER);
		scrollPane.setBounds(608, 71, 262, 153);
		frame.getContentPane().add(scrollPane);
		
		JTextArea drivingDecisions = new JTextArea();
		drivingDecisions.setEditable(false);
		scrollPane.setViewportView(drivingDecisions);
		drivingDecisions.setRows(7);
		drivingDecisions.setColumns(1);

		karta = new Map();
		
		SensorData sensors = new SensorData(txtHastighet, txtAvståndFram, txtAvståndBak, txtAvståndHöger,
				txtAvståndVänster, txtAvståndCameraRight, txtAvståndCameraLeft, drivingDecisions, karta, scrollPane);

	
		
		serialPort = new InitSerialPort(chosenPort, frame, sensors, karta);
		
				
		// Bluetooth connection
		//

		// Titel

		JLabel lblKandidatbilBilen = new JLabel("Kandidat-bil Bilen");
		lblKandidatbilBilen.setFont(new Font("YuGothic", Font.BOLD, 20));
		lblKandidatbilBilen.setBounds(358, 19, 196, 36);
		frame.getContentPane().add(lblKandidatbilBilen);

		// Styrbeslut
		JLabel lblNewLabel = new JLabel("Styrbeslut");
		lblNewLabel.setFont(new Font("YuGothic", Font.BOLD, 15));
		lblNewLabel.setBounds(608, 43, 108, 29);
		frame.getContentPane().add(lblNewLabel);

		

		JRadioButton rdbtnManuell = new JRadioButton("Manuell");
		rdbtnManuell.setBounds(314, 484, 97, 23);
		frame.getContentPane().add(rdbtnManuell);
		
		
		JRadioButton rdbtnAutonom = new JRadioButton("Autonom");
		rdbtnAutonom.setBounds(314, 511, 97, 23);
		frame.getContentPane().add(rdbtnAutonom);
		
		rdbtnManuell.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				rdbtnAutonom.setSelected(false);
			}
		});
		rdbtnAutonom.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				rdbtnManuell.setSelected(false);
			}
		});

		// Manuell körning/autonom med listener på knappen så att man får in
		// kommandon
		JButton tglbtnManuellautonomKrning = new JButton("Skicka vald körning");
		tglbtnManuellautonomKrning.setFont(new Font("Lucida Grande", Font.PLAIN, 10));
		tglbtnManuellautonomKrning.setSelected(true);
		tglbtnManuellautonomKrning.setBounds(25, 484, 262, 29);
		tglbtnManuellautonomKrning.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				// När knappen trycks från manuell körning till autonom skickas
				// noll till bilen.
//				if (!serialPort.chosenPort.isOpened()) {
//					JOptionPane.showMessageDialog(frame, "Bluetooth är inte uppkopplad!");
//				} 
//				else {
					if (rdbtnAutonom.isSelected()) {
						tglbtnManuellautonomKrning.setText("Autonom körning");
						rdbtnAutonom.setSelected(true);
						rdbtnManuell.setSelected(false);
						try {
							//Skicka att vi kör autonom körning till KOM
							serialPort.chosenPort.writeInt((int) 0xFF); // 100000000
							//karta.clearMap();
						System.out.println((int) 0xF0);
						} catch (SerialPortException e1) {
							// TODO Auto-generated catch block
							e1.printStackTrace();
						}
//						newPosition = Integer.parseInt(JOptionPane.showInputDialog("Skriv in nuvarande position i kartan",JOptionPane.INFORMATION_MESSAGE));
//						destination = Integer.parseInt(JOptionPane.showInputDialog("Skriv in slutdestination",JOptionPane.INFORMATION_MESSAGE));
//						
						karta.updateCarPosition(newPosition,destination);
					} else if (rdbtnManuell.isSelected()) {
						tglbtnManuellautonomKrning.setText("Manuell körning");
						rdbtnAutonom.setSelected(false);
						rdbtnManuell.setSelected(true);
						try {
							//Skicka att vi kör Manuell körning till KOM
							serialPort.chosenPort.writeInt((int) 0xFF); // 100000000
							System.out.println((int) 0xFF);
						} catch (SerialPortException e1) {
							// TODO Auto-generated catch block
							e1.printStackTrace();
						}
					}
				}
		//	}
		});
		// Lyssnare på piltangenterna när manuell körning är aktiverad

		
		
		ManualKeyListener drivingListener = new ManualKeyListener(serialPort, drivingDecisions,
				rdbtnManuell, karta);
		rdbtnManuell.addKeyListener(drivingListener);
		frame.getContentPane().add(tglbtnManuellautonomKrning);

		lblHastighet = new JLabel("Hastighet (cm/s)");
		lblHastighet.setBounds(608, 276, 148, 16);
		frame.getContentPane().add(lblHastighet);

		lblAvståndFram = new JLabel("Avstånd fram (cm)");
		lblAvståndFram.setBounds(608, 306, 148, 16);
		frame.getContentPane().add(lblAvståndFram);

		lblStoppFram = new JLabel("Avstånd stopplinje (cm)");
		lblStoppFram.setBounds(608, 329, 161, 16);
		frame.getContentPane().add(lblStoppFram);

		lblAvståndHöger = new JLabel("Styrbeslut (Hex)");
		lblAvståndHöger.setBounds(608, 355, 148, 16);
		frame.getContentPane().add(lblAvståndHöger);

		lblAvståndVänster = new JLabel("Position nod (Nod)");
		lblAvståndVänster.setBounds(608, 382, 148, 16);
		frame.getContentPane().add(lblAvståndVänster);
		
		
		lblAvståndKameraHöger = new JLabel("Avstånd kamera höger (cm)");
		lblAvståndKameraHöger.setBounds(608, 422, 186, 16);
		frame.getContentPane().add(lblAvståndKameraHöger);
		
		lblAvståndKameraVänster = new JLabel("Avstånd kamera vänster (cm)");
		lblAvståndKameraVänster.setBounds(608, 448, 186, 16);
		frame.getContentPane().add(lblAvståndKameraVänster);
		


		lblKarta = new JLabel("Karta");
		lblKarta.setFont(new Font("YuGothic", Font.BOLD, 15));
		lblKarta.setBounds(245, 39, 122, 36);
		frame.getContentPane().add(lblKarta);

//		lblCamera = new JLabel();
//		lblCamera.setIcon(new ImageIcon("/Users/erikkarvonen/Downloads/img.jpg"));
//		lblCamera.setBounds(594, 39, 640, 480);
//		frame.getContentPane().add(lblCamera);
		
		mapArea = new JPanel();
		mapArea.setBounds(15, 71, 563, 401);
		mapArea.setBorder(BorderFactory.createLineBorder(Color.black));
		frame.getContentPane().add(mapArea);
		
		
		JButton btnCreateMap = new JButton("Skapa och ladda in karta");
		btnCreateMap.setBounds(78, 130, 271, 77);
		mapArea.add(btnCreateMap);

		btnCreateMap.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent e) {
				// TODO Auto-generated method stub
//				if(serialPort.chosenPort.isOpened()){
					mapArea.add(karta);
					karta.createMap(serialPort, btnCreateMap, mapArea, sensors);
//				}
//				else{
//					JOptionPane.showMessageDialog(frame, "Koppla upp bluetooth först!");
//				}
				
			}
		});
		
		JButton btnClearMap = new JButton("Rensa kartan");
		btnClearMap.setBounds(15, 41, 118, 29);
		frame.getContentPane().add(btnClearMap);
		
		JButton btnBluetooth = new JButton("Bluetooth");
		btnBluetooth.setBounds(16, 6, 117, 29);
		frame.getContentPane().add(btnBluetooth);
		
		btnSendMap = new JButton("Skicka karta igen");
		btnSendMap.setBounds(444, 483, 132, 29);
		frame.getContentPane().add(btnSendMap);
		
		btnNewDestination = new JButton("Ny destination");
		btnNewDestination.setBounds(444, 512, 122, 29);
		frame.getContentPane().add(btnNewDestination);
		
	
		
		btnNewDestination.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent e) {
				try {
					karta.newDestination(Integer.parseInt(JOptionPane.showInputDialog("Välj ny startnod" ,JOptionPane.INFORMATION_MESSAGE)),
							Integer.parseInt(JOptionPane.showInputDialog("Välj ny sluttnod" ,JOptionPane.INFORMATION_MESSAGE)));
				} catch (NumberFormatException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				} catch (SerialPortException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				} catch (InterruptedException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
			}
		});
		
		btnSendMap.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent e) {
				try {
					karta.sendMap(karta.savedMap,karta.savedTypes);
				} catch (SerialPortException | InterruptedException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
				
			}
		});
		
		btnBluetooth.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				try {
					String portName = JOptionPane.showInputDialog("Skriv in portnamnet, ex på mac '/dev/tty.FireFly_D47F-SPP'",0);
					serialPort.choosePort(portName);
					serialPort.initPort();
				
					btnBluetooth.setText("Uppkopplad");
				} catch (SerialPortException e2) {
					// TODO Auto-generated catch block
					e2.printStackTrace();
				}
			}
		});
		
		btnClearMap.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				// TODO Auto-generated method stub
				karta.clearMap();
			}
		});
	}
}
