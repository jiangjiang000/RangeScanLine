#include "RangeScanLine.h"


RangeScanLine::RangeScanLine()
{
}

RangeScanLine::RangeScanLine(Model model, int width, int height)
{
	this->model = model;
	this->width = width;
	this->height = height;

	//��ʼ��buffer
	this->Buffer = new float**[height];
	for (int i = 0; i < height; i++)
	{
		this->Buffer[i] = new float*[width];
		for (int j = 0; j < width; j++)
		{
			this->Buffer[i][j] = new float[3];
			for (int k = 0; k < 3; k++)
				this->Buffer[i][j][k] = 0.0f;
		}
	}
	//cout << "scanline init!!!" << endl;
}


RangeScanLine::~RangeScanLine()
{


}

//���bufferֵ��Ϊ0
void RangeScanLine::clearBuffer()
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			for (int k = 0; k < 3; k++)
				this->Buffer[i][j][k] = 0.0f;
		}
	}
}


//����EdgeTable��PolygonTable
void RangeScanLine::build_ET_PT(Model model)
{
	//��ʼ��
	ET.clear();
	PT.clear();

	ET.resize(height);
	PT.resize(height);

	//����ET��PT
	for (int i = 0; i < model.faceNum; i++)
	{
		face f = model.faces[i];             //�ҵ���Ӧ��
		vertex v = model.vertexs[f.idx_v[0]];//�ҵ���Ӧ��ĵ�һ������

		PolygonNode polygon;                //polygon��idΪ������
		polygon.id = i;

		float ymax = -0xfffffff;            //������
		float ymin = 0xfffffff;             //������

		vector<pair<int, int>> tymax;      //�������οɴ浽ET�����ͬ���϶���ı���
		int isPolygonNull = true;          //�ж�һ��������Ƿ���Ϊ����һ�����ض��Ƿ���Ҫȥ��

		//����ET
		for (int j = 0; j < f.idx_v.size(); j++)
		{
			vertex v1, v2;                  //����������ɣ�v1��v2��ʾÿ���߶�Ӧ�Ķ���
			v1 = model.vertexs[f.idx_v[j]];

			if (j == f.idx_v.size() - 1)         //���һ�������ǿ������һ���������һ����
				v2 = model.vertexs[f.idx_v[0]];
			else
				v2 = model.vertexs[f.idx_v[j + 1]];

			if (v1.y < v2.y)                //��v1һֱ���϶˵�
			{
				vertex tmp = v1;
				v1 = v2;
				v2 = tmp;
			}

			ymin = min(v2.y, ymin);         //yminһֱΪ����ε����¶���
			ymax = max(v1.y, ymax);         //ymaxһֱΪ����ε����¶���

			if (round(v1.y) < 0)                   //������y<=0�ı�    
				continue;
			if (round(v1.y) - round(v2.y) <= 0)    //v1��v2������С��һ�����أ�����Ϊˮƽ�ߣ�������
				continue;

			isPolygonNull = false;                //�����������β�Ϊ��

			EdgeNode edge;                        //����ET����Ԫ�ص�ֵ
			edge.id = i;                          //idΪ������
			edge.x = v1.x;                        //�ߵ��϶˵�x
			edge.dx = (v2.x - v1.x) / (v1.y - v2.y); //б�ʸ���
			edge.dy = round(v1.y) - round(v2.y);   //�߿�Խɨ������Ŀ
			edge.ymax = round(v1.y);              //�ߵ��϶˵�y

			pair<int, int> tmp;                   //tmp��Ӧ������
			tmp.first = edge.ymax;                //firstΪ�ߵ��϶˵�
			tmp.second = 1;                       //secondΪ��ͬfirst�ı߸���
			int k = 0;
			for (; k < tymax.size(); k++)
				if (tymax[k].first == tmp.first)  //������ͬfirst�ı߸���
				{
					tymax[k].second++;
					break;
				}
			if (tymax.size() == 0 || k >= tymax.size())
				tymax.push_back(tmp);

			ET[edge.ymax].push_back(edge);

		}
		//����PT
		if (isPolygonNull)                         //�������δ�СС��һ�������򲻹���������  
			continue;
		polygon.flag = false;                      //��ʼ������ε�flagΪ�񣬱�ʾδ�������
		polygon.dy = round(ymax) - round(ymin);    //����ο�Խ��ɨ������Ŀ
		polygon.rgb = model.fcolors[i];            //����ε���ɫ
		polygon.a = model.fNormal[i].x;            //����η��̵�ϵ����a��b��c�ֱ��Ӧ�淨���x��y��z��dΪ�ؾ�ϵ��
		polygon.b = model.fNormal[i].y;
		polygon.c = model.fNormal[i].z;
		polygon.d = -1 * (model.fNormal[i].x * v.x + model.fNormal[i].y * v.y + model.fNormal[i].z * v.z);
		PT[round(ymax)].push_back(polygon);


		//�������Ӧ�ıߵ����ݽṹԪ�أ�������ڶ���ε����϶˵��y����Ͷ������y�µ����
		for (int j = 0; j < tymax.size(); j++)
			for (int k = 0; k < tymax[j].second; k++)
			{
				ET[tymax[j].first][ET[tymax[j].first].size() - 1 - k].py = round(ymax); //�����ڶ���ε����϶˵��y����Ϊ����ε��϶˵�y
				ET[tymax[j].first][ET[tymax[j].first].size() - 1 - k].px = PT[round(ymax)].size() - 1; //�������y�µ����Ϊ��Ӧy�¶����������
			}
		tymax.clear();
	}

}

//AE��С�Ƚϣ�����
static bool ascendingSort(ActiveEdge a1, ActiveEdge a2)
{
	return (a1.x < a2.x);
}

//���ݱߵ�id��PT���ҵ���Ӧ��polygon
PolygonNode RangeScanLine::find_Polygon_By_ID(int id, int &row, int &col)
{
	for (int i = 0; i < height; i++)
		for (int j = 0; j < PT[i].size(); j++)
			if (PT[i][j].id == id)   //�ж��Ƿ��ǶԵ�polygon
			{
				row = i;
				col = j;
				return PT[i][j];
			}

	cout << "no match polygon find .  id: " << id << endl;


}

//����polygon��PT�е�λ���ҵ���Ӧ��polygon
bool RangeScanLine::find_Polygon_By_XY(int id, int y, int x)
{
	if (y != -1 && x != -1)
		if (PT[y][x].id == id)   //�ж��Ƿ��ǶԵ�polygon
			return true;

	cout << "no match polygon find .  id: " << id << ", ymax: " << y << ", x: " << x << endl;
	return false;

}

//����idɾ��IPL�еĶ�Ӧ��Ԫ��
void RangeScanLine::delete_item_IPL(PolygonNode p)
{
	vector<PolygonNode>::const_iterator it;
	for (it = IPL.begin(); it != IPL.end();)
	{
		if ((*it).id == p.id)
		{
			it = IPL.erase(it);    //eraseɾ��Ԫ�أ��ͻ���ɵ�������ʧЧ����������Ҫ����ָ��һ����������
		}
		else
		{
			++it;
		}
	}
}

//����ActiveEdgeTable
void RangeScanLine::build_AET(int y)
{
	if (ET[y].size() == 0)    //�������ɨ�������ޱߣ�����
		return;
	//AET.clear();
	vector<EdgeNode> ET_one_scanline = ET[y];
	vector<PolygonNode> PT_one_scanline = PT[y];

	for (int i = 0; i < ET_one_scanline.size(); i++)  //����ET��Ӧɨ�����µı��飬����һ�����ŵ�AET��
	{
		//�Ҳ���������Ӧ�Ķ���Σ��򲻼���˱�
		if (!find_Polygon_By_XY(ET_one_scanline[i].id, ET_one_scanline[i].py, ET_one_scanline[i].px))
			break;
		PolygonNode aptmp = PT[ET_one_scanline[i].py][ET_one_scanline[i].px];

		/*for (int i = 0; i < PT_one_scanline.size(); i++)
			if (PT_one_scanline[i].id == ET_one_scanline[i].id)
				aptmp = PT_one_scanline[i];*/
		ActiveEdge aetmp;
		aetmp.x = ET_one_scanline[i].x;
		aetmp.dx = ET_one_scanline[i].dx;
		aetmp.dy = ET_one_scanline[i].dy;
		aetmp.ymax = ET_one_scanline[i].ymax;
		aetmp.py = ET_one_scanline[i].py;
		aetmp.px = ET_one_scanline[i].px;
		aetmp.id = ET_one_scanline[i].id;

		aetmp.z = -1 * (aptmp.a * ET_one_scanline[i].x + aptmp.b * y + aptmp.d) / aptmp.c;
		aetmp.dzx = -1 * aptmp.a / aptmp.c;   //=-kx/z
		aetmp.dzy = -1 * aptmp.b / aptmp.c;   //=-ky/z

		AET.push_back(aetmp);
	}

	//��AET����x����ֵ������������
	sort(AET.begin(), AET.end(), ascendingSort);
	//cout << "sort!" << endl;
}

//�ҵ�ae1��ae2֮���polygon
PolygonNode RangeScanLine::closest_poly_in_IPL(ActiveEdge ae1, ActiveEdge ae2, int y)
{
	float midx = (ae1.x + ae2.x) / 2;  //ȷ��ae1��ae2�м��xֵ
	float z_depth = -0xfffffff;
	PolygonNode p;
	p.id = -1;
	//ѭ��IPL�е�polygon���ж����xֵ��Ӧz����ֵ��С�������жϴ�ʱ�ĸ�polygon�ɼ�
	for (int j = 0; j < IPL.size(); j++)
	{
		PolygonNode ptmp;
		ptmp = IPL[j];
		float ztmp = -1 * (ptmp.a * midx + ptmp.b * y + ptmp.d) / ptmp.c;  //����z����ֵ
		if (z_depth < ztmp)    //ѡ��z����ֵ���polygon
		{
			z_depth = ztmp;
			p = ptmp;
		}
	}
	return p;
}

//����in-polygon-list
void RangeScanLine::build_IPL(int y)
{
	IPL.clear();

	//����AET����IPL
	for (int i = 0; i < AET.size(); i++)
	{
		ActiveEdge ae1 = AET[i];      //ȡAET�еĵ�һ��Ԫ�رߣ�ae1��ae2��ɱ߶�
		int z_id;

		if (!find_Polygon_By_XY(ae1.id, ae1.py, ae1.px))   //�����PT���Ҳ����˱߶�Ӧ�Ķ����������
			break;

		//��߱���ֻʣ���һ����ʱ����ֱ�ӽ������߶�Ӧ�Ķ���ε�flag��Ϊfalse����ʾ�����������г�ȥ
		if (i == AET.size() - 1)
		{
			PT[ae1.py][ae1.px].flag = false;
			//draw_line();
			delete_item_IPL(PT[ae1.py][ae1.px]);
			break;
		}

		//���ae1��flagΪfalse�����ѭ����ae1��flagΪtrue��������������
		if (PT[ae1.py][ae1.px].flag == false)
		{
			PT[ae1.py][ae1.px].flag = true;
			IPL.push_back(PT[ae1.py][ae1.px]);

		}
		else //������ӽ�������->�뿪����Σ���flagΪfalse
		{
			PT[ae1.py][ae1.px].flag = false;
			//draw_line();
			delete_item_IPL(PT[ae1.py][ae1.px]);    //��ӦҪɾ��IPL�б����ڵĶ����
		}

		ActiveEdge ae2 = AET[i + 1];    //ȡAET�еĵڶ�Ԫ�رߣ�ae1��ae2��ɱ߶�

		//���ae1��ae2�����������Ϊһ�����أ������� / IPL��û��Ԫ���ˣ�����
		if (round(ae1.x) == round(ae2.x) || IPL.size() == 0)
			continue;

		PolygonNode p = closest_poly_in_IPL(ae1, ae2, y); //��ae1��ae2�м�����Ķ����
		draw_line(round(ae1.x), round(ae2.x), y, p);//��ɫ

	}
}

//����AET��IPL
void RangeScanLine::update_AET_IPL()
{
	//����AET
	vector<ActiveEdge> AET_TMP;
	for (int i = 0; i < AET.size(); i++)
	{
		ActiveEdge aetmp = AET[i];
		aetmp.dy--;
		if (aetmp.dy <= 0)    //������Ѿ�ɨ�����ˣ���ȥ���ñ�
			continue;
		aetmp.x = aetmp.x + aetmp.dx;
		aetmp.z = aetmp.z + aetmp.dx * aetmp.dzx + aetmp.dzy * 1;   //ÿ�ζ���y�½�һ�����أ�x�仯dx�����أ�z�仯dzx*dx+dzy������
		AET_TMP.push_back(aetmp);
	}
	AET.clear();
	AET = AET_TMP;

	if (IPL.size() != 0)
		cout << "error in IPL����������" << endl;
	IPL.clear();
}

//�������㣬������ɫ����buffer��polygon����ɫֵ
void RangeScanLine::draw_line(int x1, int x2, int y, PolygonNode p)
{
	if (p.id == -1)
		return;
	//cout << "y:" << y << " x: " << x1<< "->" << x2 << ",  id: " << p.id << endl;
	for (int i = x1; i < x2; i++)
	{
		Buffer[y][i][0] = p.rgb.r;
		Buffer[y][i][1] = p.rgb.g;
		Buffer[y][i][2] = p.rgb.b;
	}
}

//������Ȳ���
void RangeScanLine::doRangeScanLine(Model modell)
{
	clearBuffer();
	this->model = modell;

	build_ET_PT(model);    //��ET��PT

	AET.clear();

	clock_t t_begin = clock();
	//���ϵ���ɨ��
	for (int y = height - 1; y >= 0; y--)
	{
		build_AET(y);      //��AET

		if (AET.size() == 0)  //�������ɨ������û�бߣ�����
			continue;

		build_IPL(y);      //��IPL��ͬʱ��������ɨ����
		update_AET_IPL();  //����AET��IPL

	}
	cout << "cost time: " << (clock() - t_begin) / 1000.0 << "s " << endl;
	cout << endl;
}