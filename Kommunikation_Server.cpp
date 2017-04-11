#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <Windows.h>
#include "zlib.h"

using namespace std;
using namespace sf;

typedef uint8_t byte;

/* Globala variabler */
Image received_image;
bool end_prog = false;


/* Deklaration av funktioner.*/
Socket::Status init_connection(TcpSocket&);
int receive_image(TcpSocket&);

int main()
{
	int connected = 0;
	string s{};
	TcpSocket socket;
	if (init_connection(socket) != Socket::Done)
	{
		cerr << "Can't connect to host, please restart.\n";
		return 0;
	}
	else
	{
		std::cout << "Connection established\n";
	}

	RenderWindow window(VideoMode(640, 480), "Kamerabild");
	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				window.close();
			}
		}
		receive_image(ref(socket));
		window.clear();
		Texture img;
		Sprite drawable;
		img.loadFromImage(received_image);
		drawable.setTexture(img);
		window.draw(drawable);
		window.display();
	}
	end_prog = true;
	return 0;
}

Socket::Status init_connection(TcpSocket& socket, TcpSocket& socket2)
{
	IpAddress ip;
	unsigned short port = 8080;
	Time timeout = seconds(120);
	cout << "Please enter the host-IP (default: 169.254.244.250): ";
	cin >> ip;
	cout << "Connecting to " << ip << " on port " << port << ".\n";
	return socket.connect(ip, port, timeout);
}

int receive_image(TcpSocket& socket)
{
	while (1)
	{
		Packet packet{};
		Image data;
		Socket::Status status = socket.receive(packet);

		unsigned int sizeX, sizeY;
		Uint32 comp_size{};

		packet >> sizeX >> sizeY;
		packet >> comp_size;
		byte* received_data = (byte*)malloc(comp_size);
		for (size_t i = 0; i < comp_size; ++i)
		{
			packet >> received_data[i];
		}
		uLong size = sizeX * sizeY * 4;

		byte* UncompressedData = (byte*)malloc(size);

		int z_result = uncompress(UncompressedData, &size, (received_data), comp_size);

		switch (z_result)
		{
		case Z_OK:
			cout << "Successfully decompressed\n";
			break;
		case Z_MEM_ERROR:
			cerr << "Mem_error\n";
			break;
		case Z_BUF_ERROR:
			cerr << "Buffer error\n";
			break;
		}

		Image received;
		received.create(sizeX, sizeY, UncompressedData);

		received_image = received;

		/* Deallokerar minne.
		*/
		packet.clear();
		free(received_data);
		free(UncompressedData);
		if (end_prog)
		{
			return 0;
		}
	}
}
