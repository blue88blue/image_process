#ifndef  __IMAGE_PROCESS__
#define  __IMAGE_PROCESS__

#include<complex>
extern "C" {
#include "image_tool.h"
}
using namespace std;

#define cp complex<double>


// 结构体的长度必定为最大成员长度的倍数，以4个字节为一个单元
struct tagBITMAPFILEHEADER
{
    unsigned short bfType;  //文件类型，必须是字符串“BM”
    unsigned int bfSize;  //指定文件大小
    unsigned short bfReserved1;  //保留字
    unsigned short bfReserved2;  //保留字
    unsigned int bfOffBits;  //从文件头到位图数据的偏移字节
};

struct tagBITMAPINFOHEADER
{
    unsigned int biSize; //该结构长度 40字节
    unsigned int biWidth; // 图像宽度
    unsigned int biHight; //图像高度
    unsigned short biPlanes; //必须为1
    unsigned short biBitCount; //颜色位数,如1,4,8
    unsigned int biCompression; //压缩类型
    unsigned int biSizeImage; //实际位图数据占用的字节
    unsigned int biXPelsPerMeter; //水平分辨率 
    unsigned int biYPelsPerMeter;  //垂直分辨率 
    unsigned int biClrUsed;  //实际使用颜色数
    unsigned int biClrImportant;  //重要的颜色数 
};

struct tagRGBQUAD
{
    unsigned char rgbBlue; 
    unsigned char rgbGreen;
    unsigned char rgbRed;
    unsigned char rgbReserved;
};


struct Circle
{
    int center_line; 
    int center_column;
    int r;
    int score;
};

class Bmp_Image
{
    private:
        
        tagBITMAPFILEHEADER  BITMAPFILEHEADER;
        
    public:
        unsigned char *image_data;
        tagBITMAPINFOHEADER  BITMAPINFOHEADER;
        tagRGBQUAD *RGBQUAD;

        bool read(char *image_dir);//图片读取
        bool save(char *image_name);//图片保存
        bool pseudo_color();//伪彩色化
        bool histogram_balanced();//直方图均衡
        cp* dft();//傅里叶变换
        unsigned char* idft(cp* frequence_data,char *image_name);//傅里叶逆变换
        bool save_frequence_image(cp* frequence_data,char *range_name);//保存频域幅度图
        cp* motion_blur(cp* frequence_data, double a,double b, double T);//运动模糊
        cp* wiener_deblur(cp* frequence_data, double a,double b, double T, double K);//运动模糊复原
        bool binarization(int threshold); //二值化
        bool K3M1();  //骨架提取 二值图
        bool corrosion();  //腐蚀  二值图
        bool expansion();  //膨胀  二值图
        bool edge();    //边缘提取  二值图
        void roberts();
        void conv();
        Circle* Hough_circles(int r_min, int r_max, int threshold, int circles_number); // 霍夫圆变换
        void circle(Circle a);
};


unsigned char* HSV2RGB(int H, float S, float V);//颜色空间转换



#endif
