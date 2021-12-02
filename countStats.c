/*************************************************
* \file      countStats.c
* \details   generic lib to count something and
*            allow user to query stats about the count.
* \author    Jason Neitzert
* \date      12/1/2021
* \Copyright Jason Neitzert 
*************************************************/

/****************** Includes ************************/
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include "countStats.h"

/****************** Structs and Typedefs ************/
/* Pviate CountStats data */
struct CStatsHandle 
{
    CountStats c_stats;

    /* Using mutex to make sure I am not reading partially updated data, also to
      make sure data given back is most up to date data.  As its only updated once a second,
      if a read occurred right before the update, without a mutexx, the values given back to 
      the user could be off by upto 2 seconds behind */
    pthread_mutex_t stats_lock;     
};

/****************** Public Functions ****************/

/**
 * \brief Create a new countStats object 
 * \details Stats are considered invalid until first time
 *          data is recieved after creation.
 * \return CStateHandle* - NULL if it fails.
 * \author Jason Neitzert
 */
CStatsHandle *count_stats_new()
{
    CStatsHandle *p_handle = calloc(1, sizeof(CStatsHandle)); 

    if (p_handle)
    {
        if (0 != pthread_mutex_init(&p_handle->stats_lock, NULL))
        {
            printf("failed to init mutex\n");
            free(p_handle);
            p_handle = NULL;                        
        }
    }

    return p_handle;
}

/**
 * \brief Destroys a countStats object and sets the pointer
 *        to NULL. 
 *
 * \param pp_handle - pointer count stats handle 
 * 
 * \return void - if an error return is desired could make
 *                this a bool, but if it fails user can
 *                still go on, just with mem leak. 
 * \author Jason Neitzert
 */
void count_stats_destroy(CStatsHandle **pp_handle)
{
    /* make sure both pointers passed in are valid */
    if (pp_handle && *pp_handle)
    {
        pthread_mutex_destroy(&(*pp_handle)->stats_lock);
        free(*pp_handle);
        *pp_handle = NULL;
    }
}

/**
 * \brief Resets the stats for a given handle 
 * \details Stats are considered invalid until first time
 *          data is recieved after a reset.
 * 
 * \param p_handle - handle to reset stats on.
 * 
 * \return bool - false if failed.
 * 
 * \author Jason Neitzert
 */
bool count_stats_reset(CStatsHandle *p_handle)
{
    bool retval = true;
    CountStats test = {0};

    if (!p_handle)
    {
        printf("p_handle is invalid\n");
        retval = false;
    }
    else
    {
        pthread_mutex_lock(&p_handle->stats_lock);
        /* A zero epoch time will be considered as stats are invalid */
        memset(&p_handle->c_stats, 0, sizeof(CountStats));
        pthread_mutex_unlock(&p_handle->stats_lock);
    }

    return retval;
}

/**
 * \brief Gets the current stats for a given handle 
 * \details Stats are considered invalid until first time
 *          data is recieved after a reset or create.
 * 
 * \param p_handle - handle to reset stats on.
 * \param p_stats - pointer to place stats inside of
 * 
 * \return bool - false if failed
 * \author Jason Neitzert
 */
bool count_stats_get(CStatsHandle *p_handle, CountStats *p_stats)
{
    bool retval = false;

    if (!p_handle)
    {
        printf("p_handle is invalid\n");
    }
    else if (!p_stats)
    {
        printf("p_stats is NULL\n");
    }
    else
    {
        if (0 == pthread_mutex_lock(&p_handle->stats_lock))
        {
            /* A zero epoch time will be considered as stats are invalid */
            if (p_handle->c_stats.first_epoch_time_seconds == 0)
            {
                printf("first reading has not been recieved\n");
            }
            else
            {
                /* copy the stats to the requested location */
                *p_stats = p_handle->c_stats;
                retval = true;
            }

            /* Not checking retval of unlock as data should still be valid 
            as the locked was obtained correctly. */
            pthread_mutex_unlock(&p_handle->stats_lock);
        }
    }   

    return retval;
}


/**
 * \brief Adds to stats for a given handle 
 * \details This function is responsible for getting time stamp
 * 
 * \param p_handle - handle to reset stats on.
 * \param count - number of counts being reported
 * 
 * \return void
 * \author Jason Neitzert
 */
bool count_stats_update(CStatsHandle *p_handle, unsigned int count)
{
    bool retval = true;
    CountStats *p_stats = NULL;

    if (!p_handle)
    {
        printf("p_handle is invalid\n");
        retval = false;
    }
    else
    {
        p_stats = &p_handle->c_stats;
        pthread_mutex_lock(&p_handle->stats_lock);
        /* Get Time right away so its as accurate as possibble to when it was measured */
        p_stats->last_epoch_time_seconds = time(NULL);
        p_stats->total_counts += count;
        p_stats->number_of_readings++;

        /* if its the first time after reset make sure min_cps and 
           the first time get set correctly */        
        if (0 == p_stats->first_epoch_time_seconds)
        {         
            p_stats->first_epoch_time_seconds = p_stats->last_epoch_time_seconds;
            p_stats->min_cps = count;
            p_stats->max_cps = count;                   
        }
        /* The only time min and max can change at same time is right after reset */
        else if (count < p_stats->min_cps)
        {
            p_stats->min_cps = count;
        }
        else if (p_stats->max_cps < count)
        {
            p_stats->max_cps = count;
        }
        pthread_mutex_unlock(&p_handle->stats_lock);
    }

    return retval;
}