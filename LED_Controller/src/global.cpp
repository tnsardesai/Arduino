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

#include "global.h"

Gui_Window_AppWidgets *gui_app; //!< structure to keep all interesting widgets

int ser_dev=-1; //!< integer to store serial status

char c_voltage_value[40]; //!< char array to store voltage value

int kill_all_threads; //!< bool integer to see if you need to kill threads.

GMutex *mutex_to_protect_voltage_display; //!< The mutex needed to keep voltage label in range



  

