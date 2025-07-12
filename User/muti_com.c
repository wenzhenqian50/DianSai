#include "muti_com.h"

/* --------------------静态全局变量-------------------- */

// 接收缓冲区，需要足够大以容纳最长的数据包
static uint8_t s_rx_buffer[128];
// 当前解析状态
static volatile PacketParseState_t s_rx_state = STATE_WAIT_HEADER;
// 当前已接收数据字节的计数器
static volatile uint16_t s_rx_count = 0;
// 期望接收的数据长度
static volatile uint16_t s_data_len = 0;
// 数据包接收完成标志位
volatile uint8_t g_packet_received_flag = 0;
// 解析出的数据
int g_parsed_data[30];
// 解析出的数据长度 (int的个数)
uint8_t g_parsed_data_len = 0;
// 解析出的设备ID
uint8_t g_parsed_device_id = 0;
// 用于HAL库单字节接收的变量
uint8_t g_rx_byte = 0;

volatile uint8_t EnableReturnFlag = 1;      // 默认为1,表示取走药将直接放回

/**
  * @brief  数据包解析状态机函数
  * @param  byte: 从串口接收到的单个字节
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
            s_rx_buffer[s_rx_count++] = byte; // 存入设备ID
            s_rx_state = STATE_RECEIVE_LENGTH;
            break;

        case STATE_RECEIVE_LENGTH:
            s_rx_buffer[s_rx_count++] = byte; // 存入长度
            s_data_len = byte * sizeof(int);  // 计算总数据字节数
            if (s_data_len == 0) {
                // 如果没有数据，直接跳到等待帧尾
                s_rx_state = STATE_WAIT_TAIL;
            } else if (s_data_len > (sizeof(s_rx_buffer) - 2)) {
                // 数据长度超出缓冲区，协议错误，复位状态机
                s_rx_state = STATE_WAIT_HEADER;
            } else {
                s_rx_state = STATE_RECEIVE_DATA;
            }
            break;

        case STATE_RECEIVE_DATA:
            s_rx_buffer[s_rx_count++] = byte; // 存入数据
            if (s_rx_count >= (s_data_len + 2)) { // +2因为ID和Length也存在buffer里
                s_rx_state = STATE_WAIT_TAIL;
            }
            break;

        case STATE_WAIT_TAIL:
            if (byte == PACKAGE_TAIL) {
                // 成功接收到完整数据包
                g_parsed_device_id = s_rx_buffer[0];
                g_parsed_data_len = s_rx_buffer[1];
                
                // 从缓冲区拷贝数据到全局变量
                if (g_parsed_data_len > 0) {
                    memcpy(g_parsed_data, &s_rx_buffer[2], s_data_len);
                }
                
                // 设置标志位，通知主循环处理
                g_packet_received_flag = 1;
            }
            // 无论帧尾是否正确，都复位状态机以准备接收下一个包
            s_rx_state = STATE_WAIT_HEADER;
            break;

        default:
            // 异常状态，复位
            s_rx_state = STATE_WAIT_HEADER;
            break;
    }
}


/**
  * @brief  处理已接收并解析完成的数据包.
  * @note   此函数应在主循环中被周期性调用.
  *         它会检查全局标志位 g_packet_received_flag.
  * @param  None
  */
void PackageDataProc(void)
{
    Event event;

    if (g_packet_received_flag)
    {
        // 立即清除标志位，防止重复处理。
        g_packet_received_flag = 0;
        
        /*
         * - 这里可以安全地使用以下全局变量
         * - g_parsed_device_id: 解析出的设备ID
         * - g_parsed_data_len:  解析出的int数据个数
         * - g_parsed_data[]:    存放int数据的数组
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

        // 通过串口打印接收到的信息 (用于调试)
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
  * @brief  通过串口发送一个数据包.
  * @note   本函数使用的串口和设备ID由宏定义.
  * @param  num_args: 要发送的32位int类型参数的个数.
  * @param  ...: 可变参数，所有要发送的int类型数据.
  * @retval 1: 发送成功
  * @retval 0: 发送失败 (例如，数据包过长)
  */
int SendPackage(int num_args, ...)
{
    // 计算包长
    uint16_t packet_size = 3 + (num_args * sizeof(int)) + 1;
    
    // 创建包缓冲区
    uint8_t tx_buffer[128]; 
    if (packet_size > sizeof(tx_buffer))
    {
        return 0;                            // 数据包总长度过大
    }

    uint16_t index = 0;

    tx_buffer[index++] = PACKAGE_HEAD;       // 填充帧头
    tx_buffer[index++] = DEVICE_ID;          // 填充设备ID
    tx_buffer[index++] = (uint8_t)num_args;            // 填充数据个数
    
    // 处理并填充可变参数
    va_list args;
    va_start(args, num_args);                // 初始化可变参数列表
    
    for (int i = 0; i < num_args; ++i)
    {
        int current_int = va_arg(args, int); // 获取下一个int参数

        memcpy(&tx_buffer[index], &current_int, sizeof(int)); // 使用memcpy简洁安全
        index += sizeof(int);
    }
    
    va_end(args);                           // 清理可变参数列表
    
    // 填充帧尾
    tx_buffer[index++] = PACKAGE_TAIL;
    
    // 通过UART发送数据包
    // HAL_UART_Transmit 会返回 HAL_OK, HAL_ERROR, HAL_BUSY, HAL_TIMEOUT
    if (HAL_UART_Transmit(COM_NUM, tx_buffer, packet_size, TIMEOUT) == HAL_OK) {
        return 1; // 发送成功，返回1
    }
    else {
        return 0; // 发送失败，返回0
    }
}



/**
  * @brief  串口接收完成回调函数
  * @param  huart: 发生中断的UART句柄
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3)
    {
        // 调用状态机处理接收到的字节
        ParsePackageByte(g_rx_byte);

        // 准备接收下一个字节
        HAL_UART_Receive_IT(COM_NUM, &g_rx_byte, 1);
    }
}


