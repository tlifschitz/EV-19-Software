
#ifndef SCOPE_H_
#define SCOPE_H_
#include <stdint.h>

#define BACKGROUND_COLOR 	RGB16(0,0,0)
#define SIGNAL_COLOR		RGB16(250,237,39)
#define GRID_COLOR			RGB16(100,100,100)

typedef enum{RISING,FALLING,EITHER} TriggerType;

void ScopeMain();

void drawGrid();
void setSampleFreq();
void setTrigger(uint32_t triggerLevel);
void setTriggerCondition();

#endif /* SCOPE_H_ */
