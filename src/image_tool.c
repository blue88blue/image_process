#include "image_tool.h"
#include "string.h"
#include "math.h"


signed char kernel_line[]={-1,-1,-1, 0, 1,1,1, 0};
signed char kernel_column[]={-1,0,1, 1, 1,0,-1, -1};
int weight[8]={128,1,2,4,8,16,32,64};
int A1[8]={7,14,28,56,112,131,193,224};//3??
int A2[16]={7,14,15,28,30,56,60,112,120,131,
            135,193,195,224,225,240};//3 or 4??
int A3[24]={7,14,15,28,30,31,56,60,62,112,
            120,124,131,135,143,193,195,199,224,225,
            227,240,241,248};//3 or 4 or 5??,
int A4[32]={7,14,15,28,30,31,56,60,62,63,112,120,
          124,126,131,135,143,159,193,195,199,207,
          224,225,227,231,240,241,243,248,249,252};//3 or 4 or 5 or 6??,
int A5[36]={7,14,15,28,30,31,56,60,62,63,112,120,
          124,126,131,135,143,159,191,193,195,199,
          207,224,225,227,231,239,240,241,243,248,
          249,251,252,254};//3 or 4 or 5 or 6??,

int Bisection(int a[], int direction, int number);

int H_search(uint8 *img, int iteration, int MAX_column, int MAX_line)  
{
  int line,column,count;
  int deleted_point=0;
  int score=0;
  int P_left=0,P_current=0,P_right=0;
  int NEXT=1;//0：表示跳过对后一点的操作，1：表示对后一点进行操作�
  
     for(line=0;line<=MAX_line-1;line++) //行循环    
    {
      for(column=0;column<=MAX_column-1;column++)//列循环 
      {
        if(NEXT==0)
            NEXT=1;
        else
        {  
            if(*(img+line*MAX_column+column)==WHITE)//只对白点进行处理
            {
                  P_left=0;P_right=0;score=0;//清零
                  /*****8邻域*****/
                  for(count=0;count<8;count++)
                  {
                    if(line+kernel_line[count]>=0 && line+kernel_line[count]<=(MAX_line-1) \
                        && column+kernel_column[count]>=0 && column+kernel_column[count]<=(MAX_column-1))//数组越界判断
                        {
                          P_current=*(img+(line+kernel_line[count])*MAX_column+column+kernel_column[count]);
                          if(P_current)
                              score+=weight[count];
                          
                           if(count==3 && P_current)       //找左右是否有白点
                                P_right=1;                        
                          else if(count==7 && P_current)
                                P_left=1;
                        }
                  }
                  /*****8邻域*****/
                /*****决策*删除点*****/
                if( P_right!=1 || P_left!=1)//如果改点左右都是白点，则不做处理
                {
                  if( (column==0 && P_right==1) || (column==(MAX_column-1) && P_left==1) )
                      continue;
                  switch(iteration)
                    {
                      case 1: if(Bisection(A1, score, 8))
                          { 
                              *(img+line*MAX_column+column)=BLACK;
                              deleted_point++;
                              NEXT=0;
                            } break;
                      case 2: if(Bisection(A2, score, 16))
                          { 
                              *(img+line*MAX_column+column)=BLACK;
                              deleted_point++;
                              NEXT=0;
                            } break;
                      case 3: if(Bisection(A3, score, 24))
                            { 
                              *(img+line*MAX_column+column)=BLACK;
                              deleted_point++;
                              NEXT=0;
                            } break;
                      case 4: if(Bisection(A4, score, 32))
                            { 
                              *(img+line*MAX_column+column)=BLACK;
                              deleted_point++;
                              NEXT=0;
                            } break;
                      case 5: if(Bisection(A5, score, 36))
                            { 
                              *(img+line*MAX_column+column)=BLACK;
                              deleted_point++;
                              NEXT=0;
                            } break;
                    }
                }
                /*****决策*删除点****/
              }//只对白点进行处理
        }//NEXT
        
      }//列循环
    }//行循环
    
    return  deleted_point;
}



int V_search(uint8 *img,int iteration, int MAX_column, int MAX_line)  
{
  int line,column,count;
  int deleted_point=0;
  int score=0;
  int P_up=0,P_current=0,P_down=0;
  int NEXT=1;//0：表示跳过对后一点的操作，1：表示对后一点进行操作
      
    for(column=0;column<=MAX_column-1;column++)   //列循环  
    {
      for(line=0;line<=MAX_line-1;line++) //行循环 
      {
        if(NEXT==0)
          NEXT=1;
        else
        {  
            if(*(img+line*MAX_column+column)==WHITE)//只对白点进行处理
            {
                  P_up=0;P_down=0;score=0;//清零
                  /*****8邻域*****/
                  for(count=0;count<8;count++)
                  {
                    if(line+kernel_line[count]>=0 && line+kernel_line[count]<=(MAX_line-1) \
                        && column+kernel_column[count]>=0 && column+kernel_column[count]<=(MAX_column-1))//数组越界判断
                        {
                          P_current=*(img+(line+kernel_line[count])*MAX_column+column+kernel_column[count]);
                          if(P_current)
                            score+=weight[count];
                          
                          if(count==1 && P_current)       //找上下是否有白点
                                P_up=1;                        
                          else if(count==5 && P_current)
                                P_down=1;
                        }
                  }
                  /*****8邻域*****/
                /*****决策*删除点*****/
                if( P_up!=1 || P_down!=1)//如果改点上下都是黑点，则不做处理
                {  
                  if( (line==(MAX_line-1) && P_up==1) || (line==0 && P_down==1) )
                    continue;
                  
                      switch(iteration)
                        {
                          case 1: if(Bisection(A1, score, 8))
                                    { 
                                      *(img+line*MAX_column+column)=BLACK;
                                      deleted_point++;
                                      NEXT=0;
                                    } break;
                          case 2: if(Bisection(A2, score, 16))
                                    { 
                                      *(img+line*MAX_column+column)=BLACK;
                                      deleted_point++;
                                      NEXT=0;
                                    } break;
                          case 3: if(Bisection(A3, score, 24))
                                    { 
                                      *(img+line*MAX_column+column)=BLACK;
                                      deleted_point++;
                                      NEXT=0;
                                    } break;
                          case 4: if(Bisection(A4, score, 32))
                                    { 
                                      *(img+line*MAX_column+column)=BLACK;
                                      deleted_point++;
                                      NEXT=0;
                                    } break;
                          case 5: if(Bisection(A5, score, 36))
                                    { 
                                      *(img+line*MAX_column+column)=BLACK;
                                      deleted_point++;
                                      NEXT=0;
                                    } break;
                        }
                  
                }
              /*****决策*删除点****/
            }//只对白点进行处理
        }//NEXT
        
      }//行循环
    }//列循环
    
  return  deleted_point;
}



//二分法查找   a查找数组  direction目标值  number数组大小
int Bisection(int a[], int direction, int number) 
{
	int low = 0;
	int high = number - 1;
	int middle = 0;

	while (low<high)
	{
		middle = (low + high) >> 1;
		if (direction == a[middle])
		{
			return 1;
		}
		else if (direction>a[middle])
			low = middle + 1;

		else
			high = middle - 1;
	}

        if (low == high&&direction == a[high])
          return 1;
	return 0;
}


/************拉普拉斯算子  二阶微分 **********/
//src:        输入 灰度图像
//image:      输出图像
//MAX_column: 图像宽度
//MAX_line:   图像高度
void conv(uint8 *src ,uint8 *image, int Max_Rows, int Max_Columns)
{
  int kernel_line[]={-1,-1,-1, 0, 1,1,1, 0};
  int kernel_column[]={-1,0,1, 1, 1,0,-1, -1};
  int line=0, column=0, count=0;
  int	num=0; 
  
  for(line=1;line<Max_Rows-1;line++)       
  {
    for(column=1;column<Max_Columns-1;column++)   
    {		   
        for(count=0;count<8;count++)
          {
            num += *(src + ((line+kernel_line[count])*Max_Columns) + column+kernel_column[count]);
          }
          
          num = (8* *(src + (line*Max_Columns) + column) - num);
        
        if(num<0)num=0;
        if(num>255)num=255;
        *(image + (line*Max_Columns) + column)	= num;
        num=0;
    }
  }
  
}

/*************roberts算子 梯度计算**********/
//src:        输入灰度图像
//image:      输出梯度图像
//MAX_column: 图像宽度
//MAX_line:   图像高度
void roberts(uint8 *src ,uint8 *image, int Max_Rows, int Max_Columns)
{
  int line=0, column=0, num_x=0, num_y=0;
  
  for(line=0;line<Max_Rows-1;line++)       
  {
    for(column=0;column<Max_Columns-1;column++)   
    {		   	
      num_x = *(src + (line*Max_Columns) + column) - *(src + ((line+1)*Max_Columns) + column+1);
      num_y = *(src + (line*Max_Columns) + column+1) - *(src + ((line+1)*Max_Columns) + column);
      
      *(image + (line*Max_Columns) + column) = sqrt(num_x*num_x + num_y*num_y);
          
    }
  }
  
}



//图中某点+1
void assignment_point( int *src, int line , int column,int Max_Rows, int Max_Columns)
{
	int num=0;
	if(line>=0 && line<Max_Rows && column>=0 && column<Max_Columns)
	{
		num = *(src +(line*Max_Columns) + column);
		num += 1;
		if(num > 255)
			num = 255;
		src [((line*Max_Columns) + column) ] = num;
    //*(*(src+line)+column) = num;
	}
}




//Bresenham画圆投票
void Bresenham_Circle( int *src, int line_c , int column_c , int r, int Max_Rows, int Max_Columns)
{
    int x, y, d;
    x = 0;
    y = r;
    d = 3 - 2 * r;
  
    assignment_point(src, line_c+x, column_c+y, Max_Rows, Max_Columns);
    assignment_point(src, line_c-x, column_c+y, Max_Rows, Max_Columns);
    assignment_point(src, line_c+x, column_c-y, Max_Rows, Max_Columns);
    assignment_point(src, line_c-x, column_c-y, Max_Rows, Max_Columns);
    
    assignment_point(src, line_c+y, column_c+x, Max_Rows, Max_Columns);
    assignment_point(src, line_c-y, column_c+x, Max_Rows, Max_Columns);
    assignment_point(src, line_c+y, column_c-x, Max_Rows, Max_Columns);
    assignment_point(src, line_c-y, column_c-x, Max_Rows, Max_Columns);
    while(x < y)
    {
        if(d < 0)
        {
            d = d + 4 * x + 6;
        }
        else
        {
            d = d + 4 * ( x - y ) + 10;
            y--;
        }
        x++;
        assignment_point(src, line_c+x, column_c+y, Max_Rows, Max_Columns);
        assignment_point(src, line_c-x, column_c+y, Max_Rows, Max_Columns);
        assignment_point(src, line_c+x, column_c-y, Max_Rows, Max_Columns);
        assignment_point(src, line_c-x, column_c-y, Max_Rows, Max_Columns);
        
        assignment_point(src, line_c+y, column_c+x, Max_Rows, Max_Columns);
        assignment_point(src, line_c-y, column_c+x, Max_Rows, Max_Columns);
        assignment_point(src, line_c+y, column_c-x, Max_Rows, Max_Columns);
        assignment_point(src, line_c-y, column_c-x, Max_Rows, Max_Columns);
    }
}





//点
void draw_point(unsigned char *src, int line , int column,int Max_Rows, int Max_Columns)
{
	int num=0;
	if(line>=0 && line<Max_Rows && column>=0 && column<Max_Columns)
	{
		*(src +(line*Max_Columns) + column) = 100;
	}
}



//Bresenham画圆
void draw_Circle(unsigned char *src, int line_c , int column_c , int r, int Max_Rows, int Max_Columns)
{
    int x, y, d;
    x = 0;
    y = r;
    d = 3 - 2 * r;
  
    draw_point(src, line_c+x, column_c+y, Max_Rows, Max_Columns);
    draw_point(src, line_c-x, column_c+y, Max_Rows, Max_Columns);
    draw_point(src, line_c+x, column_c-y, Max_Rows, Max_Columns);
    draw_point(src, line_c-x, column_c-y, Max_Rows, Max_Columns);
    
    draw_point(src, line_c+y, column_c+x, Max_Rows, Max_Columns);
    draw_point(src, line_c-y, column_c+x, Max_Rows, Max_Columns);
    draw_point(src, line_c+y, column_c-x, Max_Rows, Max_Columns);
    draw_point(src, line_c-y, column_c-x, Max_Rows, Max_Columns);
    while(x < y)
    {
        if(d < 0)
        {
            d = d + 4 * x + 6;
        }
        else
        {
            d = d + 4 * ( x - y ) + 10;
            y--;
        }
        x++;
        draw_point(src, line_c+x, column_c+y, Max_Rows, Max_Columns);
        draw_point(src, line_c-x, column_c+y, Max_Rows, Max_Columns);
        draw_point(src, line_c+x, column_c-y, Max_Rows, Max_Columns);
        draw_point(src, line_c-x, column_c-y, Max_Rows, Max_Columns);
        
        draw_point(src, line_c+y, column_c+x, Max_Rows, Max_Columns);
        draw_point(src, line_c-y, column_c+x, Max_Rows, Max_Columns);
        draw_point(src, line_c+y, column_c-x, Max_Rows, Max_Columns);
        draw_point(src, line_c-y, column_c-x, Max_Rows, Max_Columns);
    }
}




/*************画直线投票**********/
//image:      投票矩阵
//MAX_column: 投票图像宽度
//MAX_line:   投票图像高度
/*void draw_line(long *image, int x_line, int y_column, int MAX_column, int MAX_line) 
{
  int center_line = MAX_line/2;
  int center_column = MAX_column/2;
  int l_line=0, l_column=0, r_line=0, r_column=0;//左右两个边界点
  int point[4][2]={0};

  double slope = -x_line;//斜率
  int offset = y_column + center_column -slope*center_line;//偏置

  if (offset >=0 || offset<=MAX_column-1)
    {
      point[0][0] = 0;  //k
      point[0][1] = offset;   //b
    }
  if ((-offset/slope) >=0 || (-offset/slope)<=MAX_line-1)
    {
      point[1][0] = (-offset/slope);
      point[1][1] = 0;
    }
  if ( ((MAX_line-1)*slope+offset) >=0 || ((MAX_line-1)*slope+offset)<=MAX_column-1)
    {
      point[2][0] = MAX_line-1;
      point[2][1] = (MAX_line-1)*slope+offset;
    }
  if ( ((MAX_column-1-slope)/offset) >=0 || ((MAX_column-1-slope)/offset)<=MAX_line-1)
    {
      point[3][0] = ((MAX_column-1-slope)/offset);
      point[3][1] = MAX_column-1;
    }

  if(point[0][1]!=0)
  
  




}

*/







