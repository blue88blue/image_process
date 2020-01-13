#include "image_process.hpp"
#include<iostream>
using namespace std;


//伪彩色化demo
bool demo_1(Bmp_Image img)
{
    char *image_file_1 = (char*)"../shanghai.bmp";
    char *image_file_2 = (char*)"../shanghai_color.bmp";

    if (!img.read(image_file_1)) //读取图片
        return false;
    img.pseudo_color();     //伪彩色化
    img.save(image_file_2); //保存图片
    return true;
}

//直方图均衡demo
bool demo_2(Bmp_Image img)
{
    char *image_file_1 = (char*)"../lenna.bmp";
    char *image_file_2 = (char*)"../lenna_balanced.bmp";

    if (!img.read(image_file_1)) //读取图片
        return false;
    img.histogram_balanced();   //直方图均衡
    img.save(image_file_2);     //保存图片
    return true;
}


//运动模糊demo
bool demo_3(Bmp_Image img)
{
    unsigned char* processed_image;
    cp* frequence_data;
    cp* frequence_motion_blur;
    char *image_file = (char*)"../book.bmp";
    char *spectrum_name_1 = (char*)"../book_F_1.bmp";
    char *spectrum_name_2 = (char*)"../book_F_2.bmp";
    char *final_name = (char*)"../book_blur.bmp";

    if (!img.read(image_file)) //读取图片
        return false;

    frequence_data = img.dft();//傅里叶变换
    img.save_frequence_image(frequence_data,spectrum_name_1);//保存频域幅度图

    frequence_motion_blur = img.motion_blur(frequence_data,-0.15,0.05,1);//运动模糊
    img.save_frequence_image(frequence_motion_blur,spectrum_name_2);//保存频域幅度图

    processed_image = img.idft(frequence_motion_blur,final_name);//逆变换并保存图片

    delete[] processed_image;
    delete[] frequence_data;
    delete[] frequence_motion_blur;
    return true;
}

//运动模糊复原demo
bool demo_4(Bmp_Image img)
{
    unsigned char* processed_image;
    cp* frequence_data;
    cp* frequence_motion_blur;
    char *image_file = (char*)"../book_blur.bmp";
    char *spectrum_name_1 = (char*)"../book_blur_F_11.bmp";
    char *spectrum_name_2 = (char*)"../book_blur_F_22.bmp";
    char *final_name = (char*)"../book_blur_restore.bmp";

    if (!img.read(image_file)) //读取图片
        return false;

    frequence_data = img.dft();//傅里叶变换
    // img.save_frequence_image(frequence_data,spectrum_name_1);//保存频域幅度图

    frequence_motion_blur = img.wiener_deblur(frequence_data, -0.15,0.05,1, 0.001);//维纳滤波
    img.save_frequence_image(frequence_motion_blur,spectrum_name_2);//保存频域幅度图

    processed_image = img.idft(frequence_motion_blur,final_name);//逆变换并保存图片

    delete[] processed_image;
    delete[] frequence_data;
    delete[] frequence_motion_blur;
    return true;
}

//形态学处理，去噪
bool demo5(Bmp_Image &img)
{
    char *image_file_1 = (char*)"../mipav.bmp";
    char *image_file_2 = (char*)"../mipav_clean.bmp";

    if (!img.read(image_file_1)) //读取图片
        return false;
    img.binarization(100); //二值化
    img.corrosion();
    img.expansion();
    img.expansion();
    img.corrosion();
    img.save(image_file_2);
    return true;
}

//形态学处理，边缘提取
bool demo6(Bmp_Image &img)
{
    char *image_file_1 = (char*)"../hourse.bmp";
    char *image_file_2 = (char*)"../hourse_edge.bmp";

    if (!img.read(image_file_1)) //读取图片
        return false;
    img.binarization(120); //二值化
    img.edge();
    img.save(image_file_2);
    return true;
}


//形态学处理，骨架提取
bool demo7(Bmp_Image &img)
{
    char *image_file_1 = (char*)"../hourse.bmp";
    char *image_file_2 = (char*)"../hourse_sk.bmp";

    if (!img.read(image_file_1)) //读取图片
        return false;
    img.binarization(200); //二值化
    img.K3M1();
    img.save(image_file_2);
    return true;
}

bool demo8(Bmp_Image &img)
{
    char *image_file_1 = (char*)"../tr.bmp";
    char *image_file_2 = (char*)"../edge.bmp";



    if (!img.read(image_file_1)) //读取图片
        return false;

    img.conv();
    img.binarization(230); //二值化
    img.save(image_file_2);

    return true;
}

bool demo9(Bmp_Image &img)
{
    char *image_file_1 = (char*)"../edge.bmp";
    char *image_file_2 = (char*)"../result.bmp";

    Circle *circles = new Circle[10];

    if (!img.read(image_file_1)) //读取图片
        return false;

    circles = img.Hough_circles(50,200,0,10);
    cout<< "worked"<<endl;
    //画圆
    for (int i=0; i< 6; i++)
    {
        img.circle(circles[i]);
    }

    img.save(image_file_2);
    return true;
}





/********************************main********************************/
int main(int argc, char *argv[])
{   
    Bmp_Image img;
    demo8(img);
    demo9(img);
}



