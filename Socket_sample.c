/*
Follow below steps:

1.Compile socket_Sample.c as follow:
gcc -o socket_sample socket_Sample.c

2. Open first terminal (assume Server terminal)

3. Run server terminal as below
./socket_sample
0

(Note:0 is for server connection)

4. Open second terminal (assume Client terminal)

5. Run client terminal as below

./socket_sample
1

(Note:1 is for client connection)

6. Observer outputs in both terminal. One sends(client) the value and other recieves it(server).

*/

#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>

#define  SERVER 0
#define CLIENT 1

void communicate(int serverDescriptor, int clientDescriptor);
int createServerConnection(char *socketName);
int createClientConnection(char *socketName);
char *getSocketName(int index1, int index2);
int id;

int main () {
	scanf("%d", &id);
	int clientDescriptor,serverDescriptor;
	char *socketName;
	socketName = getSocketName(SERVER,CLIENT);
	if (id == 0)
	{
		clientDescriptor = createServerConnection(socketName);
		//printf("server connection created %d \n",clientDescriptor);
		communicate(serverDescriptor, clientDescriptor);
		}
	else 
	{
		serverDescriptor = createClientConnection(socketName);
		//printf("client connection created %d \n",serverDescriptor);
		communicate(serverDescriptor, clientDescriptor);
	}
	if ((clientDescriptor < 0 )||(serverDescriptor < 0 )) 
		return 0;
	
	
	return 0;
}

void communicate(int serverDescriptor, int clientDescriptor) {
	
	if (id ==0){
	int neighborValue;
	    //To receive message via socket
		recv(clientDescriptor, &neighborValue, sizeof(int), 0);
		printf("received %d\n",neighborValue);
	}
	else {
		int value = rand() % 100;
		//To send a message via socket
		send(serverDescriptor, &value, sizeof(int), 0);
		printf("sent %d\n",value);
	}
}

int createClientConnection(char* socketName) {
	int descriptor;
	//The call to the function ‘socket()’ creates an UN-named socket inside the kernel and returns an integer known as socket descriptor.
    //This function takes domain/family as its first argument. For Internet family of IPv4 addresses we use AF_INET.
    //The second argument ‘SOCK_STREAM’ specifies that the transport layer protocol that we want should be reliable ie it should have acknowledgement techniques. For example : TCP
    //The third argument is generally left zero to let the kernel decide the default protocol to use for this connection. For connection oriented reliable connections, the default protocol used is TCP.
	
	if ((descriptor = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		printf("Error creating socket [%s]\n", socketName);
		return -1;
	}

	struct sockaddr_un address;
	address.sun_family = AF_UNIX;
	strcpy(address.sun_path, socketName);
	unsigned int addressLength = sizeof(address);
//Information like IP address of the remote host and its port is bundled up in a structure and a call to function connect() is made which tries to connect this socket with the socket (IP address and port) of the remote host.
	if (connect(descriptor, (struct sockaddr*)&address, addressLength) < 0) {
		printf("Error connecting socket [%s]\n", socketName);
		return -1;
	}

	return descriptor;
}

int createServerConnection(char* socketName) {
	int descriptor;
	//To create endpoint for communication
	if ((descriptor = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		printf("Error creating socket [%s]\n", socketName);
		return -1;
	}
	struct sockaddr_un address;
	address.sun_family = AF_UNIX;
	strcpy(address.sun_path, socketName);
	unsigned int addressLength = sizeof(address);
  //To remove a socket if it exists. Its called before bind()

	unlink(socketName);
	
   //To assign a local socket address.
	if (bind(descriptor, (struct sockaddr*)&address, addressLength) < 0) {
		printf("Error binding socket [%s]\n", socketName);
		return -1;
	}
	
	//To instruct a socket to listen for incoming
    //connections. It specifies the number of pending connections
    //that can be queued for a server socket.

   //After the call to listen(), this socket becomes a fully functional listening socket.
	if (listen(descriptor, 1) < 0) {
		printf("Error listening to socket [%s]\n", socketName);
		return -1;
	}
	int connectedDescriptor;
	//To accept the connection request from a client
	if ((connectedDescriptor = accept(descriptor, (struct sockaddr*) &address, &addressLength)) < 0) {
		printf("Error accepting socket connection [%s]\n", socketName);
		return -1;
	}
	return connectedDescriptor;
}

char *getSocketName(int index1, int index2) {
	char *socketName = (char *)calloc(256, sizeof(char));
	socketName ="mySocket";
	return socketName;
}
