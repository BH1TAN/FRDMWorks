/*
 * MusicMod.h
 *
 *  Created on: Feb 5, 2015
 *      Author: CTY
 */

#ifndef MUSICMOD_H_
#define MUSICMOD_H_



#endif /* MUSICMOD_H_ */


#define PWM0_CLK_FREQ 50000*3.28

#define B1 L1*2
#define B2 L2*2
#define B3 L3*2
#define B4 L4*2
#define B5 L5*2
#define B6 L6*2
#define B7 L7*2

#define L1 M1*2
#define L2 M2*2
#define L3 M3*2
#define L4 M4*2
#define L5 M5*2
#define L6 M6*2
#define L7 M7*2

#define M1 (unsigned short)(PWM0_CLK_FREQ/523) 
#define M2 (unsigned short)(PWM0_CLK_FREQ/587) 
#define M3 (unsigned short)(PWM0_CLK_FREQ/659) 
#define M4 (unsigned short)(PWM0_CLK_FREQ/698) 
#define M5 (unsigned short)(PWM0_CLK_FREQ/784) 
#define M6 (unsigned short)(PWM0_CLK_FREQ/880) 
#define M7 (unsigned short)(PWM0_CLK_FREQ/988) 

#define H1 M1/2
#define H2 M2/2
#define H3 M3/2
#define H4 M4/2
#define H5 M5/2
#define H6 M6/2
#define H7 M7/2

#define T1 H1/2
#define T2 H2/2
#define T3 H3/2
#define T4 H4/2
#define T5 H5/2
#define T6 H6/2
#define T7 H7/2

#define NN 0
