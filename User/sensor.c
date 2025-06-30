#include "sensor.h"


float imu_angle[3];	//�洢�Ƕ�ֵ

float gyro_offset_x = 0.0f;
float gyro_offset_y = 0.0f;
float gyro_offset_z = 0.0f;

/* �������������� */
void imuUpdata(void) {
	nowtime++;
}

/* ��������ƫ���㺯�� */
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
        HAL_Delay(10); // ÿ�β���֮��������ʱ���������������ݸ����ʵ���
    }

    // ����ƽ��ֵ��ע��ǿ������ת��Ϊ���������г�������
    gyro_offset_x = (float)gyro_sum_x / average_count;
    gyro_offset_y = (float)gyro_sum_y / average_count;
    gyro_offset_z = (float)gyro_sum_z / average_count;

    printf("Gyro calibration finished.\n");
    printf("Offset X: %.2f, Offset Y: %.2f, Offset Z: %.2f\n", gyro_offset_x, gyro_offset_y, gyro_offset_z);
}

/* ��ȡ��������ƫ�������ֵ */
void read_gyro_corrected(float* cgx, float* cgy, float* cgz) {
    int16_t raw_gx, raw_gy, raw_gz;

    // ��ȥ��ƫֵ����У��
    *cgx = (float)imu_angle[0] - gyro_offset_x;
    *cgy = (float)imu_angle[1] - gyro_offset_y;
    *cgz = (float)imu_angle[2] - gyro_offset_z;
}