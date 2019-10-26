#include "RangeScanLine.h"


RangeScanLine::RangeScanLine()
{
}

RangeScanLine::RangeScanLine(Model model, int width, int height)
{
	this->model = model;
	this->width = width;
	this->height = height;

	//初始化buffer
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

//清空buffer值，为0
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


//构建EdgeTable和PolygonTable
void RangeScanLine::build_ET_PT(Model model)
{
	//初始化
	ET.clear();
	PT.clear();

	ET.resize(height);
	PT.resize(height);

	//构建ET和PT
	for (int i = 0; i < model.faceNum; i++)
	{
		face f = model.faces[i];             //找到对应面
		vertex v = model.vertexs[f.idx_v[0]];//找到对应面的第一个顶点

		PolygonNode polygon;                //polygon的id为面的序号
		polygon.id = i;

		float ymax = -0xfffffff;            //负无穷
		float ymin = 0xfffffff;             //正无穷

		vector<pair<int, int>> tymax;      //这个多边形可存到ET里的相同边上顶点的边组
		int isPolygonNull = true;          //判断一个多边形是否因为不够一个像素而是否需要去除

		//构建ET
		for (int j = 0; j < f.idx_v.size(); j++)
		{
			vertex v1, v2;                  //面由三边组成，v1和v2表示每条边对应的顶点
			v1 = model.vertexs[f.idx_v[j]];

			if (j == f.idx_v.size() - 1)         //最后一个顶点是可以与第一个顶点组成一条边
				v2 = model.vertexs[f.idx_v[0]];
			else
				v2 = model.vertexs[f.idx_v[j + 1]];

			if (v1.y < v2.y)                //让v1一直是上端点
			{
				vertex tmp = v1;
				v1 = v2;
				v2 = tmp;
			}

			ymin = min(v2.y, ymin);         //ymin一直为多边形的最下顶点
			ymax = max(v1.y, ymax);         //ymax一直为多边形的最下顶点

			if (round(v1.y) < 0)                   //不考虑y<=0的边    
				continue;
			if (round(v1.y) - round(v2.y) <= 0)    //v1与v2两点间距小于一个像素，则视为水平边，不考虑
				continue;

			isPolygonNull = false;                //表明这个多边形不为空

			EdgeNode edge;                        //构建ET单个元素的值
			edge.id = i;                          //id为面的序号
			edge.x = v1.x;                        //边的上端点x
			edge.dx = (v2.x - v1.x) / (v1.y - v2.y); //斜率负数
			edge.dy = round(v1.y) - round(v2.y);   //边跨越扫描线数目
			edge.ymax = round(v1.y);              //边的上端点y

			pair<int, int> tmp;                   //tmp对应此条边
			tmp.first = edge.ymax;                //first为边的上端点
			tmp.second = 1;                       //second为相同first的边个数
			int k = 0;
			for (; k < tymax.size(); k++)
				if (tymax[k].first == tmp.first)  //计算相同first的边个数
				{
					tymax[k].second++;
					break;
				}
			if (tymax.size() == 0 || k >= tymax.size())
				tymax.push_back(tmp);

			ET[edge.ymax].push_back(edge);

		}
		//构建PT
		if (isPolygonNull)                         //这个多边形大小小于一个像素则不构建，跳过  
			continue;
		polygon.flag = false;                      //初始化多边形的flag为否，表示未进入此面
		polygon.dy = round(ymax) - round(ymin);    //多边形跨越的扫描线数目
		polygon.rgb = model.fcolors[i];            //多边形的颜色
		polygon.a = model.fNormal[i].x;            //多边形方程的系数，a、b、c分别对应面法向的x、y、z，d为截距系数
		polygon.b = model.fNormal[i].y;
		polygon.c = model.fNormal[i].z;
		polygon.d = -1 * (model.fNormal[i].x * v.x + model.fNormal[i].y * v.y + model.fNormal[i].z * v.z);
		PT[round(ymax)].push_back(polygon);


		//完善面对应的边的数据结构元素，如边所在多边形的最上端点的y坐标和多边形在y下的序号
		for (int j = 0; j < tymax.size(); j++)
			for (int k = 0; k < tymax[j].second; k++)
			{
				ET[tymax[j].first][ET[tymax[j].first].size() - 1 - k].py = round(ymax); //边所在多边形的最上端点的y坐标为多边形的上端点y
				ET[tymax[j].first][ET[tymax[j].first].size() - 1 - k].px = PT[round(ymax)].size() - 1; //多边形在y下的序号为对应y下多边形组的序号
			}
		tymax.clear();
	}

}

//AE大小比较，升序
static bool ascendingSort(ActiveEdge a1, ActiveEdge a2)
{
	return (a1.x < a2.x);
}

//根据边的id在PT中找到对应的polygon
PolygonNode RangeScanLine::find_Polygon_By_ID(int id, int &row, int &col)
{
	for (int i = 0; i < height; i++)
		for (int j = 0; j < PT[i].size(); j++)
			if (PT[i][j].id == id)   //判断是否是对的polygon
			{
				row = i;
				col = j;
				return PT[i][j];
			}

	cout << "no match polygon find .  id: " << id << endl;


}

//根据polygon在PT中的位置找到对应的polygon
bool RangeScanLine::find_Polygon_By_XY(int id, int y, int x)
{
	if (y != -1 && x != -1)
		if (PT[y][x].id == id)   //判断是否是对的polygon
			return true;

	cout << "no match polygon find .  id: " << id << ", ymax: " << y << ", x: " << x << endl;
	return false;

}

//根据id删除IPL中的对应的元素
void RangeScanLine::delete_item_IPL(PolygonNode p)
{
	vector<PolygonNode>::const_iterator it;
	for (it = IPL.begin(); it != IPL.end();)
	{
		if ((*it).id == p.id)
		{
			it = IPL.erase(it);    //erase删除元素，就会造成迭代器的失效，所以这里要重新指定一个迭代器。
		}
		else
		{
			++it;
		}
	}
}

//建立ActiveEdgeTable
void RangeScanLine::build_AET(int y)
{
	if (ET[y].size() == 0)    //如果这条扫描线下无边，则跳
		return;
	//AET.clear();
	vector<EdgeNode> ET_one_scanline = ET[y];
	vector<PolygonNode> PT_one_scanline = PT[y];

	for (int i = 0; i < ET_one_scanline.size(); i++)  //根据ET对应扫描线下的边组，将其一个个放到AET中
	{
		//找不到边所对应的多边形，则不加入此边
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

	//对AET按照x坐标值进行升序排序
	sort(AET.begin(), AET.end(), ascendingSort);
	//cout << "sort!" << endl;
}

//找到ae1与ae2之间的polygon
PolygonNode RangeScanLine::closest_poly_in_IPL(ActiveEdge ae1, ActiveEdge ae2, int y)
{
	float midx = (ae1.x + ae2.x) / 2;  //确定ae1和ae2中间的x值
	float z_depth = -0xfffffff;
	PolygonNode p;
	p.id = -1;
	//循环IPL中的polygon，判断这个x值对应z坐标值大小，用于判断此时哪个polygon可见
	for (int j = 0; j < IPL.size(); j++)
	{
		PolygonNode ptmp;
		ptmp = IPL[j];
		float ztmp = -1 * (ptmp.a * midx + ptmp.b * y + ptmp.d) / ptmp.c;  //计算z坐标值
		if (z_depth < ztmp)    //选择z坐标值大的polygon
		{
			z_depth = ztmp;
			p = ptmp;
		}
	}
	return p;
}

//建立in-polygon-list
void RangeScanLine::build_IPL(int y)
{
	IPL.clear();

	//根据AET建立IPL
	for (int i = 0; i < AET.size(); i++)
	{
		ActiveEdge ae1 = AET[i];      //取AET中的第一个元素边，ae1与ae2组成边对
		int z_id;

		if (!find_Polygon_By_XY(ae1.id, ae1.py, ae1.px))   //如果在PT中找不到此边对应的多边形则跳过
			break;

		//活化边表中只剩最后一条边时，可直接将这条边对应的多边形的flag置为false，表示从这个多边形中出去
		if (i == AET.size() - 1)
		{
			PT[ae1.py][ae1.px].flag = false;
			//draw_line();
			delete_item_IPL(PT[ae1.py][ae1.px]);
			break;
		}

		//如果ae1的flag为false，则此循环改ae1的flag为true，表明进入多边形
		if (PT[ae1.py][ae1.px].flag == false)
		{
			PT[ae1.py][ae1.px].flag = true;
			IPL.push_back(PT[ae1.py][ae1.px]);

		}
		else //否则，则从进入多边形->离开多边形，该flag为false
		{
			PT[ae1.py][ae1.px].flag = false;
			//draw_line();
			delete_item_IPL(PT[ae1.py][ae1.px]);    //对应要删除IPL中边所在的多边形
		}

		ActiveEdge ae2 = AET[i + 1];    //取AET中的第二元素边，ae1与ae2组成边对

		//如果ae1和ae2很相近，几乎为一个像素，则跳过 / IPL中没有元素了，跳过
		if (round(ae1.x) == round(ae2.x) || IPL.size() == 0)
			continue;

		PolygonNode p = closest_poly_in_IPL(ae1, ae2, y); //找ae1和ae2中间最近的多边形
		draw_line(round(ae1.x), round(ae2.x), y, p);//着色

	}
}

//更新AET和IPL
void RangeScanLine::update_AET_IPL()
{
	//更新AET
	vector<ActiveEdge> AET_TMP;
	for (int i = 0; i < AET.size(); i++)
	{
		ActiveEdge aetmp = AET[i];
		aetmp.dy--;
		if (aetmp.dy <= 0)    //当活化边已经扫描完了，则去除该边
			continue;
		aetmp.x = aetmp.x + aetmp.dx;
		aetmp.z = aetmp.z + aetmp.dx * aetmp.dzx + aetmp.dzy * 1;   //每次都是y下降一个像素，x变化dx个像素，z变化dzx*dx+dzy个像素
		AET_TMP.push_back(aetmp);
	}
	AET.clear();
	AET = AET_TMP;

	if (IPL.size() != 0)
		cout << "error in IPL！！！！！" << endl;
	IPL.clear();
}

//根据两点，进行着色，即buffer赋polygon的颜色值
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

//进行深度测试
void RangeScanLine::doRangeScanLine(Model modell)
{
	clearBuffer();
	this->model = modell;

	build_ET_PT(model);    //建ET和PT

	AET.clear();

	clock_t t_begin = clock();
	//从上到下扫描
	for (int y = height - 1; y >= 0; y--)
	{
		build_AET(y);      //建AET

		if (AET.size() == 0)  //如果这条扫描线上没有边，则跳
			continue;

		build_IPL(y);      //建IPL，同时进行区间扫描线
		update_AET_IPL();  //更新AET和IPL

	}
	cout << "cost time: " << (clock() - t_begin) / 1000.0 << "s " << endl;
	cout << endl;
}