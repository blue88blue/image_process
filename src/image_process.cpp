#include <iostream>
#include "image_process.hpp"
#include <cmath>
#include<string.h>
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

    cout<< "save OK! "<<image_name<<endl;
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
                (double)*(image_data+n+line*(BITMAPINFOHEADER.biWidth))*pow(-1,n+line+2)*temp;
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
            *(image+column+line*BITMAPINFOHEADER.biWidth)=pixel_temp.real()*pow(-1,column+line+2)\
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
    cout<< "save OK! "<<image_name<<endl;
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
            u = line-BITMAPINFOHEADER.biHight/2.0;
            v = column-BITMAPINFOHEADER.biWidth/2.0;
            temp=M_PI*((double)u*a+(double)v*b);
            if(fabs(temp)<1e-5)
            {
                if (temp>0)
                    temp=1e-5;
                else
                    temp=-1e-5;
            }

            H = T*sin(temp)/temp*(cos(temp)-sin(temp)*1i);//运动模糊退化函数
            // H=exp(-(u*u+v*v)/800);//高斯低通
            // H = exp(-0.0025*pow((u*u+v*v),5.0/6.0));//大气湍流退化函数

            if (H.real()==0 && H.imag()==0)
            {
                *(frequence_motion_blur+column+line*(BITMAPINFOHEADER.biWidth))=\
                *(frequence_data+column+line*(BITMAPINFOHEADER.biWidth));
                continue;
            }
            *(frequence_motion_blur+column+line*(BITMAPINFOHEADER.biWidth))=\
            H**(frequence_data+column+line*(BITMAPINFOHEADER.biWidth));
        }
    }
    return frequence_motion_blur;
}

//运动模糊复原
cp* Bmp_Image::wiener_deblur(cp* frequence_data, double a,double b, double T, double K)
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
            u = line-BITMAPINFOHEADER.biHight/2.0;
            v = column-BITMAPINFOHEADER.biWidth/2.0;
            temp=M_PI*((double)u*a+(double)v*b);
            if(fabs(temp)<1e-5)
            {
                if (temp>0)
                    temp=1e-5;
                else
                    temp=-1e-5;
            }
            H = T*sin(temp)/temp*(cos(temp)-sin(temp)*1i);//运动模糊退化函数
            H_mode = sqrt(H.real()*H.real()+H.imag()*H.imag());//退化函数的模
            H_mode *= H_mode;

            if (fabs(H.real())<1e-6 && fabs(H.imag())<1e-6)
            {
                *(deblur+column+line*(BITMAPINFOHEADER.biWidth))=\
                *(frequence_data+column+line*(BITMAPINFOHEADER.biWidth));
                continue;
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
        temp[i] = log2(temp[i]+1);//对数变换
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
    fwrite(RGBQUAD,4,256,fp);
    fwrite(range_image,(int)BITMAPINFOHEADER.\
    biHight*(int)BITMAPINFOHEADER.biWidth,1,fp);
    cout<< "save OK! "<<range_name<< endl;
    fclose(fp);//关闭文件

    delete[] range_image;
    delete[] temp;
    return true;
}


//二值化
bool Bmp_Image::binarization(int threshold)
{
    int line=0,column=0;
    int temp=0;

    for(line=0; line<BITMAPINFOHEADER.biHight; line++)
    {
        for(column=0; column<BITMAPINFOHEADER.biWidth; column++)
        {
            temp = *(image_data+column+line*(BITMAPINFOHEADER.biWidth));
            if(temp>=threshold)
            {
                *(image_data+column+line*(BITMAPINFOHEADER.biWidth))=WHITE;
            }
            else
            {
                *(image_data+column+line*(BITMAPINFOHEADER.biWidth))=BLACK;
            }
        }
    }
}

/**************骨架提取算法******************/
/*************对边缘噪声敏感，二值化前应做平滑处理******图像中白块越少，迭代速度越快***********/
bool Bmp_Image::K3M1()  
{
    int deleted_points=0;
    int iteration=0;
        
    while(1)
    {
        deleted_points=H_search(image_data, iteration, BITMAPINFOHEADER.biWidth, BITMAPINFOHEADER.biHight);//水平扫描
    
        iteration++;
        if(iteration==5)
        {
        iteration=1;
        if(deleted_points==0)break;
        } 
        deleted_points=0;
        
        deleted_points=V_search(image_data, iteration, BITMAPINFOHEADER.biWidth, BITMAPINFOHEADER.biHight);//垂直扫描
        
        iteration++;
        if(iteration==5)
        {
        iteration=1;
        if(deleted_points==0)break;
        } 
        deleted_points=0;
    }
}


//腐蚀
bool Bmp_Image::corrosion()
{
    int line=0,column=0;
    int count=0,temp_line=0,temp_column=0;
    int kernel_line[]={-1,-1,-1, 0, 1,1,1, 0};
    int kernel_column[]={-1,0,1, 1, 1,0,-1, -1};
    int num=8;
    // int kernel_line[]={-1,0,1,0};
    // int kernel_column[]={0,1,0,-1};
    // int num=4;

    unsigned char* img = new unsigned char[BITMAPINFOHEADER.biHight*BITMAPINFOHEADER.biWidth];

    for(line=0; line<BITMAPINFOHEADER.biHight; line++)       
    {
        for(column=0; column<BITMAPINFOHEADER.biWidth; column++)   
        {	
            if(*(image_data+column+line*(BITMAPINFOHEADER.biWidth)) == WHITE)
            {
                for(count=0;count<num;count++)
                {
                    temp_line = line+kernel_line[count];
                    temp_column = column+kernel_column[count];
                    if ((temp_line<0 || temp_line>=BITMAPINFOHEADER.biHight)||(temp_column<0||temp_column>=BITMAPINFOHEADER.biWidth))
                    {
                        *(img+BITMAPINFOHEADER.biWidth*line+column)=BLACK;
                        break;
                    }
                    if( *(image_data+(line+kernel_line[count])*BITMAPINFOHEADER.biWidth+(column+kernel_column[count])) == BLACK)
                    {
                        *(img+BITMAPINFOHEADER.biWidth*line+column)=WHITE;//记下要腐蚀的点
                        break;
                    }
                }
                if(count ==num )*(img+BITMAPINFOHEADER.biWidth*line+column)=BLACK;
            }
        }
    }
    for(line=0; line<BITMAPINFOHEADER.biHight; line++)
    {
        for(column=0; column<BITMAPINFOHEADER.biWidth; column++)
        {
            if (*(img+BITMAPINFOHEADER.biWidth*line+column)==WHITE)
            {
                *(image_data+column+line*(BITMAPINFOHEADER.biWidth))=BLACK;
            }
        }
    }
    delete[] img;
    return true;
}

//膨胀
bool Bmp_Image::expansion()
{
    int line=0,column=0;
    int count=0,temp_line=0,temp_column=0;
    int kernel_line[]={-1,-1,-1, 0, 1,1,1, 0};
    int kernel_column[]={-1,0,1, 1, 1,0,-1, -1};
    int num=8;
    // int kernel_line[]={-1,0,1,0};
    // int kernel_column[]={0,1,0,-1};
    // int num=4;

    unsigned char* img = new unsigned char[BITMAPINFOHEADER.biHight*BITMAPINFOHEADER.biWidth];

    for(line=0; line<BITMAPINFOHEADER.biHight; line++)       
    {
        for(column=0; column<BITMAPINFOHEADER.biWidth; column++)   
        {	
            if(*(image_data+column+line*(BITMAPINFOHEADER.biWidth)) == BLACK)
            {
                for(count=0;count<num;count++)
                {
                    temp_line = line+kernel_line[count];
                    temp_column = column+kernel_column[count];
                    if ((temp_line<0 || temp_line>=BITMAPINFOHEADER.biHight)||(temp_column<0||temp_column>=BITMAPINFOHEADER.biWidth))
                    {
                        *(img+BITMAPINFOHEADER.biWidth*line+column)=BLACK;
                        break;
                    }
                    if( *(image_data+(line+kernel_line[count])*BITMAPINFOHEADER.biWidth+(column+kernel_column[count])) == WHITE)
                    {
                        *(img+BITMAPINFOHEADER.biWidth*line+column)=WHITE;//记下要腐蚀的点
                        break;
                    }
                }
                if(count ==num )*(img+BITMAPINFOHEADER.biWidth*line+column)=BLACK;
            }
        }
    }
    for(line=0; line<BITMAPINFOHEADER.biHight; line++)
    {
        for(column=0; column<BITMAPINFOHEADER.biWidth; column++)
        {
            if (*(img+BITMAPINFOHEADER.biWidth*line+column)==WHITE)
            {
                *(image_data+column+line*(BITMAPINFOHEADER.biWidth))=WHITE;
            }
        }
    }
    delete[] img;
    return true;
}

//边缘提取
bool Bmp_Image::edge()
{
    int line=0,column=0;
    int count=0,temp_line=0,temp_column=0;
    int kernel_line[]={-1,-1,-1, 0, 1,1,1, 0};
    int kernel_column[]={-1,0,1, 1, 1,0,-1, -1};
    int num=8;
    // int kernel_line[]={-1,0,1,0};
    // int kernel_column[]={0,1,0,-1};
    // int num=4;

    unsigned char* img = new unsigned char[BITMAPINFOHEADER.biHight*BITMAPINFOHEADER.biWidth];

    for(line=0; line<BITMAPINFOHEADER.biHight; line++)       
    {
        for(column=0; column<BITMAPINFOHEADER.biWidth; column++)   
        {	
            if(*(image_data+column+line*(BITMAPINFOHEADER.biWidth)) == BLACK)
            {
                for(count=0;count<num;count++)
                {
                    temp_line = line+kernel_line[count];
                    temp_column = column+kernel_column[count];
                    if ((temp_line<0 || temp_line>=BITMAPINFOHEADER.biHight)||(temp_column<0||temp_column>=BITMAPINFOHEADER.biWidth))
                    {
                        *(img+BITMAPINFOHEADER.biWidth*line+column)=BLACK;
                        break;
                    }
                    if( *(image_data+(line+kernel_line[count])*BITMAPINFOHEADER.biWidth+(column+kernel_column[count])) == WHITE)
                    {
                        *(img+BITMAPINFOHEADER.biWidth*line+column)=WHITE;
                        break;
                    }
                }
                if(count ==num )*(img+BITMAPINFOHEADER.biWidth*line+column)=BLACK;
            }
        }
    }
    for(line=0; line<BITMAPINFOHEADER.biHight; line++)
    {
        for(column=0; column<BITMAPINFOHEADER.biWidth; column++)
        {
            *(image_data+column+line*(BITMAPINFOHEADER.biWidth)) = *(img+BITMAPINFOHEADER.biWidth*line+column);
        }
    }
    delete[] img;
    return true;
}


//霍夫圆变换
//r_min: 半径最小值
//r_max:半径最大值
Circle* Bmp_Image::Hough_circles(int r_min, int r_max, int threshold, int circles_number)
{
    int line,column;
    Circle *circles = new Circle[r_max-r_min+1];
    Circle *result_circles = new Circle[circles_number];
    int  *vote_img = new  int[BITMAPINFOHEADER.biHight*BITMAPINFOHEADER.biWidth];
    int r = 0, i=0 ,j=0 , max_score;
    
    for (r=r_min; r<=r_max; r++)
    {
        memset(vote_img,0,BITMAPINFOHEADER.biHight*BITMAPINFOHEADER.biWidth*sizeof(int));//数组清零，重新投票
        for(line=0;line<BITMAPINFOHEADER.biHight;line++)
        {
            for(column=0;column<BITMAPINFOHEADER.biWidth;column++)
            {
                if(*(image_data+column+line*(BITMAPINFOHEADER.biWidth)) >100 )
                {     
                    Bresenham_Circle(vote_img, line , column , r,  BITMAPINFOHEADER.biHight, BITMAPINFOHEADER.biWidth);
                }
            }
        }
/*************************************************************/
        /*if(r==55)
        {
                //保存图像
            char *image_name = (char*)"../vote_img.bmp";
            FILE *fp = fopen(image_name,"wb");
            if (fp==0)
                return 0;
            fwrite(&BITMAPFILEHEADER, 14,1, fp);
            fwrite(&BITMAPINFOHEADER, 40, 1, fp);
            fwrite(RGBQUAD,4,256,fp);
            fwrite(vote_img,(int)BITMAPINFOHEADER.\
            biHight*(int)(BITMAPINFOHEADER.biWidth),1,fp);
            cout<< "save OK! "<<image_name<<endl;
            fclose(fp);
        }*/
/***************************************************************/       
        circles[r-r_min].r = r;//半径
        max_score = 0;
        //找到投票矩阵中分数最大的点
        for(line=0;line<BITMAPINFOHEADER.biHight;line++)
        {
            for(column=0;column<BITMAPINFOHEADER.biWidth;column++)
            {
                if(*(vote_img+column+line*(BITMAPINFOHEADER.biWidth)) > max_score)
                {
                    max_score = *(vote_img+column+line*(BITMAPINFOHEADER.biWidth));
                    circles[r-r_min].center_line = line;
                    circles[r-r_min].center_column = column;
                    circles[r-r_min].score = *(vote_img+column+line*(BITMAPINFOHEADER.biWidth));
                }
            }
        }
    }

    int max_index=0;
    for (i=0; i<circles_number; i++)
    {
        //找出分数前几名，并且大于阈值的圆
        max_score = 0;
        for (j=0; j<=r_max-r_min; j++)
        {
            if(circles[j].score > max_score)
            {
                max_score = circles[j].score;
                max_index = j;
            }
        }
        result_circles[i] = circles[max_index];
        cout<< circles[max_index].center_line<<" "<<circles[max_index].center_column<<" "<<circles[max_index].r<<" "<<circles[max_index].score<<endl;   //输出
        circles[max_index].score = 0;
    }
    
    
    delete[] vote_img;
    delete[] circles;
    return result_circles;
}


/*************roberts算子 梯度计算**********/
void Bmp_Image::roberts()
{
    int line=0, column=0, num_x=0, num_y=0;
    unsigned char* edge = new unsigned char[BITMAPINFOHEADER.biHight*BITMAPINFOHEADER.biWidth];

    for(line=1;line<BITMAPINFOHEADER.biHight-1;line++)       
    {
        for(column=1;column<BITMAPINFOHEADER.biWidth-1;column++)   
            {		   	
                num_x = *(image_data + (line*BITMAPINFOHEADER.biWidth) + column) - *(image_data + ((line+1)*BITMAPINFOHEADER.biWidth) + column+1);
                num_y = *(image_data + (line*BITMAPINFOHEADER.biWidth) + column+1) - *(image_data + ((line+1)*BITMAPINFOHEADER.biWidth) + column);
                
                *(edge + (line*BITMAPINFOHEADER.biWidth) + column) = sqrt(num_x*num_x + num_y*num_y);
            }
    }

    memcpy(image_data,edge,BITMAPINFOHEADER.biHight*BITMAPINFOHEADER.biWidth*sizeof(unsigned char));
    delete[] edge;
}

/************拉普拉斯算子  二阶微分 **********/
void Bmp_Image::conv()
{
    int kernel_line[]={-1,-1,-1, 0, 1,1,1, 0};
    int kernel_column[]={-1,0,1, 1, 1,0,-1, -1};
    int line=0, column=0, count=0;
    int	num=0; 
    unsigned char* edge = new unsigned char[BITMAPINFOHEADER.biHight*BITMAPINFOHEADER.biWidth];

    for(line=1;line<BITMAPINFOHEADER.biHight-1;line++)       
        {
        for(column=1;column<BITMAPINFOHEADER.biWidth-1;column++)   
        {		   
            for(count=0;count<8;count++)
            {
                num += *(image_data + ((line+kernel_line[count])*BITMAPINFOHEADER.biWidth) + column+kernel_column[count]);
            }
                        
                        num = (8* *(image_data + (line*BITMAPINFOHEADER.biWidth) + column) - num);
                    
                    if(num<0)num=0;
                    if(num>255)num=255;
                    *(edge + (line*BITMAPINFOHEADER.biWidth) + column)	= num;
                    num=0;
        }
    }
    memcpy(image_data,edge,BITMAPINFOHEADER.biHight*BITMAPINFOHEADER.biWidth*sizeof(unsigned char));
    delete[] edge;
}

//画圆
void Bmp_Image::circle(Circle a)
{
    unsigned char* midimg = new unsigned char[BITMAPINFOHEADER.biHight*BITMAPINFOHEADER.biWidth];
    memcpy(midimg,image_data,BITMAPINFOHEADER.biHight*BITMAPINFOHEADER.biWidth*sizeof(unsigned char));

    draw_Circle(midimg, a.center_line , a.center_column , a.r, BITMAPINFOHEADER.biHight ,BITMAPINFOHEADER.biWidth);
    RGBQUAD[100].rgbRed = 255;
    RGBQUAD[100].rgbGreen = 255;
    RGBQUAD[100].rgbBlue = 0;

    memcpy(image_data,midimg,BITMAPINFOHEADER.biHight*BITMAPINFOHEADER.biWidth*sizeof(unsigned char));
    delete[] midimg;
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





