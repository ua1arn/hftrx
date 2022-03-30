const char* getPS7MessageInfo(unsigned key);
unsigned long ps7GetSiliconVersion (void);
void perf_disable_clock(void);
void perf_reset_and_start_timer(void);
int ps7_init(void);
int ps7_post_config(void);
int ps7_debug(void);
int get_number_of_cycles_for_delay(unsigned int delay);
