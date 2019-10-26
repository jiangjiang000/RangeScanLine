#pragma once
#include"Model.h"
#include<algorithm>
#include<list>
#include<ctime>

//�߱�
typedef struct EdgeNode
{
	float x;                 //�ߵ��϶˵��x����
	int ymax;                //�ߵ��϶˵��y���꣬�����ҵ���ET�е����
	int py;                  //�����ڶ���ε����϶˵��y���꣬�����ҵ���Ӧ�������PT�е�λ��
	int px;                  //�����ڶ���ε����϶˵�y�µĶ�����������ţ������ҵ���Ӧ�������PT[y]�е�λ��
	float dx;                //����ɨ���߼��x�����ϵ�������-1/k
	float dy;                //Ӧ��������zbuffer��ı������dy���߿����ɨ��������
	int id;                  //�����������id
};
//����α�
typedef struct PolygonNode
{
	int id;                   //����Σ���Ƭ����id
	float a, b, c, d;            //��ķ��̵ĸ���ϵ��
	float dy;                 //������ɨ��������
	bool flag;                //��ʼΪout,��false
	color rgb;                //����ε���ɫ
};
//��߱�
typedef struct ActiveEdge
{
	float x;                  // �����x����
	float dx;                 // (�������)������ɨ���߽����x����֮��.-1/k
	int dy;                   // �Ժͽ������ڱ��ཻ��ɨ������Ϊ��ֵ���Ժ�����ÿ����һ��ɨ���߼�1
	int ymax;                 //�ߵ��϶˵��y����

	int py;                   //�����ڶ���ε����϶˵��y���꣬�����ҵ���Ӧ�������PT�е�λ��
	int px;                   //�����ڶ���ε����϶˵�y�µĶ�����������ţ������ҵ���Ӧ�������PT[y]�е�λ��

	float z;                  // ���㴦���������ƽ������ֵ
	float dzx;                // ��ɨ���������߹�һ������ʱ�����������ƽ����������������ƽ�淽�̣�dzx = -a/c(c!=0)
	float dzy;                // ��y���������ƹ�һ��ɨ����ʱ�����������ƽ����������������ƽ�淽�̣�dzy = b/c(c!=0)
	int id;                   // ��������ڵĶ���εı��
};

class RangeScanLine
{
private:
	vector<vector<EdgeNode>> ET;           //EdgeTable���߱�
	vector<vector<PolygonNode>> PT;        //polygonTable������α�
	vector<ActiveEdge> AET;                //ActiveEdgeTable�������α�
	vector<PolygonNode> IPL;               //in-polygon-list

	Model model;                           //ģ��

public:
	int height, width;                     //buffer�ĸߡ���Ҳ�Ǵ��ڵĸߡ���
	float*** Buffer;

	RangeScanLine();
	RangeScanLine(Model model, int width, int height);
	~RangeScanLine();

	void build_ET_PT(Model model);         //����EdgeTable��PolygonTable

	PolygonNode find_Polygon_By_ID(int id, int &row, int &col);  //���ݱߵ�id��PT���ҵ���Ӧ��polygon
	bool find_Polygon_By_XY(int id, int y, int x);      //����polygon��PT�е�λ���ҵ���Ӧ��polygon

	void delete_item_IPL(PolygonNode p);    //����idɾ��IPL�еĶ�Ӧ��Ԫ��
	void build_AET(int y);                  //����ActiveEdgeTable
	PolygonNode closest_poly_in_IPL(ActiveEdge ae1, ActiveEdge ae2, int y); //�ҵ�ae1��ae2֮���polygon
	void build_IPL(int y);                  //����in-polygon-list
	void update_AET_IPL();                  //����AET��IPL
	void draw_line(int x1, int x2, int y, PolygonNode p); //�������㣬������ɫ����buffer��polygon����ɫֵ
	void doRangeScanLine(Model modell);     //������Ȳ���
	void clearBuffer();                     //���bufferֵ��Ϊ0

};

