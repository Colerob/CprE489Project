#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "header.h"

int Server(char *my_ip_addr, char *other_ip_addr, int my_port);
int Client(int port_no, char *other_ip_addr);
void sendPacket();


//Setup Packet Struct
struct Packet
{
    char syn_syn[2];
    char dle_stx[2];
    char destination;
    char source;
    char text[81];
    char dle_etx[2];
};
    
struct Packet packet;
char buffer[3];
int client, server;

int main(int argc, char **argv)
{
    //Set packet message last bit to null for printing purposes >>>>DO NOT CHANGE<<<<
    packet.text[80] = '\0';

   

    
    char node_num;//This node number
    char clientaddress[20] = "127.0.0.1";
    char serveraddress[20]="127.0.0.1";
    int clientport;
    int serverport;
    char yesno;
    
    //TOKEN is the correct token for the network to check against
    char TOKEN[2] = {'1','q'};
    
    
  //Setup Node Number
    printf("Please enter this machine's node number: ");
    node_num=getchar();
    printf("Node number set as %c\n", node_num);
    

 
    
      //TODO Initialize Client and Server (Check stdin for node numbers)
/*
      
    printf("Please enter the IP address of this machine: ");
    scanf("%s",serveraddress);
    
    printf("Please enter IP address to connect to: ");
    scanf("%s",clientaddress);
    
    printf("Please enter server port number: ");
    scanf("%10d" ,&serverport);
    

    printf("Please enter port number to connect to: ");
    scanf("%10d" ,&clientport); */

    if(node_num == 'A')
    {

   	//Server
        printf("Listening for connection from node C ...\n");
        //TODO Server connection
      	server =  Server(serveraddress, clientaddress, 50001);
        printf("Connection accepted from node C!\n");

	client = Client(50002, clientaddress);
		buffer[0] = 'I';
	buffer[1] = 'P';
		printf("%c%c\n", buffer[0], buffer[1]);
	printf("Connected to B!\n");

    
    }
    
    if(node_num == 'B')
    {
    
	//Client
        printf("Connecting to node C %s ...\n", clientaddress);
        //TODO Client connection
	client = Client(50003, clientaddress); 
        printf("Connected to node C)\n");

	
	//Server
	
        printf("Listening for connection from node A...\n");
        //TODO Server connection
       
	server =  Server(serveraddress, clientaddress, 50002);
	printf("%d\n",server);
	buffer[0] = 'N';
	buffer[1] = 'O';
	printf("%c%c\n", buffer[0], buffer[1]);
	 printf("Connection accepted from node A\n");       

    }
    if(node_num == 'C'){
	//Client
        printf("Connecting to node A ...\n");
        //TODO Client connection
	client = Client(50001, clientaddress); 
        printf("Connected to node A\n");

	//Server
        printf("Listening for connection from node B...\n");
        //TODO Server connection
	server =  Server(serveraddress, clientaddress, 50003);
	printf("Connection accepted from node B\n");
	packet.syn_syn[0] = 'h';
	packet.syn_syn[1] = 'i';
	printf("Trying to send\n");
	buffer[0] = packet.syn_syn[0];
	buffer[1] = packet.syn_syn[1];
	buffer[2] = 'y';
	send(client, buffer, sizeof(buffer), 0);
	buffer[0] = 'L';
	buffer[1] = 'M';
		printf("%c%c\n", buffer[0], buffer[1]);
    //sendPacket();
    }
    
        
        recv(server, &buffer, 3, 0);
        packet.syn_syn[0] = buffer[0];
        packet.syn_syn[1] = buffer[0];
       
            printf("Received %c%c\n", buffer[0], buffer[1]);
            send(client, buffer, sizeof(buffer), 0);
            	buffer[0] = 'n';
	buffer[1] = 'n';
	buffer[2] = 'n';
	printf("%c%c\n", buffer[0], buffer[1]);
        
    
}















int Server(char *my_ip_addr, char *other_ip_addr, int my_port)
{
	

	int s, fd, otherlength = sizeof(otherlength);
	FILE *fdopen(), *fp;
	char ch;
	struct sockaddr_in myaddr;
	struct sockaddr_in otheraddr;
	otheraddr.sin_addr.s_addr=inet_addr(other_ip_addr);
	char buff[1024];
	int n;


	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) ReportError ("socket");
	myaddr.sin_addr.s_addr=inet_addr(my_ip_addr);
	myaddr.sin_family  = AF_INET;
	myaddr.sin_port = htons(my_port);
	
	bind(s,(struct sockaddr *) &myaddr, sizeof(myaddr));

	listen(s, 1);
	
	fd = accept(s, (struct sockaddr *) &otheraddr, &otherlength);

	//n = read(fd, buff, sizeof(buff) );
	//printf("received message:%s\n",buff);

	//fprintf(stdout, "Connected");

	return(fd);
}

int Client(int port_no, char *other_ip_addr)
{
	int s;
	int n;
	int code;
	FILE *fp;
	char ch;
	struct sockaddr_in otheraddr;
	char sMessage[32]="Send me uptime information\n"; 
	
	otheraddr.sin_addr.s_addr=inet_addr(other_ip_addr);
	otheraddr.sin_family = AF_INET;
	otheraddr.sin_port = htons(port_no);
	
	s = socket(AF_INET, SOCK_STREAM, 0);

	for(;;){
		n=connect(s,(struct sockaddr *) &otheraddr, sizeof(otheraddr));
		//printf("%d\n",n);
		if(n!=-1){
			break;	
		}

		
	}

	//write(s,sMessage,sizeof(sMessage));
      // printf("Request to server: %s\n",sMessage);
       //printf("\n");
	

	if ( n < 0)
		return(n);
	else
		return(s);
}


void sendPacket()
{
	int i;
    //Serialize packet for sending
    buffer[0] = packet.syn_syn[0];
    buffer[1] = packet.syn_syn[1];
    buffer[2] = packet.dle_stx[0];
    buffer[3] = packet.dle_stx[1];
    buffer[4] = packet.destination;
    buffer[5] = packet.source;
    for(i = 0; i < 80; i++)
    {
        buffer[i + 6] = packet.text[i];
    }
    buffer[86] = packet.dle_etx[0];
    buffer[87] = packet.dle_etx[1];
    //TODO Send the packet
    send(client, buffer, 88, 0);
}
