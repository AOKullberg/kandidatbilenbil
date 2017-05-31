#include <iostream>
#include <mutex>
#include <condition_variable>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "kommunikation_klient.hpp"
#include "../Compression/compression.hpp"

using namespace std;
using namespace sf;

extern compression wifi_data;
extern mutex mut_data;
extern bool data_ready;
extern condition_variable data_cv;
TcpSocket socket;

Socket::Status init_connection()
{
	unsigned short port = 8000;
	TcpListener listener;
	cout << "Local Address: " << IpAddress::getLocalAddress() << endl;
	cout << "Establishing connection\n";
	if(listener.listen(port) != Socket::Done)
	{
		cerr << "Can't connect to client\n";
	}
	return listener.accept(socket);
}

void send_image()
{
	while(1)
	{
		unique_lock<mutex> lk(mut_data);
		while(!data_ready)
		{
			data_cv.wait(lk);
		}
		compression data = wifi_data;
		data_ready = false;
		data_cv.notify_one();
		
		//byte* ack = (byte*)malloc(1);
		uLong* comp_size = &data.comp_size;
		byte* data_to_send = data.data;
		//size_t received_bytes;
		/*
		if(socket.receive(ack,1, received_bytes) != Socket::Done)
		{
			cerr << "Can't receive ack\n";
		}*/
		if(socket.send(comp_size, 8)!= Socket::Done)
		{
			cerr << "Socket error, can't send image-size\n";
		}
		
		if(socket.send(data_to_send, *comp_size)!=Socket::Done)
		{
			cerr << "Socket error, can't send image\n";
		}
		
		/* De-allokerar minne
		 */
		free(data_to_send);
	}
}
