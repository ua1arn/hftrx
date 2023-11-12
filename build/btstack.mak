# btstack
# https://github.com/bluekitchen/btstack.git

BTSTACK_ROOT := $(PRJROOT)/lib/btstack

C_SOURCES =  \
${BTSTACK_ROOT}/3rd-party/hxcmod-player/hxcmod.c \
${BTSTACK_ROOT}/3rd-party/hxcmod-player/mods/nao-deceased_by_disease.c \
${BTSTACK_ROOT}/3rd-party/micro-ecc/uECC.c \
${BTSTACK_ROOT}/3rd-party/yxml/yxml.c \
${BTSTACK_ROOT}/3rd-party/md5/md5.c \
${BTSTACK_ROOT}/chipset/cc256x/btstack_chipset_cc256x.c \
${BTSTACK_ROOT}/example/sco_demo_util.c \
${BTSTACK_ROOT}/platform/embedded/btstack_audio_embedded.c \
${BTSTACK_ROOT}/platform/embedded/btstack_run_loop_embedded.c \
${BTSTACK_ROOT}/platform/embedded/btstack_stdin_embedded.c \
${BTSTACK_ROOT}/platform/embedded/btstack_tlv_flash_bank.c \
${BTSTACK_ROOT}/platform/embedded/btstack_uart_block_embedded.c \
${BTSTACK_ROOT}/platform/embedded/hci_dump_embedded_stdout.c \
${BTSTACK_ROOT}/platform/embedded/hci_dump_segger_rtt_stdout.c \
${BTSTACK_ROOT}/src/ad_parser.c \
${BTSTACK_ROOT}/src/ble/att_db.c \
${BTSTACK_ROOT}/src/ble/att_dispatch.c \
${BTSTACK_ROOT}/src/ble/att_server.c \
${BTSTACK_ROOT}/src/ble/gatt-service/ancs_client.c \
${BTSTACK_ROOT}/src/ble/gatt-service/battery_service_client.c \
${BTSTACK_ROOT}/src/ble/gatt-service/battery_service_server.c \
${BTSTACK_ROOT}/src/ble/gatt-service/device_information_service_client.c \
${BTSTACK_ROOT}/src/ble/gatt-service/device_information_service_server.c \
${BTSTACK_ROOT}/src/ble/gatt-service/hids_device.c \
${BTSTACK_ROOT}/src/ble/gatt_client.c \
${BTSTACK_ROOT}/src/ble/le_device_db_memory.c \
${BTSTACK_ROOT}/src/ble/le_device_db_tlv.c \
${BTSTACK_ROOT}/src/ble/sm.c \
${BTSTACK_ROOT}/src/btstack_audio.c \
${BTSTACK_ROOT}/src/btstack_sample_rate_compensation.c \
${BTSTACK_ROOT}/src/btstack_crypto.c \
${BTSTACK_ROOT}/src/btstack_hid_parser.c \
${BTSTACK_ROOT}/src/btstack_linked_list.c \
${BTSTACK_ROOT}/src/btstack_memory.c \
${BTSTACK_ROOT}/src/btstack_memory_pool.c \
${BTSTACK_ROOT}/src/btstack_resample.c \
${BTSTACK_ROOT}/src/btstack_ring_buffer.c \
${BTSTACK_ROOT}/src/btstack_run_loop.c \
${BTSTACK_ROOT}/src/btstack_tlv.c \
${BTSTACK_ROOT}/src/btstack_util.c \
${BTSTACK_ROOT}/src/classic/a2dp.c \
${BTSTACK_ROOT}/src/classic/a2dp_sink.c \
${BTSTACK_ROOT}/src/classic/a2dp_source.c \
${BTSTACK_ROOT}/src/classic/avdtp.c \
${BTSTACK_ROOT}/src/classic/avdtp_acceptor.c \
${BTSTACK_ROOT}/src/classic/avdtp_initiator.c \
${BTSTACK_ROOT}/src/classic/avdtp_sink.c \
${BTSTACK_ROOT}/src/classic/avdtp_source.c \
${BTSTACK_ROOT}/src/classic/avdtp_util.c \
${BTSTACK_ROOT}/src/classic/avrcp.c \
${BTSTACK_ROOT}/src/classic/avrcp_browsing_controller.c \
${BTSTACK_ROOT}/src/classic/avrcp_controller.c \
${BTSTACK_ROOT}/src/classic/avrcp_media_item_iterator.c \
${BTSTACK_ROOT}/src/classic/avrcp_target.c \
${BTSTACK_ROOT}/src/classic/bnep.c \
${BTSTACK_ROOT}/src/classic/btstack_cvsd_plc.c \
${BTSTACK_ROOT}/src/classic/btstack_link_key_db_tlv.c \
${BTSTACK_ROOT}/src/classic/btstack_sbc_decoder_bluedroid.c \
${BTSTACK_ROOT}/src/classic/btstack_sbc_encoder_bluedroid.c \
${BTSTACK_ROOT}/src/classic/btstack_sbc_plc.c \
${BTSTACK_ROOT}/src/classic/device_id_server.c \
${BTSTACK_ROOT}/src/classic/goep_client.c \
${BTSTACK_ROOT}/src/classic/hfp.c \
${BTSTACK_ROOT}/src/classic/hfp_ag.c \
${BTSTACK_ROOT}/src/classic/hfp_gsm_model.c \
${BTSTACK_ROOT}/src/classic/hfp_hf.c \
${BTSTACK_ROOT}/src/classic/hfp_codec.c \
${BTSTACK_ROOT}/src/classic/hfp_msbc.c \
${BTSTACK_ROOT}/src/classic/hid_device.c \
${BTSTACK_ROOT}/src/classic/hid_host.c \
${BTSTACK_ROOT}/src/classic/hsp_ag.c \
${BTSTACK_ROOT}/src/classic/hsp_hs.c \
${BTSTACK_ROOT}/src/classic/obex_parser.c \
${BTSTACK_ROOT}/src/classic/obex_message_builder.c \
${BTSTACK_ROOT}/src/classic/pan.c \
${BTSTACK_ROOT}/src/classic/pbap_client.c \
${BTSTACK_ROOT}/src/classic/rfcomm.c \
${BTSTACK_ROOT}/src/classic/sdp_client.c \
${BTSTACK_ROOT}/src/classic/sdp_client_rfcomm.c \
${BTSTACK_ROOT}/src/classic/sdp_server.c \
${BTSTACK_ROOT}/src/classic/sdp_util.c \
${BTSTACK_ROOT}/src/classic/spp_server.c \
${BTSTACK_ROOT}/src/hci.c \
${BTSTACK_ROOT}/src/hci_cmd.c \
${BTSTACK_ROOT}/src/hci_dump.c \
${BTSTACK_ROOT}/src/hci_transport_h4.c \
${BTSTACK_ROOT}/src/l2cap.c \
${BTSTACK_ROOT}/src/l2cap_signaling.c \
${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/def.c \
${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/inet_chksum.c \
${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/init.c \
${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/ip.c \
${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/mem.c \
${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/memp.c \
${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/netif.c \
${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/pbuf.c \
${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/tcp.c \
${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/tcp_in.c \
${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/tcp_out.c \
${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/timeouts.c \
${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/udp.c \
${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/ipv4/acd.c \
${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/ipv4/dhcp.c \
${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/ipv4/etharp.c \
${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/ipv4/icmp.c \
${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/ipv4/ip4.c \
${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/ipv4/ip4_addr.c \
${BTSTACK_ROOT}/3rd-party/lwip/core/src/core/ipv4/ip4_frag.c \
${BTSTACK_ROOT}/3rd-party/lwip/core/src/netif/ethernet.c \
${BTSTACK_ROOT}/3rd-party/lwip/core/src/apps/http/altcp_proxyconnect.c \
${BTSTACK_ROOT}/3rd-party/lwip/core/src/apps/http/fs.c \
${BTSTACK_ROOT}/3rd-party/lwip/core/src/apps/http/httpd.c \
${BTSTACK_ROOT}/3rd-party/lwip/dhcp-server/dhserver.c \
${BTSTACK_ROOT}/platform/lwip/bnep_lwip.c \


C_INCLUDES += $(BUILD_DIR)
C_INCLUDES += ${BTSTACK_ROOT}/src/ble
C_INCLUDES += ${BTSTACK_ROOT}/src/ble/gatt-service
C_INCLUDES += ${BTSTACK_ROOT}/src/classic
C_INCLUDES += ${BTSTACK_ROOT}/src
C_INCLUDES += ${BTSTACK_ROOT}/3rd-party/micro-ecc
C_INCLUDES += ${BTSTACK_ROOT}/3rd-party/bluedroid/decoder/include
C_INCLUDES += ${BTSTACK_ROOT}/3rd-party/bluedroid/encoder/include
C_INCLUDES += ${BTSTACK_ROOT}/3rd-party/hxcmod-player
C_INCLUDES += ${BTSTACK_ROOT}/3rd-party/hxcmod-player/mods
C_INCLUDES += ${BTSTACK_ROOT}/3rd-party/lwip/core/src/include
C_INCLUDES += ${BTSTACK_ROOT}/3rd-party/lwip/dhcp-server
C_INCLUDES += ${BTSTACK_ROOT}/3rd-party/md5
C_INCLUDES += ${BTSTACK_ROOT}/3rd-party/yxml
C_INCLUDES += ${BTSTACK_ROOT}/3rd-party/segger-rtt
C_INCLUDES += ${BTSTACK_ROOT}/platform/embedded
C_INCLUDES += ${BTSTACK_ROOT}/platform/lwip
C_INCLUDES += ${BTSTACK_ROOT}/platform/lwip/port

#DINCDIR += $(BTSTACK_ROOT)/src
#SRCDIRS += ${BTSTACK_ROOT}/src/ble

UINCDIR += $(C_INCLUDES)
CSRC += $(C_SOURCES)
