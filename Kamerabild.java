import java.awt.Image;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.zip.ZipInputStream;

import javax.imageio.ImageIO;

public class Kamerabild {

	public Image recievedImage;
	
	
	public Kamerabild(){	
	}
	public boolean initSocket(){
		try {
			Socket raspberry = new Socket("", 0);
		
	        ObjectInputStream ios = new ObjectInputStream(raspberry.getInputStream());
	       
			//recievedImage = (Image) ios.readObject();
			recievedImage = ImageIO.read(ios);
			drawCameraImage(recievedImage);
			
		} catch (UnknownHostException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		
		return true;
	}
	public void drawCameraImage(Image cameraImage){
		
		
	}
	public static void main(String[]args){
		
//	try {
//		    Socket echoSocket = new Socket(hostName, portNumber);
//		    PrintWriter out =
//		        new PrintWriter(echoSocket.getOutputStream(), true);
//		    BufferedReader in =
//		        new BufferedReader(
//		            new InputStreamReader(echoSocket.getInputStream()));
//		    BufferedReader stdIn =
//		        new BufferedReader(
//		            new InputStreamReader(System.in));
//		    ZipInputStream zipImage = new ZipInputStream(echoSocket.getInputStream());
//	}
//	catch(Exception ex){
//		System.out.println(ex);
//	}
	
	
	
	}
}
