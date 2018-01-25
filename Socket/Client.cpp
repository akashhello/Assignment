// Client side C/C++ program to demonstrate Socket programming
#include <sys/socket.h> // Core BSD socket functions and data structures.
#include <netinet/in.h> // AF_INET and AF_INET6 address families and their
#include <arpa/inet.h>  // Functions for manipulating numeric IP addresses.
#include <netdb.h> 
#include <iostream>
#include <string>
#include <string.h>
#define PORT 8080

#include "message.pb.h"
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>


using namespace google::protobuf::io;

using namespace std;  

int main()
{
    tutorial::log_packet payload ;
    payload.set_log_msg("What shall we say then");
    cout<<"size after serilizing is "<<payload.ByteSize()<<endl;
    int siz = payload.ByteSize()+4;
    char *pkt = new char [siz];
    google::protobuf::io::ArrayOutputStream aos(pkt,siz);
    CodedOutputStream *coded_output = new CodedOutputStream(&aos);
    coded_output->WriteVarint32(payload.ByteSize());
    payload.SerializeToCodedStream(coded_output);

    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cout<<"\n Socket creation error \n";
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
    {
        cout<<"Invalid address/ Address not supported"<<endl;
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        cout<<"Connection Failed"<<endl;
        return -1;
    }
    for(int i=0;i<1;i++)
    {
    	send(sock , (void*)pkt , siz , 0 );
	usleep(1);
    }	
    return 0;
}
