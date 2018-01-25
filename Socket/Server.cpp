// Server side C/C++ program to demonstrate Socket programming
#include <sys/socket.h> // Core BSD socket functions and data structures.
#include <netinet/in.h> // AF_INET and AF_INET6 address families and their
// corresponding protocol families PF_INET and PF_INET6.
#include <arpa/inet.h>  // Functions for manipulating numeric IP addresses.
#include <netdb.h> 
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "message.pb.h"
#include <iostream>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <cerrno>

using namespace google::protobuf::io;
using namespace std;

#define PORT 8080

void* SocketHandler(void* lp);
pthread_mutex_t lock;

int main()
{
	int server_fd, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	pthread_t thread_id=0;

	int *new_socket = (int*)malloc(sizeof(int));
	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
				&opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );

	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr *)&address, 
				sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	cout<<"waiting for Client to Connect"<<endl; 
	if (pthread_mutex_init(&lock, NULL) != 0)
	{
		cout<<"mutex init has failed"<<endl;
		return 1;
	}
	while(1){
		if ((*new_socket = accept(server_fd, (struct sockaddr *)&address, 
						(socklen_t*)&addrlen))!= -1)
		{
			cout<<"---------------------Received connection"<<endl;
			pthread_create(&thread_id,0,&SocketHandler,(void*)new_socket);
			pthread_detach(thread_id);

		}
		else
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}
	}
	return 0;
}

google::protobuf::uint32 readHdr(char *buf)
{
	google::protobuf::uint32 size;
	google::protobuf::io::ArrayInputStream ais(buf,4);
	CodedInputStream coded_input(&ais);
	coded_input.ReadVarint32(&size);//Decode the HDR and get the size
	cout<<"size of payload is "<<size<<endl;
	return size;
}

void readBody(int csock,google::protobuf::uint32 siz)
{
	int bytecount;
	tutorial::log_packet payload;
	char buffer [siz+4];//size of the payload and hdr
	//Read the entire buffer including the hdr
	if((bytecount = recv(csock, (void *)buffer, 4+siz, MSG_WAITALL))== -1){
		fprintf(stderr, "Error receiving data %d\n", errno);
	}
	cout<<"Second read byte count is "<<bytecount<<endl;
	//Assign ArrayInputStream with enough memory 
	google::protobuf::io::ArrayInputStream ais(buffer,siz+4);
	CodedInputStream coded_input(&ais);
	//Read an unsigned integer with Varint encoding, truncating to 32 bits.
	coded_input.ReadVarint32(&siz);
	//After the message's length is read, PushLimit() is used to prevent the CodedInputStream 
	//from reading beyond that length.Limits are used when parsing length-delimited 
	//embedded messages
	google::protobuf::io::CodedInputStream::Limit msgLimit = coded_input.PushLimit(siz);
	//De-Serialize
	payload.ParseFromCodedStream(&coded_input);
	//Once the embedded message has been parsed, PopLimit() is called to undo the limit
	coded_input.PopLimit(msgLimit);
	//Print the message
	cout<<"Message is "<<payload.DebugString();

}

void* SocketHandler(void* lp)
{
	pthread_mutex_lock(&lock);    
	int *csock = (int*)lp;

	char buffer[4];
	int bytecount=0;
	string output,pl;
	tutorial::log_packet logp;

	memset(buffer, '\0', 4);

	while(1)
	{
		//Peek into the socket and get the packet size
		if((bytecount = recv(*csock,
						buffer,
						4, MSG_PEEK))== -1){
			fprintf(stderr, "Error receiving data %d\n", errno);
		}else if (bytecount == 0)
			break;
		cout<<"First read byte count is "<<bytecount<<endl;
		readBody(*csock,readHdr(buffer));
	}
	pthread_mutex_unlock(&lock);

}
