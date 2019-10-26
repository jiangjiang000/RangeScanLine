#pragma once
#include<iostream>
#include<string>
#include<fstream>
#include<sstream>
#include<vector>
#include<math.h>
#include<algorithm>
#include"class.h"
using namespace std;

class Model
{
public:
	int vertexNum;                            //点数
	int faceNum;                              //面数
	int vnormalNum;                           //点法向数
	int fnormalNum;                           //面法向数
	int fcolorNum;                            //面颜色数
	int vcolorNum;                            //点颜色数

	vector<vertex> vertexs;                   //点
	vector<face> faces;                       //面
	vector<vertex> fNormal;                   //面法向
	vector<vertex> vNormal;                   //点法向
	vector<color> fcolors;                    //面颜色
	vector<color> vcolors;                    //点颜色

	vertex center_point;                      //中心点
	//blinnPhong


public:
	Model();
	~Model();
	void LoadOBJModel(string filename);        //加载模型
	void PrintModel();                         //输出信息
	void ComputeNormal();                      //计算法向
	void Resize(int width, int height);         //调整大小
	void GetColors();                          //根据光照得到模型颜色
	void Translate(float tx, float ty, float tz); //平移
	void Scale(float sx, float sy, float sz);   //缩放
	void Rotate(float RotateMat[3][3]);       //旋转
	void Clear();
};

