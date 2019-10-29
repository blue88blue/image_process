#include <iostream>
#include "image_process.hpp"
#include <cmath>
using namespace std;

//读取bmp图像
bool Bmp_Image::read(char *image_dir)
{
    FILE *fp = fopen(image_dir,"rb");
    if (fp==0)
    {
        cout<< "no image file"<<endl;
        return 0;
    } 
    fread(&BITMAPFILEHEADER, 14, 1, fp);
    fread(&BITMAPINFOHEADER, 40, 1, fp);
    
    // RGBQUAD=new tagRGBQUAD[256];
    fread(RGBQUAD,4,256,fp);

    image_data = new unsigned char[BITMAPINFOHEADER\
    .biHight*(BITMAPINFOHEADER.biWidth)];

    fread(image_data,(int)BITMAPINFOHEADER\
    .biHight*(int)(BITMAPINFOHEADER.biWidth),1,fp);
    // cout<<BITMAPINFOHEADER.biWidth<< BITMAPINFOHEADER.biHight<<endl;
    
    fclose(fp);//关闭文件
    return true;
}

//保存bmp图像
bool Bmp_Image::save(char *image_name)
{
    FILE *fp = fopen(image_name,"wb");
    if (fp==0)
        return 0;

    fwrite(&BITMAPFILEHEADER, 14,1, fp);
    fwrite(&BITMAPINFOHEADER, 40, 1, fp);
    
    fwrite(RGBQUAD,4,256,fp);
    fwrite(image_data,(int)BITMAPINFOHEADER.\
    biHight*(int)(BITMAPINFOHEADER.biWidth),1,fp);

    cout<< "save "<<image_name<< " OK!"<<endl;
    fclose(fp);//关闭文件
    delete[] image_data;

    return true;
}

//伪彩色化
bool Bmp_Image::pseudo_color()
{
    int i=0,j=0,k=0;
    int H=0;
    float S=0.8,V=0;
    unsigned char *p_rgb;

    for(i=0; i<255; i++)
    {
        H=i;
        V=i/255.0;
        p_rgb = HSV2RGB(H,S,V);  //色度连续转换
        //更改调色板
        RGBQUAD[i].rgbRed = p_rgb[0];
        RGBQUAD[i].rgbGreen = p_rgb[1];
        RGBQUAD[i].rgbBlue = p_rgb[2];
    }
    return true;
}


//直方图均衡化
bool Bmp_Image::histogram_balanced()
{
    int line=0, column=0,i=0,j=0; 
    float sum_pixel=BITMAPINFOHEADER.biHight*(BITMAPINFOHEADER.biWidth);  //像素点总量
    float count[255]={0};  // 每个灰度值的概率
    float transfer[255]={0};
    for(line=0; line<BITMAPINFOHEADER.biHight; line++)
    {
        for(column=0; column<BITMAPINFOHEADER.biWidth; column++)
        {
            count[*(image_data+column+line*(BITMAPINFOHEADER.biWidth))]++;  //像素点计数
        }
    }
    for(i=0; i<255; i++)
    {   
        count[i] /= sum_pixel;  //概率
        for (j=0; j<=i; j++)  //对前i个概率求和
        {
            transfer[i] += count[j];  //点变换数组
        }
    }
    for(line=0; line<BITMAPINFOHEADER.biHight; line++)
    {
        for(column=0; column<BITMAPINFOHEADER.biWidth; column++)
        {
            // //灰度变换
            *(image_data+column+line*(BITMAPINFOHEADER.biWidth))=\
            transfer[*(image_data+column+line*(BITMAPINFOHEADER.biWidth))]*255;
        }
    }
    return true;

}

//二维离散傅里叶变换
cp* Bmp_Image::dft()
{
    int line=0,column=0,n=0;
    cp temp={0,0};
    cp* frequence_data_1 = new cp[BITMAPINFOHEADER.biHight*(BITMAPINFOHEADER.biWidth)]; 
    cp* frequence_data = new cp[BITMAPINFOHEADER.biHight*(BITMAPINFOHEADER.biWidth)]; 
    //对每一行分别做傅里叶变换
    for(line=0; line<BITMAPINFOHEADER.biHight; line++)
    {
        for(column=0; column<BITMAPINFOHEADER.biWidth; column++)
        {
            *(frequence_data_1+column+line*(BITMAPINFOHEADER.biWidth))={0,0};
            for(n=0; n<BITMAPINFOHEADER.biWidth; n++)  
            {
                temp = cos(2*M_PI*column*n/BITMAPINFOHEADER.biWidth)\
                -sin(2*M_PI*column*n/BITMAPINFOHEADER.biWidth)*1i;
                *(frequence_data_1+column+line*(BITMAPINFOHEADER.biWidth))+=\
                (double)*(image_data+n+line*(BITMAPINFOHEADER.biWidth))*temp;
            }
        }
    }
    //对每一列分别做傅里叶变换
    for(column=0; column<BITMAPINFOHEADER.biWidth; column++)
    {
        for(line=0; line<BITMAPINFOHEADER.biHight; line++)
        {
            *(frequence_data+column+line*BITMAPINFOHEADER.biWidth)={0,0};
            for(n=0; n<BITMAPINFOHEADER.biHight; n++)  
            {
                temp = cos(2*M_PI*line*n/BITMAPINFOHEADER.biHight)\
                -sin(2*M_PI*line*n/BITMAPINFOHEADER.biHight)*1i;
                *(frequence_data+column+line*BITMAPINFOHEADER.biWidth)+=\
                *(frequence_data_1+column+n*BITMAPINFOHEADER.biWidth)*temp;
            }
        }
    }
    delete[] frequence_data_1;
    return frequence_data;
}


//傅里叶逆变换 
unsigned char* Bmp_Image::idft(cp* frequence_data, char *image_name)
{
    int line=0,column=0,n=0;
    cp temp={0,0};
    cp pixel_temp;
    unsigned char *image = new unsigned char[BITMAPINFOHEADER.biHight*(BITMAPINFOHEADER.biWidth)]; 
    cp* frequence_data_1 = new cp[BITMAPINFOHEADER.biHight*(BITMAPINFOHEADER.biWidth)]; 
    //对每一行分别做逆傅里叶变换
    for(line=0; line<BITMAPINFOHEADER.biHight; line++)
    {
        for(column=0; column<BITMAPINFOHEADER.biWidth; column++)
        {
            pixel_temp={0,0};
            for(n=0; n<BITMAPINFOHEADER.biWidth; n++)  
            {
                temp = cos(2*M_PI*column*n/BITMAPINFOHEADER.biWidth)\
                +sin(2*M_PI*column*n/BITMAPINFOHEADER.biWidth)*1i; //逆变换负号变加号
                pixel_temp+=*(frequence_data+n+line*(BITMAPINFOHEADER.biWidth))*temp;
            }
            *(frequence_data_1+column+line*(BITMAPINFOHEADER.biWidth)) =\
            pixel_temp*(1.0/BITMAPINFOHEADER.biWidth);
        }
    }
    //对每一列分别做逆傅里叶变换
    for(column=0; column<BITMAPINFOHEADER.biWidth; column++)
    {
        for(line=0; line<BITMAPINFOHEADER.biHight; line++)
        {
            pixel_temp={0,0};
            for(n=0; n<BITMAPINFOHEADER.biHight; n++)  
            {
                temp = cos(2*M_PI*line*n/BITMAPINFOHEADER.biHight)\
                +sin(2*M_PI*line*n/BITMAPINFOHEADER.biHight)*1i;
                pixel_temp += *(frequence_data_1+column+n*BITMAPINFOHEADER.biWidth)*temp;
            }
            *(image+column+line*BITMAPINFOHEADER.biWidth)=pixel_temp.real()\
            /BITMAPINFOHEADER.biHight;
        }
    }
    delete[] frequence_data_1;

    //保存逆变换后的图像
    FILE *fp = fopen(image_name,"wb");
    if (fp==0)
        return 0;
    fwrite(&BITMAPFILEHEADER, 14,1, fp);
    fwrite(&BITMAPINFOHEADER, 40, 1, fp);
    fwrite(RGBQUAD,4,256,fp);
    fwrite(image,(int)BITMAPINFOHEADER.\
    biHight*(int)(BITMAPINFOHEADER.biWidth),1,fp);
    cout<< "save "<<image_name<< " OK!"<<endl;
    fclose(fp);

    return image;
}


//运动模糊
cp* Bmp_Image::motion_blur(cp* frequence_data, double a,double b, double T)
{
    cp* frequence_motion_blur =new cp[BITMAPINFOHEADER.biHight*(BITMAPINFOHEADER.biWidth)];
    double temp=0;
    cp H={0,0};
    int* uv;
    int line=0,column=0,u=0,v=0;

    for(line=0; line<BITMAPINFOHEADER.biHight; line++)
    {
        for(column=0; column<BITMAPINFOHEADER.biWidth; column++)
        {
            uv = conver_coordinates(line,column);
            v=*(uv);//纵
            u=*(uv+1);//横
            temp=M_PI*((double)u*a+(double)v*b);
            H = T*sin(temp)/temp*(cos(temp)-sin(temp)*1i);//退化函数
            // H=exp(-(u*u+v*v)/500);//高斯
            cout<<u<<endl;
            if (H.real()==0 && H.imag()==0)
            {
                *(frequence_motion_blur+column+line*(BITMAPINFOHEADER.biWidth))=\
                *(frequence_data+column+line*(BITMAPINFOHEADER.biWidth));
                break;
            }
            *(frequence_motion_blur+column+line*(BITMAPINFOHEADER.biWidth))=\
            H**(frequence_data+column+line*(BITMAPINFOHEADER.biWidth));
        }
    }
    return frequence_motion_blur;
}

//运动模糊复原
cp* Bmp_Image::wiener(cp* frequence_data, double a,double b, double T, double K)
{
    cp* deblur =new cp[BITMAPINFOHEADER.biHight*(BITMAPINFOHEADER.biWidth)];
    double temp=0,H_mode=0;
    cp H={0,0};
    int* uv;
    int line=0,column=0,u=0,v=0;

    for(line=0; line<BITMAPINFOHEADER.biHight; line++)
    {
        for(column=0; column<BITMAPINFOHEADER.biWidth; column++)
        {
            uv = conver_coordinates(line,column);
            v=*(uv);//纵
            u=*(uv+1);//横
            temp=M_PI*((double)u*a+(double)v*b);
            H = T*sin(temp)/temp*(cos(temp)-sin(temp)*1i);//退化函数
            H_mode = sqrt(H.real()*H.real()+H.imag()*H.imag());//退化函数的模
            H_mode *= H_mode;

            if (H.real()==0 && H.imag()==0)
            {
                *(deblur+column+line*(BITMAPINFOHEADER.biWidth))=\
                *(frequence_data+column+line*(BITMAPINFOHEADER.biWidth));
                break;
            }
            *(deblur+column+line*(BITMAPINFOHEADER.biWidth))=\
            *(frequence_data+column+line*(BITMAPINFOHEADER.biWidth))/H*H_mode/(H_mode+K);
        }
    }
    return deblur;
}



//保存频域幅度图像
bool Bmp_Image::save_frequence_image(cp* frequence_data,char *range_name)
{
    unsigned char* range_image = new unsigned char\
    [BITMAPINFOHEADER.biHight*BITMAPINFOHEADER.biWidth];
    float* temp = new float[BITMAPINFOHEADER.biHight*BITMAPINFOHEADER.biWidth];
    int i=0;
    int max=0;
    for(i=0; i<BITMAPINFOHEADER.biHight*BITMAPINFOHEADER.biWidth; i++)
    {
        temp[i] = sqrt(frequence_data[i].real()*frequence_data[i].real()+\
        frequence_data[i].imag()*frequence_data[i].imag());
        if (temp[i] >max)
        {
            max=temp[i]; 
        }
    }
    //灰度归一化到0-255
    for(i=0; i<BITMAPINFOHEADER.biHight*BITMAPINFOHEADER.biWidth; i++)
    {
        range_image[i] = (unsigned char)(temp[i]*255.0/max);
    }
    FILE *fp = fopen(range_name,"wb");
    if (fp==0)
        return 0;
    fwrite(&BITMAPFILEHEADER, 14,1, fp);
    fwrite(&BITMAPINFOHEADER, 40, 1, fp);
    fwrite(RGBQUAD,sizeof(RGBQUAD),256,fp);
    fwrite(range_image,(int)BITMAPINFOHEADER.\
    biHight*(int)BITMAPINFOHEADER.biWidth,1,fp);
    cout<< "save "<<range_name<< "OK!"<<endl;
    fclose(fp);//关闭文件
    delete[] range_image;
    return true;
}




//坐标转换，第一个为纵坐标
int* Bmp_Image::conver_coordinates(int line,int column)
{
    int* uv = new int[2];
    int mid_w=BITMAPINFOHEADER.biWidth/2;
    int mid_h=BITMAPINFOHEADER.biHight/2;
    if (line<=mid_h && column<=mid_w)//3
    {
        uv[0]=line;
        uv[1]=column;
    }
    else if(line<=mid_h && column>mid_w)//4
    {
        uv[0]=line;
        uv[1]=column-(BITMAPINFOHEADER.biWidth);
    }
    else if(line>mid_h && column<=mid_w)//1
    {
        uv[0]=line-(BITMAPINFOHEADER.biHight);
        uv[1]=column;
    }
    else
    {
        uv[0]=line-(BITMAPINFOHEADER.biHight);
        uv[1]=column-(BITMAPINFOHEADER.biWidth);
    }
    return uv;
}






//颜色空间转换  H:0~255, S:0~1, V:0~1
unsigned char* HSV2RGB(int H, float S, float V)
{
    static unsigned char rgb[3];
    float rgb_0[3];
    int h_i = (int)((float)H/42.55);

    float f = ((float)H/42.55)-h_i;
    float p = V*(1.0-S);
    float q = V*(1.0-f*S);
    float t = V*(1.0-(1.0-f)*S);

    if (S==0)
    {
        rgb[0] = (unsigned char)255*V;
        rgb[1] = (unsigned char)255*V;
        rgb[2] = (unsigned char)255*V; 
        return rgb;
    }
    switch(h_i)
    {
        case 0: rgb_0[0] = V; rgb_0[1] = t; rgb_0[2] = p;break;
        case 1: rgb_0[0] = q; rgb_0[1] = V; rgb_0[2] = p;break;
        case 2: rgb_0[0] = p; rgb_0[1] = V; rgb_0[2] = t;break;
        case 3: rgb_0[0] = p; rgb_0[1] = q; rgb_0[2] = V;break;
        case 4: rgb_0[0] = t; rgb_0[1] = p; rgb_0[2] = V;break;
        case 5: rgb_0[0] = V; rgb_0[1] = p; rgb_0[2] = q;break;
    }
    rgb[0] = (unsigned char)255*rgb_0[0];
    rgb[1] = (unsigned char)255*rgb_0[1];
    rgb[2] = (unsigned char)255*rgb_0[2];

    return rgb;
}