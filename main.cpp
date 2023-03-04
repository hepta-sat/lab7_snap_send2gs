#include "mbed.h"
#include "HEPTA_EPS.h"
#include "HEPTA_CDH.h"
#include "HEPTA_SENSOR.h"
#include "HEPTA_COM.h"

Serial pc(USBTX,USBRX);
HEPTA_EPS eps(p16,p26);
HEPTA_CDH cdh(p5,p6,p7,p8,"sd");
HEPTA_SENSOR sensor(p17, 
                    p28,p27,0xD0,0x18,
                    p13,p14,p25,p24);
HEPTA_COM com(p9,p10);

int main()
{
    pc.baud(9600);
    int rcmd=0,cmdflag=0;
    FILE *dummy = fopen("/sd/dummy.txt","w");
    if(dummy == NULL) {
        error("Could not open file for write\r\n");
    }
    fclose(dummy);
    
    while(1)
    {
        wait(0.2);
        com.xbee_receive(&rcmd,&cmdflag);
        // pc.printf("rcmd=%d, cmdflag=%d\r\n",rcmd, cmdflag);
        if (cmdflag == 1) {
            pc.printf("Command Get %d\r\n",rcmd);
            if(rcmd == 'a'){
                com.printf("CAMERA\r\n");
                sensor.Sync();
                sensor.initialize(HeptaCamera_GPS::Baud115200, HeptaCamera_GPS::JpegResolution320x240);
                sensor.test_jpeg_snapshot_data("/sd/test.txt");
                FILE *fp = fopen("/sd/test.txt", "r");
                if(fp == NULL) {
                    pc.printf("Could not open file for write\r\n");
                } else {
                    char str[1024];
                    com.printf("INICIO\r\n");
                    while((fgets(str,256,fp))!=NULL) {
                        com.printf("%s",str);
                        wait(0.001);
                    }
                    com.printf("FIN\r\n");
                    pc.printf("Data transmitting finished:)\r\n");
                    fclose(fp);
                }
            }
            com.initialize();
        }
    }
}
