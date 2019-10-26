#include<iostream>
#include<io.h>
#include<gl/freeglut.h>
#include<gl/glut.h>
#include"Model.h"
#include"RangeScanLine.h"

using namespace std;

#define PI  3.1415926

//��ʼ���ڴ�С
int w_width = 600;
int w_height = 400;

vector<string> modelfiles;  //����ģ���ļ����б�
Model model;                //ģ��
RangeScanLine rsl;          //ɨ����

//��ȡģ��Ŀ¼
void GetAllModels(string path, vector<string>& files)
{
	//�ļ����
	long hFile = 0;
	//�ļ���Ϣ
	struct _finddata_t fileinfo;  //�����õ��ļ���Ϣ��ȡ�ṹ
	string p;  //string�������˼��һ����ֵ����:assign()���кܶ����ذ汾
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do {
			if ((fileinfo.attrib & _A_SUBDIR))
			{  //�Ƚ��ļ������Ƿ����ļ���,��������
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					continue;
				}
			}
			else
			{
				files.push_back(fileinfo.name);

			}
		} while (_findnext(hFile, &fileinfo) == 0);  //Ѱ����һ�����ɹ�����0������-1
		_findclose(hFile);

	}
}

void init(int num)
{
	int k;
	if (num == -1)
	{

		GetAllModels("model", modelfiles);

		cout << "****************************** model list ********************************" << endl;
		for (int i = 0; i < modelfiles.size(); i++)
			cout << i << "." << modelfiles[i] << endl;

		cout << "please choose one model : ";
		cin >> k;
		cout << endl;
	}
	else
	{
		model.Clear();   //�л�ģ��
		k = num;
	}
	model.LoadOBJModel("model/" + modelfiles[k]);
	model.Resize(w_width, w_height);
	model.GetColors();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	cout << endl;
	cout << "********************************* help ****************************************" << endl;
	cout << "translate:   press A/D/W/S:       x- / x+ / y+ / y-   " << endl;
	cout << "rotate:      press a/d/w/s/q/e:   y+ / y- / x+ / x- / z+ / z-  " << endl;
	//cout << "scale:   press 1/2   + / -  " << endl;
	cout << endl;

	rsl = RangeScanLine(model, w_width, w_height);
	rsl.doRangeScanLine(model);
}

void reshape(int w, int h)
{
	int ratio = 1.0f * w / h;

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);

	glViewport(0, 0, w, h);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//���ڷ����仯ʱ������ģ�ʹ�С�Լ�ɨ����
	if (w_width != w || w_height != h)
	{
		cout << "window size changed" << endl;
		w_width = w;
		w_height = h;
		model.Resize(w_width, w_height);
		rsl = RangeScanLine(model, w_width, w_height);
		rsl.doRangeScanLine(model);
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);  //û�п�����ȣ����Բ�ҪGL_DEPTH_BUFFER_BIT

	glPushMatrix();

	glBegin(GL_POINTS);
	for (int i = 0; i < rsl.height; i++)
	{
		for (int j = 0; j < rsl.width; j++)
		{
			glColor3f(rsl.Buffer[i][j][0], rsl.Buffer[i][j][1], rsl.Buffer[i][j][2]);  //����ɨ���ߵ�buffer��ģ�͵�
			glVertex2i(j, i);
		}
	}
	glEnd();

	glPopMatrix();
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
	float theta = 0.0;
	float costheta, sintheta;
	float RotateMat[3][3] = { 0 };
	float tx, ty, tz, s;
	tx = 0; ty = 0; tz = 0, s = 1;
	switch (key)
	{
		//ƽ��
	case 'A':   //x�Ḻ��
	{
		cout << "translate x-" << endl;
		tx -= 10;
		model.Translate(tx, ty, tz);
		//cout << "1.(x,y) = " << model.vertexs[0].x << "," << model.vertexs[0].y << endl;
		break;
	}
	case 'D':   //x������
	{
		cout << "translate x+" << endl;
		tx += 10;
		model.Translate(tx, ty, tz);
		//cout << "1.(x,y) = " << model.vertexs[0].x << "," << model.vertexs[0].y << endl;
		break;
	}
	case 'W':   //y������
	{
		cout << "translate y+" << endl;
		ty += 10;
		model.Translate(tx, ty, tz);
		//cout << "1.(x,y) = " << model.vertexs[0].x << "," << model.vertexs[0].y << endl;
		break;
	}
	case 'S':   //y�Ḻ��
	{
		cout << "translate y-" << endl;
		ty -= 10;
		model.Translate(tx, ty, tz);
		//cout << "1.(x,y) = " << model.vertexs[0].x << "," << model.vertexs[0].y << endl;
		break;
	}
	//��ת
	case 'a':   //y��˳ʱ��
	{
		cout << "rotate y+ " << endl;
		theta = PI / 6;
		costheta = cos(theta);
		sintheta = sin(theta);
		float RotateMat[3][3] = { costheta,0,-1 * sintheta,0,1,0,sintheta,0,costheta };
		model.Rotate(RotateMat);
		//cout << "1.(x,y) = " << model.vertexs[0].x << "," << model.vertexs[0].y << endl;

		break;
	}

	case 'd':   //y����ʱ��
	{
		cout << "rotate y- " << endl;
		theta = -1 * PI / 6;
		costheta = cos(theta);
		sintheta = sin(theta);
		float RotateMat[3][3] = { costheta,0,-1 * sintheta,0,1,0,sintheta,0,costheta };
		model.Rotate(RotateMat);

		break;
	}

	case 'w':    //x��˳ʱ��
	{
		cout << "rotate x+ " << endl;
		theta = PI / 6;
		costheta = cos(theta);
		sintheta = sin(theta);
		float RotateMat[3][3] = { 1,0,0 ,0, costheta,sintheta,0,-1 * sintheta,costheta };
		model.Rotate(RotateMat);

		break;
	}

	case 's':   //x����ʱ��
	{
		cout << "rotate x- " << endl;
		theta = -1 * PI / 6;
		costheta = cos(theta);
		sintheta = sin(theta);
		float RotateMat[3][3] = { 1,0,0 ,0, costheta,sintheta,0,-1 * sintheta,costheta };
		model.Rotate(RotateMat);

		break;
	}
	case 'q':   //z��˳ʱ��
	{
		cout << "rotate z+ " << endl;
		theta = PI / 6;
		costheta = cos(theta);
		sintheta = sin(theta);
		float RotateMat[3][3] = { costheta,sintheta,0,-1 * sintheta,costheta,0,0,0,1 };
		model.Rotate(RotateMat);

		break;
	}
	case 'e':   //z����ʱ��
	{
		cout << "rotate z- " << endl;
		theta = -1 * PI / 6;
		costheta = cos(theta);
		sintheta = sin(theta);
		float RotateMat[3][3] = { costheta,sintheta,0,-1 * sintheta,costheta,0,0,0,1 };
		model.Rotate(RotateMat);

		break;
	}
	////����
	//case '1':   //�Ŵ�
	//{
	//	s += 0.25;
	//	model.Scale(s, s, s);
	//	cout << "1.(x,y) = " << model.vertexs[0].x << "," << model.vertexs[0].y << endl;
	//	break;
	//}
	//case '2':   //��С
	//{
	//	s -= 0.25;
	//	model.Scale(s, s, s);
	//	cout << "1.(x,y) = " << model.vertexs[0].x << "," << model.vertexs[0].y << endl;
	//	break;
	//}
	default:
		break;
	}

	rsl.doRangeScanLine(model);
	glutPostRedisplay();
}

//�˵��¼����л�ģ��
void mainMenu(int id)
{
	switch (id)
	{
	case 0:  
		init(0);
		break;
	case 1:
		init(1);
		break;
	case 2:
		init(2);
		break;
	case 3:
		init(3);
		break;
	case 4:
		init(4);
		break;
	case 5:
		init(5);
		break;
	default:
		break;
	}
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(w_width, w_height);
	glutInitWindowPosition(100, 100);


	init(-1);
	glutCreateWindow("Load a model");
	glutCreateMenu(mainMenu);
	for (int i = 0; i < modelfiles.size(); i++)   //�˵����л�ģ��
		glutAddMenuEntry(modelfiles[i].c_str(), i);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutKeyboardFunc(keyboard);                  //����
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();

	system("pause");
	return 0;
}