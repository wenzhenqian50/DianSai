#include "sensor.h"


float imu_angle[3];	//存储角度值

float gyro_offset_x = 0.0f;
float gyro_offset_y = 0.0f;
float gyro_offset_z = 0.0f;

/* 陀螺仪心跳函数 */
void imuUpdata(void) {
	nowtime++;
}

/* 陀螺仪零偏计算函数 */
void calibrate_gyro(uint16_t sample_count) {
    long long gyro_sum_x = 0;
    long long gyro_sum_y = 0;
    long long gyro_sum_z = 0;

    printf("Gyro calibration started. Do not move the device...\n");
	
	HAL_Delay((int)(sample_count * 0.9f) * 10);
	
	int average_count = (int)(sample_count * 0.1f);
    for (uint16_t i = 0; i < average_count; i++) {
        gyro_sum_x += imu_angle[0];
        gyro_sum_y += imu_angle[1];
        gyro_sum_z += imu_angle[2];
        HAL_Delay(10); // 每次采样之间稍作延时，根据陀螺仪数据更新率调整
    }

    // 计算平均值，注意强制类型转换为浮点数进行除法运算
    gyro_offset_x = (float)gyro_sum_x / average_count;
    gyro_offset_y = (float)gyro_sum_y / average_count;
    gyro_offset_z = (float)gyro_sum_z / average_count;

    printf("Gyro calibration finished.\n");
    printf("Offset X: %.2f, Offset Y: %.2f, Offset Z: %.2f\n", gyro_offset_x, gyro_offset_y, gyro_offset_z);
}

/* 获取陀螺仪零偏补偿后的值 */
void read_gyro_corrected(float* cgx, float* cgy, float* cgz) {
    int16_t raw_gx, raw_gy, raw_gz;

    // 减去零偏值进行校正
    *cgx = (float)imu_angle[0] - gyro_offset_x;
    *cgy = (float)imu_angle[1] - gyro_offset_y;
    *cgz = (float)imu_angle[2] - gyro_offset_z;
}