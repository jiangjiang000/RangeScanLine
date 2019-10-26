#pragma once
#include"Model.h"
#include<algorithm>
#include<list>
#include<ctime>

//边表
typedef struct EdgeNode
{
	float x;                 //边的上端点的x坐标
	int ymax;                //边的上端点的y坐标，用于找到是ET中的序号
	int py;                  //边所在多边形的最上端点的y坐标，用于找到对应多边形在PT中的位置
	int px;                  //边所在多边形的最上端点y下的多边形组里的序号，用于找到对应多边形在PT[y]中的位置
	float dx;                //相邻扫描线间的x方向上的坐标差，即-1/k
	float dy;                //应该类似于zbuffer里的边链表的dy，边跨过的扫描线数量
	int id;                  //边所属的面的id
};
//多边形表
typedef struct PolygonNode
{
	int id;                   //多边形（面片）的id
	float a, b, c, d;            //面的方程的各项系数
	float dy;                 //面跨过的扫描线数量
	bool flag;                //初始为out,即false
	color rgb;                //多边形的颜色
};
//活化边表
typedef struct ActiveEdge
{
	float x;                  // 交点的x坐标
	float dx;                 // (交点边上)两相邻扫描线交点的x坐标之差.-1/k
	int dy;                   // 以和交点所在边相交的扫描线数为初值，以后向下每处理一条扫描线减1
	int ymax;                 //边的上端点的y坐标

	int py;                   //边所在多边形的最上端点的y坐标，用于找到对应多边形在PT中的位置
	int px;                   //边所在多边形的最上端点y下的多边形组里的序号，用于找到对应多边形在PT[y]中的位置

	float z;                  // 交点处多边形所在平面的深度值
	float dzx;                // 沿扫描线向右走过一个像素时，多边形所在平面的深度增量。对于平面方程，dzx = -a/c(c!=0)
	float dzy;                // 沿y方向向下移过一根扫描线时，多边形所在平面的深度增量。对于平面方程，dzy = b/c(c!=0)
	int id;                   // 交点对所在的多边形的编号
};

class RangeScanLine
{
private:
	vector<vector<EdgeNode>> ET;           //EdgeTable，边表
	vector<vector<PolygonNode>> PT;        //polygonTable，多边形表
	vector<ActiveEdge> AET;                //ActiveEdgeTable，活化多边形表
	vector<PolygonNode> IPL;               //in-polygon-list

	Model model;                           //模型

public:
	int height, width;                     //buffer的高、宽，也是窗口的高、宽
	float*** Buffer;

	RangeScanLine();
	RangeScanLine(Model model, int width, int height);
	~RangeScanLine();

	void build_ET_PT(Model model);         //构建EdgeTable和PolygonTable

	PolygonNode find_Polygon_By_ID(int id, int &row, int &col);  //根据边的id在PT中找到对应的polygon
	bool find_Polygon_By_XY(int id, int y, int x);      //根据polygon在PT中的位置找到对应的polygon

	void delete_item_IPL(PolygonNode p);    //根据id删除IPL中的对应的元素
	void build_AET(int y);                  //建立ActiveEdgeTable
	PolygonNode closest_poly_in_IPL(ActiveEdge ae1, ActiveEdge ae2, int y); //找到ae1与ae2之间的polygon
	void build_IPL(int y);                  //建立in-polygon-list
	void update_AET_IPL();                  //更新AET和IPL
	void draw_line(int x1, int x2, int y, PolygonNode p); //根据两点，进行着色，即buffer赋polygon的颜色值
	void doRangeScanLine(Model modell);     //进行深度测试
	void clearBuffer();                     //清空buffer值，为0

};

