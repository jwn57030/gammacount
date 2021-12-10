/*************************************************
* \file      countStats.cpp
* \details   generic lib to count something and
*            allow user to query stats about the count.
* \author    Jason Neitzert
* \date      12/1/2021
* \Copyright Jason Neitzert 
*************************************************/

/****************** Includes ************************/
#include <iostream>
#include <cstring>
#include "countStats.hpp"

using namespace std;
/****************** Public Functions ****************/

/**
 * \brief   Create a new Count Stats object 
 * \details Stats are considered invalid until first time
 *          data is recieved after creation.
 * \author  Jason Neitzert
 */
CountStats::CountStats(void)
{
    /* could add a try/catch block here */
    if (0 != pthread_mutex_init(&this->stats_lock, NULL))
    {
        cerr << "failed to init mutex\n";      
    }

    this->count_stats_reset();
}

/**
 * \brief Destroys a countStats object 
 * 
 * \author Jason Neitzert
 */
CountStats::~CountStats(void)
{
    pthread_mutex_destroy(&this->stats_lock);
}

/**
 * \brief   Resets the stats for a given handle 
 * \details Stats are considered invalid until first time
 *          data is recieved after a reset.
 * 
 * \return void
 * 
 * \author Jason Neitzert
 */
void CountStats::count_stats_reset()
{
    pthread_mutex_lock(&this->stats_lock);
    /* A zero epoch time will be considered as stats are invalid */
    memset(&this->c_stats, 0, sizeof(CountStats));
    pthread_mutex_unlock(&this->stats_lock);
}

/**
 * \brief   Gets the current stats for a given handle 
 * \details Stats are considered invalid until first time
 *          data is recieved after a reset or create.
 * 
 * \param get_stats - reference to place stats inside of
 * 
 * \return bool - false if failed
 * \author Jason Neitzert
 */
bool CountStats::count_stats_get(CountData &get_stats)
{
    bool retval = false;

    pthread_mutex_lock(&this->stats_lock);
    /* A zero epoch time will be considered as stats are invalid */
    if (this->c_stats.first_epoch_time_seconds == 0)
    {
        printf("first reading has not been recieved\n");
    }
    else
    {
        /* copy the stats to the requested location */
        get_stats = this->c_stats;
        retval = true;
    }
    pthread_mutex_unlock(&this->stats_lock);  

    return retval;
}

/**
 * \brief   Adds to stats for a given handle 
 * \details This function is responsible for getting time stamp
 * 
 * \param p_handle - handle to reset stats on.
 * \param count    - number of counts being reported
 * 
 * \return void
 * \author Jason Neitzert
 */
void CountStats::count_stats_update(unsigned int count)
{
    bool        retval  = true;
    CountData  *p_data = NULL;

    p_data = &this->c_stats;
    
    pthread_mutex_lock(&this->stats_lock);
    /* Get Time right away so its as accurate as possibble to when it was measured */
    p_data->last_epoch_time_seconds = time(NULL);
    p_data->total_counts += count;
    p_data->number_of_readings++;

    /* if its the first time after reset make sure min_cps and 
        first_epoch_time get set correctly */        
    if (0 == p_data->first_epoch_time_seconds)
    {         
        p_data->first_epoch_time_seconds = p_data->last_epoch_time_seconds;
        p_data->min_cps = count;
        p_data->max_cps = count;                   
    }
    /* The only time min and max can change at same time is right after reset */
    else if (count < p_data->min_cps)
    {
        p_data->min_cps = count;
    }
    else if (p_data->max_cps < count)
    {
        p_data->max_cps = count;
    }
    pthread_mutex_unlock(&this->stats_lock);
}

/**
 * \brief Prints everything in stats structure 
 * 
 * \return void
 * \author Jason Neitzert
 */
void CountStats::print_stats()
{
    pthread_mutex_lock(&this->stats_lock);
    cout << "Min: " << this->c_stats.min_cps << " Max: " <<   this->c_stats.max_cps << 
        " Total Counts: " << this->c_stats.total_counts << " Total Measurements: "
         << this->c_stats.number_of_readings << endl;
    cout << "Start time: " << this->c_stats.first_epoch_time_seconds << " Last Time: " <<
        this->c_stats.last_epoch_time_seconds << endl;
    pthread_mutex_unlock(&this->stats_lock);        
}