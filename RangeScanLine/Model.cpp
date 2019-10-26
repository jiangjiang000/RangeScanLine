#include "Model.h"



Model::Model()
{
	//初始化
	vertexNum = 0;
	vnormalNum = 0;
	faceNum = 0;
	fnormalNum = 0;
	fcolorNum = 0;
	vcolorNum = 0;

}


Model::~Model()
{
}

//加载模型
void Model::LoadOBJModel(string filename)
{
	ifstream infile(filename.c_str());
	string sline;//每一行

	string str;

	cout << "************ begin load obj model ************" << endl;
	cout << "load obj name : " << filename << endl;
	while (getline(infile, sline))
	{
		if (sline.empty())
			continue;

		//顶点数据
		if (sline[0] == 'v')
		{
			if (sline[1] == 'n')	//vn   //点法向
			{
				istringstream vin(sline);  //istringstream对象可以绑定一行字符串，然后以空格为分隔符把该行分隔开来。
				vertex vtmp;
				vin >> str >> vtmp.x >> vtmp.y >> vtmp.z;
				vNormal.push_back(vtmp);
				vnormalNum++;
			}
			else if (sline[1] == 't')	//vt  //点颜色
			{
				istringstream vin(sline);
				color ctmp;
				vin >> str >> ctmp.r >> ctmp.g >> ctmp.b;
				vcolors.push_back(ctmp);
				vcolorNum++;
			}
			else	//v                 //点坐标
			{
				istringstream vin(sline);
				vertex vtmp;
				vin >> str >> vtmp.x >> vtmp.y >> vtmp.z;
				vertexs.push_back(vtmp);
				vertexNum++;

			}
		}

		//面数据
		if (sline[0] == 'f')
		{
			if (sline[1] == 'n')    //面法向
			{
				istringstream fin(sline);
				vertex fntmp;
				fin >> str >> fntmp.x >> fntmp.y >> fntmp.z;
				fNormal.push_back(fntmp);
				fnormalNum++;
			}
			else if (sline[1] == 't')   //面颜色
			{
				istringstream fin(sline);
				color fctmp;
				fin >> str >> fctmp.r >> fctmp.g >> fctmp.b;
				fcolors.push_back(fctmp);
				fcolorNum++;
			}
			else
			{
				int a;
				for (int k = sline.size() - 1; k >= 0; k--)
					if (sline[k] == '/')
						sline[k] = ' ';
				istringstream fin(sline); //面的顶点序号
				face ftmp, fttmp;
				vertex fntmp;
				int tmp;
				fin >> str;
				for (int k = 0; k < 3; k++)
				{
					if (vertexNum != 0)
					{
						fin >> tmp;
						ftmp.idx_v.push_back(tmp - 1);
					}
					if (vnormalNum != 0)
					{
						fin >> tmp;
						/*fntmp.x = vNormal[tmp - 1].x;
						fntmp.x = vertexs[tmp - 1].x;*/
					}
					if (vcolorNum != 0)
					{
						fin >> tmp;
						//fntmp.x = vertexs[tmp - 1].x;
					}
				}
				faces.push_back(ftmp);
				faceNum++;
			}
		}
	}
	infile.close();

	//计算面法线和各点法线
	ComputeNormal();
	//GetColors();
	cout << "vertex num : " << vertexNum << endl;
	cout << "face num : " << faceNum << endl;
	cout << "************ load obj model finished ************" << endl;
}

//输出模型信息
void Model::PrintModel()
{
	cout << endl;
	cout << "************print info of model******************" << endl;
	cout << "vertex num : " << vertexNum << endl;
	for (int i = 0; i < vertexNum / 1000; i++)
		cout << vertexs[i].x << ", " << vertexs[i].y << ", " << vertexs[i].z << endl;

	cout << "face num : " << faceNum << endl;
	for (int i = 0; i < faceNum / 1000; i++)
		cout << faces[i].idx_v[0] << ", " << faces[i].idx_v[1] << ", " << faces[i].idx_v[2] << endl;

	cout << "************print finished!*******************" << endl;
}

//计算法向
void Model::ComputeNormal()
{
	//计算面的法线，即也是面Ax + By + Cz + D = 0中的A,B,C
	if (fnormalNum == 0)
	{
		for (int i = 0; i < faceNum; i++)
		{
			vertex v1 = vertexs[faces[i].idx_v[2]] - vertexs[faces[i].idx_v[1]];
			vertex v2 = vertexs[faces[i].idx_v[0]] - vertexs[faces[i].idx_v[1]];

			//两边叉乘
			vertex fn = v1.cross(v2);

			//归一化
			fn.normalize();

			fNormal.push_back(fn);
			fnormalNum++;
		}
	}

	if (vnormalNum == 0)
	{
		/*for (int i = 0; i < vertexNum; i++)
		{
			vertex vn(0,0,0);

			for (int j = 0; j < faceNum; j++)
			{
				if (faces[j].idx_v[0] == i || faces[j].idx_v[1] == i || faces[j].idx_v[2] == i)
				{
					vn.x += fNormal[j].x;
					vn.y += fNormal[j].y;
					vn.z += fNormal[j].z;
				}
			}
			float modvn = (float)sqrt(vn.x * vn.x + vn.y * vn.y + vn.z * vn.z);
			vn.x = vn.x / modvn;
			vn.y = vn.y / modvn;
			vn.z = vn.z / modvn;

			vNormal.push_back(vn);
			vnormalNum++;
			if (i % 1000 == 0)
				cout << i << endl;
		}*/
	}
}

//调整模型大小
void Model::Resize(int width, int height)
{
	vertex minv(0xfffffff, 0xfffffff, 0xfffffff);
	vertex maxv(-0xfffffff, -0xfffffff, -0xfffffff);
	vertex centerv(0, 0, 0);

	//得到模型的包围盒点minv 和maxv
	for (int i = 0; i < vertexNum; i++)
	{
		vertex vtmp = vertexs[i];
		minv.x = min(minv.x, vtmp.x);
		minv.y = min(minv.y, vtmp.y);
		minv.z = min(minv.z, vtmp.z);
		maxv.x = max(maxv.x, vtmp.x);
		maxv.y = max(maxv.y, vtmp.y);
		maxv.z = max(maxv.z, vtmp.z);
	}

	//centerv = (minv + maxv) / 2.00;
	centerv.x = (minv.x + maxv.x) / 2;
	centerv.y = (minv.y + maxv.y) / 2;
	centerv.z = (minv.z + maxv.z) / 2;

	float model_width = maxv.x - minv.x;
	float model_height = maxv.y - minv.y;
	float max_model_len = max(model_width, model_height);

	float scale = min(width, height) / max_model_len * 2 / 3;

	//Scale(scale,scale,scale);

	//缩放模型，并移至屏幕左上角
	for (int i = 0; i < vertexNum; i++)
	{
		vertexs[i].x = (vertexs[i].x - centerv.x) * scale + width / 2;
		vertexs[i].y = (vertexs[i].y - centerv.y) * scale + height / 2;
		vertexs[i].z = (vertexs[i].z - centerv.z) * scale;
	}

	this->center_point = vertex(width / 2, height / 2, 0);
}

//得到模型颜色
void Model::GetColors()
{
	float ka, kd;                             //系数
	ka = 0.2; kd = 0.8;
	color ambient_color(0.6, 0.6, 0.6);        //环境光
	color diffuse_color(0.8, 0.8, 0.8);        //漫反射 
	vertex diffuse_position(400.0f, 600.0f, 500.0f);  //光源位置


	if (fcolorNum == 0)
	{
		for (int i = 0; i < faceNum; i++)
		{
			face f = faces[i];
			vertex fn = fNormal[i];
			vertex fp(0, 0, 0);

			for (int j = 0; j < f.idx_v.size(); j++)   //计算每个面片的中心位置
				fp = fp + vertexs[f.idx_v[j]];
			fp = fp / f.idx_v.size();

			vertex L = diffuse_position - fp; //光源与面片的位置矢量L
			L.normalize();

			float N_L = L.dot(fn);            //光源与面片法向点乘，得到N*L


			color fc;
			fc = ambient_color * ka + diffuse_color * kd * N_L;  //计算得面片颜色

			//面片颜色必须在0-1之间
			if (fc.r > 1.0f)
				fc.r = 1.0f;
			if (fc.r < 0.0f)
				fc.r = 0.0f;
			if (fc.g > 1.0f)
				fc.g = 1.0f;
			if (fc.g < 0.0f)
				fc.g = 0.0f;
			if (fc.b > 1.0f)
				fc.b = 1.0f;
			if (fc.b < 0.0f)
				fc.b = 0.0f;
			fcolors.push_back(fc);
		}
	}



}

//平移
void Model::Translate(float tx, float ty, float tz)
{
	for (int i = 0; i < vertexNum; i++)
	{
		vertexs[i] = vertexs[i] + vertex(tx, ty, tz);
	}
}

//缩放
void Model::Scale(float sx, float sy, float sz)
{
	for (int i = 0; i < vertexNum; i++)
	{
		vertex tmp_point = vertexs[i] - center_point;  //去掉中心位置，使模型绕屏幕中心缩放
		vertexs[i] = tmp_point * vertex(sx, sy, sz);
		vertexs[i] = vertexs[i] + center_point;
	}
}

//旋转
void Model::Rotate(float RotateMat[3][3])
{
	//顶点
	for (int i = 0; i < vertexNum; ++i)
	{
		vertex tmp_point = vertexs[i] - center_point;  //去掉中心位置，使模型绕屏幕中心旋转
		vertexs[i].x = RotateMat[0][0] * tmp_point.x + RotateMat[0][1] * tmp_point.y + RotateMat[0][2] * tmp_point.z;
		vertexs[i].y = RotateMat[1][0] * tmp_point.x + RotateMat[1][1] * tmp_point.y + RotateMat[1][2] * tmp_point.z;
		vertexs[i].z = RotateMat[2][0] * tmp_point.x + RotateMat[2][1] * tmp_point.y + RotateMat[2][2] * tmp_point.z;

		vertexs[i] = vertexs[i] + center_point;
	}

	// 面的法向量
	for (int i = 0; i < fnormalNum; ++i)
	{
		vertex tmp_point = fNormal[i];
		fNormal[i].x = RotateMat[0][0] * tmp_point.x + RotateMat[0][1] * tmp_point.y + RotateMat[0][2] * tmp_point.z;
		fNormal[i].y = RotateMat[1][0] * tmp_point.x + RotateMat[1][1] * tmp_point.y + RotateMat[1][2] * tmp_point.z;
		fNormal[i].z = RotateMat[2][0] * tmp_point.x + RotateMat[2][1] * tmp_point.y + RotateMat[2][2] * tmp_point.z;
	}
}


void Model::Clear()
{
	vertexs.clear();
	faces.clear();
	vNormal.clear();
	fNormal.clear();
	fcolors.clear();
	vcolors.clear();
	center_point = vertex(0,0,0);
	vertexNum = 0;
	vnormalNum = 0;
	faceNum = 0;
	fnormalNum = 0;
	fcolorNum = 0;
	vcolorNum = 0;
}