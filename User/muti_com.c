#include "muti_com.h"

/* --------------------��̬ȫ�ֱ���-------------------- */

// ���ջ���������Ҫ�㹻��������������ݰ�
static uint8_t s_rx_buffer[128];
// ��ǰ����״̬
static volatile PacketParseState_t s_rx_state = STATE_WAIT_HEADER;
// ��ǰ�ѽ��������ֽڵļ�����
static volatile uint16_t s_rx_count = 0;
// �������յ����ݳ���
static volatile uint16_t s_data_len = 0;
// ���ݰ�������ɱ�־λ
volatile uint8_t g_packet_received_flag = 0;
// ������������
int g_parsed_data[30];
// �����������ݳ��� (int�ĸ���)
uint8_t g_parsed_data_len = 0;
// ���������豸ID
uint8_t g_parsed_device_id = 0;
// ����HAL�ⵥ�ֽڽ��յı���
uint8_t g_rx_byte = 0;

volatile uint8_t EnableReturnFlag = 1;      // Ĭ��Ϊ1,��ʾȡ��ҩ��ֱ�ӷŻ�

/**
  * @brief  ���ݰ�����״̬������
  * @param  byte: �Ӵ��ڽ��յ��ĵ����ֽ�
  * @retval None
  */
void ParsePackageByte(uint8_t byte)
{
    switch (s_rx_state)
    {
        case STATE_WAIT_HEADER:
            if (byte == PACKAGE_HEAD) {
                s_rx_count = 0;
                s_rx_state = STATE_RECEIVE_ID;
            }
            break;

        case STATE_RECEIVE_ID:
            s_rx_buffer[s_rx_count++] = byte; // �����豸ID
            s_rx_state = STATE_RECEIVE_LENGTH;
            break;

        case STATE_RECEIVE_LENGTH:
            s_rx_buffer[s_rx_count++] = byte; // ���볤��
            s_data_len = byte * sizeof(int);  // �����������ֽ���
            if (s_data_len == 0) {
                // ���û�����ݣ�ֱ�������ȴ�֡β
                s_rx_state = STATE_WAIT_TAIL;
            } else if (s_data_len > (sizeof(s_rx_buffer) - 2)) {
                // ���ݳ��ȳ�����������Э����󣬸�λ״̬��
                s_rx_state = STATE_WAIT_HEADER;
            } else {
                s_rx_state = STATE_RECEIVE_DATA;
            }
            break;

        case STATE_RECEIVE_DATA:
            s_rx_buffer[s_rx_count++] = byte; // ��������
            if (s_rx_count >= (s_data_len + 2)) { // +2��ΪID��LengthҲ����buffer��
                s_rx_state = STATE_WAIT_TAIL;
            }
            break;

        case STATE_WAIT_TAIL:
            if (byte == PACKAGE_TAIL) {
                // �ɹ����յ��������ݰ�
                g_parsed_device_id = s_rx_buffer[0];
                g_parsed_data_len = s_rx_buffer[1];
                
                // �ӻ������������ݵ�ȫ�ֱ���
                if (g_parsed_data_len > 0) {
                    memcpy(g_parsed_data, &s_rx_buffer[2], s_data_len);
                }
                
                // ���ñ�־λ��֪ͨ��ѭ������
                g_packet_received_flag = 1;
            }
            // ����֡β�Ƿ���ȷ������λ״̬����׼��������һ����
            s_rx_state = STATE_WAIT_HEADER;
            break;

        default:
            // �쳣״̬����λ
            s_rx_state = STATE_WAIT_HEADER;
            break;
    }
}


/**
  * @brief  �����ѽ��ղ�������ɵ����ݰ�.
  * @note   �˺���Ӧ����ѭ���б������Ե���.
  *         ������ȫ�ֱ�־λ g_packet_received_flag.
  * @param  None
  */
void PackageDataProc(void)
{
    Event event;

    if (g_packet_received_flag)
    {
        // ���������־λ����ֹ�ظ�����
        g_packet_received_flag = 0;
        
        /*
         * - ������԰�ȫ��ʹ������ȫ�ֱ���
         * - g_parsed_device_id: ���������豸ID
         * - g_parsed_data_len:  ��������int���ݸ���
         * - g_parsed_data[]:    ���int���ݵ�����
         */

        if(g_parsed_data_len == 1) {
            if(g_parsed_data[0] == 0) {
                EnableReturnFlag = 0;
            }
            else {
                EnableReturnFlag = 1;
            }
        }

        event = GetCarData;
        queue_push(&EventQueue, event);

        // ͨ�����ڴ�ӡ���յ�����Ϣ (���ڵ���)
        // printf("--- Packet Processed ---\r\n");
        // printf("  Device ID : 0x%02X\r\n", g_parsed_device_id);
        // printf("  Data Count: %d\r\n", g_parsed_data_len);
        // for (int i = 0; i < g_parsed_data_len; i++)
        // {
        //     printf("  Data[%d]   : %d\r\n", i, g_parsed_data[i]);
        // }
        // printf("------------------------\r\n\r\n");

    }
}


/**
  * @brief  ͨ�����ڷ���һ�����ݰ�.
  * @note   ������ʹ�õĴ��ں��豸ID�ɺ궨��.
  * @param  num_args: Ҫ���͵�32λint���Ͳ����ĸ���.
  * @param  ...: �ɱ����������Ҫ���͵�int��������.
  * @retval 1: ���ͳɹ�
  * @retval 0: ����ʧ�� (���磬���ݰ�����)
  */
int SendPackage(int num_args, ...)
{
    // �������
    uint16_t packet_size = 3 + (num_args * sizeof(int)) + 1;
    
    // ������������
    uint8_t tx_buffer[128]; 
    if (packet_size > sizeof(tx_buffer))
    {
        return 0;                            // ���ݰ��ܳ��ȹ���
    }

    uint16_t index = 0;

    tx_buffer[index++] = PACKAGE_HEAD;       // ���֡ͷ
    tx_buffer[index++] = DEVICE_ID;          // ����豸ID
    tx_buffer[index++] = (uint8_t)num_args;            // ������ݸ���
    
    // �������ɱ����
    va_list args;
    va_start(args, num_args);                // ��ʼ���ɱ�����б�
    
    for (int i = 0; i < num_args; ++i)
    {
        int current_int = va_arg(args, int); // ��ȡ��һ��int����

        memcpy(&tx_buffer[index], &current_int, sizeof(int)); // ʹ��memcpy��లȫ
        index += sizeof(int);
    }
    
    va_end(args);                           // ����ɱ�����б�
    
    // ���֡β
    tx_buffer[index++] = PACKAGE_TAIL;
    
    // ͨ��UART�������ݰ�
    // HAL_UART_Transmit �᷵�� HAL_OK, HAL_ERROR, HAL_BUSY, HAL_TIMEOUT
    if (HAL_UART_Transmit(COM_NUM, tx_buffer, packet_size, TIMEOUT) == HAL_OK) {
        return 1; // ���ͳɹ�������1
    }
    else {
        return 0; // ����ʧ�ܣ�����0
    }
}



/**
  * @brief  ���ڽ�����ɻص�����
  * @param  huart: �����жϵ�UART���
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3)
    {
        // ����״̬��������յ����ֽ�
        ParsePackageByte(g_rx_byte);

        // ׼��������һ���ֽ�
        HAL_UART_Receive_IT(COM_NUM, &g_rx_byte, 1);
    }
}


