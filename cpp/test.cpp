/*************************************************
* \file      test.c
* \details   testcode for gammacount/countStats lib
* \author    Jason Neitzert
* \date      12/1/2021
* \Copyright Jason Neitzert 
*************************************************/

/****************** Includes ************************/
#include <unistd.h>
#include <iostream>
#include "gammaStats.hpp"

using namespace std;

/***************** Private Functions ****************/

/**
 * \brief Test failure cases with valid handle 
 * 
 * \param gamma_stats - GammaStats object
 * 
 * \return void
 * \author Jason Neitzert
 */
static void test_failure_cases_with_valid_handle(GammaStats gamma_stats)
{
    GammaData gdata = {0};

    if (gamma_stats.count_stats_get(gdata))
    {
        cerr << "count stats get failed to fail when it was not inited" << endl;
    }    
}

/**
 * \brief Test a valid case  
 * 
 * \param gamma_stats - GammaStats object
 * 
 * \return void
 * \author Jason Neitzert
 */
static void test_good_cases_with_single_thread(GammaStats gamma_stats)
{
    /* test good cases with single thread */
    gamma_stats.count_stats_update(20);
    gamma_stats.print_stats();
    sleep(1);

    gamma_stats.count_stats_update(2);
    gamma_stats.print_stats();
    sleep(1);

    gamma_stats.count_stats_update(500);
    gamma_stats.print_stats();
    sleep(1);

    gamma_stats.count_stats_update(0);
    gamma_stats.print_stats();
    sleep(1);

    /* verify reset works correctly */
    gamma_stats.count_stats_reset();
    gamma_stats.print_stats();

    gamma_stats.count_stats_update(20);
    gamma_stats.print_stats();
}

/****************** Public Functions ****************/
int main()
{
    GammaStats gstats;

    cout << "Start Tests" << endl;

    test_failure_cases_with_valid_handle(gstats);
    test_good_cases_with_single_thread(gstats);

    /* Could add tests here for mutexes with multiple threads */

    return 0;
}