// Copyright (c) 2015-19, Joe Krachey
// All rights reserved.
//
// Redistribution and use in source or binary form, with or without modification, 
// are permitted provided that the following conditions are met:
//
// 1. Redistributions in source form must reproduce the above copyright 
//    notice, this list of conditions and the following disclaimer in 
//    the documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "main.h"
#include "project_interrupts.h"

// Set to true first as the user can have special ability right away
volatile bool timer_2 = false;
volatile bool timer_3 = false;
volatile bool timer_4 = false;
volatile bool timer_5 = false;
volatile bool sw1_pressed = false;
volatile bool sw2_pressed = false;
volatile bool ps2_input = false;
volatile bool io_expander = false;
volatile bool expander_btn_pressed = false;

// Used for starting screen
volatile bool wait_center = false;

volatile uint16_t expander_val;

bool alternate_read = true;

int sw1_counter = 0;
int sw2_counter = 0;
int sw1_data;
uint16_t the_data;
uint16_t ps2_x;
uint16_t ps2_y;

ADC0_Type  *myADC;

ps2_direction p2_dir;


// Push buttons handler
void GPIOF_Handler( void ) {
	// Counter used for debouncing
	static int count = 0;
	
	io_expander = true;
	GPIOF->ICR |= GPIO_ICR_GPIO_M;	
	
	// Debounce signal
	count = (count + 1) % 2;
		expander_val = io_expander_read_reg(MCP23017_GPIOB_R);
	if(expander_val != 255){
		expander_btn_pressed = true;
	}
	
}

//*****************************************************************************
// TIMER2 ISR
//*****************************************************************************
void TIMER2A_Handler(void){
	
	// Make Timer 2 of type timer.
	TIMER0_Type *gp_timer;
	gp_timer = (TIMER0_Type *) TIMER3_BASE;
	
	timer_2 = true;
	
	// NEW, clear interrupt register for timer A
	 gp_timer->ICR |= TIMER_ICR_TATOCINT;	
	
}

//*****************************************************************************
// TIMER3 ISR
//*****************************************************************************
void TIMER3A_Handler(void){
	
	// Make Timer 3 of type timer.
	TIMER0_Type *gp_timer;
	gp_timer = (TIMER0_Type *) TIMER3_BASE;
	
	timer_3 = true;
	
	// NEW, clear interrupt register for timer A
	 gp_timer->ICR |= TIMER_ICR_TATOCINT;	
	
}

//*****************************************************************************
// TIMER4 ISR
//*****************************************************************************
void TIMER4A_Handler(void)
{	
	// Make Timer 4 of type timer.
	TIMER0_Type *gp_timer;
	gp_timer = (TIMER0_Type *) TIMER4_BASE;
	
	sw1_data = GPIOF->DATA;	
	
	if ((sw1_data & 0x10) == 0) // SW1 is pressed
		sw1_counter++;
	else
		sw1_counter = 0;
	
	if ((sw1_data & 0x1) == 0) // SW2 is pressed
		sw2_counter++;
	else
		sw2_counter = 0;
	
	if (sw1_counter == 8) // 7 interrupts..
		sw1_pressed = true;
	
	if (sw2_counter == 8)
		sw2_pressed = true;
	
	timer_4 = true;
	
	// NEW, clear interrupt register for timer A
	 gp_timer->ICR |= TIMER_ICR_TATOCINT;	
}

//*****************************************************************************
// TIMER5 ISR
//*****************************************************************************
void TIMER5A_Handler(void)
{
	// Make Timer 5 of type timer.
	TIMER0_Type *gp_timer;
	gp_timer = (TIMER0_Type *) TIMER5_BASE;
	
	myADC = (ADC0_Type *) ADC0_BASE;
	
	timer_5 = true;
	
//	if ( (myADC->SSFSTAT1 & ADC_SSFSTAT1_FULL) == 0)
		// Trigger a processor event
		myADC->PSSI = ADC_PSSI_SS1;
	
	// Clear interrupt register for timer A
	 gp_timer->ICR |= TIMER_ICR_TATOCINT;	
}

//*****************************************************************************
// ADC0 SS1 ISR
//*****************************************************************************
void ADC0SS1_Handler(void)
{
  ADC0_Type  *myADC = (ADC0_Type *) ADC0_BASE;

	ps2_input = true;
	
	if (alternate_read)
		myADC->SSMUX1 = PS2_X_ADC_CHANNEL;          // Set the Channel
	else
		myADC->SSMUX1 = PS2_Y_ADC_CHANNEL;          // Set the Channel

	the_data = myADC->SSFIFO1 & 0xFFF;

	
	// Should now read up or down
	if (alternate_read){
		alternate_read = false;
		ps2_x = the_data;
	if (the_data <= 0x2E8)
			p2_dir = D_RIGHT;
	else if (the_data >= 0xD93)
		p2_dir = D_LEFT;
	else{
		p2_dir = D_CENTER;
		wait_center = true;
	}
	}
	else{
		alternate_read = true;
		ps2_y = the_data;
	if (the_data <= 0x2E8)
		p2_dir = D_DOWN;
	else if (the_data >= 0xD93)
		p2_dir = D_UP;
	else
		p2_dir = D_CENTER;
	
	}
	
		// Clear interrupt by writing a 1 to SS1
	 myADC->ISC |= ADC_ISC_IN1;
	
	}




