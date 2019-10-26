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
	int vertexNum;                            //����
	int faceNum;                              //����
	int vnormalNum;                           //�㷨����
	int fnormalNum;                           //�淨����
	int fcolorNum;                            //����ɫ��
	int vcolorNum;                            //����ɫ��

	vector<vertex> vertexs;                   //��
	vector<face> faces;                       //��
	vector<vertex> fNormal;                   //�淨��
	vector<vertex> vNormal;                   //�㷨��
	vector<color> fcolors;                    //����ɫ
	vector<color> vcolors;                    //����ɫ

	vertex center_point;                      //���ĵ�
	//blinnPhong


public:
	Model();
	~Model();
	void LoadOBJModel(string filename);        //����ģ��
	void PrintModel();                         //�����Ϣ
	void ComputeNormal();                      //���㷨��
	void Resize(int width, int height);         //������С
	void GetColors();                          //���ݹ��յõ�ģ����ɫ
	void Translate(float tx, float ty, float tz); //ƽ��
	void Scale(float sx, float sy, float sz);   //����
	void Rotate(float RotateMat[3][3]);       //��ת
	void Clear();
};

