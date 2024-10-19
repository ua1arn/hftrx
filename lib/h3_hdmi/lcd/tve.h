#ifdef __cplusplus
extern "C" {
#endif

void tve_update_buffer(void);
void tve_init(int pal);
void tve_de2_init(void);
void tve_set_visible_buffer(volatile uint32_t *buf);

#define TVE_NORM_NTSC 0
#define TVE_NORM_PAL  1

#ifdef __cplusplus
}
#endif
