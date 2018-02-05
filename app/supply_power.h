#ifndef _SUPPLY_POWER_H_INCLUDE_
#define _SUPPLY_POWER_H_INCLUDE_

#include <rtthread.h>
#include "Public.h"

void  power_gate_init(void);
void  open_powergate(void);
void  close_powergate(void); 
void  Status_change(void);
rt_err_t SupplyPower_init(void);
 


#endif
