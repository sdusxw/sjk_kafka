#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <stdio.h>
#include <libyuv.h>
#include "jpg_codec.h"

using namespace std;

#include <sys/time.h>

#include <time.h>
#include <unistd.h>

double t1 =0;
double t2 = 0;
double  getCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return (double)tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
}

void starttime()
{
    t1 = getCurrentTime();
}


void endtime(char *title)
{
    t2  = getCurrentTime ();
    double timeuse = t2 -t1 ;
    
    printf ("title  %s usetime =%2.2f ms\n" ,title,timeuse);
    
}

int main(int argc, char ** argv)
{
    if(argc < 2)
    {
        cout << argv[0] << " filename" << endl;
        return 0;
    }
    
        cout<<"LPR_ALG init OK"<<endl;
        std::ifstream file(argv[1], std::ios::in | std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            fprintf(stderr, "open file fail: %s\n", argv[1]);
            return -1;
        }
        
        std::streampos size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::string buffer(size, ' ');
        file.read(&buffer[0], size);
        file.close();
    
    char * argb_data=(char *)malloc(4096*2160*4);
    EyeJpegCodec ejc;
    int c=4;
    int w=0, h=0;
    starttime();
    bool ret = ejc.JpegUnCompress((const char *)buffer.c_str(), size, (char *)argb_data,
                                  4096*2160*3, w, h, c);
    endtime("jpeg decode");
    if(ret)
    {
        cout << "w:\t" << w << "\th:\t" << h << endl;
        uint8_t *y_data=(uint8_t *)malloc(w*h);
        uint8_t *uv_data=(uint8_t *)malloc(w*h/2);
        starttime();
        libyuv::ARGBToNV12((const uint8_t*)argb_data, w*c, y_data, w, uv_data, w, w, h);
        endtime("jpeg -> nv12");
        FILE * outfile;
        starttime();
        outfile = fopen("p.yuv", "wb" );
        fwrite(y_data, w*h, 1, outfile);
        fwrite(uv_data, w*h/2, 1, outfile);
        fclose(outfile);
        endtime("save yuv");
    }
    else
        cout << "jpeg decode error!" << endl;
}
/*
 LIBYUV_API
 int ARGBToNV12(const uint8_t* src_argb,
 int src_stride_argb,
 uint8_t* dst_y,
 int dst_stride_y,
 uint8_t* dst_uv,
 int dst_stride_uv,
 int width,
 int height);
 */
/*
int main(int argc, char ** argv)
{
    if(argc < 2)
    {
        cout << argv[0] << " filename" << endl;
        return 0;
    }
    if(vlpr_init())
    {
        cout<<"LPR_ALG init OK"<<endl;
        std::ifstream file(argv[1], std::ios::in | std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            fprintf(stderr, "open file fail: %s\n", argv[1]);
            return -1;
        }
        
        std::streampos size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::string buffer(size, ' ');
        file.read(&buffer[0], size);
        file.close();
        VPR vpr;
        for(int i=0;i<100;i++)
        {
            clock_t t=clock();
            if(vlpr_analyze((const unsigned char *)buffer.c_str(), (int)size, &vpr))
            {
                cout << "Time\t" << (clock()-t)/1000 << " ms" << endl;
                cout << "OK\t" << vpr.license << "\t" << vpr.color << "\t" << vpr.nColor << "\t" << vpr.nConfidence << endl;
                cout << "Coor\t"  << vpr.left << "\t" << vpr.top << "\t" << vpr.right << "\t" << vpr.bottom << endl;
            }else{
                cout << "Time\t" << (clock()-t)/1000 << " ms" << endl;
                cout << "Fail" << endl;
            }
        }
        
    }else{
        cout<<"LPR_ALG init Fail, Exit"<<endl;
    }
    
    return 0;
}
*/
