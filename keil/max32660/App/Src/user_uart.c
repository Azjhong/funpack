#include "user_uart.h"
#include "watch.h"

/* **** Definitions **** */
#define UART_BAUD           115200 /* 1000000 */
#define BUFF_SIZE           512
#define SEND_DATA_SIZE      21


static uint8_t g_buff[BUFF_SIZE] = {0};
static uart_req_t read_req;

/**
 * @description: �û����ڳ�ʼ��
 * @param None
 * @return None
 */
void uart_init(void)
{
    const sys_cfg_uart_t sys_uart_cfg = {
		MAP_A,
		UART_FLOW_DISABLE,
    };
    int error;
  
	/* Setup the interrupt */
    NVIC_ClearPendingIRQ(MXC_UART_GET_IRQ(1));
    NVIC_DisableIRQ(MXC_UART_GET_IRQ(1));
    NVIC_SetPriority(MXC_UART_GET_IRQ(1), 1);
    NVIC_EnableIRQ(MXC_UART_GET_IRQ(1));
	
	/* Initialize the UART */
    uart_cfg_t cfg;
    cfg.parity = UART_PARITY_DISABLE;
    cfg.size = UART_DATA_SIZE_8_BITS;
    cfg.stop = UART_STOP_1;
    cfg.flow = UART_FLOW_CTRL_DIS;
    cfg.pol = UART_FLOW_POL_DIS;
    cfg.baud = UART_BAUD;
	
	error = UART_Init(MXC_UART_GET_UART(1), &cfg, &sys_uart_cfg);
	if (error != E_NO_ERROR) {
		while(1);
    }
	
	
	UART_Write(MXC_UART_GET_UART(1), g_buff, 8);
	
	read_req.data = g_buff;
	read_req.len = SEND_DATA_SIZE;
	read_req.callback = read_cb;
    error = UART_ReadAsync(MXC_UART_GET_UART(1), &read_req);
    if (error != E_NO_ERROR) {
        while(1) {}
    }	
	

}

/**
 * @description: ��ӡ���ݣ�ʹ�ô��ڹ��� VOFA+��https://www.vofa.plus��
 * @param *buf ָ��uint8_t���͵Ļ���ָ�� size �����С
 * @return None
 */
void print_data(uint8_t *buf, uint8_t size)
{
	uint8_t buffer[100] = {0};
	uint8_t i = 0;
	uint16_t index = 0;
	
	/* ֡ͷ */
	buffer[0] = 'a';
	buffer[1] = ':';
	index += 2;
	/* 311 222 133 */
	while (size--)
	{
		buffer[index] = *(buf + i);
		i++;
		index++;
	}
	/* ֡β */
	buffer[index] = '\n';
	index += 1;
	UART_Write(MXC_UART_GET_UART(1), buffer, index);
}


/**
 * @description: �û������жϽ��ջص�����
 * @param *req ָ��uart_req_t�ṹ��ָ�� error ����״̬
 * @return None
 */
void read_cb(uart_req_t *req, int error)
{
	UART_ReadAsync(MXC_UART_GET_UART(1), &read_req);
	
	/* UART1ֻ����У׼ʱ�书�� */
	/* ʱ��У׼ 2020-03-13 16:26:49 */
	p_time.year[3] = g_buff[0] - 0x30;
	p_time.year[2] = g_buff[1] - 0x30;
	p_time.year[1] = g_buff[2] - 0x30;
	p_time.year[0] = g_buff[3] - 0x30;
	
	p_time.mon[1] = g_buff[5] - 0x30;
	p_time.mon[0] = g_buff[6] - 0x30;
	
	p_time.day[1] = g_buff[8] - 0x30;
	p_time.day[0] = g_buff[9] - 0x30;
	
	p_time.hr[1] = g_buff[11] - 0x30;
	p_time.hr[0] = g_buff[12] - 0x30;
	
	p_time.min[1] = g_buff[14] - 0x30;
	p_time.min[0] = g_buff[15] - 0x30;
	
	p_time.sec[1] = g_buff[17] - 0x30;
	p_time.sec[0] = g_buff[18] - 0x30;
	
	p_time.wday = g_buff[20] - 0x30;

	p_time.update_state = UPDATE_SUCCESS;
	r_time.update.all = 0x7F;
	
	rtc_init();
	
}

/**
 * @description: UART1�����жϺ���
 * @param None
 * @return None
 */
void UART1_IRQHandler(void)
{
	UART_Handler(MXC_UART1);	
}

