/*************************************************
* \file      gammmaStats.h
* \details   Keeps track of gamma ray statistics using 
*            countStats lib.
* \author    Jason Neitzert
* \date      12/1/2021
* \Copyright Jason Neitzert 
*************************************************/
#ifndef __GAMMASTATS_H
#define __GAMMASTATS_H

/****************** Includes ************************/
#include "countStats.h"

/****************** Structs and Typedefs ************/
/* Rename public Count Stats structs for convience to user */
typedef struct CStatsHandle GStatsHandle;
typedef struct CountStats   GammaStats;

/* If their was something specific to gamma ray counts we needed
   to keep track of, it could be added here. We would also
   need to add the implementation in a new .c file
   In that case we would need to define a new handle and struct
   that inherited the CountStats struct. Example:
   
   typedef struct GammaStats
   {
       CountStats c_stats;
       unsigned int average_gamma_count;
   } GammaStats;

 */
#endif /* _GAMMASTATS_H */