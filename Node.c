#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "header.h"

int Server(char *my_ip_addr, char *other_ip_addr, int my_port);
int Client(int port_no, char *other_ip_addr);
void sendPacket();
void emptyPacket();

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
char buffer[88];
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

    }
    
   
    //Select.c

    
    int n, i;
    fd_set rset;
    int rec;		/* declare an fd_set for read descriptors */
    int sd;		/* declare an fd_set for read descriptors */
    
    for (;;) {	/* endless loop, if you want continuous operation */
        FD_ZERO(&rset);		/* clear all bits in rset */
		FD_SET(STDIN_FILENO, &rset);	/* set the standard input bit */
		FD_SET(sd, &rset);	/* set the socket descriptor bit */
 		n = select((sd>STDIN_FILENO? sd:STDIN_FILENO)+1, &rset, NULL, NULL, NULL);
		/* select blocks, and n is the number of ready descriptors */
		if ( (n == -1 ) && (errno == EINTR) ) /* interruption */
		    continue;
	  	if (n== -1) {	/* error: you may handle it, if you want */
		 	/* code to handle errors */
			printf("Error");
	  	}
        //TODO Generate token if CNTRL-P (Hex 10) is typed on machine
        
        /* after this point, handle the ready descriptor(s) */
        
        
        //TODO If server is ready, read packets
	  	if ((n > 0 ) && (FD_ISSET(sd, &rset)) ) {
	  	    /* socket is ready for reading */
	  	    
			/* read data from socket */
			if(rec = recv(sd, buffer, 88, 0) == -1) { 
                printf("ERROR on recv\n");
            }
            
            //Store received message into packet struct
            //Unserialize packet
            packet.syn_syn[0] = buffer[0];
            packet.syn_syn[1] = buffer[1];
            packet.dle_stx[0] = buffer[2];
            packet.dle_stx[1] = buffer[3];
            packet.destination = buffer[4];
            packet.source = buffer[5];
            for(i = 0; i < 80; i++)
            {
                packet.text[0] = buffer[i + 6];
            }
            packet.dle_etx[0] = buffer[86];
            packet.dle_etx[1] = buffer[87];
            
            //TODO If packet is just token (ie, all fields except DEL-ETX are ' ') and there is input from keyboard
            /* check for ready data from the keyboard */

	  	    if (FD_ISSET(STDIN_FILENO, &rset)) {
	  	    /* read data from the standard input*/

                    //Read stdin, designate destination machine and add to packet
                    printf("Please enter destination: ");
                    packet.destination = fgetc(stdin);
                    
                    //Add source address to packet
                    packet.source = node_num;
                    //Read stdin, add input text (up to 80 chars) and add to packet
                    i = 0;
                    //TODO if getchar() doesn't work, it might be reading '\n' from the previous fgets, add in a getchar before loop if it doesnt work
                    while(i < 80)
                    {
                        packet.text[i] = getchar();
                        //If character pressed was return, stop taking in characters
                        if(packet.text[i] = '\n')
                        {
                            packet.text[i] = ' ';
                            break;
                        }
                        i++;
                    }
                    //After reading is complete, replace all remaining empty text bits as spaces
                    for(; i < 80; i++)
                    {
                        packet.text[i] = ' ';
                    }
                
 
                    //Send packet, reset packet in storage
                    sendPacket();
                    emptyPacket();
                    packet.dle_etx[0] = ' ';
                    packet.dle_etx[1] = ' ';
			    n--;
	  	    }
             
			//else, If packet is for this machine, display it
			else if(packet.destination == node_num)
			{
                //Diplay message to terminal, preceded by sender's node address
                printf("Sender: %c\n", packet.source);
                printf("Message: %s\n", packet.text);
                //Remove message up to, but not including, DLE-ETX
                emptyPacket();
                //TODO Change packet's DLE-ETX (maybe)?
            }    
            //If packet is from this machine, meaning it has made a full loop around token and not been received,
            // delete the message to prevent it from infinitely looping around the network
            else if(packet.source == node_num)
            {
                emptyPacket();
            }
            
            //If machine has token and doesn't want to send a message, pass the token along
            //OR if packet is not for this machine, send it to next machine, and completely empty packet from storage
            else
            {
                sendPacket();
                emptyPacket();
                packet.dle_etx[0] = ' ';
                packet.dle_etx[1] = ' ';
            }
	    }

} 

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
       //printf("Request to server: %s\n",sMessage);
       //printf("\n");
	

	if ( n < 0)
		return(n);
	else
		return(s);
}

void emptyPacket()
{
    int i;
    packet.syn_syn[0] = buffer[0];
    packet.syn_syn[1] = buffer[1];
    packet.dle_stx[0] = buffer[2];
    packet.dle_stx[1] = buffer[3];
    packet.destination = buffer[4];
    packet.source = buffer[5];
    for(i = 0; i < 80; i++)
    {
        packet.text[0] = buffer[i + 6];
    }
    packet.dle_etx[0] = buffer[86];
    packet.dle_etx[1] = buffer[87];
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


