#include "am.h"

am::am(QObject *parent) : QObject(parent)
{

}

uint8_t am::crc8_calcluate(uint8_t *pdata, int len, uint8_t init_crc)
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

bool am::crc8_verify(uint8_t *pdata, int len, uint8_t init_crc)
{
    uint8_t expected_crc  = 0;
    expected_crc = crc8_calcluate(pdata, len - 1, init_crc);
    if (expected_crc != pdata[len - 1])
    {
        return false;
    }
    else
    {
        return true;
    }
}

void am::rx_window_init(uint8_t packet_size, uint16_t sampling_rate)
{
    g_win_size = ( MAX_DELAY/(packet_size /(2 * sampling_rate))) << 1;//接收窗比发送窗宽一倍
    rx_win_info_ch0 = new RX_WINDOW_T[g_win_size];
    rx_win_info_ch1 = new RX_WINDOW_T[g_win_size];
}

void am::am_check_data(uint8_t* pdata, uint8_t *ch_idx, uint8_t* pdata_out, int* data_ok, int* is_re_tx_flag)
{
    uint8_t sn, crc_8;
    bool data_is_ok = CRC_FAIL;

    data_is_ok = crc8_verify(pdata, PACKET_SIZE + 4, CRC_INIT);
    *ch_idx = pdata[1];
    sn = (pdata[2] >> 1) & 0x7F;
    *is_re_tx_flag = pdata[2] & 0x01;

    if (data_is_ok == CRC_OK)//接收的数据是正确的
    {
        //组数据包，给STM32回ack
        pdata_out[0] = 0xFF;
        pdata_out[1] = 0xFF;
        pdata_out[2] = 0xFF;
        pdata_out[3] = *ch_idx;
        pdata_out[4] = ((sn << 1) & 0xFE) | (ACK & 0x01);
        crc_8 = crc8_calcluate(&pdata_out[2], 3, CRC_INIT);
        pdata_out[5] = crc_8;
        *data_ok = CRC_OK;
    }
    else//接收的数据是错误的
    {
        //组数据包，给STM32回ack
        pdata_out[0] = 0xFF;
        pdata_out[1] = 0xFF;
        pdata_out[2] = 0xFF;
        pdata_out[3] = *ch_idx;
        pdata_out[4] = ((sn << 1) & 0xFE) | (NACK & 0x01);
        crc_8 = crc8_calcluate(&pdata_out[2], 3, CRC_INIT);
        pdata_out[5] = crc_8;
        *data_ok = CRC_FAIL;
    }
}

void am::update_rx_window_and_save_data(uint8_t* pdata, uint8_t packet_size, DATA_T0_SAVE_T *head)
{
    uint8_t ch_idx, sn, i, j, k;

    ch_idx = pdata[1];
    sn = (pdata[2] >> 1) & 0x7F;
    DATA_T0_SAVE_T *pnode, *end;
    head = new DATA_T0_SAVE_T[1];
    end = head;
    if (ch_idx == CH_0)
    {
        if ((sn > head_sn_ch0) && (sn - head_sn_ch0 < g_win_size))/*说明是新传的数据，移动接收窗*/
        {
            for(i = 1; i < g_win_size; i++)
            {
                if (rx_win_info_ch0[g_win_size - i].sn != (sn - i))/*说明新传的sn与上次传的sn(即head_sn_ch0)之间有漏包*/
                {
                    if (rx_win_info_ch0[0].recv_flag == CRC_OK)
                    {
                        //rx_win_info_ch0[0].data_buf need save to txt  添加到链表来管理要存txt的数据
                        pnode = new  DATA_T0_SAVE_T[1];
                        for (j = 0; j < packet_size / 2; j++)
                        {
                            pnode->data[j] = rx_win_info_ch0[0].data_buf[j];
                        }
                        end->next = pnode;
                        end = pnode;
                    }
                    for (j = 0; j < g_win_size - i; j++)
                    {
                        rx_win_info_ch0[j].sn = rx_win_info_ch0[j + 1].sn;
                        rx_win_info_ch0[j].recv_flag = rx_win_info_ch0[j + 1].recv_flag;
                        for (k = 0; k < PACKET_SIZE / 2; k++)
                        {
                            rx_win_info_ch0[j].data_buf[k] = rx_win_info_ch0[j + 1].data_buf[k];
                        }
                    }
                    /*将漏的数据包填0，在接收窗先占上位置*/
                    rx_win_info_ch0[g_win_size - i].sn = head_sn_ch0 + i;
                    rx_win_info_ch0[g_win_size - i].recv_flag = CRC_FAIL;
                    for (j = 0; j < PACKET_SIZE / 2; j++)
                    {
                        rx_win_info_ch0[g_win_size - i].data_buf[j] = 0;
                    }
                }
                else
                {
                    if (rx_win_info_ch0[0].recv_flag == CRC_OK)
                    {
                        //rx_win_info_ch0[0].data_buf need save to txt  添加到链表来管理要存txt的数据
                        pnode = new  DATA_T0_SAVE_T[1];
                        for (j = 0; j < packet_size / 2; j++)
                        {
                            pnode->data[j] = rx_win_info_ch0[0].data_buf[j];
                        }
                        end->next = pnode;
                        end = pnode;
                    }
                    for (j = 0; j < g_win_size - 1; j++)
                    {
                        rx_win_info_ch0[j].sn = rx_win_info_ch0[j + 1].sn;
                        rx_win_info_ch0[j].recv_flag = rx_win_info_ch0[j + 1].recv_flag;
                        for (k = 0; k < PACKET_SIZE / 2; k++)
                        {
                            rx_win_info_ch0[j].data_buf[k] = rx_win_info_ch0[j + 1].data_buf[k];
                        }
                    }
                    /*将新传的数据加到窗头*/
                    head_sn_ch0 = sn;
                    rx_win_info_ch0[g_win_size - 1].sn = sn;
                    rx_win_info_ch0[g_win_size - 1].recv_flag = CRC_OK;
                    for (j = 0; j < PACKET_SIZE / 2; j++)
                    {
                        rx_win_info_ch0[g_win_size - 1].data_buf[j] = (pdata[3 + j] & 0xFF) | ((pdata[4 + j] << 8) & (0xFF00));
                    }
                    break;
                }
            }
        }
        else if (((sn - head_sn_ch0) >= -127) &&((sn - head_sn_ch0) <= -127 + g_win_size))/*说明也是新传的数据，只是sn循环了*/
        {
            for(i = 1; i < g_win_size; i++)
            {
                if (rx_win_info_ch0[g_win_size - i].sn != (sn - 1 + 128) % 128)/*说明新传的sn与上次传的sn(即head_sn_ch0)之间有漏包*/
                {
                    if (rx_win_info_ch0[0].recv_flag == CRC_OK)
                    {
                        //rx_win_info_ch0[0].data_buf need save to txt  添加到链表来管理要存txt的数据
                        pnode = new  DATA_T0_SAVE_T[1];
                        for (j = 0; j < packet_size / 2; j++)
                        {
                            pnode->data[j] = rx_win_info_ch0[0].data_buf[j];
                        }
                        end->next = pnode;
                        end = pnode;
                    }
                    for (j = 0; j < g_win_size - i; j++)
                    {
                        rx_win_info_ch0[j].sn = rx_win_info_ch0[j + 1].sn;
                        rx_win_info_ch0[j].recv_flag = rx_win_info_ch0[j + 1].recv_flag;
                        for (k = 0; k < PACKET_SIZE / 2; k++)
                        {
                            rx_win_info_ch0[j].data_buf[k] = rx_win_info_ch0[j + 1].data_buf[k];
                        }
                    }
                    /*将漏的数据包填0，在接收窗先占上位置*/
                    rx_win_info_ch0[g_win_size - i].sn = (head_sn_ch0 + i) % 128;
                    rx_win_info_ch0[g_win_size - i].recv_flag = CRC_FAIL;
                    for (j = 0; j < PACKET_SIZE / 2; j++)
                    {
                        rx_win_info_ch0[g_win_size - i].data_buf[j] = 0;
                    }
                }
                else
                {
                    if (rx_win_info_ch0[0].recv_flag == CRC_OK)
                    {
                        //rx_win_info_ch0[0].data_buf need save to txt  添加到链表来管理要存txt的数据
                        pnode = new  DATA_T0_SAVE_T[1];
                        for (j = 0; j < packet_size / 2; j++)
                        {
                            pnode->data[j] = rx_win_info_ch0[0].data_buf[j];
                        }
                        end->next = pnode;
                        end = pnode;
                    }
                    for (j = 0; j < g_win_size - 1; j++)
                    {
                        rx_win_info_ch0[j].sn = rx_win_info_ch0[j + 1].sn;
                        rx_win_info_ch0[j].recv_flag = rx_win_info_ch0[j + 1].recv_flag;
                        for (k = 0; k < PACKET_SIZE / 2; k++)
                        {
                            rx_win_info_ch0[j].data_buf[k] = rx_win_info_ch0[j + 1].data_buf[k];
                        }
                    }
                    /*将新传的数据加到窗头*/
                    head_sn_ch0 = sn;
                    rx_win_info_ch0[g_win_size - 1].sn = sn;
                    rx_win_info_ch0[g_win_size - 1].recv_flag = CRC_OK;
                    for (j = 0; j < PACKET_SIZE / 2; j++)
                    {
                        rx_win_info_ch0[g_win_size - 1].data_buf[j] = (pdata[3 + j] & 0xFF) | ((pdata[4 + j] << 8) & (0xFF00));
                    }
                    break;
                }
            }
        }
        else/*说明是重传的数据，找到窗中的对应的sn，填写数据*/
        {
            for(i = g_win_size - 1; i >= 0; i--)
            {
                if (rx_win_info_ch0[i].sn == sn)
                {
                    for (j = 0; j < PACKET_SIZE / 2; j++)
                    {
                        rx_win_info_ch0[i].data_buf[j] = (pdata[3 + j] & 0xFF) | ((pdata[4 + j] << 8) & (0xFF00));
                    }
                    rx_win_info_ch0[i].recv_flag = CRC_OK;
                }
                break;
            }
        }
    }
    else if(ch_idx == CH_1)
    {
        {
            if ((sn > head_sn_ch1) && (sn - head_sn_ch1 < g_win_size))/*说明是新传的数据，移动接收窗*/
            {
                for(i = 1; i < g_win_size; i++)
                {
                    if (rx_win_info_ch1[g_win_size - i].sn != (sn - i))/*说明新传的sn与上次传的sn(即head_sn_ch0)之间有漏包*/
                    {
                        if (rx_win_info_ch1[0].recv_flag == CRC_OK)
                        {
                            //rx_win_info_ch1[0].data_buf need save to txt  添加到链表来管理要存txt的数据
                            pnode = new  DATA_T0_SAVE_T[1];
                            for (j = 0; j < packet_size / 2; j++)
                            {
                                pnode->data[j] = rx_win_info_ch1[0].data_buf[j];
                            }
                            end->next = pnode;
                            end = pnode;
                        }
                        for (j = 0; j < g_win_size - i; j++)
                        {
                            rx_win_info_ch1[j].sn = rx_win_info_ch1[j + 1].sn;
                            rx_win_info_ch1[j].recv_flag = rx_win_info_ch1[j + 1].recv_flag;
                            for (k = 0; k < PACKET_SIZE / 2; k++)
                            {
                                rx_win_info_ch1[j].data_buf[k] = rx_win_info_ch1[j + 1].data_buf[k];
                            }
                        }
                        /*将漏的数据包填0，在接收窗先占上位置*/
                        rx_win_info_ch1[g_win_size - i].sn = head_sn_ch1 + i;
                        rx_win_info_ch1[g_win_size - i].recv_flag = CRC_FAIL;
                        for (j = 0; j < PACKET_SIZE / 2; j++)
                        {
                            rx_win_info_ch1[g_win_size - i].data_buf[j] = 0;
                        }
                    }
                    else
                    {
                        if (rx_win_info_ch1[0].recv_flag == CRC_OK)
                        {
                            //rx_win_info_ch1[0].data_buf need save to txt  添加到链表来管理要存txt的数据
                            pnode = new  DATA_T0_SAVE_T[1];
                            for (j = 0; j < packet_size / 2; j++)
                            {
                                pnode->data[j] = rx_win_info_ch1[0].data_buf[j];
                            }
                            end->next = pnode;
                            end = pnode;
                        }
                        for (j = 0; j < g_win_size - 1; j++)
                        {
                            rx_win_info_ch1[j].sn = rx_win_info_ch1[j + 1].sn;
                            rx_win_info_ch1[j].recv_flag = rx_win_info_ch1[j + 1].recv_flag;
                            for (k = 0; k < PACKET_SIZE / 2; k++)
                            {
                                rx_win_info_ch1[j].data_buf[k] = rx_win_info_ch1[j + 1].data_buf[k];
                            }
                        }
                        /*将新传的数据加到窗头*/
                        head_sn_ch0 = sn;
                        rx_win_info_ch1[g_win_size - 1].sn = sn;
                        rx_win_info_ch1[g_win_size - 1].recv_flag = CRC_OK;
                        for (j = 0; j < PACKET_SIZE / 2; j++)
                        {
                            rx_win_info_ch1[g_win_size - 1].data_buf[j] = (pdata[3 + j] & 0xFF) | ((pdata[4 + j] << 8) & (0xFF00));
                        }
                        break;
                    }
                }
            }
            else if (((sn - head_sn_ch1) >= -127) &&((sn - head_sn_ch1) <= -127 + g_win_size))/*说明也是新传的数据，只是sn循环了*/
            {
                for(i = 1; i < g_win_size; i++)
                {
                    if (rx_win_info_ch1[g_win_size - i].sn != (sn - 1 + 128) % 128)/*说明新传的sn与上次传的sn(即head_sn_ch0)之间有漏包*/
                    {
                        if (rx_win_info_ch1[0].recv_flag == CRC_OK)
                        {
                            //rx_win_info_ch1[0].data_buf need save to txt  添加到链表来管理要存txt的数据
                            pnode = new  DATA_T0_SAVE_T[1];
                            for (j = 0; j < packet_size / 2; j++)
                            {
                                pnode->data[j] = rx_win_info_ch1[0].data_buf[j];
                            }
                            end->next = pnode;
                            end = pnode;
                        }
                        for (j = 0; j < g_win_size - i; j++)
                        {
                            rx_win_info_ch1[j].sn = rx_win_info_ch1[j + 1].sn;
                            rx_win_info_ch1[j].recv_flag = rx_win_info_ch1[j + 1].recv_flag;
                            for (k = 0; k < PACKET_SIZE / 2; k++)
                            {
                                rx_win_info_ch1[j].data_buf[k] = rx_win_info_ch1[j + 1].data_buf[k];
                            }
                        }
                        /*将漏的数据包填0，在接收窗先占上位置*/
                        rx_win_info_ch1[g_win_size - i].sn = (head_sn_ch1 + i) % 128;
                        rx_win_info_ch1[g_win_size - i].recv_flag = CRC_FAIL;
                        for (j = 0; j < PACKET_SIZE / 2; j++)
                        {
                            rx_win_info_ch1[g_win_size - i].data_buf[j] = 0;
                        }
                    }
                    else
                    {
                        if (rx_win_info_ch1[0].recv_flag == CRC_OK)
                        {
                            //rx_win_info_ch1[0].data_buf need save to txt  添加到链表来管理要存txt的数据
                            pnode = new  DATA_T0_SAVE_T[1];
                            for (j = 0; j < packet_size / 2; j++)
                            {
                                pnode->data[j] = rx_win_info_ch1[0].data_buf[j];
                            }
                            end->next = pnode;
                            end = pnode;
                        }
                        for (j = 0; j < g_win_size - 1; j++)
                        {
                            rx_win_info_ch1[j].sn = rx_win_info_ch1[j + 1].sn;
                            rx_win_info_ch1[j].recv_flag = rx_win_info_ch1[j + 1].recv_flag;
                            for (k = 0; k < PACKET_SIZE / 2; k++)
                            {
                                rx_win_info_ch1[j].data_buf[k] = rx_win_info_ch1[j + 1].data_buf[k];
                            }
                        }
                        /*将新传的数据加到窗头*/
                        head_sn_ch1 = sn;
                        rx_win_info_ch1[g_win_size - 1].sn = sn;
                        rx_win_info_ch1[g_win_size - 1].recv_flag = CRC_OK;
                        for (j = 0; j < PACKET_SIZE / 2; j++)
                        {
                            rx_win_info_ch1[g_win_size - 1].data_buf[j] = (pdata[3 + j] & 0xFF) | ((pdata[4 + j] << 8) & (0xFF00));
                        }
                        break;
                    }
                }
            }
            else/*说明是重传的数据，找到窗中的对应的sn，填写数据*/
            {
                for(i = g_win_size - 1; i >= 0; i--)
                {
                    if (rx_win_info_ch1[i].sn == sn)
                    {
                        for (j = 0; j < PACKET_SIZE / 2; j++)
                        {
                            rx_win_info_ch1[i].data_buf[j] = (pdata[3 + j] & 0xFF) | ((pdata[4 + j] << 8) & (0xFF00));
                        }
                        rx_win_info_ch1[i].recv_flag = CRC_OK;
                    }
                    break;
                }
            }
        }
    }
}

void am::delete_node_mem_after_save_data(DATA_T0_SAVE_T *head)
{
    DATA_T0_SAVE_T * temp;
    while (head != NULL)
    {
        temp = head->next;
        delete [] head;
        head = temp;
    }
}
