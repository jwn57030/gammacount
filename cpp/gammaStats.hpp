/*************************************************
* \file      gammmaStats.hpp
* \details   Keeps track of gamma ray statistics using 
*            countStats lib.
* \author    Jason Neitzert
* \date      12/9/2021
* \Copyright Jason Neitzert 
*************************************************/
#pragma once

/****************** Includes ************************/
#include "countStats.hpp"

/****************** Class Definition **************/
class GammaStats: public CountStats{};
typedef struct CountData GammaData;