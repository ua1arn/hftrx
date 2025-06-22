#ifndef __PORT_H
#define __PORT_H

void spp_service_setup(void);

int btstack_main(int argc, const char * argv[]);
int a2dp_sink_btstack_main(int argc, const char * argv[]);
int a2dp_source_btstack_main(int argc, const char * argv[]);
int hfp_hf_btstack_main(int argc, const char * argv[]);
int hfp_ag_btstack_main(int argc, const char * argv[]);
int hsp_hs_btstack_main(int argc, const char * argv[]);

#endif
