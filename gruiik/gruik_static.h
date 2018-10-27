gruik_obj gruik_obj::dummy;
gruik_obj gruik_obj::gruik_top;
gruik_obj* gruik_obj::focus_obj = NULL;
gruik_obj* gruik_obj::locking_obj = NULL;
uint8_t gruik_obj::active_layer = 0;
lcd_drv* gruik_obj::lcd = NULL;
style_t gruik_obj::style;
