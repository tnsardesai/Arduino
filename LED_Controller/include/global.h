#include <gtk/gtk.h>
#include <stdlib.h>
#include <iostream>
#define __STDC_FORMAT_MACROS  //!< macro to format macros


#ifndef _MY__GLOBAL__H
#define _MY__GLOBAL__H //!< Macro to define global.h

using namespace std;


/**************************************************************
 * GUI window stuff
 **************************************************************/

typedef struct 
{
  GtkWidget *window1;  //!< pointer to the main window of GUI

  GtkWidget *entry_sd;  //!< pointer to the the text field to contain the name of the serial device of GUI
  GtkWidget *label_voltage; //!< pointer to the the label on which we display the voltage of GUI
GtkWidget *send_button; //!< pointer to the send button of the GUI
GtkWidget *open_button; //!< pointer to the open button of the GUI
GtkWidget *close_button; //!< pointer to the close button of the GUI
GtkWidget *exit_button; //!< pointer to the exit button of the GUI
GtkWidget *entryR; //!< pointer to the text box for entry for Red of the GUI
GtkWidget *entryG; //!< pointer to the text box for entry for Green of the GUI
GtkWidget *entryB; //!< pointer to the text box for entry for Bule of the GUI
GtkWidget *scaleR; //!< pointer to the slider for Red of the GUI
GtkWidget *scaleG; //!< pointer to the slider for Red of the GUI
GtkWidget *scaleB; //!< pointer to the slider for Red of the GUI
GtkWidget *tx_label; //!< pointer to the label to on which we display the RGB string sent.

  
} Gui_Window_AppWidgets;

extern Gui_Window_AppWidgets *gui_app;

//this is the serial devices handle
extern int ser_dev;

//this is to gracefully shut down threads
extern int kill_all_threads;

//this variable is for communicating the voltage value string
extern char c_voltage_value[40];

//this is the mutex for the above variable
extern GMutex *mutex_to_protect_voltage_display;

//prototype of function for serial read thread
gpointer Serial_Read_Thread();



#endif
