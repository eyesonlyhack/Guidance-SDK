#include <SoftwareSerial.h>
#include "protocal_uart_sdk.h"
#include "DJI_guidance.h"

SoftwareSerial mySerial(2, 1); // RX, TX

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("startup!");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(115200);
}

void loop() 
 {   
  for ( int i = 0; i < 1000; ++i )
  {
    char datat[1000] = {0};
    int max_size = (int)sizeof(datat);
    int timeout = 1000;
    Serial.setTimeout(timeout);
    int n = Serial.readBytes(datat, max_size) ;
    if( n <= 0 )
    {
    	continue;
    }
    
    unsigned char data[1000] = {0};
    for (int j = 0; j < (int)sizeof(datat); j++)
      data[j] = datat[j];
    
    push( data, sizeof(data) );
    for ( ; ; )
    {
      unsigned int len = 0;
      int has_packet = pop( data, len );
      if ( has_packet )
      {
        if ( len )
        {
          unsigned char cmd_id = data[1];
          if ( e_imu == cmd_id )
          {
            imu imu_data;
            memcpy( &imu_data, data + 2, sizeof(imu_data) );
            printf( "imu:%f %f %f,%f %f %f %f\n", imu_data.acc_x, imu_data.acc_y, imu_data.acc_z, 
            imu_data.q[0], imu_data.q[1], imu_data.q[2], imu_data.q[3] );
            printf( "frame index:%d,stamp:%d\n", imu_data.frame_index, imu_data.time_stamp );
            printf( "\n" );
          }
          if ( e_ultrasonic == cmd_id )
          {
            ultrasonic_data ultrasonic;
            memcpy( &ultrasonic, data + 2, sizeof(ultrasonic) );
            for ( int d = 0; d < CAMERA_PAIR_NUM; ++d )
            {
              printf( "distance:%f,reliability:%d\n", ultrasonic.ultrasonic[d] * 0.001f, (int)ultrasonic.reliability[d] );
            }
            printf( "frame index:%d,stamp:%d\n", ultrasonic.frame_index, ultrasonic.time_stamp );
            printf( "\n" );
          }
          if ( e_velocity == cmd_id )
          {
            velocity vo;
            soc2pc_vo_can_output output;
            memcpy( &output, data + 2, sizeof(vo) );
            vo.vx = output.m_vo_output.vx;
            vo.vy = output.m_vo_output.vy;
            vo.vz = output.m_vo_output.vz;
            printf( "vx:%f vy:%f vz:%f\n", 0.001f * vo.vx, 0.001f * vo.vy, 0.001f * vo.vz );
            printf( "frame index:%d,stamp:%d\n", vo.frame_index, vo.time_stamp );
            printf( "\n" );
          }
          if ( e_obstacle_distance == cmd_id )
          {
            obstacle_distance oa;
            memcpy( &oa, data + 2, sizeof(oa) );
            printf( "obstacle distance:" );
            for ( int direction = 0; direction < CAMERA_PAIR_NUM; ++direction )
            {
              printf( " %f ", 0.01f * oa.distance[direction] );
            }
            printf( "\n" );
            printf( "frame index:%d,stamp:%d\n", oa.frame_index, oa.time_stamp );
            printf( "\n" );
          }
        }
        else
        {
          printf( "err\n" );
        }
      }
      else
      {
        break;
      }
    }
  }

}
