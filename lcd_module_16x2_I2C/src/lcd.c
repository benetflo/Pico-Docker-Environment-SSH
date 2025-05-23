#include "hardware/i2c.h"
#include "lcd.h"


static int i2c_addr = 39;

void i2c_write_byte(uint8_t value){
	i2c_write_blocking(i2c0, i2c_addr, &value, 1, false);
}


// Reads and processes data or command
void lcd_enable_pulse_toggle(uint8_t value){
	#define DELAY_US 600
	sleep_us(DELAY_US); // wait to ensure LCD is ready
	i2c_write_byte(value | LCD_ENABLE_BIT); // set LCD_ENABLE_BIT high
	sleep_us(DELAY_US); // wait to allow LCD to detect pulse
	i2c_write_byte(value & ~LCD_ENABLE_BIT); // set LCD_ENABLE_BIT low
	sleep_us(DELAY_US); // wait for LCD to process before sending next command
}

void lcd_send_byte(uint8_t value, int mode){
	uint8_t high = mode | (value & 0xF0) | LCD_BACKLIGHT;
	uint8_t low = mode | ((value << 4) & 0xF0) | LCD_BACKLIGHT;

	i2c_write_byte(high);
	lcd_enable_pulse_toggle(high);
	i2c_write_byte(low);
	lcd_enable_pulse_toggle(low);
}

void lcd_clear(void){
	lcd_send_command(LCD_CLEAR_SCREEN);
	sleep_ms(2);
}

void lcd_char(char value){
	lcd_send_byte(value, LCD_CHARACTER);
}

void lcd_set_cursor(uint8_t row, uint8_t col){
	uint8_t address = (row == 0) ? 0x00 + col : 0x40 + col;
	lcd_send_command(0x80 | address);
}

void lcd_send_command(uint8_t value){
	lcd_send_byte(value, LCD_COMMAND);
}

void lcd_init(void) {
    sleep_ms(50);
    lcd_send_command(0x03);
    sleep_ms(5);
    lcd_send_command(0x03);
    sleep_us(150);
    lcd_send_command(0x03);
    sleep_ms(1);
    lcd_send_command(0x02);

    lcd_send_command(LCD_FUNCTIONSET | LCD_2LINE);

    lcd_send_command(LCD_DISPLAYCONTROL | LCD_DISPLAYON);

    lcd_send_command(LCD_ENTRYMODESET | LCD_ENTRYLEFT);

    lcd_clear();
}
