#include "shell.h"

/* 系统参数 */
#define SHELL_BUFFER_SIZE  64	    // 命令缓冲区大小
#define MAX_ARGS           8		// 最大参数个数
#define EEPROM_SIZE        32767    // EEPROM大小 单位:字节

/* 系统局部变量 */
static char ShellRxBuf[SHELL_BUFFER_SIZE];
static bool CmdReady = false; 		// 命令准备好被处理
static uint16_t SystemVoltageSample = 0;	// 系统电压采样值

/* 结构体变量指针定义 */
typedef void (*cmd_handler_t)(int argc, char *argv[]);

typedef struct {
    const char *name;
    const char *help;
    cmd_handler_t handler;
} shell_command_t;

/* --------------------命令回调函数声明-------------------- */
static void cmd_handler_help(int argc, char *argv[]);
static void cmd_handler_reset(int argc, char *argv[]);
static void cmd_handler_gpio(int argc, char *argv[]);
static void cmd_handler_runtime(int argc, char *argv[]);
static void cmd_handler_test(int argc, char *argv[]);
static void cmd_handler_eeprom(int argc, char *argv[]);
static void cmd_handler_pid(int argc, char *argv[]);
static void cmd_handler_voltage(int argc, char *argv[]);
//static void cli_parse_and_execute(char *input_str);
static void CmdProcess(char *str);


/* --------------------命令注册表-------------------- */
static const shell_command_t command_table[] = {
    {"help",       "Show This Help Message",        		     cmd_handler_help},
    {"reset",      "Reset The System",              		     cmd_handler_reset},
    {"gpio",       "Control GPIO: gpio gpio_pin [h|l]",          cmd_handler_gpio},
	{"runtime",    "Show RunTime: runtime [h|m|s]", 		     cmd_handler_runtime},
	{"test",       "Used for Debugging",						 cmd_handler_test},
    {"eeprom",     "Operate EEPROM: eeprom [r|w] addr [data]",   cmd_handler_eeprom},
    {"pid",        "Adjust PID Param: pid [p|i|d] value",        cmd_handler_pid},
	{"voltage",     "Get Power Voltage",						 cmd_handler_voltage},	
};
static const int command_table_size = sizeof(command_table) / sizeof(shell_command_t);

/* --------------------用户实现函数-------------------- */

/* eeprom写 */
static void EEPROM_WRITE(uint16_t addr, uint8_t data) {
	
}

/* eeprom读 */
static uint8_t EEPROM_READ(uint16_t addr) {
	uint8_t data = 0;
	
	return data;
}

/* 设置PID参数 */
static void SET_PID(uint8_t select, float value) {
    switch(select) {
        case 0: {

        }
        break;
        case 1: {

        }
        break;
        case 2: {

        }
    }
}

/* --------------------公共函数-------------------- */

static void icon(void) {
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@..@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%%#*+=-:.... .@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@......@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%%%%@-.            .@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@..@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#=:  -*=            :@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@..@@@@@@@@@@@@@@@@@@@@%%*-        =#=          +@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@......@@@@@@@@@@@@@@@%%*:             =#=        %%@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@..@@@@@@@@@@@@@@@#-     .::-::       =#=     +@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*:     =#@@@@@@@#=      =#+. :@@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+.     :%%@@@@@@@@@@@%%-      =#=#@@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@..@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*.      .@@@@@@@@@@@@@@@:   .   %%@@@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@......@@@@@@@@@@@@@@@@@@@%%*+=-==+#@*:        +@@@@@@@@@@@@@@@+      *@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@..@@@@@@@@@@@@@@@@@@@%%-       -#-          -@@@@@@@@@@@@@@@=     #@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*       .**.            +@@@@@@@@@@@@@*    .%%@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@-       :%%=               -#@@@@@@@@@#-    -@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@#.       =%%.                  .-+***+-.    .*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@=        +#                                =%%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@%%:        *#                               :#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@*        .+#                              :*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@@@@@@%%-  :-=+*#%%@@:                            :*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@@@@@@#*%%@@@@@@@@@@@#:                        -#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@%%*+==+*@@#:                   :+*#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@#-        :*@#:              .=*+- .@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@%%-            .*@*:         :=*+-.    %%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@%%.               :@@*:   :=+*+-.      .@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@.                 -@@@**+=-.         .#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@@@@@@@- =+               .@@@@#           :+@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@@@@@@#:%%@-            .  *@@@@:        .=#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@@@@@@#@@*      .:  .   .*@@@@*      .=#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@%%    -*%%@%%    :+@@@@@@:   =#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@@@@@@@%%. -*@@@@*..-+#@@@@@@@+ .-*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@@@@@@@++@@@@@@#*%%@@@@@@@@@@@=*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
}

static void SystemLog(void) {
	printf("|--------------------------------------------MUSK SHELL--------------------------------------------|\r\n");
	printf("|--------------------------------------------------------------------------------------------------|\r\n");
	printf("|----------------------------------------Athor:Wen Zhenqian----------------------------------------|\r\n");
	printf("|--------------------------------------------------------------------------------------------------|\r\n");
	printf("|-------------------------------------------Version:0.0.1------------------------------------------|\r\n");
}

/* 初始化 */
void ShellInit(void) {
	icon();
	SystemLog();
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1,(uint8_t *)ShellRxBuf,SHELL_BUFFER_SIZE);
	HAL_ADC_Start_DMA(&hadc1,(uint32_t *)&SystemVoltageSample, 1);
}


/* 运行命令解释器 */
void ShellRun(void) {
    if (CmdReady) {
        CmdProcess(ShellRxBuf);
        CmdReady = false; 	// 清除标志
		memset(ShellRxBuf, 0, SHELL_BUFFER_SIZE);
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1,(uint8_t *)ShellRxBuf,SHELL_BUFFER_SIZE);		// 继续接收命令
    }
}



/* 命令解析器 */
static void CmdProcess(char *str) {
    char *argv[MAX_ARGS];
    int argc = 0;

    char *token = strtok(str, " ");
    while (token != NULL && argc < MAX_ARGS) {
        argv[argc++] = token;		// 命令切片
        token = strtok(NULL, " ");
    }

    if (argc == 0) {
        return;
    }

    for (int i = 0; i < command_table_size; i++) {
        if (strcmp(argv[0], command_table[i].name) == 0) {
            command_table[i].handler(argc, argv);		// 与命令表命令逐一匹配
            return;
        }
    }
    printf(">Error: Command \"%s\" not found.\r\n", argv[0]);
}

/* 字符串转浮点数 */
double StringToDouble(char* str, int* success_flag) {
    if (str == NULL || str[0] == '\0') {
        if (success_flag) *success_flag = 0;
        return NAN; 
    }

    char* endptr; 
    double result;

    errno = 0;

    // 核心转换
    result = strtod(str, &endptr);

    if (errno == ERANGE) {
        if (success_flag) *success_flag = 0;
        return result;
    }

    if (endptr == str) {
        if (success_flag) *success_flag = 0;
        return NAN;
    }

    while (isspace((unsigned char)*endptr)) {
        endptr++;
    }

    if (*endptr != '\0') {
        if (success_flag) *success_flag = 0;
        return NAN;
    }

    if (success_flag) *success_flag = 1;
    return result;
}

/* --------------------回调函数-------------------- */

/* 打印命令表 */
static void cmd_handler_help(int argc, char *argv[]) {
    printf("Available Commands:\r\n");
    for (int i = 0; i < command_table_size; i++) {
        printf("  %-10s - %s\r\n", command_table[i].name, command_table[i].help);
    }
}

/* 系统复位重启 */
static void cmd_handler_reset(int argc, char *argv[]) {
    printf("System Reseting...\r\n");
    HAL_Delay(100); // 留出时间让串口发送完毕
	__set_FAULTMASK(1);
    HAL_NVIC_SystemReset();
}

/* 操作IO端口电平 */
static void cmd_handler_gpio(int argc, char *argv[]) {
    if (argc < 3) {
        printf(">Usage: gpio gpio_port&gpio_pin [h|l]\r\n");
        return;
    }
	
	if(!strcmp(argv[1], "pc8")) {
		if (!strcmp(argv[2], "h")) {
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);
			printf(">GPIO is HIGH\r\n");
		} else if (!strcmp(argv[2], "l")) {
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);
			printf(">GPIO is LOW\r\n");
		} else {
			printf(">Invalid argument.\r\n");
		}
	}
	else {
		printf(">Invalid argument.\r\n");
	} 
}

/* 获取系统运行时间 */
static void cmd_handler_runtime(int argc, char *argv[]) {
	int runtime = 0;
	int hours   = 0;
	int minutes = 0;
	int seconds = 0;
	
	runtime = (int)(uwTick / 1000.0f);
	hours   = runtime / 3600;
	runtime = runtime % 3600;
	minutes = runtime / 60;
	runtime = runtime % 60;
	seconds = runtime;
	
	if(argc == 1) {
		printf(">System RunTime:%02dh%02dm%02ds\r\n",hours,minutes,seconds);
	}
	else if(!strcmp(argv[1],"h")) {
		printf(">System RunTime:%02dh\r\n",hours);
	}
	else if(!strcmp(argv[1],"m")) {
		printf(">System RunTime:%02dm\r\n",hours * 60 + minutes);
	}
	else if(!strcmp(argv[1],"s")) {
		printf(">System RunTime:%02ds\r\n",hours * 3600 + minutes * 60 + seconds);
	}
	else {
		printf(">Invalid argument.\r\n");
	}
}

/* 操作eeprom进行读写 */
static void cmd_handler_eeprom(int argc, char *argv[]) {
    int addr = 0;
    int data = 0;
    int flag = 0;

    if(argc == 4) {
        if(!strcmp(argv[1],"w")) {          // 写操作
            addr = StringToDouble(argv[2], &flag);
            if(!flag || addr < 0 || addr > EEPROM_SIZE) {
                printf(">Invalid Address.\r\n");
                return;
            }
            data = StringToDouble(argv[3], &flag);
            if(!flag || data < 0 || data > 255) {
                printf(">Invalid Data.\r\n");
                return;
            }
            /* -----eeprom write function----- */
            EEPROM_WRITE(addr,data);
            printf(">EEPROM WRITE SUCCESS.\r\n");
        }
        else {
            printf(">Invalid argument.\r\n");
        }
    }
    else if(argc == 3) {
        if(!strcmp(argv[1],"r")) {     // 读操作
            addr = StringToDouble(argv[2], &flag);
            if(!flag || addr < 0 || addr > EEPROM_SIZE) {
                printf(">Invalid Address.\r\n");
                return;
            }
            /* -----eeprom read function----- */
            data = EEPROM_READ(addr);
            printf(">The value of the %d cell is %d\r\n",addr,data);
        }
        else {
            printf(">Invalid argument.\r\n");
        } 
    }
    else {
        printf(">Invalid argument.\r\n");
    }
}

/* 调整PID函数 */
static void cmd_handler_pid(int argc, char *argv[]) {
    uint8_t times  = 0;
    uint8_t select = 0;
    float   value  = 0;
	int     flag   = 0;

    

    if(argc == 3 || argc == 5 || argc == 7) {
        times = (argc - 1) / 2;
        for(uint8_t i = 0; i < times; i++) {
            if(!strcmp(argv[1 + i * 2],"p")) {
                select = 0;
                value  = StringToDouble(argv[(i + 1) * 2], &flag);
                if(flag) {
                    SET_PID(select,value);
                    printf(">P Set Success.\r\n");
                }
                else {
                    printf(">Invalid P.\r\n");
                    return;
                }
            }
            else if(!strcmp(argv[1 + i * 2],"i")) {
                select = 1;
                value  = StringToDouble(argv[(i + 1) * 2], &flag);
                if(flag) {
                    SET_PID(select,value);
                    printf(">I Set Success.\r\n");
                }
                else {
                    printf(">Invalid I.\r\n");
                    return;
                }
            }
            else if(!strcmp(argv[1 + i * 2],"d")) {
                select = 2;
                value  = StringToDouble(argv[(i + 1) * 2], &flag);
                if(flag) {
                    SET_PID(select,value);
                    printf(">D Set Success.\r\n");
                }
                else {
                    printf(">Invalid D.\r\n");
                    return;
                }
            }
            else {
                printf(">Invalid argument.\r\n");
            }           
        }
    }
    else {
        printf(">Invalid argument.\r\n");
    }
}

/* 获取系统供电电压 */
static void cmd_handler_voltage(int argc, char *argv[]) {
	float voltage = 0;
	
	voltage = 36.3f * SystemVoltageSample / 4095.0f;
	printf("Power Voltage:%.2f\r\n",voltage);
}

/* 回调测试函数 */
static void cmd_handler_test(int argc, char *argv[]) {
	int flag = 0;
	double num = 0;
	
	if(argc == 2) {
		num = StringToDouble(argv[1],&flag);
		if(flag) {
			printf("Number:%f\r\n",num);
		}
		else {
			printf("The Number is Error!\r\n");
		}
	}
}

/* --------------------系统函数-------------------- */

/* 串口重定向 */
int fputc(int ch, FILE *f) {
	HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,HAL_MAX_DELAY);
	return ch;
}

/* 串口空闲中断回调 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
	if(huart->Instance == USART1) {
		CmdReady = true;
	}
}


