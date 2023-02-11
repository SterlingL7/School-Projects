/* Lab6_cdev_user.c
 * ECE4220/7220
 * Author: Luis Alberto Rivera
 
 This program allows you to enter a message on the terminal, and then it writes the
 message to a Character Device. The Device should be created beforehand, as described
 in the Lab6_cdev_kmod.c file.
 Try this example together with the Lab6_cdev_kmod module. Once you enter a message, run
	dmesg | tail     ( | tail  is not needed. With that, you'll only see the last few entries).
 on a different terminal. You should see the message in the system log (printk'd in the
 module). That would mean that the message is getting to kernel space.
 Use elements from this example and the Lab6_cdev_user module in your Lab 6 programs. You may
 need to modify a bit the callback functions in the module, according to your needs.
 
 For the extra credit part of lab 6, you'll need to think about how to read messages coming
 from kernel space, and how to create those messages in the module...
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include "MQTTClient.h"

#define MY_ADDRESS     "128.206.19.17"
#define O_ADDRESS      "128.206.19.11"

#define CLIENTID    "Sterling@128.206.19.17"

#define TOPIC_1     "Election"
#define TOPIC_2     "EC"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L
#define CHAR_DEV "/dev/Lab6" // "/dev/YourDevName"
#define MSG_SIZE 50


char buffer[MSG_SIZE];
int master;


///functions from paho example:
volatile MQTTClient_deliveryToken deliveredtoken;
MQTTClient client;
char* rcv_IP;
int rcv_vote;
int my_vote;
int myIP = 17;





void delivered(void *context, MQTTClient_deliveryToken dt)
{
	printf("Message with token value %d delivery confirmed\n", dt);
	deliveredtoken = dt;
}
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{	
	bzero(buffer, sizeof(buffer));
	int cdev_id, dummy;
	MQTTClient_message pubmsg = MQTTClient_message_initializer;
	MQTTClient_deliveryToken token;

		// Open the Character Device for writing
		//opening in msgarrvd to use MQTT msg as msg for
		//char device
	if((cdev_id = open(CHAR_DEV, O_WRONLY)) == -1) {
		printf("Cannot open device %s\n", CHAR_DEV);
		exit(1);
	}
	
	int i;
	char* payloadptr;
	printf("Message arrived\n");
	printf("     topic: %s\n", topicName);
	printf("   message: ");
	payloadptr = message->payload;


	for(i=0; i<message->payloadlen; i++)
	{
		buffer[i] = *payloadptr;
		putchar(*payloadptr++);
	}
	putchar('\n');
	
	//string compare topic then message
	if(strncmp(topicName, "EC", 2) == 0){
		if(master = -1){
			if(strncmp(buffer, "@", 1) == 0){

				int dummy = write(cdev_id, buffer, sizeof(buffer));
				if(dummy != sizeof(buffer)) {
					printf("Write failed, leaving...\n");
					return 1;
				}
			}
		}
	}else if(strncmp(topicName, "Election", 8) ==0){
		char msg[40];
		if(strncmp(buffer, "WHOIS", 5) == 0){//message asking for master ID
			//bzero(buffer, sizeof(buffer));
			printf("WHOIS received\n");
			sprintf(msg, "%s is master", CLIENTID);
			if(master){
				pubmsg.payload = msg;
				pubmsg.payloadlen = sizeof(msg);
				pubmsg.qos = QOS;
				pubmsg.retained = 0;
				deliveredtoken = 0;
				MQTTClient_publishMessage(client, TOPIC_1, &pubmsg, &token);
			}
			
		}
		else if(strncmp(buffer, "VOTE", 4) == 0){//message asking to vote for new master
			my_vote = rand()%10 + 1; //generate random # 1-10
			printf("VOTE received");
			//bzero(buffer, sizeof(buffer));
			sprintf(msg, "# %s %d", MY_ADDRESS, my_vote);
			//put vote msg in bffer and send to socket
			pubmsg.payload = msg;
			pubmsg.payloadlen = sizeof(msg);
			pubmsg.qos = QOS;
			pubmsg.retained = 0;
			deliveredtoken = 0;
			MQTTClient_publishMessage(client, TOPIC_1, &pubmsg, &token);
			
		}
		if(buffer[0] == '#'){//messages with votes start with #
			if(master != -1){// master flag is set to -1 if this device votes lower than another
				strtok(buffer, " ");
				strcpy(rcv_IP, strtok(NULL, " "));
				rcv_vote = atoi(strtok(NULL, " "));
				
				if(my_vote > rcv_vote){//set master to one while still winning
					master = 1;
				}
				else if(my_vote < rcv_vote){//-1 if lost
					master = -1;
				}
				else if(my_vote == rcv_vote){// compare IPs if tie vote
					strtok(rcv_IP, ".");
					strtok(NULL, ".");
					strtok(NULL, ".");
					int tmpIP = atoi(strtok(NULL, " ."));
					
					if(myIP > tmpIP) master = 1;
					else if(myIP < tmpIP) master = -1;
				}
			}
		}
	}

	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);
	close(cdev_id);
	return 1;
}
void connlost(void *context, char *cause)
{
	printf("\nConnection lost\n");
	printf("     cause: %s\n", cause);
}



int main(void)
{
	
	MQTTClient client;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	MQTTClient_message pubmsg = MQTTClient_message_initializer;
	MQTTClient_deliveryToken token;
	int rc;
	int ch;

	MQTTClient_create(&client, MY_ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;

	
	srand(time(NULL));//seed random

	MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
	{
	printf("Failed to connect, return code %d\n", rc);
	exit(EXIT_FAILURE);
	}
	printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
	   "Press Q<Enter> to quit\n\n", TOPIC_1, CLIENTID, QOS);
	MQTTClient_subscribe(client, TOPIC_1, QOS);
		printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
	   "Press Q<Enter> to quit\n\n", TOPIC_2, CLIENTID, QOS);
	MQTTClient_subscribe(client, TOPIC_2, QOS);
	do
	{
	ch = getchar();
	} while(ch!='Q' && ch != 'q');
	MQTTClient_disconnect(client, 10000);
	MQTTClient_destroy(&client);


	
	//close(cdev_id);	// close the device.
	return 0;  		// dummy used to prevent warning messages...
}
