#include "am.h"

am::am(QObject *parent) : QObject(parent)
{

}

unsigned char am::crc8_calcluate(unsigned char *pdata, int len, unsigned char init_crc)
{
    unsigned char byte = 0;
    while(len--)
    {
        byte = init_crc ^ (*pdata);
        init_crc = crc8_table[byte];
        pdata ++;
    }
    return init_crc;
}

bool am::crc8_verify(unsigned char *pdata, int len, unsigned char init_crc)
{
    unsigned char expected_crc  = 0;
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

void am::am_check_data(unsigned char* pdata, unsigned char* pdata_out, int* data_ok, int* is_re_tx_flag)
{
    unsigned char ch_idx, sn, crc_8;
    bool data_is_ok = CRC_FAIL;

    data_is_ok = crc8_verify(pdata, PACKET_SIZE + 4, CRC_INIT);
    ch_idx = pdata[1];
    sn = (pdata[2] >> 1) & 0x7F;//这里还需改进：检查head 的CRC,需要在STM32发送的时候在head后添加head的crc。后面再加
    *is_re_tx_flag = pdata[2] & 0x01;

    if (data_is_ok == CRC_OK)//接收的数据是正确的
    {
        //组数据包，给STM32回ack
        pdata_out[0] = 0xFF;
        pdata_out[1] = 0xFF;
        pdata_out[2] = 0xFF;
        pdata_out[3] = ch_idx;
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
        pdata_out[3] = ch_idx;
        pdata_out[4] = ((sn << 1) & 0xFE) | (NACK & 0x01);
        crc_8 = crc8_calcluate(&pdata_out[2], 3, CRC_INIT);
        pdata_out[5] = crc_8;
        *data_ok = CRC_FAIL;
    }
}
