/*
 * Copyright 2023 NXP
 * SPDX-License-Identifier: MIT
 * The auto-generated can only be used on NXP devices
 */

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "custom.h"

#include "lv_analogclock.h"



static int blueCounter_analog_clock_1_hour_value = 4;
static int blueCounter_analog_clock_1_min_value = 55;
static int blueCounter_analog_clock_1_sec_value = 50;

void blueCounter_analog_clock_1_timer(lv_timer_t *timer)
{	clock_count(&blueCounter_analog_clock_1_hour_value, &blueCounter_analog_clock_1_min_value, &blueCounter_analog_clock_1_sec_value);
	if (lv_obj_is_valid(guider_ui.blueCounter_analog_clock_1))
	{
		lv_analogclock_set_time(guider_ui.blueCounter_analog_clock_1, blueCounter_analog_clock_1_hour_value, blueCounter_analog_clock_1_min_value, blueCounter_analog_clock_1_sec_value);
	}
}

void setup_scr_blueCounter(lv_ui *ui){

	//Write codes blueCounter
	ui->blueCounter = lv_obj_create(NULL);
	lv_obj_set_scrollbar_mode(ui->blueCounter, LV_SCROLLBAR_MODE_OFF);

	//Set style for blueCounter. Part: LV_PART_MAIN, State: LV_STATE_DEFAULT
	lv_obj_set_style_bg_color(ui->blueCounter, lv_color_make(0x59, 0x9c, 0xc7), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_color(ui->blueCounter, lv_color_make(0x8d, 0xcd, 0xeb), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->blueCounter, LV_GRAD_DIR_VER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->blueCounter, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	static bool blueCounter_analog_clock_1_timer_enabled = false;

	//Write codes blueCounter_analog_clock_1
	ui->blueCounter_analog_clock_1 = lv_analogclock_create(ui->blueCounter);
	lv_obj_set_pos(ui->blueCounter_analog_clock_1, 0, 0);
	lv_obj_set_size(ui->blueCounter_analog_clock_1, 240, 240);
	lv_analogclock_hide_digits(ui->blueCounter_analog_clock_1, false);
	lv_analogclock_set_major_ticks(ui->blueCounter_analog_clock_1, 2, 10, lv_color_make(0x55, 0x55, 0x55), 10);
	lv_analogclock_set_ticks(ui->blueCounter_analog_clock_1, 2, 5, lv_color_make(0x33, 0x33, 0x33));
	lv_analogclock_set_hour_needle_line(ui->blueCounter_analog_clock_1, 2, lv_color_make(0x00, 0xff, 0x00), -40);
	lv_analogclock_set_min_needle_line(ui->blueCounter_analog_clock_1, 2, lv_color_make(0xE1, 0xFF, 0x00), -30);
	lv_analogclock_set_sec_needle_line(ui->blueCounter_analog_clock_1, 2, lv_color_make(0x66, 0x00, 0xFF), -10);
	lv_analogclock_set_time(ui->blueCounter_analog_clock_1, blueCounter_analog_clock_1_hour_value, blueCounter_analog_clock_1_min_value, blueCounter_analog_clock_1_sec_value);

	//create timer
	if (!blueCounter_analog_clock_1_timer_enabled) {
		lv_timer_create(blueCounter_analog_clock_1_timer, 1000, NULL);
		blueCounter_analog_clock_1_timer_enabled = true;
	}
	//Set style for blueCounter_analog_clock_1. Part: LV_PART_MAIN, State: LV_STATE_DEFAULT
	lv_obj_set_style_bg_color(ui->blueCounter_analog_clock_1, lv_color_make(0xff, 0xff, 0xff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_color(ui->blueCounter_analog_clock_1, lv_color_make(0xff, 0xff, 0xff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->blueCounter_analog_clock_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->blueCounter_analog_clock_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->blueCounter_analog_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_color(ui->blueCounter_analog_clock_1, lv_color_make(0x21, 0x95, 0xf6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_opa(ui->blueCounter_analog_clock_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_spread(ui->blueCounter_analog_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_ofs_x(ui->blueCounter_analog_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_ofs_y(ui->blueCounter_analog_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->blueCounter_analog_clock_1, lv_color_make(0x21, 0x95, 0xf6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->blueCounter_analog_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->blueCounter_analog_clock_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Set style for blueCounter_analog_clock_1. Part: LV_PART_TICKS, State: LV_STATE_DEFAULT
	lv_obj_set_style_text_color(ui->blueCounter_analog_clock_1, lv_color_make(0xff, 0x00, 0x00), LV_PART_TICKS|LV_STATE_DEFAULT);
	//lv_obj_set_style_text_font(ui->blueCounter_analog_clock_1, &lv_font_montserratMedium_12, LV_PART_TICKS|LV_STATE_DEFAULT);
}