#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "kommunikation_klient.hpp"
#include "../Compression/compression.hpp"

using namespace std;
using namespace sf;

extern bool end_program;
extern compression wifi_data;
extern mutex mut_data;
extern bool data_ready;
extern condition_variable data_cv;
TcpSocket socket;
TcpSocket socket2;

Socket::Status init_connection()
{
	unsigned short port = 8080;
	unsigned short port2 = 8100;
	TcpListener listener;
	cout << "Local Address: " << IpAddress::getLocalAddress() << endl;
	cout << "Establishing connection\n";
	if(listener.listen(port) != Socket::Done)
	{
		cerr << "Can't connect to client\n";
	}
	listener.accept(socket);
	if(listener.listen(port2) != Socket::Done)
	{
		cerr << "Can't connect to client\n";
	}
	return listener.accept(socket2);
}

int com_main()
{
	thread worker(send_image,1);
	send_image(0);
	worker.join();
	return 0;
}

int send_image(int id)
{
	while(1)
	{
		/* Synkar med kompressionen.
		 */
		unique_lock<mutex> lk(mut_data);
		while(!data_ready)
		{
			data_cv.wait(lk);
		}
		compression data = wifi_data;
		data_ready = false;
		data_cv.notify_one();
		
		/* Packeterar datan i ett "packet". Ordningen spelar roll
		 * för hur den sedan packas upp. Varje pixel packas
		 * för sig.
		 */
		Packet packet;		
		packet << data.sizeX << data.sizeY;
		packet << (Uint32)data.comp_size;
		for(size_t i = 0; i < data.comp_size; ++i)
		{
			packet << data.data[i];
		}
		
		if(id == 0)
		{
			if(socket.send(packet)!= Socket::Done)
			{
				cerr << "Could not send data\n";
			}
		}
		else
		{
			if(socket2.send(packet)!= Socket::Done)
			{
				cerr << "Could not send data\n";
			}
		}
		/* Frigör minnet som allokerades av kompressionen.
		 */
		packet.clear();
		free(data.data);
		
		if(end_program)
		{
			return 0;
		}
	}
}
