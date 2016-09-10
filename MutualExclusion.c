/* CS 370
   Project #5
   Gerardo Gomez-Martinez
   December 2, 2014

   Program implements Dijkstra's K-State mututal exclusion algorithm, using sockets. The 
   program implements a mutual exclusion, self-stabilizing algorith. Program accepts input via
   redirection containing the index of the current node in the network  and the total number 
   of nodes in the network.
*/

#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int createClientConnection(char* socketName) 
{
	int descriptor;
	//The call to the function ‘socket()’ creates an UN-named socket inside the kernel and returns an integer known as socket descriptor.
  //This function takes domain/family as its first argument. For Internet family of IPv4 addresses we use AF_INET.
  //The second argument ‘SOCK_STREAM’ specifies that the transport layer protocol that we want should be reliable ie it should have acknowledgement techniques. For example : TCP
  //The third argument is generally left zero to let the kernel decide the default protocol to use for this connection. For connection oriented reliable connections, the default protocol used is TCP.
	
	if ((descriptor = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) 
	  {
		  printf("Error creating socket [%s]\n", socketName);
		  return -1;
	  }

	struct sockaddr_un address;
	address.sun_family = AF_UNIX;
	strcpy(address.sun_path, socketName);
	unsigned int addressLength = sizeof(address);
	
//Information like IP address of the remote host and its port is bundled up in a structure and a call to function connect() is made which tries to connect this socket with the socket (IP address and port) of the remote host.
	if (connect(descriptor, (struct sockaddr*)&address, addressLength) < 0) 
	  {
		  printf("Error connecting socket [%s]\n", socketName);
		  return -1;
	  }

	return descriptor;
}

int createServerConnection(char* socketName) {
	int descriptor;
	
	//To create endpoint for communication
	if ((descriptor = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) 
	  {
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
	if (bind(descriptor, (struct sockaddr*)&address, addressLength) < 0) 
	  {
		  printf("Error binding socket [%s]\n", socketName);
		  return -1;
	  }
	
	//To instruct a socket to listen for incoming
  //connections. It specifies the number of pending connections
  //that can be queued for a server socket.

  //After the call to listen(), this socket becomes a fully functional listening socket.
	if (listen(descriptor, 1) < 0) 
	  {
		  printf("Error listening to socket [%s]\n", socketName);
		  return -1;
	  }
	  
	int connectedDescriptor;
	//To accept the connection request from a client
	if ((connectedDescriptor = accept(descriptor, (struct sockaddr*) &address, &addressLength)) < 0) 
	  {
		  printf("Error accepting socket connection [%s]\n", socketName);
		  return -1;
	  }
	  
	return connectedDescriptor;
}

char *getSocketName(int index)
{
  char *socketName = (char *)calloc(256, sizeof(char));
  
  // Creates a unique name for a socket based on the index of the calling node
  sprintf(socketName, "%s%d", "Socket", index);
	
	return socketName;
}

int main()
{
  char buffer[50]; /* buffer for input */
  int index; // index of the current node
  int size; // total number of nodes
  int s; // initial state of the node
  int l; // left neightbor's state
  char *socketName; // socket name
  int leftDescriptor, rightDescriptor; // right and left sockets' descriptors
  
  // Reads the node's ndex and total number of nodes
  fgets(buffer, sizeof(buffer), stdin);
  sscanf(buffer, "%d %d", &index, &size);
  
  // Initializes the node's state to a random value
  srand (time(NULL));
  s = rand() % size;

  if (index == 0)
    {
      // Create a server connection to connect to its right neighbor
      socketName = getSocketName(index);
      rightDescriptor = createServerConnection(socketName);
      
      // Create a server connection to connect to its left neighbor
      socketName = getSocketName(size-1);
      leftDescriptor = createServerConnection(socketName);
      
      while(1)
        {
          send(rightDescriptor, &s, sizeof(int), 0);
          recv(leftDescriptor, &l, sizeof(int), 0);
          
          printf("Received: %d\n", l);
          
          if (s == l)
            {
              s = (s+1) % size;
              printf("#########################################\n");
              printf("   In Critical Section\n");
              printf("#########################################\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
              usleep( 900000 );
            }
          else
            {
              printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
              usleep( 900000 );
            }
        }
    }
  else if (index == size-1)
    {
      // Client connection to its left neighbor
      socketName = getSocketName(index-1);
      leftDescriptor = createClientConnection(socketName);

      // Client connection to its right neighbor
      socketName = getSocketName(index);
      rightDescriptor = createClientConnection(socketName);

      while(1)
        {
          send(rightDescriptor, &s, sizeof(int), 0);
          recv(leftDescriptor, &l, sizeof(int), 0);
          
          if (s != l)
            {
              s = l;
              printf("#########################################\n");
              printf("   In Critical Section\n");
              printf("#########################################\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
              usleep( 900000 );
            }
          else
            {
              printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
              usleep( 900000 );
            }
        }
    }
  else
    {
      // Client connection to its left neighbor
      socketName = getSocketName(index-1);
      leftDescriptor = createClientConnection(socketName);
      
      // Create a server connection to connect to its right neighbor
      socketName = getSocketName(index);
      rightDescriptor = createServerConnection(socketName);

      while(1)
        {
          send(rightDescriptor, &s, sizeof(int), 0);
          recv(leftDescriptor, &l, sizeof(int), 0);
          
          if (s != l)
            {
              s = l;
              printf("#########################################\n");
              printf("   In Critical Section\n");
              printf("#########################################\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
              usleep( 900000 );
            }
          else
            {
              printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
              usleep( 900000 );
            }
        }
    }
  
  return(0);
}
