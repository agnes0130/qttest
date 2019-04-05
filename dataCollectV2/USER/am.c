#include"am.h"
#include "stm32f10x_usart.h"
#include "stm32f10x.h"

uint16_t g_win_size;
uint16_t g_pre_write_ptr_ch0;
uint16_t g_pre_write_ptr_ch1;
uint8_t	 g_sn_ch0 = 0;
uint8_t	 g_sn_ch1 = 0;
uint8_t  g_tx_num_ch0 = 0;
uint8_t  g_tx_num_ch1 = 0;

TX_WINDOW_T *g_tx_win_info_ch0;
TX_WINDOW_T *g_tx_win_info_ch1;


const uint8_t crc8_table[256] =
{
  0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
  157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
  35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
  190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
  70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
  219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
  101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
  248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
  140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
  17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
  175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
  50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
  202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
  87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
  233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
  116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
};

uint8_t crc8_calcluate(uint8_t *pdata, int len, uint8_t init_crc)
{
	uint8_t byte = 0;
    while(len--)
    {
        byte = init_crc ^ (*pdata);
        init_crc = crc8_table[byte];
        pdata ++;
    }
    return init_crc;
}

bool crc8_verify(uint8_t *pdata, int len, uint8_t init_crc)
{
    uint8_t expected_crc  = 0;
    expected_crc = crc8_calcluate(pdata, len - 1, init_crc);
    if (expected_crc != pdata[len - 1])
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

void datacopy(uint8_t *send_data, uint16_t* FIFO, uint8_t packet_size)
{
	uint8_t i;	
	for (i = 0; i < packet_size; i++)
	{
		send_data[i] = (i % 2 == 0) ? (FIFO[i/2] & 0xFF) : ((FIFO[i/2] & 0xFF00) >> 8);
	}
}

/*���㷢�ʹ��ڳ��ȣ�����Ϊ�������ʱ�������£���Ӧ�ķ��ʹ��ڵĳ��ȣ�
�����ʱ�ӷ�Χ�ڣ����л�������λ���ط� packet_size Ϊÿ�����ݰ�����Ч�ֽ�����
sampling_rateΪAD�Ĳ���Ƶ�ʡ�             ��ʹ��am�ش���tx_window_init ���ڳ�ʼ��ʱ����*/
void tx_window_init(uint8_t packet_size, uint16_t sampling_rate)
{
	g_win_size = MAX_DELAY/(packet_size /(2 * sampling_rate));
	g_tx_win_info_ch0 = (TX_WINDOW_T*)malloc(sizeof(TX_WINDOW_T) * g_win_size);
	g_tx_win_info_ch1 = (TX_WINDOW_T*)malloc(sizeof(TX_WINDOW_T) * g_win_size);
}

void send_data_by_usart(uint8_t *send_data, uint8_t data_size)
{
	int8_t i;
	for (i = 0; i < data_size; i++)
	{
		USART_SendData(USART1, send_data[i]);
	}
}

void am_re_send_due_to_no_ack_or_nack(uint16_t* FIFO, uint16_t FIFO_size, uint16_t re_tx_write_ptr, uint16_t re_tx_pre_write_ptr, uint8_t packet_size, uint8_t ch_idx, uint8_t sn)
{
	uint8_t crc_8;
	uint16_t first_byte_ptr;
	uint16_t byte_num_before_loop;
	uint8_t *send_data;

	if (ch_idx == CH_0)
	{
		if (re_tx_write_ptr - re_tx_pre_write_ptr > 0)
		{
			send_data = (uint8_t *)malloc(sizeof(uint8_t) * packet_size + 4);//4����˼��3�ֽ�����head,1�ֽ�����crc-8
			first_byte_ptr = re_tx_write_ptr - (packet_size /2 + 1);//��ǰ��Ҫ���͵����ݰ��ĵ�һ���ֽ����ڵ�λ��
			datacopy(&send_data[3], &FIFO[first_byte_ptr], packet_size);
		}
		else//˵��FIFO�Ѿ�ѭ����
		{
			send_data = (uint8_t *)malloc(sizeof(uint8_t) * packet_size + 4);//4����˼��3�ֽ�����head,1�ֽ�����crc-8
			first_byte_ptr = re_tx_pre_write_ptr + 1;
			
			byte_num_before_loop = (FIFO_size - first_byte_ptr) * 2;
			datacopy(&send_data[3], &FIFO[first_byte_ptr], byte_num_before_loop);//����FIFO���Ĳ����ֽ�		
			datacopy(&send_data[3 + byte_num_before_loop], &FIFO[0], packet_size - byte_num_before_loop);//����FIFO�Ѿ�ѭ����ǰ��Ĳ����ֽ�
		}
		
		send_data[0] = 0xFF;
		send_data[1] = CH_0;
		send_data[2] = (sn | 0x01);//bit7~bit1Ϊsn(0~127), bit0 ��0��ʾ�ǳ��δ��� bit0 ��1��ʾ�ش�
		crc_8 = crc8_calcluate(&send_data[0], packet_size + 3, CRC_INIT);
		send_data[packet_size - 1] = crc_8;

		send_data_by_usart(send_data, packet_size + 4);
		free(send_data);
	}
	else if (ch_idx == CH_1)
	{
		if (re_tx_write_ptr - re_tx_pre_write_ptr > 0)
		{
			send_data = (uint8_t *)malloc(sizeof(uint8_t) * packet_size + 4);//4����˼��3�ֽ�����head,1�ֽ�����crc-8
			first_byte_ptr = re_tx_write_ptr - (packet_size /2 + 1);//��ǰ��Ҫ���͵����ݰ��ĵ�һ���ֽ����ڵ�λ��
			datacopy(&send_data[3], &FIFO[first_byte_ptr], packet_size);
		}
		else//˵��FIFO�Ѿ�ѭ����
		{
			send_data = (uint8_t *)malloc(sizeof(uint8_t) * packet_size + 4);//4����˼��3�ֽ�����head,1�ֽ�����crc-8
			first_byte_ptr = re_tx_pre_write_ptr + 1;
			
			byte_num_before_loop = (FIFO_size - first_byte_ptr) * 2;
			datacopy(&send_data[3], &FIFO[first_byte_ptr], byte_num_before_loop);//����FIFO���Ĳ����ֽ�		
			datacopy(&send_data[3 + byte_num_before_loop], &FIFO[0], packet_size - byte_num_before_loop);//����FIFO�Ѿ�ѭ����ǰ��Ĳ����ֽ�
		}
		send_data[0] = 0xFF;
		send_data[1] = CH_1;
		send_data[2] = (sn | 0x01);//bit7~bit1Ϊsn(0~127), bit0 ��0��ʾ�ǳ��δ��� bit0 ��1��ʾ�ش�
		crc_8 = crc8_calcluate(&send_data[0], packet_size + 3, CRC_INIT);
		send_data[packet_size - 1] = crc_8;

		send_data_by_usart(send_data, packet_size + 4);
		free(send_data);
	}
}

void updata_tx_window_and_re_tx(uint16_t* FIFO, uint16_t FIFO_size, uint16_t write_ptr, uint8_t packet_size, uint8_t ch_idx)
{
	uint8_t i;
	uint8_t re_tx_write_ptr, re_tx_pre_write_ptr, sn;

	if (ch_idx == CH_0)
	{
		if (g_tx_num_ch0 < g_win_size)//��ǰ�ϵ緢�͵����ݰ�������С�� g_win_size
		{
			g_tx_win_info_ch0[g_tx_num_ch0].re_tx_info.sn = g_sn_ch0;
			g_tx_win_info_ch0[g_tx_num_ch0].re_tx_info.re_tx_cnt = 0;
			g_tx_win_info_ch0[g_tx_num_ch0].re_tx_info.re_tx_write_ptr = write_ptr;
			g_tx_win_info_ch0[g_tx_num_ch0].re_tx_info.re_tx_pre_write_ptr = g_pre_write_ptr_ch0;
		
			g_tx_win_info_ch0[g_tx_num_ch0].ack_info.sn = g_sn_ch0;
			g_tx_win_info_ch0[g_tx_num_ch0].ack_info.ack_flag = NACK;
			g_tx_num_ch0 ++;
		}
		else
		{
			for (i = 0; i < g_win_size; i++)
			{
				g_tx_win_info_ch0[i].re_tx_info.sn = g_tx_win_info_ch0[i + 1].re_tx_info.sn;
				g_tx_win_info_ch0[i].re_tx_info.re_tx_cnt = g_tx_win_info_ch0[i + 1].re_tx_info.re_tx_cnt;
				g_tx_win_info_ch0[i].re_tx_info.re_tx_write_ptr = g_tx_win_info_ch0[i + 1].re_tx_info.re_tx_write_ptr;
				g_tx_win_info_ch0[i].re_tx_info.re_tx_pre_write_ptr = g_tx_win_info_ch0[i + 1].re_tx_info.re_tx_pre_write_ptr;
		
				g_tx_win_info_ch0[i].ack_info.sn = g_tx_win_info_ch0[i + 1].ack_info.sn;
				g_tx_win_info_ch0[i].ack_info.ack_flag = g_tx_win_info_ch0[i + 1].ack_info.ack_flag;
			}
			g_tx_win_info_ch0[g_win_size - 1].re_tx_info.sn = g_sn_ch0;
			g_tx_win_info_ch0[g_win_size - 1].re_tx_info.re_tx_cnt = 0;
			g_tx_win_info_ch0[g_win_size - 1].re_tx_info.re_tx_write_ptr = write_ptr;
			g_tx_win_info_ch0[g_win_size - 1].re_tx_info.re_tx_pre_write_ptr = g_pre_write_ptr_ch0;
		
			g_tx_win_info_ch0[g_win_size - 1].ack_info.sn = g_sn_ch0;
			g_tx_win_info_ch0[g_win_size - 1].ack_info.ack_flag = NACK;
		
			for (i = 0; i< g_win_size - 1; i++)
			{//���֮ǰ���͵ļ����g_win_size/MAX_RE_TX_NUM�������ݻ�û�յ���ȷ��ACK���������ش���
			//û�յ���ȷ��ACK������PCû��ʱ������ȥ�����ݡ��п�����PCû��ʱ��ACK���п�����ACK���ݰ������ʱ�������
				if (i % MAX_RE_TX_NUM == 0)
				{
					if (g_tx_win_info_ch0[i].ack_info.ack_flag != ACK)
					{
						re_tx_write_ptr = g_tx_win_info_ch0[i].re_tx_info.re_tx_write_ptr;
						re_tx_pre_write_ptr = g_tx_win_info_ch0[i].re_tx_info.re_tx_pre_write_ptr;
						sn = g_tx_win_info_ch0[i].re_tx_info.sn;
						am_re_send_due_to_no_ack_or_nack(&FIFO[0], FIFO_size, re_tx_write_ptr, re_tx_pre_write_ptr, packet_size, ch_idx, sn);
						//g_tx_win_info_ch0[i].re_tx_info.re_tx_cnt ++;
					}
				}
			}
		}
	}
	else if (ch_idx == CH_1)
	{
		if (g_tx_num_ch1 < g_win_size)//��ǰ�ϵ緢�͵����ݰ�������С�� g_win_size
		{
			g_tx_win_info_ch1[g_tx_num_ch1].re_tx_info.sn = g_sn_ch1;
			g_tx_win_info_ch1[g_tx_num_ch1].re_tx_info.re_tx_cnt = 0;
			g_tx_win_info_ch1[g_tx_num_ch1].re_tx_info.re_tx_write_ptr = write_ptr;
			g_tx_win_info_ch1[g_tx_num_ch1].re_tx_info.re_tx_pre_write_ptr = g_pre_write_ptr_ch1;
		
			g_tx_win_info_ch1[g_tx_num_ch1].ack_info.sn = g_sn_ch1;
			g_tx_win_info_ch1[g_tx_num_ch1].ack_info.ack_flag = NACK;
			
			g_tx_num_ch1 ++;
		}
		else
		{
			for (i = 0; i < g_win_size; i++)
			{
				g_tx_win_info_ch1[i].re_tx_info.sn = g_tx_win_info_ch1[i + 1].re_tx_info.sn;
				g_tx_win_info_ch1[i].re_tx_info.re_tx_cnt = g_tx_win_info_ch1[i + 1].re_tx_info.re_tx_cnt;
				g_tx_win_info_ch1[i].re_tx_info.re_tx_write_ptr = g_tx_win_info_ch1[i + 1].re_tx_info.re_tx_write_ptr;
				g_tx_win_info_ch1[i].re_tx_info.re_tx_pre_write_ptr = g_tx_win_info_ch1[i + 1].re_tx_info.re_tx_pre_write_ptr;
		
				g_tx_win_info_ch1[i].ack_info.sn = g_tx_win_info_ch1[i + 1].ack_info.sn;
				g_tx_win_info_ch1[i].ack_info.ack_flag = g_tx_win_info_ch1[i + 1].ack_info.ack_flag;
			}
			g_tx_win_info_ch1[g_win_size - 1].re_tx_info.sn = g_sn_ch1;
			g_tx_win_info_ch1[g_win_size - 1].re_tx_info.re_tx_cnt = 0;
			g_tx_win_info_ch1[g_win_size - 1].re_tx_info.re_tx_write_ptr = write_ptr;
			g_tx_win_info_ch1[g_win_size - 1].re_tx_info.re_tx_pre_write_ptr = g_pre_write_ptr_ch1;
		
			g_tx_win_info_ch1[g_win_size - 1].ack_info.sn = g_sn_ch1;
			g_tx_win_info_ch1[g_win_size - 1].ack_info.ack_flag = NACK;
		
			for (i = 0; i< g_win_size - 1; i++)
			{//���֮ǰ���͵ļ����g_win_size/MAX_RE_TX_NUM�������ݻ�û�յ���ȷ��ACK���������ش���
			//û�յ���ȷ��ACK������PCû��ʱ������ȥ�����ݡ��п�����PCû��ʱ��ACK���п�����ACK���ݰ������ʱ�������
				if (i % MAX_RE_TX_NUM == 0)
				{
					if (g_tx_win_info_ch1[i].ack_info.ack_flag != ACK)
					{
						re_tx_write_ptr = g_tx_win_info_ch1[i].re_tx_info.re_tx_write_ptr;
						re_tx_pre_write_ptr = g_tx_win_info_ch1[i].re_tx_info.re_tx_pre_write_ptr;
						sn = g_tx_win_info_ch1[i].re_tx_info.sn;
						am_re_send_due_to_no_ack_or_nack(&FIFO[0], FIFO_size, re_tx_write_ptr, re_tx_pre_write_ptr, packet_size, ch_idx, sn);
						//g_tx_win_info_ch1[i].re_tx_info.re_tx_cnt ++;
					}
				}
			}
		}
	}

}
/*��AD7606��ĳ��ch������packet_size/2��������ֵ�����am_send, FIFOΪch_idx��Ӧ�ĵ�FIFO��packet_size�Ƽ� 32*/
void am_send(uint16_t* FIFO, uint16_t FIFO_size, uint16_t write_ptr, uint8_t packet_size, uint8_t ch_idx)
{
	uint8_t crc_8;
	uint16_t first_byte_ptr;
	uint16_t byte_num_before_loop;
	uint8_t *send_data;
	
	if (ch_idx == CH_0)
	{
		if (write_ptr - g_pre_write_ptr_ch0 > 0)
		{
			send_data = (uint8_t *)malloc(sizeof(uint8_t) * packet_size + 4);//4����˼��3�ֽ�����head,1�ֽ�����crc-8
			first_byte_ptr = write_ptr - (packet_size /2 + 1);//��ǰ��Ҫ���͵����ݰ��ĵ�һ���ֽ����ڵ�λ��
			datacopy(&send_data[3], &FIFO[first_byte_ptr], packet_size);
		}
		else//˵��FIFO�Ѿ�ѭ����
		{
			send_data = (uint8_t *)malloc(sizeof(uint8_t) * packet_size + 4);//4����˼��3�ֽ�����head,1�ֽ�����crc-8
			first_byte_ptr = g_pre_write_ptr_ch0 + 1;
			
			byte_num_before_loop = (FIFO_size - first_byte_ptr) * 2;
			datacopy(&send_data[3], &FIFO[first_byte_ptr], byte_num_before_loop);//����FIFO���Ĳ����ֽ�		
			datacopy(&send_data[3 + byte_num_before_loop], &FIFO[0], packet_size - byte_num_before_loop);//����FIFO�Ѿ�ѭ����ǰ��Ĳ����ֽ�
		}
		send_data[0] = 0xFF;
		send_data[1] = CH_0;
		send_data[2] = ((g_sn_ch0 << 1) & 0xFE) | 0x00;//bit7~bit1Ϊsn(0~127), bit0 ��0��ʾ�ǳ��δ��� bit0 ��1��ʾ�ش�
		crc_8 = crc8_calcluate(&send_data[0], packet_size + 3, CRC_INIT);
		send_data[packet_size - 1] = crc_8;

		send_data_by_usart(send_data, packet_size + 4);
		free(send_data);
		
		updata_tx_window_and_re_tx(&FIFO[0], FIFO_size, write_ptr, packet_size, ch_idx);//ά��tx_window
		g_sn_ch0 ++;
		g_pre_write_ptr_ch0 = write_ptr;
	}
	else if (ch_idx == CH_1)
	{
		if (write_ptr - g_pre_write_ptr_ch1 > 0)
		{
			send_data = (uint8_t *)malloc(sizeof(uint8_t) * packet_size + 4);//4����˼��3�ֽ�����head,1�ֽ�����crc-8
			first_byte_ptr = write_ptr - (packet_size /2 + 1);//��ǰ��Ҫ���͵����ݰ��ĵ�һ���ֽ����ڵ�λ��
			datacopy(&send_data[3], &FIFO[first_byte_ptr], packet_size);
		}
		else//˵��FIFO�Ѿ�ѭ����
		{
			send_data = (uint8_t *)malloc(sizeof(uint8_t) * packet_size + 4);//4����˼��3�ֽ�����head,1�ֽ�����crc-8
			first_byte_ptr = g_pre_write_ptr_ch1 + 1;
			
			byte_num_before_loop = (FIFO_size - first_byte_ptr) * 2;
			datacopy(&send_data[3], &FIFO[first_byte_ptr], byte_num_before_loop);//����FIFO���Ĳ����ֽ�		
			datacopy(&send_data[3 + byte_num_before_loop], &FIFO[0], packet_size - byte_num_before_loop);//����FIFO�Ѿ�ѭ����ǰ��Ĳ����ֽ�
		}
		send_data[0] = 0xFF;
		send_data[1] = CH_1;
		send_data[2] = ((g_sn_ch1 << 1) & 0xFE) | 0x00;//bit7~bit1Ϊsn(0~127), bit0 ��0��ʾ�ǳ��δ��� bit0 ��1��ʾ�ش�;
		crc_8 = crc8_calcluate(&send_data[0], packet_size + 3, CRC_INIT);
		send_data[packet_size - 1] = crc_8;

		send_data_by_usart(send_data, packet_size + 4);
		free(send_data);
		
		updata_tx_window_and_re_tx(&FIFO[0], FIFO_size, write_ptr, packet_size, ch_idx);//ά��tx_window
		g_sn_ch1 ++;
		g_pre_write_ptr_ch1= write_ptr;
	}
}

void ack_nack_recv_from_pc(uint16_t* FIFO_ch0, uint16_t* FIFO_ch1, uint16_t FIFO_size_ch0, uint16_t FIFO_size_ch1, uint8_t packet_size, uint8_t *pdata)//usart�յ���PC���·���ACK����Ҫ����TX_WINDOW�е�ack_info, �յ�NACK����Ҫ�����ش�
{//pdata�����:3byte head + 1byte(sn ռ��7bit��ack/nack ռbit0��1��ʾack��0��ʾnack)
	bool pc_data_is_ok = CRC_FAIL;
	uint8_t ch_idx, sn, ack_flag, i, re_tx_write_ptr, re_tx_pre_write_ptr;
	uint16_t win_size;
	
	//��У���յ�����PC�����ݰ��Ƿ�����ȷ��
	pc_data_is_ok = crc8_verify(pdata, DATA_LEN_FROM_PC, CRC_INIT);
	if (pc_data_is_ok == CRC_OK)//CRC OK �Ž��д���
	{
		ch_idx = (pdata[1] == CH_0) ? CH_0 : CH_1; 
		if (ch_idx == CH_0)
		{	
			win_size = (g_tx_num_ch0 < g_win_size) ? g_tx_num_ch0 : g_win_size;
			sn = (pdata[2] >> 1) & 0x7F;
			ack_flag = pdata[2] & 0x01;
			if (ack_flag == ACK)//sn��Ӧ��������ݣ�PC�Ѿ���ȷ����
			{
				//��tx_win���ҵ�sn��������tx_win�е���Ϣ�����û�ҵ���˵���Ѿ�����
				for (i = 0; i < win_size; i++)
				{
					if (g_tx_win_info_ch0[i].ack_info.sn == sn)
					{
						g_tx_win_info_ch0[i].ack_info.ack_flag = ACK;
					}
				}
			}
			else//sn��Ӧ��������ݣ�PCû����ȷ����
			{
				//��tx_win���ҵ�sn��������tx_win�е���Ϣ,�������ش������û�ҵ���˵���Ѿ�����
				for (i = 0; i < win_size; i++)
				{
					if (g_tx_win_info_ch0[i].ack_info.sn == sn)
					{
						g_tx_win_info_ch0[i].ack_info.ack_flag = NACK;
						if (g_tx_win_info_ch0[i].re_tx_info.re_tx_cnt <= MAX_RE_TX_NUM)//������ش�MAX_RE_TX_NUM��
						{
							re_tx_write_ptr = g_tx_win_info_ch0[i].re_tx_info.re_tx_write_ptr;
							re_tx_pre_write_ptr = g_tx_win_info_ch0[i].re_tx_info.re_tx_pre_write_ptr;
							am_re_send_due_to_no_ack_or_nack(&FIFO_ch0[0], FIFO_size_ch0, re_tx_write_ptr, re_tx_pre_write_ptr, packet_size, ch_idx, sn);
							g_tx_win_info_ch0[i].re_tx_info.re_tx_cnt ++;
						}
					}
				}
			}
		}
		else if (ch_idx == CH_1)
		{	
			win_size = (g_tx_num_ch1 < g_win_size) ? g_tx_num_ch1 : g_win_size;
			sn = (pdata[2] >> 1) & 0x7F;
			ack_flag = pdata[2] & 0x01;
			if (ack_flag == ACK)//sn��Ӧ��������ݣ�PC�Ѿ���ȷ����
			{
				//��tx_win���ҵ�sn��������tx_win�е���Ϣ�����û�ҵ���˵���Ѿ�����
				for (i = 0; i < win_size; i++)
				{
					if (g_tx_win_info_ch1[i].ack_info.sn == sn)
					{
						g_tx_win_info_ch1[i].ack_info.ack_flag = ACK;
					}
				}
			}
			else//sn��Ӧ��������ݣ�PCû����ȷ����
			{
				//��tx_win���ҵ�sn��������tx_win�е���Ϣ,�������ش������û�ҵ���˵���Ѿ�����
				for (i = 0; i < win_size; i++)
				{
					if (g_tx_win_info_ch1[i].ack_info.sn == sn)
					{
						g_tx_win_info_ch1[i].ack_info.ack_flag = NACK;
						if (g_tx_win_info_ch1[i].re_tx_info.re_tx_cnt <= MAX_RE_TX_NUM)//������ش�MAX_RE_TX_NUM��
						{
							re_tx_write_ptr = g_tx_win_info_ch1[i].re_tx_info.re_tx_write_ptr;
							re_tx_pre_write_ptr = g_tx_win_info_ch1[i].re_tx_info.re_tx_pre_write_ptr;
							am_re_send_due_to_no_ack_or_nack(&FIFO_ch1[0], FIFO_size_ch1, re_tx_write_ptr, re_tx_pre_write_ptr, packet_size, ch_idx, sn);
							g_tx_win_info_ch1[i].re_tx_info.re_tx_cnt ++;
						}
					}
				}
			}
		}	
	}
	else
	{
		//do nothing	
	}
}

