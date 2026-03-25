#if 0
// 引入OpenCV核心头文件（包含图像读取、显示等核心功能）
#include <opencv2/opencv.hpp>
// 引入C++标准输入输出头文件（用于cout输出提示信息）
#include <iostream>

// 使用std和cv命名空间，避免重复写std::和cv::
using namespace std;
using namespace cv;

int main() {
    // 读取彩色图像（imread第二个参数默认IMREAD_COLOR，可省略）
    Mat color = imread("./saidao3.jpg", IMREAD_COLOR);

      // 检查图像是否成功读取
    if (!color.data) { // 等价于 if (color.empty())
        cout << "图片打开失败，请检查文件路径或文件是否存在！" << endl;
        return -1; // 异常退出程序
    }
    
    Mat gray_image,small_image;
    Size2d size1(80,60);
    cvtColor(color, gray_image, COLOR_BGR2GRAY);
    resize(gray_image,small_image,size1);
    //cout<<small_image.size();
    

    #if 1
    /****************大津法*******************/
    uint huidu[256] = {0};    //灰度直方图数组
    uchar YUZHI;                //二值化阈值

    //part1直方图统计
    for(int i=0;i<60;i++)  //遍历所有像素
    {
        for(int j=0;j<80;j++)
        {
            huidu[small_image.at<uchar>(i,j)]++; //统计灰度直方图(先行后列)
        }
        
    }


   //part2直方图显示
    Mat hist_image(256,256,CV_8UC3,Scalar(255,255,255));
    for(int i=0;i<256;i++)
    {
        int norm_value=huidu[i];
        line(hist_image,Point(i,256),Point(i,256-norm_value),Scalar(0,0,0),1);
    }


    //part3阈值确定
    uint H1=0;   //第一高峰海拔值
    uchar D1=0;   //第一高峰位置
 
    #define KUAN 30  //山峰宽度阈值
 
    for(int i=0;i<255;i++) //遍历灰度直方图
    {
        if(huidu[i]>H1)  //寻找最大值
        {   
            H1 = huidu[i]; //记录第一高峰海拔
            D1 = i;         //记录第一高峰位置
        }
    }
    uint H2=0;  //第二高峰海拔值
    uchar D2=0;      //第二高峰位置
    bool OK=0 ;    //是否找到第二高峰
    for(int i=H1-5;i>0;i-=5) //向下切，找第二高峰
    {
        for(int j=0;j<256;j++) //遍历这一行
        {
            if(huidu[j]>i && abs(j-D1) > KUAN) //有上面部分
            {
                H2 = i; //记录第二高峰海拔
                D2 = j; //记录第二高峰位置
                OK = 1; //标志置位
                break;
            }
        }
        if(OK)  break;  //如果找到，直接跳出
    }
 
    uint H3=4800;  //山谷海拔值
    uchar D3=0;      //山谷位置
 
    if(OK) //已经找到2座山
    {
        if(D1<D2) //找山谷,左向右找
        {
            for(int i=D1;i<D2;i++) //遍历两座山峰之间
            {
                if(huidu[i]<H3) //寻找最小值
                {
                    H3 = huidu[i]; //记录山谷海拔
                    D3 = i; //记录山谷位置
                }
            }
        }
        else //右向左找
        {
            for(int i=D2;i<D1;i++)//遍历两座山峰之间
            {
                if(huidu[i]<H3) //寻找最小值
                {
                    H3 = huidu[i];//记录山谷海拔
                    D3 = i;//记录山谷位置
                }
            }
        }
     
        YUZHI = D3;   //获取阈值
        cout<<(int)D3<<" "<<(int)D2<<" "<<(int)D1;
    }

    //part4二值化
    
    for(int i=0;i<60;i++)
    {
        for(int j=0;j<80;j++)
        {
            if(small_image.at<uchar>(i,j)<YUZHI)
            {
                small_image.at<uchar>(i,j)=0;
            }
            else
            {
                small_image.at<uchar>(i,j)=255;
            }
        }
   
    }
    #endif
    #if 1

    /*************************图像处理（灵魂）********************/
    uchar AX,AY; //A点
    uchar BX,BY; //B点
    uchar CX,CY; //C点
    uchar DX,DY; //D点
 
    AY=59; //获取AB点
    BY=59;
//先找最底下一行中心线
    for(int i=39;i>=1;i--) //从中间向左找上升沿
    {
        if(small_image.at<uchar>(AY,i-1)-small_image.at<uchar>(AY,i) == -255) //找到上升沿
        {
            AX = i; //A横坐标
        }
    }
    for(int i=39;i<79;i++) //从中间向右找上升沿
    {
        if(small_image.at<uchar>(BY,i+1)-small_image.at<uchar>(BY,i) == -255) //找到上升沿
        {
            BX = i; //B横坐标
        }
    }
    #endif


    #if 0
    CY = AY-1; //迭代C点
    CX = AX-1;
    for(int i=CY;i>0;i--) //由近及远
    {
        for(int j=CX;j<80;j++) //由左向右
        {
            if(small_image.at<uchar>(i,j) == 0) //找到白点
            {
                CX = j-1; //得到上一行C点X位置
                break;
            }
        }
        if(small_image.at<uchar>(i-1,CX) == 255) //判断上方还有没有黑点
        {
            CY = i;   //得到C点Y位置
            break;
        }
    }
 
    DY = BY-1; //迭代D点
    DX = BX-1;
    for(int i=DY;i>0;i--) //由近及远
    {
        for(int j=DX;j>0;j--) //由左向右
        {
            if(small_image.at<uchar>(i,j) == 0) //找到白点
            {
                DX = j+1; //得到上一行D点X位置
                break; //跳出此循环
            }
        }
        if(small_image.at<uchar>(i-1,DX) == 255) //判断上方还有没有黑点
        {
            DY = i;   //得到C点Y位置
            break;//跳出此循环
        }
    }
    cout<<(int)(CY-DY)<<" "<<(int)CY<<" "<<(int)DY;
    cout<<" "<<(int)CX<<" "<<(int)DX;
    if(abs(CY-DY)<20 && CY>30 && DY>30)  //初级判断十字路口,原来不是20是10
    {
        cout<<"shizi1";
        uchar Y = min(CY,DY);   //取得CD高度较小值
        uchar HEI = 0;          //十字路口上方区域黑点数量
        for(int i=Y;i>Y-10;i-=2)  //Y抽点轮询
        {
            for(int j=10;j<70;j+=5) //X抽点轮询
            {
                if(small_image.at<uchar>(i,j) == 255)  //有黑点
                {
                    HEI++;  //计数变量++
                }
            }
        }
        cout<<(int)HEI;
        if(HEI < 15) //最终判断十字路口，并补线 原来是10不是15
        {
            cout<<"shizi";
            float K;    //补线斜率
            K = (CX-AX)/(AY-CY); //计算AC点斜率
            //K = (CY-AY)/(CX-AX);
            cout<<" "<<K;
            for(int i=CY;i>CY-20;i--)  //补AC延长2像素宽线
            {
                small_image.at<uchar>(i,CX+(CY-i)/K) = 255;     //把图像对应点涂黑
                small_image.at<uchar>(i,(CX+(CY-i)/K)-1) = 255; //把图像对应点涂黑
            }
         
            K = ((BX-DX)+1)/(BY-DY); //计算BD点斜率
            //K = (DY-BY)/(BX-DX);
            cout<<" "<<K;
            for(int i=DY;i>DY-20;i--)  //补BD延长2像素宽线
            {
                small_image.at<uchar>(i,DX+(DY-i)/K) = 255;     //把图像对应点涂黑
                small_image.at<uchar>(i,(DX+(DY-i)/K)-1) = 255; //把图像对应点涂黑
            }
        }
    }
    #endif
/*************************************************************/
    #if 1
/*************************八邻域找中线******************************/
    uchar ZHONGJIAN[60]={39}; //中线位置
    uchar ZUO[60]={0}; //左线位置
    uchar YOU[60]={79}; //右线位置
 
    ZHONGJIAN[59] = (AX + BX) /2; //最底下一行中心线位置找到
 

    vector<vector<int>> balinyuL = {{1,0},{1,-1},{0,-1},{-1,-1},
                                    {-1,0},{-1,1},{0,1},{1,1}};
    vector<vector<int>> balinyuR = {{-1,0},{-1,-1},{0,-1},{1,-1},
                                    {1,0},{1,1},{0,1},{-1,1}};

    
    int Xha=AX-1,Yha=59;
    for(int i=0;i<5;i++)//最下面的点，左边
    {
        Xha=AX-1;
        Yha=59;
        Xha+=balinyuL[i][0];
        Yha+=balinyuL[i][1];
        if(small_image.at<uchar>(Yha,Xha) == 0)
        {
            small_image.at<uchar>(Yha,Xha) = 100;
            ZUO[Yha]=Xha;
            break;
        }
        
        
    }    
    for(int j=0;Yha>20 && Xha>1 && j<60;j++)
    {   
        int Xha_cp=Xha;
        int Yha_cp=Yha;
        for(int i=0;i<8;i++)
        {
            Xha=Xha_cp;
            Yha=Yha_cp;
            Xha+=balinyuL[i][0];
            Yha+=balinyuL[i][1];
            if(small_image.at<uchar>(Yha,Xha) == 0)
            {
                small_image.at<uchar>(Yha,Xha) = 100;
                ZUO[Yha]=Xha;
                break;
            }
        }
    }


    int Xhb=BX+1,Yhb=59;
    for(int i=0;i<5;i++)
    {
        Xhb=BX+1;
        Yhb=59;
        Xhb+=balinyuR[i][0];
        Yhb+=balinyuR[i][1];
        if(small_image.at<uchar>(Yhb,Xhb) == 0)
        {
            small_image.at<uchar>(Yhb,Xhb) = 100;
            YOU[Yhb]=Xhb;
            break;
        }
        
        
    }    
    for(int j=0;Yhb>20 && Xhb<79 && j<60;j++)
    {   
        int Xhb_cp=Xhb;
        int Yhb_cp=Yhb;
        for(int i=0;i<8;i++)
        {
            Xhb=Xhb_cp;
            Yhb=Yhb_cp;
            Xhb+=balinyuR[i][0];
            Yhb+=balinyuR[i][1];
            if(small_image.at<uchar>(Yhb,Xhb) == 0)
            {
                small_image.at<uchar>(Yhb,Xhb) = 100;
                YOU[Yhb]=Xhb;
                break;
            }
        }
    }
    for(int i=59;i>20;i--)
    {
        ZHONGJIAN[i]=(ZUO[i]+YOU[i])/2;
        small_image.at<uchar>(i,ZHONGJIAN[i]) = 100;

    }
    #endif

    
    




    // 创建名为"RGB"的窗口并显示图像
    imshow("RGB", small_image);
    
    // 等待按键输入（参数0表示无限等待，直到按下任意键）
    waitKey(0);
    
    // 销毁所有创建的窗口，释放资源
    destroyAllWindows();

    return 0; // 正常退出程序
}
#endif

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <stdint.h>
using namespace std;
using namespace cv;

// 固定图像尺寸 60行 × 80列
#define IMAGE_H 60
#define IMAGE_W 80
#define threshold_max  (255*5)  // 补断线阈值
#define threshold_min  (255*2)  // 去噪点阈值
#define border_min  5    // 左边不要太靠边
#define border_max  75   // 右边不要太靠边
uchar flag_yuanhuan=4;
uchar flag_shizi=0;


// 显示 60x80 二维数组图像（调试专用）
// 传入：uchar img[60][80]
void show_array_image(uchar img[60][80])
{
    // 1. 把二维数组转成 OpenCV 的 Mat
    cv::Mat mat(60, 80, CV_8UC1, img);

    // 2. 放大显示（不然60x80太小看不见）
    cv::Mat big_mat;
    cv::resize(mat, big_mat, cv::Size(320, 240), 0, 0, cv::INTER_NEAREST);

    // 3. 显示
    cv::imshow("Array Image Debug", big_mat);
    cv::waitKey(1);  // 刷新窗口
}

 

// 功能：把彩色cv::Mat 转成 你的灰度二维数组 uchar gray_arr[60][80]（调试专用）
void color_to_gray_array(cv::Mat &color, uchar gray_arr[60][80])
{
    cv::Mat gray;
    // 彩色转灰度
    cv::cvtColor(color, gray, cv::COLOR_BGR2GRAY);
    // 缩放到 80x60（宽×高）
    cv::resize(gray, gray, cv::Size(80, 60));

    // 复制到你的二维数组
    for (int i = 0; i < 60; i++) {
        for (int j = 0; j < 80; j++) {
            gray_arr[i][j] = gray.at<uchar>(i, j);
        }
    }
}



//功能：由输入的灰度图数组得到更新的二值化阈值
unsigned char otsu_binary0(uchar (*gray_array)[IMAGE_W])
{
    unsigned int huidu[256] = {0};
    unsigned char YUZHI = 0;

    // ==================== 统计直方图（指针加速） ====================
    for (int i = 0; i < IMAGE_H; i++) {
        uchar* row = gray_array[i];  // 取行指针（加速）
        for (int j = 0; j < IMAGE_W; j++) {
            huidu[row[j]]++;
        }
    }

    // ==================== 找第一高峰 ====================
    unsigned int H1 = 0;
    unsigned char D1 = 0;
#define KUAN 30

    for (int i = 0; i < 256; i++) {
        if (huidu[i] > H1) {
            H1 = huidu[i];
            D1 = i;
        }
    }

    // ==================== 找第二高峰 ====================
    unsigned int H2 = 0;
    unsigned char D2 = 0;
    bool OK = false;

    for (int i = H1 - 5; i > 0 && !OK; i -= 5) {
        for (int j = 0; j < 256; j++) {
            if (huidu[j] > i && abs(j - D1) > KUAN) {
                H2 = i;
                D2 = j;
                OK = true;
                break;
            }
        }
    }

    // ==================== 找山谷（阈值） ====================
    if (OK) {
        unsigned int H3 = 4800;
        unsigned char D3 = 0;

        if (D1 < D2) {
            for (int i = D1; i < D2; i++) {
                if (huidu[i] < H3) {
                    H3 = huidu[i];
                    D3 = i;
                }
            }
        } else {
            for (int i = D2; i < D1; i++) {
                if (huidu[i] < H3) {
                    H3 = huidu[i];
                    D3 = i;
                }
            }
        }
        YUZHI = D3;
    } else {
        YUZHI = 100; // 没找到就给默认值
    }
    return YUZHI;

    
}



//根据更新的阈值进行二值化
void otsu_binary(uchar (*gray_array)[IMAGE_W],uchar YUZHI)
{
    for (int i = 0; i < IMAGE_H; i++) {
        uchar* row = gray_array[i];  // 行指针
        for (int j = 0; j < IMAGE_W; j++) {
            row[j] = (row[j] < YUZHI) ? 0 : 255;
        }
    }
}



//功能：将输入的二值数组进行补线和滤波
void image_filter(uint8_t (*bin_image)[IMAGE_W])
{
    uint16_t i, j;
    uint32_t sum;

    // 只遍历内部像素（避免越界）
    for (i = 1; i < IMAGE_H - 1; i++)
    {
        // 取当前行 上一行 下一行 指针（加速核心！）
        uint8_t *prev = bin_image[i-1];
        uint8_t *curr = bin_image[i];
        uint8_t *next = bin_image[i+1];

        for (j = 1; j < IMAGE_W - 1; j++)
        {
            // 纯指针访问 8 邻域，速度比数组下标快非常多
            sum = prev[j-1] + prev[j] + prev[j+1]
                + curr[j-1] +          curr[j+1]
                + next[j-1] + next[j] + next[j+1];

            // 补断线：周围很白，自己是黑 → 变白
            if (sum >= threshold_max && curr[j] == 0)
                curr[j] = 255;

            // 去噪点：周围很黑，自己是白 → 变黑
            if (sum <= threshold_min && curr[j] == 255)
                curr[j] = 0;
        }
    }
}



//功能：加入边框，防止八邻域出界
void image_draw_rectan(uint8_t (*image)[IMAGE_W])
{
    uint8_t i;
    for(i=0;i<IMAGE_H;i++)
    {
        image[i][0] = 0;
        image[i][1] = 0;
        image[i][IMAGE_W-1] =0;
        image[i][IMAGE_W-2] =0;
    }
    for(i=0;i<IMAGE_W;i++)
    {
        image[0][i] =0;
        image[1][i] =0;
    }
}



/**
 * @brief 计算两点直线，仅更新边界数组：border[y] = x
 * @param start     起点数组 {x, y}
 * @param end       终点数组 {x, y}
 * @param border    边界数组首地址（y为索引，存储对应x）
 */
void line_to_border(
                    const uint16_t start[2],
                    const uint16_t end[2],
                    uint8_t *border)
{
    int x1 = (int)start[0];
    int y1 = (int)start[1];
    int x2 = (int)end[0];
    int y2 = (int)end[1];

    int dx = x2 - x1;
    int dy = y2 - y1;

    int stepX = dx >= 0 ? 1 : -1;
    int stepY = dy >= 0 ? 1 : -1;

    dx = dx >= 0 ? dx : -dx;
    dy = dy >= 0 ? dy : -dy;

    int x = x1;
    int y = y1;

    if (dx > dy)
    {
        int err = 2 * dy - dx;
        for (int i = 0; i <= dx; i++)
        {
            // 只更新边界：border[y] = x
            *(border + y) = (uint8_t)x;

            if (err >= 0)
            {
                y += stepY;
                err -= 2 * dx;
            }
            x += stepX;
            err += 2 * dy;
        }
    }
    else
    {
        int err = 2 * dx - dy;
        for (int i = 0; i <= dy; i++)
        {
            // 只更新边界
            *(border + y) = (uint8_t)x;

            if (err >= 0)
            {
                x += stepX;
                err -= 2 * dy;
            }
            y += stepY;
            err += 2 * dx;
        }
    }
}



//功能：找起点,AB点（白到黑的跳变点）找到的点在白色像素
unsigned char start_point_l[2] = {0};
unsigned char start_point_r[2] = {0};
unsigned char get_start_point(unsigned char start_row , uint8_t (*bin_image)[IMAGE_W])
{
    unsigned char i = 0, l_found = 0, r_found = 0;

    start_point_l[0] = 0;
    start_point_l[1] = 0;
    start_point_r[0] = 0;
    start_point_r[1] = 0;

    // 找左边界：白→黑跳变
    for (i = IMAGE_W/2; i > 0; i--)//原来是i > border_min
    {
        start_point_l[0] = i;
        start_point_l[1] = start_row;
        if (bin_image[start_row][i] == 255 && bin_image[start_row][i-1] == 0)
        {
            l_found = 1;
            break;
        }
    }

    // 找右边界：白→黑跳变
    for (i = IMAGE_W/2; i < IMAGE_W; i++)//原来是i < border_max
    {
        start_point_r[0] = i;
        start_point_r[1] = start_row;
        if (bin_image[start_row][i] == 255 && bin_image[start_row][i+1] == 0)
        {
            r_found = 1;
            break;
        }
    }

    if(l_found && r_found)
        return 1;
    else
        return 0;
}



#if 1
// 最大搜索点数（防止数组越界，取高度*3足够安全）
#define USE_num    IMAGE_H*3

// 左边界点数组：points_l[编号][0]=x, [1]=y
uint16_t points_l[USE_num][2] = {0};
// 右边界点数组：points_r[编号][0]=x, [1]=y
uint16_t points_r[USE_num][2] = {0};

// 左/右边界每一步的生长方向（0~7，对应8个邻域方向）
uint16_t dir_r[USE_num] = {0};
uint16_t dir_l[USE_num] = {0};
//345    543
//2 6    6 2
//107左  701右

// 实际搜到的左/右边界点数量
uint16_t data_stastics_l = 0;
uint16_t data_stastics_r = 0;

// 左右边界相遇的最高行（巡线终止位置）
uint8_t hightest = 0;

// 绝对值函数（防止调用库函数）
#define my_abs(a) ((a)>(0)?(a):(-a))

// =====================================================================================
// 函数功能：八邻域搜索左右跑道边界
// 入口：
//   break_flag    最大搜索次数（防止死循环）
//   image         二值图像数组（0黑=线，255白=背景）
//   l_stastic     左边界点数量（指针，输出）
//   r_stastic     右边界点数量（指针，输出）
//   l_start_x/y   左边界起始点（底部）
//   r_start_x/y   右边界起始点（底部）
//   hightest      左右边界相遇的最高行（输出）
// =====================================================================================
void search_l_r(uint16_t break_flag, uint8_t(*image)[IMAGE_W],
                uint16_t *l_stastic, uint16_t *r_stastic,
                uint8_t l_start_x, uint8_t l_start_y,
                uint8_t r_start_x, uint8_t r_start_y,
                uint8_t* hightest)
{
    uint8_t i = 0, j = 0;

    // ==================== 左边界8邻域初始化 ====================
    // 8个邻域坐标缓存
    uint8_t search_filds_l[8][2] = {0};
    // 左边界候选点数量
    uint8_t index_l = 0;
    // 左边界候选点缓存
    uint8_t temp_l[8][2] = {0};
    // 左边界当前中心点
    uint8_t center_point_l[2] = {0};
    // 左边界点计数
    uint16_t l_data_statics;

    // 左边界8邻域搜索顺序（顺时针）
    // 顺序：下 → 左下 → 左 → 左上 → 上 → 右上 → 右 → 右下
    static int8_t seeds_l[8][2] = {
        {0,1}, {-1,1}, {-1,0}, {-1,-1},
        {0,-1},{1,-1},{1,0},{1,1}
    };

    // ==================== 右边界8邻域初始化 ====================
    uint8_t search_filds_r[8][2] = {0};
    uint8_t center_point_r[2] = {0};
    uint8_t index_r = 0;
    uint8_t temp_r[8][2] = {0};
    uint16_t r_data_statics;

    // 右边界8邻域搜索顺序（逆时针，保证对称搜索）
    static int8_t seeds_r[8][2] = {
        {0,1},{1,1},{1,0},{1,-1},
        {0,-1},{-1,-1},{-1,0},{-1,1}
    };

    // 将外部计数读入本地变量
    l_data_statics = *l_stastic;
    r_data_statics = *r_stastic;

    // 设置左右边界的起始点（从底部开始）
    center_point_l[0] = l_start_x;
    center_point_l[1] = l_start_y;
    center_point_r[0] = r_start_x;
    center_point_r[1] = r_start_y;

    // ==================== 开始循环向上搜索 ====================
    while (break_flag--)
    {
        // ========== 1. 计算左点8个邻域的坐标 ==========
        for (i=0; i<8; i++)
        {
            search_filds_l[i][0] = center_point_l[0] + seeds_l[i][0];
            search_filds_l[i][1] = center_point_l[1] + seeds_l[i][1];
        }
        // 保存当前左中心点到边界数组
        points_l[l_data_statics][0] = center_point_l[0];
        points_l[l_data_statics][1] = center_point_l[1];
        l_data_statics++;

        // ========== 2. 计算右点8个邻域的坐标 ==========
        for (i=0; i<8; i++)
        {
            search_filds_r[i][0] = center_point_r[0] + seeds_r[i][0];
            search_filds_r[i][1] = center_point_r[1] + seeds_r[i][1];
        }
        // 保存当前右中心点到边界数组
        points_r[r_data_statics][0] = center_point_r[0];
        points_r[r_data_statics][1] = center_point_r[1];
        // ========== 3. 左边界：寻找下一个边缘点 ==========
        index_l = 0;
        // 清空候选点
        for(i=0;i<8;i++){ temp_l[i][0]=0; temp_l[i][1]=0; }

        // 遍历8个方向，寻找【黑点→白点】跳变（跑道边缘）
        for(i=0;i<8;i++)
        {
            // 当前点是黑线(0)，下一个点是背景(255) → 找到边缘
            if( image[search_filds_l[i][1]][search_filds_l[i][0]] == 0
             && image[search_filds_l[(i+1)&7][1]][search_filds_l[(i+1)&7][0]] == 255 )
            {
                // 保存这个边缘点
                temp_l[index_l][0] = search_filds_l[i][0];
                temp_l[index_l][1] = search_filds_l[i][1];
                index_l++;
                // 记录方向（用于十字、弯道判断）
                dir_l[l_data_statics-1] = i;
                cout<<(int)i;
            }

            // 如果找到候选点，选最靠上的那个作为下一个中心点
            if(index_l)
            {
                center_point_l[0] = temp_l[0][0];
                center_point_l[1] = temp_l[0][1];
                // 遍历所有候选，取y最小（最上面）的点
                for(j=0;j<index_l;j++)
                {
                    if(center_point_l[1] > temp_l[j][1])
                    {
                        center_point_l[0] = temp_l[j][0];
                        center_point_l[1] = temp_l[j][1];
                    }
                }
            }
        }

        // ========== 4. 防死循环 & 边界相遇 & 同步保护 ==========
        // 连续3个点不动 → 卡死，退出
        if( (points_r[r_data_statics][0]==points_r[r_data_statics-1][0] && points_r[r_data_statics][0]==points_r[r_data_statics-2][0]
          && points_r[r_data_statics][1]==points_r[r_data_statics-1][1] && points_r[r_data_statics][1]==points_r[r_data_statics-2][1])
          ||(points_l[l_data_statics-1][0]==points_l[l_data_statics-2][0] && points_l[l_data_statics-1][0]==points_l[l_data_statics-3][0]
          && points_l[l_data_statics-1][1]==points_l[l_data_statics-2][1] && points_l[l_data_statics-1][1]==points_l[l_data_statics-3][1]))
        {
            break;
        }

        // 左右点几乎重合 → 边界相遇，巡线结束
        if( my_abs(points_r[r_data_statics][0] - points_l[l_data_statics-1][0]) < 2
         && my_abs(points_r[r_data_statics][1] - points_l[l_data_statics-1][1]) < 2 )
        {
            // 记录相遇的最高行
            *hightest = (points_r[r_data_statics][1] + points_l[l_data_statics-1][1])/2;
            break;
        }

        // 右边比左边高 → 左边等待右边，不更新,需要同步更新
        if( points_r[r_data_statics][1] < points_l[l_data_statics-1][1] )
        {
            continue;
        }

        // 特殊方向修正，防止跑偏，7表示方向为右下
        if( dir_l[l_data_statics-1] ==7 && (points_r[r_data_statics][1] > points_l[l_data_statics-1][1]) )
        {
            center_point_l[0] = points_l[l_data_statics-1][0];
            center_point_l[1] = points_l[l_data_statics-1][1];
            l_data_statics--;
        }

        // 右点计数+1
        r_data_statics++;

        // ========== 5. 右边界：寻找下一个边缘点 ==========
        index_r = 0;
        for(i=0;i<8;i++){ temp_r[i][0]=0; temp_r[i][1]=0; }

        // 同左边界逻辑：找 黑→白 跳变
        for(i=0;i<8;i++)
        {
            if( image[search_filds_r[i][1]][search_filds_r[i][0]] ==0
             && image[search_filds_r[(i+1)&7][1]][search_filds_r[(i+1)&7][0]] ==255 )
            {
                temp_r[index_r][0] = search_filds_r[i][0];
                temp_r[index_r][1] = search_filds_r[i][1];
                index_r++;
                dir_r[r_data_statics-1] = i;
                //cout<<(int)i;
            }

            if(index_r)
            {
                center_point_r[0] = temp_r[0][0];
                center_point_r[1] = temp_r[0][1];
                // 选最靠上的点
                for(j=0;j<index_r;j++)
                {
                    if(center_point_r[1] > temp_r[j][1])
                    {
                        center_point_r[0] = temp_r[j][0];
                        center_point_r[1] = temp_r[j][1];
                    }
                }
            }
        }
    }

    // 把最终计数写回外部变量
    *l_stastic = l_data_statics;
    *r_stastic = r_data_statics;
}




// 提取每行左线
//-----------------------------------------------------------------------------------
uint8_t l_border[IMAGE_H];
uint8_t r_border[IMAGE_H];
uint8_t center_line[IMAGE_H];

void get_left(uint16_t total_L)//原理同下，看下面的函数
{
    uint8_t i;
    uint16_t j;
    uint8_t h;

    for(i=0;i<IMAGE_H;i++) l_border[i] = border_min;
    h = IMAGE_H - 2;

    for(j=0;j<total_L;j++)
    {
        if(points_l[j][1] == h)
        {
            l_border[h] = points_l[j][0]+1;
        }
        else
            continue;
        h--;
        if(h == 0) break;
    }
}

//-----------------------------------------------------------------------------------
// 提取每行右线
//-----------------------------------------------------------------------------------
void get_right(uint16_t total_R)
{
    uint8_t i;
    uint16_t j;
    uint8_t h;

    for(i=0;i<IMAGE_H;i++) r_border[i] = border_max;//默认右线
    h = IMAGE_H -2;//从倒数第三行开始

    for(j=0;j<total_R;j++)//每行只保留一个边界点
    {
        if(points_r[j][1] == h)
        {
            r_border[h] = points_r[j][0]-1;
        }
        else
            continue;
        h--;
        if(h ==0) break;//完成标志
    }
}



bool is_yuanhuan(uint8_t* hightest,uint16_t dir_r[USE_num],uint16_t dir_l[USE_num],uint16_t data_stastics_l,uint16_t data_stastics_r
 ,uchar small_image[60][80])
{
    for(int i=20;i<data_stastics_r;i++)
    {
        if((dir_r[i-1]==5 |  dir_r[i]==5 ) &dir_r[i+2]==4 & (dir_r[i+4]==3 | dir_r[i+5]==3))//有凹处
        {
            if((dir_r[i-20]==4 & dir_r[i-19]==4 &dir_r[i-18]==4)|(dir_r[i-26]==4 & dir_r[i-27]==4 &dir_r[i-28]==4))//下面是岔路
            {
                if(small_image[2][2]==255)//右上角点为白色（要进的岔路）
                {
                    return true;//找到了，改变特殊点的值
                }
            }
        }
        
    }
    return false;

}


void yuanhuan_deal(uint8_t hightest,uint16_t dir_r[USE_num],uint16_t dir_l[USE_num],uint16_t data_stastics_l,uint16_t data_stastics_r
 ,uchar small_image[60][80],uint16_t (*points_r)[2],uint16_t (*points_l)[2],uint8_t (*l_border)[IMAGE_H],uint8_t (*r_border)[IMAGE_H])
 {
    uint16_t point_angle[2],point_fuzhu[2];
    if(flag_yuanhuan==1)//进入环岛标志位
    {
        if(hightest>10)
        {
            flag_yuanhuan++;
        }
    }
    else if(flag_yuanhuan==2)//环岛绕圈标志位
    {
        if(hightest<10)
        {
            flag_yuanhuan++;
        }
    }
    else if(flag_yuanhuan==3)//出环岛标志位
    {
        if(small_image[57][2]==255 & small_image[57][78]==255)//底下全是白色
        {
            flag_yuanhuan++;
        }
    }
    else//回归赛道标志位
    {
        if(small_image[50][2]==0 | small_image[57][77]==0)//注意原来不是50是57
            flag_yuanhuan=0;
    }
    switch (flag_yuanhuan)
    {
    case 1:
        for(int i=0;i<data_stastics_r-2;i++)//优化方向：可以从大一点的数开始
        {
            if(dir_r[i]==7 & (dir_r[i+1]==5 | dir_r[i+2]==5))//找到拐点，参见图yuanhuan4
            {
                point_angle[0]=points_r[i][0];
                point_angle[1]=points_r[i][1];
                point_fuzhu[0]=2;
                point_fuzhu[1]=57;
                line_to_border(point_fuzhu,point_angle,l_border[0]);
                break;
            }
        }
        break;
    case 2:
        break;
    case 3:
        for(int i=2;i<data_stastics_l;i++)//优化方向：可以从大一点的数开始
        {
            if(dir_l[i]==3 & (dir_l[i-1]==5 | dir_l[i-2]==5))//找到左凹点,参见图yuanhuan6
            {
                
                point_angle[0]=points_l[i][0];
                point_angle[1]=points_l[i][1];
                point_fuzhu[0]=77;//写死了固定点，待优化
                point_fuzhu[1]=20;
                line_to_border(point_fuzhu,point_angle,l_border[0]);
                //补线
                break;
            }

        }
        break;
    case 4:
    cout<<"ru";
                point_angle[0]=2;
                point_angle[1]=58;
                point_fuzhu[0]=77;//写死了固定点，待优化
                point_fuzhu[1]=20;
                line_to_border(point_fuzhu,point_angle,l_border[0]);
        break;
        //下全空白补线
    case 0:
        break;    
    
    }

 }

#endif
int main()
{
    cv::Mat color = cv::imread("saidao_pic/yuanhuan7.jpg");//读取图片（jpg）

    uchar small_image[60][80] = {0};//数组图像初始化

    color_to_gray_array(color, small_image);//将mat转化为二维数组

    uchar YUZHI=otsu_binary0(small_image);//更新阈值

    otsu_binary(small_image,YUZHI);//根据阈值二值化

    image_filter(small_image);//补断线，去噪声

    image_draw_rectan(small_image);//画黑框（算法需要


    #if 0
    // ========================
    // 4. 八邻域巡线（纯数组版）
    // ========================
    unsigned char ZHONGJIAN[IMAGE_H] = {39};
    unsigned char ZUO[IMAGE_H] = {0};
    unsigned char YOU[IMAGE_H] = {79};

    ZHONGJIAN[59] = (AX + BX) / 2;

    // 八邻域方向
    int balinyuL[8][2] = {{1,0},{1,-1},{0,-1},{-1,-1},
                          {-1,0},{-1,1},{0,1},{1,1}};

    int balinyuR[8][2] = {{-1,0},{-1,-1},{0,-1},{1,-1},
                          {1,0},{1,1},{0,1},{-1,1}};

    // ========== 左线跟踪 ==========
    int Xha = AX ;
    int Yha = 59;

    for (int i = 0; i < 5; i++)
    {
        Xha = AX ;
        Yha = 59;
        Xha += balinyuL[i][0];
        Yha += balinyuL[i][1];

        if (small_image[Yha][Xha] == 0)
        {
            small_image[Yha][Xha] = 100;
            ZUO[Yha] = Xha;
            break;
        }
    }

    for (int j = 0; Yha > 20 && Xha > 1 && j < 60; j++)
    {
        int Xha_cp = Xha;
        int Yha_cp = Yha;

        for (int i = 0; i < 8; i++)
        {
            Xha = Xha_cp;
            Yha = Yha_cp;
            Xha += balinyuL[i][0];
            Yha += balinyuL[i][1];

            if (small_image[Yha][Xha] == 0)
            {
                small_image[Yha][Xha] = 100;
                ZUO[Yha] = Xha;
                break;
            }
        }
    }

    // ========== 右线跟踪 ==========
    int Xhb = BX ;
    int Yhb = 59;

    for (int i = 0; i < 5; i++)
    {
        Xhb = BX ;
        Yhb = 59;
        Xhb += balinyuR[i][0];
        Yhb += balinyuR[i][1];

        if (small_image[Yhb][Xhb] == 0)
        {
            small_image[Yhb][Xhb] = 100;
            YOU[Yhb] = Xhb;
            break;
        }
    }

    for (int j = 0; Yhb > 20 && Xhb < 79 && j < 60; j++)
    {
        int Xhb_cp = Xhb;
        int Yhb_cp = Yhb;

        for (int i = 0; i < 8; i++)
        {
            Xhb = Xhb_cp;
            Yhb = Yhb_cp;
            Xhb += balinyuR[i][0];
            Yhb += balinyuR[i][1];

            if (small_image[Yhb][Xhb] == 0)
            {
                small_image[Yhb][Xhb] = 100;
                YOU[Yhb] = Xhb;
                break;
            }
        }
    }

    // ========== 计算中线 ==========
    for (int i = 59; i > 20; i--)
    {
        ZHONGJIAN[i] = (ZUO[i] + YOU[i]) / 2;
        small_image[i][ZHONGJIAN[i]] = 100;
    }
    #endif

    uint16_t i;
    uint8_t hightest = 0;//重要参数，是左右线相遇时的最高点


    data_stastics_l =0;
    data_stastics_r =0;

    if(get_start_point((IMAGE_H-2),small_image)) // 5.找起点,只有找到时才运行下面程序
    {
        search_l_r(USE_num, small_image, &data_stastics_l, &data_stastics_r,
                   start_point_l[0], start_point_l[1],
                   start_point_r[0], start_point_r[1], &hightest);  //提取左右线

        get_left(data_stastics_l);    // 6.提取左线
        get_right(data_stastics_r);   // 7.提取右线
        //元素处理
        if(is_yuanhuan(&hightest,dir_r,dir_l,data_stastics_l,data_stastics_r,small_image))flag_yuanhuan=1;
        if(flag_yuanhuan)
        {
            yuanhuan_deal(hightest,dir_r,dir_l,data_stastics_l,data_stastics_r,small_image,points_r,points_l,&l_border,&r_border);
        }
    }


    // 计算并画中线左线右线,调试用
    for(i=hightest; i<IMAGE_H-1; i++)
    {
        center_line[i] = (l_border[i] + r_border[i])/2;
        small_image[i][l_border[i]]=180;
        small_image[i][r_border[i]]=80;
        small_image[i][center_line[i]]=0;
    }



    // 创建名为"RGB"的窗口并显示图像
    show_array_image(small_image);
    
    // 等待按键输入（参数0表示无限等待，直到按下任意键）
    waitKey(0);
    
    // 销毁所有创建的窗口，释放资源
    destroyAllWindows();

    return 0;
}