
@set ccubus2=0x01C20068

@set pa_cfg0=0x01C20800
@set pa_cfg1=0x01C20804
@set pa_cfg2=0x01C20808
@set pa_cfg3=0x01C2080C
@set pa_data=0x01C20810
@set pa_drv0=0x01C20814
@set pa_drv1=0x01C20818
@set pa_pull0=0x01C2081C
@set pa_pull1=0x01C20820

@set pb_cfg0=0x01C20824
@set pb_cfg1=0x01C20828
@set pb_cfg2=0x01C2082C
@set pb_cfg3=0x01C20830
@set pb_data=0x01C20834
@set pb_drv0=0x01C20838
@set pb_drv1=0x01C2083C
@set pb_pull0=0x01C20840
@set pb_pull1=0x01C20844

@set pc_cfg0=0x01C20848
@set pc_cfg1=0x01C2084C
@set pc_cfg2=0x01C20850
@set pc_cfg3=0x01C20854
@set pc_data=0x01C20858
@set pc_drv0=0x01C2085C
@set pc_drv1=0x01C20860
@set pc_pull0=0x01C20864
@set pc_pull1=0x01C20868

@set pd_cfg0=0x01C2086c
@set pd_cfg1=0x01C20870
@set pd_cfg2=0x01C20874
@set pd_cfg3=0x01C20878
@set pd_data=0x01C2087c
@set pd_drv0=0x01C20880
@set pd_drv1=0x01C20884
@set pd_pull0=0x01C20888
@set pd_pull1=0x01C2088c

@set pe_cfg0=0x01C20890
@set pe_cfg1=0x01C20894
@set pe_cfg2=0x01C20898
@set pe_cfg3=0x01C2089c
@set pe_data=0x01C208A0
@set pe_drv0=0x01C208A4
@set pe_drv1=0x01C208A8
@set pe_pull0=0x01C208AC
@set pe_pull1=0x01C208B0

@set pf_cfg0=0x01C208B4
@set pf_cfg1=0x01C208B8
@set pf_cfg2=0x01C208BC
@set pf_cfg3=0x01C208C0
@set pf_data=0x01C208C4
@set pf_drv0=0x01C208C8
@set pf_drv1=0x01C208CC
@set pf_pull0=0x01C208D0
@set pf_pull1=0x01C208D4

@set pg_cfg0=0x01C208D8
@set pg_cfg1=0x01C208DC
@set pg_cfg2=0x01C208E0
@set pg_cfg3=0x01C208E4
@set pg_data=0x01C208E8
@set pg_drv0=0x01C208EC
@set pg_drv1=0x01C208F0
@set pg_pull0=0x01C208F4
@set pg_pull1=0x01C208F8

@set ph_cfg0=0x01C208FC
@set ph_cfg1=0x01C20900
@set ph_cfg2=0x01C20904
@set ph_cfg3=0x01C20908
@set ph_data=0x01C2090C
@set ph_drv0=0x01C20910
@set ph_drv1=0x01C20914
@set ph_pull0=0x01C20918
@set ph_pull1=0x01C2091C

@set pi_cfg0=0x01C20920
@set pi_cfg1=0x01C20924
@set pi_cfg2=0x01C20928
@set pi_cfg3=0x01C2092C
@set pi_data=0x01C20930
@set pi_drv0=0x01C20934
@set pi_drv1=0x01C20938
@set pi_pull0=0x01C2093C
@set pi_pull1=0x01C20940

@echo PB_CFG0-PB_CFG3
@xfel read32 %pb_cfg0%
@xfel read32 %pb_cfg1%
@xfel read32 %pb_cfg2%
@xfel read32 %pb_cfg3%

@echo PC_CFG0-PC_CFG0
@xfel read32 %pc_cfg0%
@xfel read32 %pc_cfg1%
@xfel read32 %pc_cfg2%
@xfel read32 %pc_cfg3%

@echo PD_CFG0-PD_CFG3
@xfel read32 %pd_cfg0%
@xfel read32 %pd_cfg1%
@xfel read32 %pd_cfg2%
@xfel read32 %pd_cfg3%

@echo PE_CFG0-PE_CFG3
@xfel read32 %pe_cfg0%
@xfel read32 %pe_cfg1%
@xfel read32 %pe_cfg2%
@xfel read32 %pe_cfg3%

@echo PF_CFG0-PF_CFG0
@xfel read32 %pf_cfg0%
@xfel read32 %pf_cfg1%
@xfel read32 %pf_cfg2%
@xfel read32 %pf_cfg3%

@echo PG_CFG0-PG_CFG3
@xfel read32 %pg_cfg0%
@xfel read32 %pg_cfg1%
@xfel read32 %pg_cfg2%
@xfel read32 %pg_cfg3%

@echo PH_CFG0-PH_CFG3
@xfel read32 %ph_cfg0%
@xfel read32 %ph_cfg1%
@xfel read32 %ph_cfg2%
@xfel read32 %ph_cfg3%

@echo BUS setup
xfel read32 %ccubus2%
xfel write32 %ccubus2% 0x0020
xfel read32 %ccubus2%

exit

@echo GPIOD registers
@xfel hexdump %pd_cfg0% 0x24

xfel write32 %pd_data% 0xFFFFFFFF
xfel write32 %pe_data% 0xFFFFFFFF

@echo GPIOD setup
rem xfel write32 %pd_data% 0xFFFFFFFF
xfel write32 %pd_data% 0x00000000
xfel read32 %pd_data% 

xfel write32 %pd_cfg0% 0x11111111
xfel write32 %pd_cfg1% 0x11111111
xfel write32 %pd_cfg2% 0x11111111
xfel write32 %pd_cfg3% 0x11111111

@echo PD_CFG0-PD_CFG3
@xfel read32 %pd_cfg0%
@xfel read32 %pd_cfg1%
@xfel read32 %pd_cfg2%
@xfel read32 %pd_cfg3%

@echo Blinking
xfel write32 %pd_data% 0x00000000
@xfel read32 %pd_data%
xfel write32 %pd_data% 0xFFFFFFFF
@xfel read32 %pd_data%
xfel write32 %pd_data% 0x00000000
@xfel read32 %pd_data%
xfel write32 %pd_data% 0xFFFFFFFF
@xfel read32 %pd_data%
xfel write32 %pd_data% 0x00000000
@xfel read32 %pd_data%

