#ifndef __AM_H
#define __AM_H 			   
#include "stdint.h"
#include <string.h>
#include "usb_type.h"


#define MAX_DELAY	3//设置允许的上下行最大时延(秒)，当前head中的sn范围：0~127，可设置的最大时延：128/(packet_size /(2 * sampling_rate))
#define CRC_INIT	0x77
#define CH_0	0
#define CH_1	1
#define MAX_RE_TX_NUM 4
#define DATA_LEN_FROM_PC 5
#define CRC_OK	1
#define CRC_FAIL 0
#define ACK		1
#define NACK	0
#define ACK_DATA_SIZE 4


extern uint16_t g_win_size;
extern uint16_t g_pre_write_ptr_ch0;
extern uint16_t g_pre_write_ptr_ch1;
extern uint8_t	g_sn_ch0;
extern uint8_t	g_sn_ch1;
extern uint8_t  g_tx_num_ch0;
extern uint8_t  g_tx_num_ch1;

 
typedef struct
{
	uint8_t sn;
	uint8_t re_tx_cnt;
	uint16_t re_tx_write_ptr;
	uint16_t re_tx_pre_write_ptr;
}RE_TX_NUM_T;


typedef struct
{
	uint8_t sn;
	uint8_t ack_flag;
}ACK_FLAG_T;


typedef struct
{
	RE_TX_NUM_T re_tx_info;
	ACK_FLAG_T  ack_info;
}TX_WINDOW_T;

extern TX_WINDOW_T *g_tx_win_info_ch0;
extern TX_WINDOW_T *g_tx_win_info_ch1;

uint8_t crc8_calcluate(uint8_t *pdata, int len, uint8_t init_rc);
bool crc8_verify(uint8_t *pdata, int len, uint8_t init_crc);
void tx_window_init(uint8_t packet_size, uint16_t sampling_rate);
void am_send(uint16_t* FIFO, uint16_t FIFO_size, uint16_t write_ptr, uint8_t packet_size, uint8_t ch_idx);
void ack_nack_recv_from_pc(uint16_t* FIFO_ch0, uint16_t* FIFO_ch1, uint16_t FIFO_size_ch0, uint16_t FIFO_size_ch1, uint8_t packet_size, uint8_t *pdata);

#endif
