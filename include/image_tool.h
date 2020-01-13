#ifndef       _IMAGE_TOOLS_H
#define       _IMAGE_TOOLS_H

#define uint8 unsigned char

#define WHITE   255
#define BLACK   0



//函数声明
int H_search(uint8 *img, int iteration, int MAX_column, int MAX_line);
int V_search(uint8 *img, int iteration, int MAX_column, int MAX_line);
void assignment_point( int *src, int line , int column,int Max_Rows, int Max_Columns);
void Bresenham_Circle( int *src, int line_c , int column_c , int r, int Max_Rows, int Max_Columns);
void draw_point(unsigned char *src, int line , int column,int Max_Rows, int Max_Columns);
void draw_Circle(unsigned char *src, int line_c , int column_c , int r, int Max_Rows, int Max_Columns);

#endif

