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
using namespace std;
using namespace cv;

// 固定图像尺寸 60行 × 80列
#define IMAGE_H 60
#define IMAGE_W 80



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






int main()
{
    // ========================
    // 1. 大津法阈值（完全不变）
    // ========================
    unsigned int huidu[256] = {0};
    unsigned char YUZHI = 0;
    cv::Mat color = cv::imread("saidao.jpg");
    uchar small_image[60][80] = {0};
    color_to_gray_array(color, small_image);




    // 直方图统计
    for (int i = 0; i < IMAGE_H; i++)
    {
        for (int j = 0; j < IMAGE_W; j++)
        {
            huidu[small_image[i][j]]++;
        }
    }

    // 找第一高峰
    unsigned int H1 = 0;
    unsigned char D1 = 0;
    #define KUAN 30

    for (int i = 0; i < 256; i++)
    {
        if (huidu[i] > H1)
        {
            H1 = huidu[i];
            D1 = i;
        }
    }

    // 找第二高峰
    unsigned int H2 = 0;
    unsigned char D2 = 0;
    bool OK = false;

    for (int i = H1 - 5; i > 0; i -= 5)
    {
        for (int j = 0; j < 256; j++)
        {
            if (huidu[j] > i && abs(j - D1) > KUAN)
            {
                H2 = i;
                D2 = j;
                OK = true;
                break;
            }
        }
        if (OK) break;
    }

    // 找山谷（阈值）
    unsigned int H3 = 4800;
    unsigned char D3 = 0;

    if (OK)
    {
        if (D1 < D2)
        {
            for (int i = D1; i < D2; i++)
            {
                if (huidu[i] < H3)
                {
                    H3 = huidu[i];
                    D3 = i;
                }
            }
        }
        else
        {
            for (int i = D2; i < D1; i++)
            {
                if (huidu[i] < H3)
                {
                    H3 = huidu[i];
                    D3 = i;
                }
            }
        }
        YUZHI = D3;
    }

    // ========================
    // 2. 二值化（替换 Mat）
    // ========================
    for (int i = 0; i < IMAGE_H; i++)
    {
        for (int j = 0; j < IMAGE_W; j++)
        {
            if (small_image[i][j] < YUZHI)
            {
                small_image[i][j] = 0;
            }
            else
            {
                small_image[i][j] = 255;
            }
        }
    }

    // ========================
    // 3. 找底部边缘 AB 点
    // ========================
    unsigned char AX, AY;
    unsigned char BX, BY;

    AY = 59;
    BY = 59;

    // 左找 A
    for (int i = 39; i >= 1; i--)
    {
        if (small_image[AY][i-1] - small_image[AY][i] == -255)
        {
            AX = i;
        }
    }

    // 右找 B
    for (int i = 39; i < 79; i++)
    {
        if (small_image[BY][i+1] - small_image[BY][i] == -255)
        {
            BX = i;
        }
    }

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
    int Xha = AX - 1;
    int Yha = 59;

    for (int i = 0; i < 5; i++)
    {
        Xha = AX - 1;
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
    int Xhb = BX + 1;
    int Yhb = 59;

    for (int i = 0; i < 5; i++)
    {
        Xhb = BX + 1;
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

    // 创建名为"RGB"的窗口并显示图像
    show_array_image(small_image);
    
    // 等待按键输入（参数0表示无限等待，直到按下任意键）
    waitKey(0);
    
    // 销毁所有创建的窗口，释放资源
    destroyAllWindows();

    return 0; // 正常退出程序



    return 0;
}