#include <iostream>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <exception>
#include <Windows.h>
#include <fstream>

using namespace std;
using namespace sf;
using namespace cv;

/* Globala variabler */
Image received_image;

/* Deklaration av funktioner.*/
Socket::Status init_connection(TcpSocket&);
Socket::Status receive_image(TcpSocket&);

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
			cerr << "Can't connect to client\n" << "Try again? y/n" << endl;
			cin >> s;
			while (s != "y")
			{
				if (s == "n")
				{
					return 0;
				}
				s.clear();
				cerr << "Non interpretable. Try again? y/n" << endl;
				cin >> s;
			}
		}
		else
		{
			connected = 1;
			cout << "Connection established!\n";
		}
	}

	cout << "Attempting to receive data\n";
	if (receive_image(socket) != Socket::Done)
	{
		cerr << "Data could not be received" << endl;
	}
	else
	{
		cout << "Data received!" << endl;
	}


	return 0;
}

Socket::Status init_connection(TcpSocket& socket)
{
	Time timeout = seconds(1);
	unsigned short port;
	cout << "What port? (Integer between 1024 and 65536)" << endl;
	cin >> port;
	TcpListener listener;
	cout << "Establishing connection";
	while (listener.listen(port) != Socket::Done)
	{
		cout << ".";
		sleep(timeout);
	}
	return listener.accept(socket);
}

Socket::Status receive_image(TcpSocket& socket)
{
	Packet packet;
	Image data;
	Socket::Status status = socket.receive(packet);

	size_t sizeX, sizeY;
	packet >> sizeX >> sizeY;
	size_t size = sizeX * sizeY;

	Uint8* tmp;
	for (size_t i = 0; i < size; ++i)
	{
		packet >> tmp[i];
	}
	data.create(sizeX, sizeY, tmp);
	received_image = data;
	return status;
}
