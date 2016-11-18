/**
  * \author Tanmay Sardesai
  * \author Satej Mhatre
  * \version 1.0
  * \date 2016-17-15
  * \bug No bugs found yet.
  *
  * \warning Typing String in the text boxes is not handled.
  *
  * \copyright GNU Public License.
  * \mainpage The Teensy LED Controller
  * \section intro_sec Introduction
  * This code was developed to control the RGB led and potentiometer connected to the Teensy Controler.
  * \section compile_sec Compilation
  * If build dir already exsists then skip step 1.
  * \subsection Step1 Step 1
  * mkdir build
  * \subsection Step2 Step 2
  * cd build
  * \subsection Step3 Step 3
  * rm -rf *
  * \subsection Step4 Step 4
  * cmake ..
  * \subsection Step5 Step 5
  * make
*/






#include "global.h"
#include "string.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#define VOLTAGE_DISPLAY_UPDATE_MS 100 //!< Every 100ms check if there is a change in the potentiometer

/**
  * \brief Function to obtain the GUIWidgets. includes GuiappGET for widgets defined in global.h
  * \param *p_builder
  */
void ObtainGuiWidgets(GtkBuilder *p_builder)
{
  #define GuiappGET(xx) gui_app->xx=GTK_WIDGET(gtk_builder_get_object(p_builder,#xx))  //!< Convertes the function on the right to the one on the left
  GuiappGET(window1);
  GuiappGET(entry_sd);
  GuiappGET(label_voltage);
   GuiappGET(send_button);
GuiappGET(open_button);
GuiappGET(close_button);
GuiappGET(exit_button);
GuiappGET(entryR);
GuiappGET(entryG);
GuiappGET(entryB);
GuiappGET(scaleR);
GuiappGET(scaleG);
GuiappGET(scaleB);
GuiappGET(tx_label);
}


/**
  * \brief This function locks the mutex to display the voltage. Once displayed it unlocks the mutex.
  * \param p_gptr
*/
gboolean  Voltage_Display_Displayer(gpointer p_gptr)
{
  // do not change this function
  g_mutex_lock(mutex_to_protect_voltage_display);
  gtk_label_set_text(GTK_LABEL(gui_app->label_voltage),c_voltage_value);
  g_mutex_unlock(mutex_to_protect_voltage_display);
  return true;
}


/**
 * \brief Action to perform when the open device button is clicked
 * \param *p_wdgt pointer to the button that called this function
 * \param p_data pointer to some data
 */

extern "C" void button_opendevice_clicked(GtkWidget *p_wdgt, gpointer p_data ) 
{
  //do not change  the next few lines
  //they contain the mambo-jumbo to open a serial port
  
  const char *t_device_value;
  struct termios my_serial;

  t_device_value = gtk_entry_get_text(GTK_ENTRY(gui_app->entry_sd));
  //open serial port with read and write, no controling terminal (we don't
  //want to get killed if serial sends CTRL-C), non-blocking 
  ser_dev = open(t_device_value, O_RDWR | O_NOCTTY );
  
  bzero(&my_serial, sizeof(my_serial)); // clear struct for new port settings 
        
  //B9600: set baud rate to 9600
  //   CS8     : 8n1 (8bit,no parity,1 stopbit)
  //   CLOCAL  : local connection, no modem contol
  //   CREAD   : enable receiving characters  */
  my_serial.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
         
  tcflush(ser_dev, TCIFLUSH);
  tcsetattr(ser_dev,TCSANOW,&my_serial);

  //You can add code beyond this line but do not change anything above this line
  
 gtk_widget_set_sensitive (gui_app->open_button,FALSE);
  //this is how you enable a button:
  gtk_widget_set_sensitive (gui_app->close_button,TRUE);

}

/**
 * \brief Action to perform when the close device button is clicked
 * \param *p_wdgt pointer to the button that called this function
 * \param p_data pointer to some data
 */

extern "C" void button_closedevice_clicked(GtkWidget *p_wdgt, gpointer p_data ) 
{
  //this is how you disable a button:
  gtk_widget_set_sensitive (gui_app->open_button,TRUE);
  //this is how you enable a button:
  gtk_widget_set_sensitive (gui_app->close_button,FALSE);

  //do not change the next two lines; they close the serial port
  close(ser_dev);
  ser_dev=-1;

}

/**
 * \brief Action to perform when the send button is clicked
 * \param *p_wdgt pointer to the button that called this function
 * \param p_data pointer to some data
 */

extern "C" void button_send_clicked(GtkWidget *p_wdgt, gpointer p_data ) 
{
  const char *t_red_value;
  //unsigned char uc_red_value;
  int g_red_value;

  const char *t_green_value;
  int g_green_value;

const char *t_blue_value;
   int g_blue_value;

  char c_cc_value[40];
  char send_buff[7];
  int length_send_buff = 7;

  //getting text from widget:
  t_red_value = gtk_entry_get_text(GTK_ENTRY(gui_app->entryR));
  g_red_value = atoi(t_red_value);

  //getting text from widget:
  t_green_value = gtk_entry_get_text(GTK_ENTRY(gui_app->entryG));
  g_green_value = atoi(t_green_value);

  //getting text from widget:
  t_blue_value = gtk_entry_get_text(GTK_ENTRY(gui_app->entryB));
  g_blue_value = atoi(t_blue_value);

 
  	send_buff[0] = 0xAA;
  	send_buff[1] = 0x07;
	send_buff[2] = 0x4C;
	send_buff[3] = g_red_value;
	send_buff[4] = g_green_value;
	send_buff[5] =g_blue_value;
	for(int i=0; i< 6; i++){
		send_buff[6] ^= send_buff[i];
	}

  	sprintf(c_cc_value,"%02X %02X %02X %02X %02X %02X %02X",send_buff[0],send_buff[1],send_buff[2],send_buff[3],send_buff[4],send_buff[5],send_buff[6]);
	gtk_label_set_text(GTK_LABEL(gui_app->tx_label),c_cc_value);

	//this is how you send an array out on the serial port:
  	write(ser_dev,send_buff,length_send_buff);

 	 //sprintf(c_cc_value,"%d",g_red_value);
  	//setting text on label
 	 gtk_label_set_text(GTK_LABEL(gui_app->tx_label),c_cc_value);


 //setting range on scale slider to uc_red_value 
  gtk_range_set_value(GTK_RANGE(gui_app->scaleR),g_red_value);      
  
  //setting range on scale slider to uc_red_value 
  gtk_range_set_value(GTK_RANGE(gui_app->scaleG),g_green_value);  

  //setting range on scale slider to uc_red_value 
  gtk_range_set_value(GTK_RANGE(gui_app->scaleB),g_blue_value);  
}
/**
 * \brief Action to perform when the slider value has changed
 * \param *p_wdgt pointer to the button that called this function
 * \param p_data pointer to some data
 */
extern "C" void scale_rgb_value_changed(GtkWidget *p_wdgt, gpointer p_data ) 
{
	
	 char c_r_value[40];
	char c_g_value[40];
	char c_b_value[40];

  char c_cc_value[40];
  char send_buff[7];
  int length_send_buff = 7;

	  //getting the value of the scale slider 
	  int g_red_value = gtk_range_get_value(GTK_RANGE(gui_app->scaleR));
	  //setting text on entry
	  

	int g_green_value = gtk_range_get_value(GTK_RANGE(gui_app->scaleG));
	  //setting text on entry
	  

	int g_blue_value = gtk_range_get_value(GTK_RANGE(gui_app->scaleB));
	  //setting text on entry
	  

	sprintf(c_r_value,"%d",g_red_value);
	sprintf(c_g_value,"%d",g_green_value);
	sprintf(c_b_value,"%d",g_blue_value);

 
  	send_buff[0] = 0xAA;
  	send_buff[1] = 0x07;
	send_buff[2] = 0x4C;
	send_buff[3] = g_red_value;
	send_buff[4] = g_green_value;
	send_buff[5] =g_blue_value;
	for(int i=0; i< 6; i++){
		send_buff[6] ^= send_buff[i];
	}

  	sprintf(c_cc_value,"%02X %02X %02X %02X %02X %02X %02X",send_buff[0],send_buff[1],send_buff[2],send_buff[3],send_buff[4],send_buff[5],send_buff[6]);
	gtk_label_set_text(GTK_LABEL(gui_app->tx_label),c_cc_value);

	//this is how you send an array out on the serial port:
  	write(ser_dev,send_buff,length_send_buff);


	gtk_entry_set_text(GTK_ENTRY(gui_app->entryR),c_r_value);
	gtk_entry_set_text(GTK_ENTRY(gui_app->entryG),c_g_value);
	gtk_entry_set_text(GTK_ENTRY(gui_app->entryB),c_b_value);  


	
	  
}

/**
 * \brief Action to perform when the exit button is clicked
 * \param *p_wdgt pointer to the button that called this function
 * \param p_data pointer to some data
 */
extern "C" void button_exit_clicked(GtkWidget *p_wdgt, gpointer p_data ) 
{
  gtk_main_quit();

}


/**
 * \brief The main function that is the program's entry point.
 * \param argc the argument count
 * \param **argv pointer to arguments
 */

int main(int argc, char **argv)
{

  GtkBuilder *builder;
  GError *err = NULL;

  GThread *read_thread;

  //this is how you allocate a Glib mutex
  g_assert(mutex_to_protect_voltage_display == NULL);
  mutex_to_protect_voltage_display = new GMutex;
  g_mutex_init(mutex_to_protect_voltage_display);

  // this is used to signal all threads to exit
  kill_all_threads=false;
  
  //spawn the serial read thread
  read_thread = g_thread_new(NULL,(GThreadFunc)Serial_Read_Thread,NULL);
  
  // Now we initialize GTK+ 
  gtk_init(&argc, &argv);
  
  //create gtk_instance for visualization
  gui_app = g_slice_new(Gui_Window_AppWidgets);

  //builder
  builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, "teensy_control.glade", &err);

  
  //error handling
  if(err)
    {
      g_error(err->message);
      g_error_free(err);
      g_slice_free(Gui_Window_AppWidgets, gui_app);
      exit(-1);
    }

  // Obtain widgets that we need
  ObtainGuiWidgets(builder);

  // Connect signals
  gtk_builder_connect_signals(builder, gui_app);

  // Destroy builder now that we created the infrastructure
  g_object_unref(G_OBJECT(builder));

  //display the gui
  gtk_widget_show(GTK_WIDGET(gui_app->window1));

  //this is going to call the Voltage_Display_Displayer function periodically
  gdk_threads_add_timeout(VOLTAGE_DISPLAY_UPDATE_MS,Voltage_Display_Displayer,NULL);

  //the main loop
  gtk_main();

  //signal all threads to die and wait for them (only one child thread)
  kill_all_threads=true;
  g_thread_join(read_thread);
  
  //destroy gui if it still exists
  if(gui_app)
    g_slice_free(Gui_Window_AppWidgets, gui_app);

  return 0;
}
