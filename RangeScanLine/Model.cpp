#include "Model.h"



Model::Model()
{
	//��ʼ��
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

//����ģ��
void Model::LoadOBJModel(string filename)
{
	ifstream infile(filename.c_str());
	string sline;//ÿһ��

	string str;

	cout << "************ begin load obj model ************" << endl;
	cout << "load obj name : " << filename << endl;
	while (getline(infile, sline))
	{
		if (sline.empty())
			continue;

		//��������
		if (sline[0] == 'v')
		{
			if (sline[1] == 'n')	//vn   //�㷨��
			{
				istringstream vin(sline);  //istringstream������԰�һ���ַ�����Ȼ���Կո�Ϊ�ָ����Ѹ��зָ�������
				vertex vtmp;
				vin >> str >> vtmp.x >> vtmp.y >> vtmp.z;
				vNormal.push_back(vtmp);
				vnormalNum++;
			}
			else if (sline[1] == 't')	//vt  //����ɫ
			{
				istringstream vin(sline);
				color ctmp;
				vin >> str >> ctmp.r >> ctmp.g >> ctmp.b;
				vcolors.push_back(ctmp);
				vcolorNum++;
			}
			else	//v                 //������
			{
				istringstream vin(sline);
				vertex vtmp;
				vin >> str >> vtmp.x >> vtmp.y >> vtmp.z;
				vertexs.push_back(vtmp);
				vertexNum++;

			}
		}

		//������
		if (sline[0] == 'f')
		{
			if (sline[1] == 'n')    //�淨��
			{
				istringstream fin(sline);
				vertex fntmp;
				fin >> str >> fntmp.x >> fntmp.y >> fntmp.z;
				fNormal.push_back(fntmp);
				fnormalNum++;
			}
			else if (sline[1] == 't')   //����ɫ
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
				istringstream fin(sline); //��Ķ������
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

	//�����淨�ߺ͸��㷨��
	ComputeNormal();
	//GetColors();
	cout << "vertex num : " << vertexNum << endl;
	cout << "face num : " << faceNum << endl;
	cout << "************ load obj model finished ************" << endl;
}

//���ģ����Ϣ
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

//���㷨��
void Model::ComputeNormal()
{
	//������ķ��ߣ���Ҳ����Ax + By + Cz + D = 0�е�A,B,C
	if (fnormalNum == 0)
	{
		for (int i = 0; i < faceNum; i++)
		{
			vertex v1 = vertexs[faces[i].idx_v[2]] - vertexs[faces[i].idx_v[1]];
			vertex v2 = vertexs[faces[i].idx_v[0]] - vertexs[faces[i].idx_v[1]];

			//���߲��
			vertex fn = v1.cross(v2);

			//��һ��
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

//����ģ�ʹ�С
void Model::Resize(int width, int height)
{
	vertex minv(0xfffffff, 0xfffffff, 0xfffffff);
	vertex maxv(-0xfffffff, -0xfffffff, -0xfffffff);
	vertex centerv(0, 0, 0);

	//�õ�ģ�͵İ�Χ�е�minv ��maxv
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

	//����ģ�ͣ���������Ļ���Ͻ�
	for (int i = 0; i < vertexNum; i++)
	{
		vertexs[i].x = (vertexs[i].x - centerv.x) * scale + width / 2;
		vertexs[i].y = (vertexs[i].y - centerv.y) * scale + height / 2;
		vertexs[i].z = (vertexs[i].z - centerv.z) * scale;
	}

	this->center_point = vertex(width / 2, height / 2, 0);
}

//�õ�ģ����ɫ
void Model::GetColors()
{
	float ka, kd;                             //ϵ��
	ka = 0.2; kd = 0.8;
	color ambient_color(0.6, 0.6, 0.6);        //������
	color diffuse_color(0.8, 0.8, 0.8);        //������ 
	vertex diffuse_position(400.0f, 600.0f, 500.0f);  //��Դλ��


	if (fcolorNum == 0)
	{
		for (int i = 0; i < faceNum; i++)
		{
			face f = faces[i];
			vertex fn = fNormal[i];
			vertex fp(0, 0, 0);

			for (int j = 0; j < f.idx_v.size(); j++)   //����ÿ����Ƭ������λ��
				fp = fp + vertexs[f.idx_v[j]];
			fp = fp / f.idx_v.size();

			vertex L = diffuse_position - fp; //��Դ����Ƭ��λ��ʸ��L
			L.normalize();

			float N_L = L.dot(fn);            //��Դ����Ƭ�����ˣ��õ�N*L


			color fc;
			fc = ambient_color * ka + diffuse_color * kd * N_L;  //�������Ƭ��ɫ

			//��Ƭ��ɫ������0-1֮��
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

//ƽ��
void Model::Translate(float tx, float ty, float tz)
{
	for (int i = 0; i < vertexNum; i++)
	{
		vertexs[i] = vertexs[i] + vertex(tx, ty, tz);
	}
}

//����
void Model::Scale(float sx, float sy, float sz)
{
	for (int i = 0; i < vertexNum; i++)
	{
		vertex tmp_point = vertexs[i] - center_point;  //ȥ������λ�ã�ʹģ������Ļ��������
		vertexs[i] = tmp_point * vertex(sx, sy, sz);
		vertexs[i] = vertexs[i] + center_point;
	}
}

//��ת
void Model::Rotate(float RotateMat[3][3])
{
	//����
	for (int i = 0; i < vertexNum; ++i)
	{
		vertex tmp_point = vertexs[i] - center_point;  //ȥ������λ�ã�ʹģ������Ļ������ת
		vertexs[i].x = RotateMat[0][0] * tmp_point.x + RotateMat[0][1] * tmp_point.y + RotateMat[0][2] * tmp_point.z;
		vertexs[i].y = RotateMat[1][0] * tmp_point.x + RotateMat[1][1] * tmp_point.y + RotateMat[1][2] * tmp_point.z;
		vertexs[i].z = RotateMat[2][0] * tmp_point.x + RotateMat[2][1] * tmp_point.y + RotateMat[2][2] * tmp_point.z;

		vertexs[i] = vertexs[i] + center_point;
	}

	// ��ķ�����
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