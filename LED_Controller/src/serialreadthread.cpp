/**
 * \author Tanmay Sardesai
 * \author Satej Mhatre
 * \version 1.0
 * \date 2016-17-15
 * \bug None.
 * \warning None.
 *
 * \copyright GNU Public License.
 *
 * \mainpage The Teensy LED Controller - Global.cpp
 * \section intro_sec Introduction
 */

#include <glib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "global.h"

#define READ_THREAD_SLEEP_DURATION_US 100000
#define BETWEEN_CHARACTERS_TIMEOUT_US 1000
#define MAX_VOLTAGE 3.3
#define MAX_ADC_VALUE 1024.0
#define POT_COMMAND 'P'

// define packet parameters
#define PACKET_START_BYTE  0xAA
#define PACKET_OVERHEAD_BYTES  3
#define PACKET_MIN_BYTES  PACKET_OVERHEAD_BYTES + 1
#define PACKET_MAX_BYTES  255

/**
 * \brief This function is used to validate an incoming packet.
 * \param packetSize integer to store packet size
 * \param packet char pointed to the packet
 */
int validatePacket(unsigned int packetSize, char *packet)
{
    // check the packet size
    if(packetSize < PACKET_MIN_BYTES || packetSize > PACKET_MAX_BYTES)
    {
        return false;
    }

    // check the start byte
    if(packet[0] != PACKET_START_BYTE)
    {
        return false;
    }

    // check the length byte
    if(packet[1] != packetSize)
    {
        return false;
    }

    // compute the checksum
    char checksum = 0x00; //!< 1 byte variable that stores the checksum that is at the end of the packet
    for(unsigned int i = 0; i < packetSize - 1; i++)
    {
        checksum = checksum ^ packet[i];
    }

    // check to see if the computed checksum and packet checksum are equal
    if(packet[packetSize - 1] != checksum)
    {	
        return false;
    }

    // all validation checks passed, the packet is valid
    return true;
}

/**
 * \brief This function is used to Read incoming packets.
 */

gpointer Serial_Read_Thread()
{
  ssize_t r_res; //!< variable to store the status of reading and storing packet
  char ob[50]; //!< char array to store packet
  unsigned int count=0; //!< counter for packet
  static char buffer[PACKET_MAX_BYTES]; //!< char array to store buffer
  unsigned int packetSize = PACKET_MIN_BYTES;
  double voltage_disp;

  while(!kill_all_threads)
    {
      if(ser_dev!=-1)
	{
	  r_res = read(ser_dev,ob,1);
	  //	  cout<<(int)ob[0]<<endl;
	  if(r_res==0)
	    {
	      usleep(BETWEEN_CHARACTERS_TIMEOUT_US);
	    }
	  else if(r_res<0)
	    {
	      cerr<<"Read error:"<<(int)errno<<" ("<<strerror(errno)<<")"<<endl;
	    }
	  else
	    {
	      //this means we have received a byte;
	      //the byte is in 
  char b = ob[0];


            // handle the byte according to the current count
            if(count == 0 && b == PACKET_START_BYTE)
            {
                // this byte signals the beginning of a new packet
                buffer[count] = b;
                count++;
                continue;
            }
            else if(count == 0)
            {
                // the first byte is not valid, ignore it and continue
                continue;
            }
            else if(count == 1)
            {
                // this byte contains the overall packet length
                buffer[count] = b;

                // reset the count if the packet length is not in range
                if(packetSize < PACKET_MIN_BYTES || packetSize > PACKET_MAX_BYTES)
                {
                    count = 0;
                }
                else
                {
                    packetSize = b;
                    count++;
                }
                continue;
            }
            else if(count < packetSize)
            {
                // store the byte
                buffer[count] = b;
                count++;
            }

            // check to see if we have acquired enough bytes for a full packet
            if(count >= packetSize)
            {
                // validate the packet
                if(validatePacket(packetSize, buffer))
                {
                    

                    if(buffer[2]=='P')
                    {
                        voltage_disp = 256* (double)buffer[3] + (double)buffer[4];
                        voltage_disp = voltage_disp * MAX_VOLTAGE / MAX_ADC_VALUE;
                          g_mutex_lock(mutex_to_protect_voltage_display);
                          sprintf(c_voltage_value,"%1.3f",voltage_disp);
                          g_mutex_unlock(mutex_to_protect_voltage_display);
                    }

                    
                    //int potValue = analogRead(POT_PIN);
                }

                // reset the count
                count = 0;
            }
        
	      //TODO: receive an entire packet and validate it.
	      
	      //TODO: Once validated, check if the third byte is a 'P'
	      //decode the payload into a single int between 0 and MAX_ADC_VALUE
	      //convert it into a voltage 0->0V  MAX_ADC_VALUE->MAX_VOLTAGE
	      //load that converted value into voltage_disp
	      //the next three lines need to be there for the value to be
	      //displayed (once you loaded voltage_value with the received
	      //voltage value

	      //once you have voltage_disp calculated us the following three
	      //lines of code to change the global variable c_voltage_value
	      //this is the variable that the main thread periodically displays
	      //in the GUI

	
	    }
	}
      else
	usleep(READ_THREAD_SLEEP_DURATION_US);
    }

  return NULL;
}



