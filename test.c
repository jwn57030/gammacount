/*************************************************
* \file      test.c
* \details   testcode for gammacount/countStats lib
* \author    Jason Neitzert
* \date      12/1/2021
* \Copyright Jason Neitzert 
*************************************************/

/****************** Includes ************************/
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include "gammaStats.h"

/***************** Private Functions ****************/
/**
 * \brief Prints everything in stats structure 
 * 
 * \param p_gstats_handle - handle to print stats on.
 * 
 * \return void
 * \author Jason Neitzert
 */
static void print_stats(GStatsHandle *p_gstats_handle)
{
    GammaStats gstats = {0};

    if (!count_stats_get(p_gstats_handle, &gstats))
    {
        printf("Failed to get stats in print stats\n");
    }
    else
    {
        printf("Min: %u Max: %u Total Counts: %u Total Measurements: %u\n",
                 gstats.min_cps, gstats.max_cps, gstats.total_counts,
                 gstats.number_of_readings);
        printf("Start Time: %ld Last Time: %ld\n", gstats.first_epoch_time_seconds, 
                                                   gstats.last_epoch_time_seconds);
    }        
}

/**
 * \brief  Test CountStats lib failures without handle 
 *  
 * \return void
 * \author Jason Neitzert
 */
static void test_failure_cases_without_handle()
{
    if (count_stats_reset(NULL))
    {
        printf("count_stats_reset failed to fail\n");
    }

    if (count_stats_get(NULL, NULL))
    {
        printf("count_stats_reset failed to catch invalid handle\n");
    }
    
    if(count_stats_update(NULL, 0))
    {
        printf("count_stats_update failed to catch invalid handle\n");
    }    
}

/**
 * \brief Test failure cases with valid handle 
 * 
 * \param p_gstats_handle - handle to test.
 * 
 * \return void
 * \author Jason Neitzert
 */
static void test_failure_cases_with_valid_handle(GStatsHandle *p_gstats_handle)
{
    GammaStats  gstats = {0};

    /* Test failure cases with a valid handle */
    if (count_stats_get(p_gstats_handle, NULL))
    {
        printf("count_stats_get failed to recognize invald input struct\n");
    }

    if (count_stats_get(p_gstats_handle, &gstats))
    {
        printf("count stats get failed to fail when it was not inited\n");
    }    
}

/**
 * \brief Test a valid case  
 * 
 * \param p_gstats_handle - handle to test.
 * 
 * \return void
 * \author Jason Neitzert
 */
static void test_good_cases_with_single_thread(GStatsHandle *p_gstats_handle)
{
    /* test good cases with single thread */
    count_stats_update(p_gstats_handle, 20);
    print_stats(p_gstats_handle);
    sleep(1);

    count_stats_update(p_gstats_handle, 2);
    print_stats(p_gstats_handle);
    sleep(1);

    count_stats_update(p_gstats_handle, 500);
    print_stats(p_gstats_handle);
    sleep(1);

    count_stats_update(p_gstats_handle, 0);
    print_stats(p_gstats_handle);
    sleep(1);

    /* verify reset works correctly */
    count_stats_reset(p_gstats_handle);
    print_stats(p_gstats_handle);

    count_stats_update(p_gstats_handle, 20);
    print_stats(p_gstats_handle);

}

/****************** Public Functions ****************/
void main()
{
    GStatsHandle *p_gstats_handle = NULL;

    printf("Start Tests\n");

    /* Verify it goes through failure cases for functions correctly when handle
       has not been created. */
    test_failure_cases_without_handle();

    p_gstats_handle = count_stats_new();

    if (!p_gstats_handle)
    {
        printf("failed to create gstats handle\n");
    }
    else
    {
        test_failure_cases_with_valid_handle(p_gstats_handle);
        test_good_cases_with_single_thread(p_gstats_handle);

        /* Could add tests here for mutexes with multiple threads */

        /* Destroy memory before exiting */
        count_stats_destroy(&p_gstats_handle);
    }
}