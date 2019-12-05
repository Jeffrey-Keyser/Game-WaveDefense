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

//*****************************************************************************
// TIMER2 Init
// Used from special power cooldown. Must be a one-shot timer
//*****************************************************************************
static void project_timer2_init(void)
{
	// Timer 2
	// (0x1 for one-shot), countdown, and generates interrupts , 5 Sec
	gp_timer_config_32(TIMER2_BASE, 0x1, false, true, 250000000);
	
	// Set interrupt priority to 4
	NVIC_SetPriority(TIMER2A_IRQn, 4);
	NVIC_EnableIRQ(TIMER2A_IRQn);  // Enable Timer A Handler in the NVIC
}


//*****************************************************************************
// TIMER3 Init
// Used to update the spawning of enemies, triggers every second
//*****************************************************************************
static void project_timer3_init(void)
{
	// Timer 3
	// (0x2 for periodic), countdown, and generates interrupts , 1 Sec
	gp_timer_config_32(TIMER3_BASE, 0x2, false, true, 50000000);
	
	// Set interrupt priority to 3
	NVIC_SetPriority(TIMER3A_IRQn, 3);
	NVIC_EnableIRQ(TIMER3A_IRQn);  // Enable Timer A Handler in the NVIC
		
}

//*****************************************************************************
// TIMER4 Init
// This function is used to update enemy/projectile positions
//*****************************************************************************
static void project_timer4_init(void)
{
	// Timer 4
	// (0x2 for periodic), countdown, and generates interrupts , 20mSec
	gp_timer_config_32(TIMER4_BASE, 0x2, false, true, 1000000);
	
	// Set interrupt priority to 0
	NVIC_SetPriority(TIMER4A_IRQn, 0);
	NVIC_EnableIRQ(TIMER4A_IRQn);  // Enable Timer A Handler in the NVIC
		
}


//*****************************************************************************
// TIMER5 Init
// This function is used for ps2 adc conversions. Signals a conversion every 
//*****************************************************************************
static void project_timer5_init(void)
{
	// Timer 5
	// (0x2 for periodic), countdown, and generates interrupts , 10mSec
	gp_timer_config_32(TIMER5_BASE, 0x2, false, true, 500000);
	
	// Set interrupt priority to 1
	NVIC_SetPriority(TIMER5A_IRQn, 1);
	NVIC_EnableIRQ(TIMER5A_IRQn);  // Enable Timer 5A Handler in the NVIC
}

//*****************************************************************************
// ADC0 Init
// This function is used to configure ADC0 based on the HW3 requirements.  
//
// DO NOT simply call the ADC0 initialization function from ICE-12.  That code
// configures the wrong sample sequencer and also does not use interrupts.
//*****************************************************************************
static void project_adc0_init(void)
{
	initialize_adc(PS2_ADC_BASE);	
	
	// Set interrupt priority to 2
	NVIC_SetPriority(ADC0SS1_IRQn, 2);
	NVIC_EnableIRQ(ADC0SS1_IRQn);  // Enable ADC0 SS1 Handler in the NVIC
}

//*****************************************************************************
// GPIO Init
// This function is used to configure all of the GPIO pins used for HW3.  You 
// should initialize the pins for the LCD, PS2 Joystick, and SW1.
//*****************************************************************************
static void project_gpio_init(void)
{	
	
	// PORT E - Setup X, Y PS2 controller 
	gpio_enable_port(PS2_GPIO_BASE);
	
	// DIR - Configure pins as input
	gpio_config_enable_input(PS2_GPIO_BASE, PS2_X_DIR_MASK);
	gpio_config_enable_input(PS2_GPIO_BASE, PS2_Y_DIR_MASK);

	// AMSEL - Configure as analog input
	gpio_config_analog_enable(PS2_GPIO_BASE, PS2_X_DIR_MASK);
	gpio_config_analog_enable(PS2_GPIO_BASE, PS2_Y_DIR_MASK);

	// AFSEL - Configure as alternate function
	gpio_config_alternate_function(PS2_GPIO_BASE, PS2_X_DIR_MASK);
	gpio_config_alternate_function(PS2_GPIO_BASE, PS2_Y_DIR_MASK);
	// PORT E - SETUP FINISHED
	
	
	// Seems to be configured correctly
	// PORT F - Setup SW1, IO_Expander
	gpio_enable_port(GPIOF_BASE);

	// SW1
	gpio_config_digital_enable(GPIOF_BASE, SW1_M);
	gpio_config_enable_input(GPIOF_BASE, SW1_M);
	gpio_config_enable_pullup(GPIOF_BASE, SW1_M);

	// IO_Expander
	gpio_config_digital_enable(GPIOF_BASE, PF0);
	gpio_config_enable_input(GPIOF_BASE, PF0);
	gpio_config_enable_pullup(GPIOF_BASE, PF0);
	gpio_config_falling_edge_irq(GPIOF_BASE, PF0);
	
	NVIC_SetPriority(GPIOF_IRQn, 0);
	NVIC_EnableIRQ(GPIOF_IRQn);
	
	// PORT F - SETUP FINISHED
	
	
}

//*****************************************************************************
// This function calls all of the routines used to configure the hardware
// for HW3.
//*****************************************************************************
void project_initialize_hardware(void)
{
	lcd_config_screen();

	init_serial_debug(true, true);
  lp_io_init();
  //lcd_config_gpio();
	
	//bluetooth init
	spi_select_init();
	eeprom_init();
	accel_initialize();
	//led
	io_expander_init();
	
	ps2_initialize();
	//lp_io_clear_pin(BLUE_M);
  project_gpio_init();
	project_adc0_init();
	project_timer3_init();
  project_timer4_init();
  project_timer5_init();
	eeprom_init();
	
}

