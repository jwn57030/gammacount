/*************************************************
* \file      countStats.hpp
* \details   generic lib to count something and
*            allow user to query stats about the count.
* \Warning   Stats will be considered invalid until first
*            reading is recieved.
* \author    Jason Neitzert
* \date      12/9/2021
* \Copyright Jason Neitzert 
*************************************************/
#pragma once

/****************** Includes ************************/
#include <time.h>
#include <pthread.h>

/****************** Questions/Assumptions ***********/
/*
   1. Is this multithreaded enviornment? Assuming it could be
   2. Could the lib be used by multiple users at once: Assuming it could be
   3. Assumming my function update stats will not be called in an interrupt, 
      otherwise using logging would be bad. Also would need to disable interrups
      during read so it didn't get out of whack values.  
   4. The countStats is generic so it can be used by other things, for example you
      may want a GammaStats and a AlphaStats that both inherit countStats.
*/

/****************** Enums ************/
/* Could add an enum for error return values if more
   detail is needed other than true/false. */

/****************** Structs and Typedefs ************/
/* Public Structure to return stats back to user in */
/* All stats will be considered invalid until first reading is recieved.  If you
   were to consider the values of 0 to be valid it wouldn't make sense on a graph
   as you actually don't know the value before you started measurring. */
typedef struct CountData
{
    unsigned int total_counts; /* Can numbers of readings be so high that this could overflow? */
    unsigned int number_of_readings;
    
    /* min/max cps are instantaneous and not a moving average over a long period of time */
    /* min_cps will always be inited on first update otherwise min would always be 0 */
    unsigned int min_cps;
    unsigned int max_cps;
    
    /* User will be responsible for converting too a human readable time */
    /* As updates and requests are only expected every second, using
       anything more granular than sec is probably not needed. Depends
       on what user interface looks like and how granular time will be displayed */
    time_t first_epoch_time_seconds;
    time_t last_epoch_time_seconds;
     
} CountData;

/****************** Class Definition ************/
/* Assuming lib could be used by multiple
   users/sensors in system at same time. If its one sensor only, the data
   could be stored in private global variable within the lib instead of using
   class. */
class CountStats
{
   public:
      CountStats(void);
      ~CountStats(void);

      void count_stats_reset();
      bool count_stats_get(CountData &get_data);
      void count_stats_update(unsigned int count);
      /* Note: If required could add functions to get stats individually */
      
      /* For Testing */
      void print_stats();

   private:
      CountData c_stats;

      /* Using mutex to make sure I am not reading partially updated data */
      pthread_mutex_t stats_lock; 
};
