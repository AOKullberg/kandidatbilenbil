#include <iostream>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <exception>
#include <Windows.h>
#include <fstream>
#include <opencv2\core.hpp>
#include <opencv2\imgcodecs.hpp>

using namespace std;
using namespace sf;
using namespace cv;

/* Globala variabler */

/* Deklaration av funktioner.*/
Socket::Status init_connection(TcpSocket&);
Socket::Status send_image(Mat&, TcpSocket&);

class net_except : public exception
{
public:
	net_except(const char* errMessage) :errMessage_(errMessage) {}

	const char* what() const throw() { return errMessage_; }

private:
	const char* errMessage_;
};

int main()
{
	int connected = 0;
	string s{};
	TcpSocket socket;
	
	while (connected = 0)
	{
		if (init_connection(socket) != Socket::Done)
		{
			cerr << "Can't connect to host\n" << "Try again? y/n" << endl;
			cin >> s;
			while (s != "y")
			{
				if (s == "n")
				{
					return 0;
				}
				s.clear();
				cerr << "Non interpretable. Try again? y\n" << endl;
				cin >> s;
			}
		}
		else
		{
			connected = 1;
			cout << "Connection established!\n";
		}
	}

	Mat image;
	image = imread("car.jpg");

	if (send_image(image, socket) != Socket::Done)
	{
		cerr << "Data could not be sent" << endl;
	}


	return 0;
}

Socket::Status init_connection(TcpSocket& socket)
{
	IpAddress ip;
	unsigned short port;
	Time timeout = seconds(3);
	cout << "Please enter the host-IP (xxx.xxx.x.x) \n";
	cin >> ip;
	cout << "Please enter the remote host port (Integer between 1024 and 65536) \n";
	cin >> port;
	cout << "Connecting to " << ip << " on port " << port << ".\n";
	Socket::Status status = socket.connect(ip, port, timeout);
	return status;
}

Socket::Status send_image(Mat& img, TcpSocket& socket)
{
	Packet packet;
	Image data;
	data.create(img.cols, img.rows, img.ptr());

	size_t sizeX = data.getSize().x;
	size_t sizeY = data.getSize().y;
	size_t size = sizeX * sizeY;
	packet << sizeX << sizeY;


	const Uint8* tmp = data.getPixelsPtr();
	for (size_t i = 0; i < size; ++i)
	{
		packet << tmp[i];
	}
	return socket.send(packet);
}
