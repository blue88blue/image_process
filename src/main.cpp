#include "image_process.hpp"
#include<iostream>
using namespace std;

int main(int argc, char *argv[])
{
    Bmp_Image img;
    unsigned char* processed_image;
    cp* frequence_data;
    cp* frequence_motion_blur;
    char *image_file = (char*)"../lena512.bmp";
    char *frequence_name = (char*)"../lena512_frequence.bmp";
    char *processed_name = (char*)"../lena512_processed.bmp";

    img.read(image_file);
    //傅里叶变换
    frequence_data = img.dft();
    //保存频域幅度图
    img.save_frequence_image(frequence_data,frequence_name);
    //运动模糊
    frequence_motion_blur = img.motion_blur(frequence_data,0.1,0.1,100);
    //复原
    // frequence_motion_blur = img.wiener(frequence_data,0.4,0.1,1,0.1);
    //逆变换并保存逆变换后的图片
    processed_image = img.idft(frequence_motion_blur,processed_name);


    delete[] processed_image;
    delete[] frequence_data;
    return 0;
}














