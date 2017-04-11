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
bool image_ready = false;
std::mutex mut_image;
std::condition_variable image_cv;

/* Deklaration av funktioner.*/
Socket::Status init_connection(TcpSocket&, TcpSocket&);
int receive_image(TcpSocket&);
void kom_main(TcpSocket&, TcpSocket&);

int main()
{
	int connected = 0;
	string s{};
	TcpSocket socket;
	TcpSocket socket2;
	if (init_connection(socket, socket2) != Socket::Done)
	{
		cerr << "Can't connect to host, please restart.\n";
		return 0;
	}
	else
	{
		std::cout << "Connection established\n";
	}

	RenderWindow window(VideoMode(640, 480), "Kamerabild");
	thread kommunikation(receive_image, ref(socket));
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
		window.clear();
		Texture img;
		Sprite drawable;
		unique_lock<mutex> lk(mut_image);
		while(!image_ready)
		{
			image_cv.wait(lk);
		}
		img.loadFromImage(received_image);
		image_ready = false;
		image_cv.notify_one();
		drawable.setTexture(img);
		window.draw(drawable);
		window.display();
	}
	end_prog = true;
	kommunikation.join();
	return 0;
}

Socket::Status init_connection(TcpSocket& socket, TcpSocket& socket2)
{
	IpAddress ip;
	unsigned short port = 8080;
	unsigned short port2 = 8100;
	Time timeout = seconds(120);
	cout << "Please enter the host-IP (default: 169.254.244.250): ";
	cin >> ip;
	cout << "Connecting to " << ip << " on port " << port << ".\n";
	//socket.connect(ip, port, timeout);
	return socket.connect(ip, port, timeout);
}

void kom_main(TcpSocket& socket, TcpSocket& socket2)
{
	thread worker(receive_image, ref(socket));
	receive_image(ref(socket2));
	worker.join();
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

		/* Känslig kod (nyttjar globala variabler)
		*/
		unique_lock<mutex> lk(mut_image);
		while(image_ready)
		{
			image_cv.wait(lk);
		}
		received_image = received;
		image_ready = true;
		image_cv.notify_one();

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
