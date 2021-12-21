#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <fstream>

#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"
#include "TVector2.h"
#include "TVectorF.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TLine.h"

using namespace std;

const double PI = 3.1415926;

int NofCells;
int gX_Layers = 10; // 全局定义变量 探测器构型的XYZ
int gY_Layers = 10;
int gZ_Layers = 67;
double gdetUnit_XY = 40.4;
int RadiusMax = 175; // 要计算圆柱的半径的变化范围
int RadiusMin = 151;

int nPoints = 1e7; // 模拟撒点的个数
int Radius = 50;   // 要计算圆柱的半径 这里统一以mm为单位，故省略
int Xoffsets = 4;  // 模拟seed的中心位置（不是坐标，是索引）
int Yoffsets = 5;  // 0-9

// 控制参数
bool isDraw = 0; //  ==1 只计算一次，需指定cellID。 如果画图那么不生成csv
string filenameTitle = "WeightResult";

class DetectorUnitPoint
{

public:
    DetectorUnitPoint(int xindex, int yindex, double length, int gX_Layers, int gY_Layers)
    {
        counts = 0;
        XIndex = xindex;
        YIndex = yindex;
        sideLen = length;

        X = (XIndex - 1 / 2 * gX_Layers) * sideLen;
        Y = (YIndex - 1 / 2 * gY_Layers) * sideLen;
    }
    bool isIn(double xr, double yr)
    {
        if (xr >= X && xr < X + sideLen && yr >= Y && yr < Y + sideLen)
        {
            counts += 1;
            return 1;
        }
        else
            return 0;
    }
    void clearCounts()
    {
        counts = 0;
    }

public:
    int XIndex, YIndex;
    double X, Y;    // 该Unit左下角的坐标
    double sideLen; // 每一个unit的边长
    int counts;     // 统计撒点法求面积时，落在该Unit的数目
};

// 相关参数的声明
std::vector<DetectorUnitPoint> UnitPointList;
int x_index, y_index;
ofstream fout;                      //创建ofstream，使用 csv 来存储数据
TGraph *pointsGraph = new TGraph(); // 声明要画图的对象
string filename;

// 内联函数的声明
inline void drawDetector();
inline void makecsvTitle(string csvName);
inline void SpreadSim();

void spreading151()
{
    NofCells = gX_Layers * gY_Layers + 1; // 仿照G4中NofCell，故比总数+1
    for (int index = 0; index < NofCells - 1; index++)
    {
        x_index = index / gY_Layers;
        y_index = index - x_index * gY_Layers;
        UnitPointList.push_back(DetectorUnitPoint(x_index, y_index, gdetUnit_XY, gX_Layers, gY_Layers));
    }

    int seedX, seedY;                     // 模拟撒点的中心unit索引，0-9
    int cellID[gX_Layers * gY_Layers];    //  X Y
    double weight[gX_Layers * gY_Layers]; // 权重
    int counts;

    if (isDraw)
    {
        seedX = Xoffsets, seedY = Yoffsets;
        SpreadSim();
        TCanvas *c1 = new TCanvas("Spreading Points", "Spreading Points");
        c1->SetRealAspectRatio(1);
        pointsGraph->SetTitle("Spreading Points");
        pointsGraph->SetMarkerColor(kRed);
        pointsGraph->Draw("AP");
        drawDetector();

        for (int index = 0; index < NofCells - 1; index++)
        {
            counts = UnitPointList[index].counts;
            cellID[index] = index;
            weight[index] = counts / (nPoints + 0.) * PI * Radius * Radius / (gdetUnit_XY * gdetUnit_XY);
            if (weight[index] > 0.99)
                weight[index] = 1;                       // 归一化。。
            weight[Xoffsets * gY_Layers + Yoffsets] = 1; // 中心seed肯定为1，不论半径大小
            cout << "counts[" << index << "] = " << counts << "  weight = " << weight[index] << endl;
        }
    }
    else
    {
        for (Radius = RadiusMin; Radius < RadiusMax+1; Radius++) // 遍历不同半径
        {
            filename = filenameTitle + to_string(Radius) + ".csv";
            makecsvTitle(filename);
            for (Xoffsets = 0; Xoffsets < gX_Layers; Xoffsets++) // 遍历不同X方向层数
            {
                for (Yoffsets = 0; Yoffsets < gY_Layers; Yoffsets++) // 遍历不同Y方向层数
                {
                    for (int index = 0; index < NofCells - 1; index++) // 归0
                    {
                        UnitPointList[index].clearCounts();
                    }
                    seedX = Xoffsets, seedY = Yoffsets;
                    SpreadSim();
                    fout << seedX << "\t" << seedY << "\t";
                    for (int index = 0; index < NofCells - 1; index++)
                    {
                        counts = UnitPointList[index].counts;
                        cellID[index] = index;
                        weight[index] = counts / (nPoints + 0.) * PI * Radius * Radius / (gdetUnit_XY * gdetUnit_XY);
                        if (weight[index] > 0.99)
                            weight[index] = 1;                       // 归一化。。
                        weight[Xoffsets * gY_Layers + Yoffsets] = 1; // 中心seed肯定为1，不论半径大小
                        cout << "counts[" << index << "] = " << counts << "  weight = " << weight[index] << endl;
                        fout << weight[index] << "\t";
                    }
                    fout << endl;
                }
            }
            fout.close();
        }
    }
}

inline void drawDetector()
{
    for (int i = 0; i < gX_Layers + 1; i++)
    {
        TLine *xline = new TLine(0, i * gdetUnit_XY, gY_Layers * gdetUnit_XY, i * gdetUnit_XY);
        xline->Draw();
    }
    for (int j = 0; j < gY_Layers + 1; j++)
    {
        TLine *yline = new TLine(j * gdetUnit_XY, 0, j * gdetUnit_XY, gX_Layers * gdetUnit_XY);
        yline->Draw();
    }
}

inline void makecsvTitle(string csvName)
{
    // fout.open("WeighResult.csv",std::ios::app);   //关联一个文件
    fout.open(csvName); //关联一个文件
    fout << "SeedX\t"
         << "SeedY\t";
    for (int i = 0; i < NofCells - 1; i++)
    {
        fout << "Weight " << i << "\t";
    }
    fout << endl;
}

inline void SpreadSim()
{
    double xr, yr;      // 随机点的坐标
    double xO, yO;      // 圆心坐标
    double theta, k, r; // 用于生成圆内均匀的随机数
    int i;

    xO = Xoffsets * gdetUnit_XY + 1. / 2 * gdetUnit_XY; // 偏移到Cell中心
    yO = Yoffsets * gdetUnit_XY + 1. / 2 * gdetUnit_XY;
    for (i = 0; i < nPoints; i++)
    {
        theta = gRandom->Uniform(0, 360); // 生成 [0,360]的随机数
        k = gRandom->Uniform(0, 1);
        r = sqrt(k) * Radius;

        xr = r * cos(theta) + xO;
        yr = r * sin(theta) + yO;

        pointsGraph->SetPoint(i, xr, yr);

        for (int index = 0; index < NofCells - 1; index++)
        {
            if (UnitPointList[index].isIn(xr, yr))
                break; // 如果找到了在该unit内，那么就break
        }
    }
    pointsGraph->SetPoint(i++, -50, -50); // 保证画图的范围足够大
    pointsGraph->SetPoint(i++, (gX_Layers + 1) * gdetUnit_XY, (gY_Layers + 1) * gdetUnit_XY);
    pointsGraph->SetPoint(i++, xO, yO); // 画一下圆心
}