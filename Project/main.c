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

// Variables
bool start_once = true;

// Set to true first as the user can have special ability right away
int time_alive = 0;
uint8_t high_score;

// Flicker blue light
bool alternate_light = true;

extern bool timer_2;
extern bool timer_3;
extern bool timer_4;
extern bool timer_5;
extern bool sw1_pressed;
extern bool sw2_pressed;
extern bool ps2_input;
extern bool io_expander;
extern bool expander_btn_pressed;

// IO expander buttons
bool io_up = false;
bool io_down = false;
bool io_left = false;
bool io_right = false;

// EEprom stuff
uint16_t name_1;	
uint16_t name_2; 
uint16_t high_score_addr;
uint8_t eeprom_read_val;
char first_name[50] = " Jeffrey Keyser";
char second_name[50] = " ------------ ";

int expander_read_val;

// Used for starting screen
extern bool wait_center;

extern uint16_t ps2_x;
extern uint16_t ps2_y;
extern ps2_direction p2_dir;

// Projectiles
int max_proj = 5;
int proj_x_pos[5];
int proj_y_pos[5];
int proj_x_dir[5];
int proj_y_dir[5];
bool proj_in_wall[5] = {false};
bool proj_alive[5] = {false};

// Enemies
int max_enemies = 5;
int spawn_limit = 1;
bool enemies_alive[5] = {false};
int enemies_x_pos[5];
int enemies_y_pos[5];
int num_enemies_hit_wall = 0;

// User selection
// 0 | 1 | 2
// Corresponds to the tank, infantry, and fighter
int user_selection = 1;
int user_x_pos;
int user_y_pos;
int user_width;
int user_height;
int user_bitmaps;

// Special Abilities
bool special_on_cooldown = false;
bool fighter_special_ability = false;

int accel_x_val;
int accel_y_val;

extern uint16_t ps2_x;
extern uint16_t ps2_y;

int i;
int j;

void DisableInterrupts(void)
{
  __asm {
         CPSID  I
  }
}

//*****************************************************************************
//*****************************************************************************
void EnableInterrupts(void)
{
  __asm {
    CPSIE  I
  }
}



/*******************************************************************************
* Function Name: lcd_draw_rectangle
********************************************************************************
* Summary: Draws a rectangle centered at x_center, y_center.
* Returns:
*  Nothing
*******************************************************************************/
void lcd_draw_rectangle_centered
(
  uint16_t x_center, 
  uint16_t x_len, 
  uint16_t y_center, 
  uint16_t y_len,  
  uint16_t fg_color
)
{
  uint16_t x0;
  uint16_t x1;
  uint16_t y0;
  uint16_t y1;
  uint16_t i,j;
 
  x0 = x_center - (x_len/2);
  x1 = x_center + (x_len/2);
  if( (x_len & 0x01) == 0x00)
  {
    x1--;
  }
  
  y0 = y_center  - (y_len/2);
  y1 = y_center  + (y_len/2) ;
  if( (y_len & 0x01) == 0x00)
  {
    y1--;
  }
  
  lcd_set_pos(x0,x1, y0, y1);
  
  for (i=0;i< y_len ;i++)
  {
    for(j= 0; j < x_len; j++)
    {
      lcd_write_data_u16(fg_color);
    }
  }
}



void project_startup( void ){
	
	// Only init once
	if (start_once){
	 start_once = false;
	 project_initialize_hardware();
	}
	
	// Clear
	lcd_clear_screen(LCD_COLOR_BLACK);
	
	// Starting cursor
	lcd_draw_rectangle_centered(CENTER_X, 64, LOWER_Y + 40, 5, LCD_COLOR_RED);
	
	// Starting screen
	lcd_draw_image(CENTER_X, warWidthPixels, CENTER_Y, warHeightPixels, warBitmaps, LCD_COLOR_RED, LCD_COLOR_BLACK);
	lcd_draw_image(CENTER_X, selectWidthPixels, UPPER_Y - 20, selectHeightPixels, selectBitmaps, LCD_COLOR_WHITE, LCD_COLOR_BLACK);

	// User images
	lcd_draw_image(LEFT_X, tankWidthPixels, LOWER_Y, tankHeightPixels, tankBitmaps, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
  lcd_draw_image(CENTER_X, soldierWidthPixels, LOWER_Y, soldierHeightPixels, soldierBitmaps, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
	lcd_draw_image(RIGHT_X, fighterWidthPixels, LOWER_Y, fighterHeightPixels, fighterBitmaps, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
	
	
	// Loop until selection
	while(1){
		
		if (sw1_pressed || sw2_pressed){
			sw1_pressed = false;
			sw2_pressed = false;
			break;
		}
		
		if (expander_btn_pressed){
			expander_btn_pressed = false;
			break;
		}
		
		// Switch middle and left
		if (p2_dir == D_LEFT && user_selection == 1 && wait_center){
			wait_center = false;
			lcd_draw_rectangle_centered(CENTER_X, 64, LOWER_Y + 40, 5, LCD_COLOR_BLACK);
			lcd_draw_rectangle_centered(LEFT_X, 64, LOWER_Y + 40, 5, LCD_COLOR_RED);
			user_selection--;
		} // Middle and right
		else if (p2_dir == D_RIGHT && user_selection == 1 && wait_center){
			wait_center = false;
			lcd_draw_rectangle_centered(CENTER_X, 64, LOWER_Y + 40, 5, LCD_COLOR_BLACK);
			lcd_draw_rectangle_centered(RIGHT_X, 64, LOWER_Y + 40, 5, LCD_COLOR_RED);
			user_selection++;
		} // Right and middle
		else if (p2_dir == D_LEFT && user_selection == 2 && wait_center){
			wait_center = false;			
			lcd_draw_rectangle_centered(RIGHT_X, 64, LOWER_Y + 40, 5, LCD_COLOR_BLACK);
			lcd_draw_rectangle_centered(CENTER_X, 64, LOWER_Y + 40, 5, LCD_COLOR_RED);
			user_selection--;
		} // Left and middle
		else if (p2_dir == D_RIGHT && user_selection == 0 && wait_center){
			wait_center = false;
			lcd_draw_rectangle_centered(LEFT_X, 64, LOWER_Y + 40, 5, LCD_COLOR_BLACK);
			lcd_draw_rectangle_centered(CENTER_X, 64, LOWER_Y + 40, 5, LCD_COLOR_RED);
			user_selection++;
		}
	}

	
}



void update_background( void ){
	
	int wall_color;
	if (num_enemies_hit_wall > 5)
		wall_color = LCD_COLOR_RED;
	else if (num_enemies_hit_wall > 2)
		wall_color = LCD_COLOR_YELLOW;
	else
		wall_color = LCD_COLOR_GREEN_LIGHT;
	
	// Special case where the fighter's special ability is working
	if (fighter_special_ability)
		wall_color = LCD_COLOR_BLUE;
	
	lcd_draw_image(CENTER_X, wallWidthPixels, LOWER_HORIZONTAL_LINE_Y + 10, wallHeightPixels, wallBitmaps, wall_color, LCD_COLOR_BLACK);
	lcd_draw_image(CENTER_X + wallWidthPixels, wallWidthPixels, LOWER_HORIZONTAL_LINE_Y + 10, wallHeightPixels, wallBitmaps, wall_color, LCD_COLOR_BLACK);
	lcd_draw_image(CENTER_X + (2 * wallWidthPixels), wallWidthPixels, LOWER_HORIZONTAL_LINE_Y + 10, wallHeightPixels, wallBitmaps, wall_color, LCD_COLOR_BLACK);
	lcd_draw_image(CENTER_X - wallWidthPixels, wallWidthPixels, LOWER_HORIZONTAL_LINE_Y + 10, wallHeightPixels, wallBitmaps, wall_color, LCD_COLOR_BLACK);
	lcd_draw_image(CENTER_X - (2 * wallWidthPixels), wallWidthPixels, LOWER_HORIZONTAL_LINE_Y + 10, wallHeightPixels, wallBitmaps, wall_color, LCD_COLOR_BLACK);
	
}

void io_btn_pressed( void ){
	if ((expander_read_val & (1 << DIR_BTN_UP_PIN)) == 0) 
		io_up = true;
	else
		io_up = false;
	if ((expander_read_val & (1 << DIR_BTN_DOWN_PIN)) == 0) 
		io_down = true;
	else
		io_down = false;
	if ((expander_read_val & (1 << DIR_BTN_LEFT_PIN)) == 0) 
		io_left = true;
	else
		io_left = false;
	if ((expander_read_val & (1 << DIR_BTN_RIGHT_PIN)) == 0)
		io_right = true;
	else
		io_right = false;
	
}
void write_eeprom( void ){
	uint8_t reset = 0;	//reset high score value
			for(name_1 = 0; name_1 < (0+4); name_1++){
					eeprom_byte_write(I2C1_BASE, name_1, first_name[name_1]);
				  printf("Writing %i\n\r",first_name[name_1]);
			}
			for(name_2 = 0 + 4; name_2 < (0 + (2* 4)); name_2++){
				eeprom_byte_write(I2C1_BASE, name_2, first_name[name_2]);
				printf("Writing %i\n\r",second_name[name_2]);
			}
			high_score_addr = 0 + (3*4);
			eeprom_byte_write(I2C1_BASE, high_score_addr, 0);
}
void read_eeprom( void ){
	
		 for(name_1 = 0; name_1 <(0 + 4); name_1++){
      eeprom_byte_read(I2C1_BASE, name_1, &eeprom_read_val);
			printf("%c", (char)eeprom_read_val);
		 	 if( eeprom_read_val != first_name[name_1]){
        printf("ERROR: addr: %i write: %i read %i\n\r",name_1, first_name[name_1], eeprom_read_val);
      }
		}
	 printf("\n");
	 for(name_2 = 0 + 4 ; name_2 <(0+(2 * 4)); name_2++){
      eeprom_byte_read(I2C1_BASE, name_2, &eeprom_read_val);
		  printf("%c", (char)eeprom_read_val);
		 	 if( eeprom_read_val != first_name[name_2]){
        printf("ERROR: addr: %i write: %i read %i\n\r",name_1, first_name[name_2], eeprom_read_val);
      }
		}
	 printf("\n");
		
		printf("\n");
		high_score_addr = 0 + (3 * 4);
		eeprom_byte_read(I2C1_BASE, high_score_addr, &high_score);
		printf("High Score: %d\n", high_score);
	
}
void end_game( void ){
	
	// Game over
	lcd_draw_image(CENTER_X, game_overWidthPixels, CENTER_Y, game_overHeightPixels, game_overBitmaps, LCD_COLOR_GREEN, LCD_COLOR_BLACK);
	
	// TODO: Diplay high score

	// Wait for user to start another game
	while (1){
		
		// Flicker blue light
		if(timer_3){
			timer_3 = false;
			if(alternate_light){
				alternate_light = false;
				lp_io_set_pin(BLUE_BIT);
			}
			else{
				alternate_light = true;
				lp_io_clear_pin(BLUE_BIT);
			}
		}
		
		if (expander_btn_pressed){
			expander_btn_pressed = false;
			expander_read_val = io_expander_read_reg(MCP23017_GPIOB_R);
			io_btn_pressed();
			// Begin another game
			if (io_down){
				io_down = false;
				break;
			}
		}
	}
	
	// Assume if entering this point, another game started
	
	// Reinit everything
	num_enemies_hit_wall = 0;
	
	project_startup();
	lcd_clear_screen(LCD_COLOR_BLACK);
	
	// Depending on selection draw correct thing
	switch(user_selection){
		case 0: lcd_draw_image(CENTER_X, user_tankWidthPixels, LOWER_Y + 30, user_tankHeightPixels, user_tankBitmaps, LCD_COLOR_WHITE, LCD_COLOR_BLACK); 
		user_width = user_tankWidthPixels; user_height = user_tankHeightPixels; break; 
		case 1: lcd_draw_image(CENTER_X, soldierWidthPixels, LOWER_Y + 30, soldierHeightPixels, soldierBitmaps, LCD_COLOR_WHITE, LCD_COLOR_BLACK); break;
		user_width = soldierWidthPixels; user_height = soldierHeightPixels; break; 
		case 2: lcd_draw_image(CENTER_X, user_fighterWidthPixels, LOWER_Y + 30, user_fighterHeightPixels, user_fighterBitmaps, LCD_COLOR_WHITE, LCD_COLOR_BLACK); break;
		user_width = user_fighterWidthPixels; user_height = user_fighterHeightPixels; break; 	
	}
	
	// Default user position
	user_x_pos = CENTER_X;
	user_y_pos = LOWER_Y + 20;
	
	// Write RED leds. Full health
	io_expander_write_reg(MCP23017_OLATA_R, 0xFF);

	// eeprom read
	read_eeprom();
	
	// Kill and erase everything before starting again
	for (i = 0; i < max_enemies; i++){
		if (enemies_alive[i]){
			enemies_alive[i] = false;
			lcd_draw_image(enemies_x_pos[i], enemy_32_32WidthPixels, enemies_y_pos[i], enemy_32_32HeightPixels, enemy_32_32Bitmaps, LCD_COLOR_BLACK, LCD_COLOR_BLACK);
		}
	}
	for (i = 0; i < max_proj; i++){
		if (proj_alive[i]){
			proj_alive[i] = false;
			lcd_draw_image(proj_x_pos[i], projectileWidthPixels, proj_y_pos[i], projectileHeightPixels, projectileBitmaps, LCD_COLOR_BLACK, LCD_COLOR_BLACK); 
		}
	}
}


void special_ability( void ){
	
	if (!special_on_cooldown){
	// Each character has a special ability
		switch(user_selection){
		// Tank
		case 0: 
		break;
		// Soldier
		case 1:
		break;
		// Fight - Wall Defense
		// Makes wall invincible for a few seconds
		case 2:
		// Have to press the wall
		if ( (ft6x06_read_x() >= CENTER_X - (2 * wallWidthPixels)) && (ft6x06_read_x() <= CENTER_X + (2 * wallWidthPixels)) &&
		(ft6x06_read_y() <= LOWER_HORIZONTAL_LINE_Y + 10 + (wallHeightPixels / 2)) && (ft6x06_read_y() >= LOWER_HORIZONTAL_LINE_Y + 10 - (wallHeightPixels / 2)) ){
			fighter_special_ability = true;
			// Setup cooldown timer
			special_on_cooldown = true;
		//	project_timer2_init();
			lp_io_clear_pin( RED_M | GREEN_M | BLUE_M );
			lp_io_set_pin(RED_M);
		}
			
		break;
	}
}
	
	
	
}

void update_projectile_pos( void ){
	
	for (i = 0; i < max_proj; i++){
		if (proj_alive[i]){
			
			// Erase
			if (!proj_in_wall[i])
				lcd_draw_image(proj_x_pos[i], projectileWidthPixels, proj_y_pos[i], projectileHeightPixels, projectileBitmaps, LCD_COLOR_BLACK, LCD_COLOR_BLACK); 
			
			// update position / bounds checking.
			// Kill if out of bounds
			proj_x_pos[i] += proj_x_dir[i];
				if(proj_x_pos[i] > 239 - projectileWidthPixels/2) {
					// Just kill
					proj_alive[i] = false;
				}
				else if(proj_x_pos[i] < projectileWidthPixels/2){
					proj_alive[i] = false;
				}
			
				proj_y_pos[i] += proj_y_dir[i];
				if(proj_y_pos[i] > 319 - projectileWidthPixels/2) {
					proj_alive[i] = false;
				}
				else if(proj_y_pos[i] < projectileWidthPixels/2){
					proj_alive[i] = false;
				}
			
			// Don't want to draw while it is in WALL area, so don't draw if it within the bounds
			if ( (proj_y_pos[i] < LOWER_HORIZONTAL_LINE_Y + 10 + (wallHeightPixels / 2)) && (proj_y_pos[i] > LOWER_HORIZONTAL_LINE_Y + 10 - (wallHeightPixels / 2)))
				proj_in_wall[i] = true;
			else
				proj_in_wall[i] = false;
			
			// If still alive and not in wall, draw
			if (proj_alive[i] && !proj_in_wall[i])
				lcd_draw_image(proj_x_pos[i], projectileWidthPixels, proj_y_pos[i], projectileHeightPixels, projectileBitmaps, LCD_COLOR_BLACK, LCD_COLOR_ORANGE); 
				
		}
	}
	
}

void create_projectile( void ){
	for (i = 0; i < max_proj; i++){
		if (proj_alive[i] == false){
			proj_alive[i] = true;
			
			proj_x_pos[i] = user_x_pos;
			proj_y_pos[i] = user_y_pos;
			
			//move if 0-7
			proj_x_dir[i] = (ps2_x >> 9);
			proj_y_dir[i] = (ps2_y >> 9);
			
			//0-6
			if( proj_x_dir[i] == 7) proj_x_dir[i]--;
			if( proj_y_dir[i] == 0) proj_y_dir[i]++;
			
			//-3 - 3
			proj_x_dir[i] -= 3;
			proj_y_dir[i] -= 4;
			
			//x original is flipped
			proj_x_dir[i] = -1 * proj_x_dir[i];
			proj_y_dir[i] =  -1 * proj_y_dir[i];
			
			if(proj_x_dir[i] < 0 ) proj_x_pos[i] -= 19;
			if(proj_x_dir[i] > 0) proj_x_pos[i] += 19;

			if(proj_y_dir[i] > 0) proj_y_pos[i] += 28;
			if(proj_y_dir[i] < 0) proj_y_pos[i] -= 28;
			
			// Draw
			lcd_draw_image(proj_x_pos[i], projectileWidthPixels, proj_y_pos[i], projectileHeightPixels, projectileBitmaps, LCD_COLOR_BLACK, LCD_COLOR_ORANGE); 
			break;
		}
	}
	
}

void check_collisions( void ){
	
	for (i = 0; i < max_enemies; i++){
		if (enemies_alive[i]){
			// HITS WALL
			if (LOWER_HORIZONTAL_LINE_Y + 10 - enemies_y_pos[i] < enemy_32_32HeightPixels){
				// Delete
				lcd_draw_image(enemies_x_pos[i], enemy_32_32WidthPixels, enemies_y_pos[i], enemy_32_32HeightPixels, enemy_32_32Bitmaps, LCD_COLOR_BLACK, LCD_COLOR_BLACK);
				enemies_alive[i] = false;
				
				// Update health
				io_expander_write_reg(MCP23017_OLATA_R, 0xFF >> ++num_enemies_hit_wall);
			}
			// CHECK PROJECTILE HITS
			for (j = 0; j < max_proj; j++){
				if (proj_alive[j]){
					
					// HIT
					if (sqrt( ((proj_x_pos[j] - enemies_x_pos[i]) * (proj_x_pos[j] - enemies_x_pos[i])) + ((proj_y_pos[j] - enemies_y_pos[i]) * (proj_y_pos[j] - enemies_y_pos[i])) ) < enemy_32_32WidthPixels - 10){
					// Kill proj
					proj_alive[j] = false;
					lcd_draw_image(proj_x_pos[j], projectileWidthPixels, proj_y_pos[j], projectileHeightPixels, projectileBitmaps, LCD_COLOR_BLACK, LCD_COLOR_BLACK); 
					
					// Kill enemy
					enemies_alive[i] = false;
					lcd_draw_image(enemies_x_pos[i], enemy_32_32WidthPixels, enemies_y_pos[i], enemy_32_32HeightPixels, enemy_32_32Bitmaps, LCD_COLOR_BLACK, LCD_COLOR_BLACK);
					}
					
				}
			}
		}
	}
	
}

void update_enemies_pos( void ){
	
	for (i = 0; i < max_enemies; i++){
		if (enemies_alive[i]){
			
			// First delete
			lcd_draw_image(enemies_x_pos[i], enemy_32_32WidthPixels, enemies_y_pos[i], enemy_32_32HeightPixels, enemy_32_32Bitmaps, LCD_COLOR_BLACK, LCD_COLOR_BLACK);
			
			// Update x position
			enemies_y_pos[i] = enemies_y_pos[i] + 1;
			
			// Redraw
			lcd_draw_image(enemies_x_pos[i], enemy_32_32WidthPixels, enemies_y_pos[i], enemy_32_32HeightPixels, enemy_32_32Bitmaps, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
			
		}
	}
	
}

void create_enemy( int enemy_num ){
	srand(timer_value(TIMER3_BASE, true)); // Make actually random
	
	// TODO: Create multiple that don't overlap
	enemies_x_pos[enemy_num] = (rand() % (190 - 20 + 1)) + 20;
	enemies_y_pos[enemy_num] = 40;

	lcd_draw_image(enemies_x_pos[enemy_num], enemy_32_32WidthPixels, enemies_y_pos[enemy_num], enemy_32_32HeightPixels, enemy_32_32Bitmaps, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
}

void spawn_enemies(){
	int spawn_counter = 0;
	for (i = 0; i < max_enemies; i++){
		if (!enemies_alive[i]){
			create_enemy(i);
			enemies_alive[i] = true;
			spawn_counter++;
		}
		// Stop spawning
		if (spawn_counter == spawn_limit)
			break;
	}
	
}

void update_user_pos( void ){

	//read the accel and scale it
	accel_x_val = accel_read_x();
	accel_y_val = accel_read_y();
	if(accel_y_val > 6000){
		accel_y_val = 6000;
	}else if(accel_y_val < -6000){
	accel_y_val = -6000;
	}
	if(accel_x_val > 6000){
		accel_x_val = 6000;
	}else if(accel_x_val < -6000){
		accel_x_val = -6000;
	}
	accel_x_val = -(accel_x_val / 1024);	//accel_x_val >> 6; with bit shift lose sign and make negative so goes correct way
	accel_y_val = (accel_y_val /1024);
			
	// Erase first
	// TODO: FIX
	if (accel_x_val != 0 || accel_y_val != 0)
		lcd_draw_rectangle_centered(user_x_pos, 64, user_y_pos, 75, LCD_COLOR_BLACK);
	
	//update the movement
	user_x_pos = user_x_pos + accel_x_val;
	user_y_pos = user_y_pos + accel_y_val;
	
	//current position boundary detection
	if(user_x_pos >= 239 - 32) user_x_pos = 239 - 32;
	else if(user_x_pos <= 32) user_x_pos = 32;
	if(user_y_pos >= 319 - 32)user_y_pos = 319 - 32;
	else if(user_y_pos <= 32 + LOWER_HORIZONTAL_LINE_Y + 30) user_y_pos = 32 + LOWER_HORIZONTAL_LINE_Y + 30;

	// Now draw
		switch(user_selection){
		case 0: lcd_draw_image(user_x_pos, user_tankWidthPixels, user_y_pos, user_tankHeightPixels, user_tankBitmaps, LCD_COLOR_WHITE, LCD_COLOR_BLACK); 
		break;
		case 1: lcd_draw_image(user_x_pos, soldierWidthPixels, user_y_pos, soldierHeightPixels, soldierBitmaps, LCD_COLOR_WHITE, LCD_COLOR_BLACK); break;
		break; 
		case 2: lcd_draw_image(user_x_pos, user_fighterWidthPixels, user_y_pos, user_fighterHeightPixels, user_fighterBitmaps, LCD_COLOR_WHITE, LCD_COLOR_BLACK); break;
		break; 	
	}
}

//*****************************************************************************
//*****************************************************************************
int 
main(void)
{
	project_startup();
	lcd_clear_screen(LCD_COLOR_BLACK);
	
	// Depending on selection draw correct thing
	switch(user_selection){
		case 0: lcd_draw_image(CENTER_X, user_tankWidthPixels, LOWER_Y + 30, user_tankHeightPixels, user_tankBitmaps, LCD_COLOR_WHITE, LCD_COLOR_BLACK); 
		user_width = user_tankWidthPixels; user_height = user_tankHeightPixels; break; 
		case 1: lcd_draw_image(CENTER_X, soldierWidthPixels, LOWER_Y + 30, soldierHeightPixels, soldierBitmaps, LCD_COLOR_WHITE, LCD_COLOR_BLACK); break;
		user_width = soldierWidthPixels; user_height = soldierHeightPixels; break; 
		case 2: lcd_draw_image(CENTER_X, user_fighterWidthPixels, LOWER_Y + 30, user_fighterHeightPixels, user_fighterBitmaps, LCD_COLOR_WHITE, LCD_COLOR_BLACK); break;
		user_width = user_fighterWidthPixels; user_height = user_fighterHeightPixels; break; 	
	}
	
	// Default user position
	user_x_pos = CENTER_X;
	user_y_pos = LOWER_Y + 20;
	
	// Write RED leds. Full health
	io_expander_write_reg(MCP23017_OLATA_R, 0xFF);

	// Special power isn't on cooldown, write a green light
	//lp_io_set_pin(GREEN_BIT);

	// eeprom read
	read_eeprom();
	
	while (1)
	{
		
		// Game end
		if (num_enemies_hit_wall >= 8){
				if(time_alive > high_score){
					printf("NEW HIGH SCORE: %d\n " , time_alive);
					high_score = time_alive;
					eeprom_byte_write(I2C1_BASE, high_score_addr, time_alive);
				}
				end_game();
		}
		
		update_background();
		
		update_user_pos();
		
		// Special ability cooldown finished
		if (timer_2){
			timer_2 = false;
			special_on_cooldown = false;
			lp_io_set_pin(GREEN_M);
		}
		
		if (timer_3){
			timer_3 = false;
			time_alive++;
			spawn_enemies();
		}
		
		// Create projectiles
		if (expander_btn_pressed){
			expander_btn_pressed = false;
			expander_read_val = io_expander_read_reg(MCP23017_GPIOB_R);
			io_btn_pressed();
		}
		
		// Only create a projectile on a down button press
		if (io_up){
			io_up = false;
			create_projectile();
		}

		
		// Update projectile/enemy position
		if (timer_4){
			timer_4 = false;
			update_enemies_pos();
			update_projectile_pos();
		}
		
		// Check collisions
		check_collisions();
		
		if (sw1_pressed){
			sw1_pressed = false;
			write_eeprom();
		}
		
		// Touch screen detection, special ability
		if(ft6x06_read_td_status()  == 1 ||  ft6x06_read_td_status()  == 2 || ft6x06_read_td_status()  == 3)
			special_ability();
		
	}
	
}
