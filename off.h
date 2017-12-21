#include <vector>
#include <Windows.h>
#include <GL\glut.h>
using std::vector;

class Points {
public:
	float x, y, z;
	Points(float _x, float _y, float _z) :x(_x), y(_y), z(_z) {}
};

class Faces {
public:
	int count;
	int order[3];
	Faces(int _count, int a, int b, int c){
		count = _count;
		order[0] = a;
		order[1] = b;
		order[2] = c;
	}
};

class Off {
public:
	vector<Points> ps;
	vector<Faces> fs;
	int point_num, face_num, edge_num;
	//vector<vector<vector<int>>> edgeVertice;
	Off() {}
	void readoff(const char* filename)
	{
		ps.clear();
		fs.clear();
		FILE* fp;
		errno_t err;
		int i = 0;
		err = fopen_s(&fp, filename, "r");
		if (err != 0)
		{
			fprintf(stderr, "open fail");
		}
		else
		{
			char buffer[1024];
			if (fgets(buffer, 1023, fp))
			{
				if (!strstr(buffer, "OFF"))
				{
					printf("file is not a .OFF file");
					return;
				}
				if (fgets(buffer, 1023, fp))
				{
					sscanf(buffer, "%d %d %d", &point_num, &face_num, &edge_num);
					//printf("%d %d %d", point_num, face_num, edge_num);
					for (i = 0; i<point_num; i++)
					{
						float t0, t1, t2;
						fgets(buffer, 1023, fp);
						sscanf(buffer, "%f%f%f", &t0, &t1, &t2);
						ps.push_back(Points(t0,t1,t2));
					}
					for (i = 0; i<face_num; i++)
					{
						fgets(buffer, 1023, fp);
						int p0,p1,p2,p3;
						sscanf(buffer, "%d%d%d%d", &p0, &p1, &p2, &p3);
						fs.push_back(Faces(p0,p1,p2,p3));
					}
				}
			}
		}
		fclose(fp);
		//printf("endread");
	}

	void normalize(float v[3])
	{
		float d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
		if (d == 0.0)
		{
			printf("zero length vector");
			return;
		}
		else
		{
			v[0] /= d;
			v[1] /= d;
			v[2] /= d;
		}
	}

	void normcrosspord(float v1[3], float v2[3], float out[3])
	{
		out[0] = v1[1] * v2[2] - v1[2] * v2[1];
		out[1] = v1[2] * v2[0] - v1[0] * v2[2];
		out[2] = v1[0] * v2[1] - v1[1] * v2[0];
		normalize(out);
	}


	void draw(bool flag) {
		glPushMatrix();
		if (flag == 1)
		{
			glBegin(GL_TRIANGLES);
			for (int i = 0; i < face_num; i++)
			{
				float d1[3], d2[3], nor[3];
				d1[0] = ps[fs[i].order[0]].x - ps[fs[i].order[1]].x;
				d1[1] = ps[fs[i].order[0]].y - ps[fs[i].order[1]].y;
				d1[2] = ps[fs[i].order[0]].z - ps[fs[i].order[1]].z;
				d2[0] = ps[fs[i].order[1]].x - ps[fs[i].order[2]].x;
				d2[1] = ps[fs[i].order[1]].y - ps[fs[i].order[2]].y;
				d2[2] = ps[fs[i].order[1]].z - ps[fs[i].order[2]].z;
				normcrosspord(d1, d2, nor);
				glNormal3fv(nor);
				int index = fs[i].order[0];
				glVertex3f(ps[index].x, ps[index].y, ps[index].z);
				index = fs[i].order[1];
				glVertex3f(ps[index].x, ps[index].y, ps[index].z);
				index = fs[i].order[2];
				glVertex3f(ps[index].x, ps[index].y, ps[index].z);
			}
			glEnd();
		}
		else
		{
			glBegin(GL_POINTS);
			for (int i = 0; i < face_num; i++)
			{
				int index = fs[i].order[0];
				glVertex3f(ps[index].x, ps[index].y, ps[index].z);
				index = fs[i].order[1];
				glVertex3f(ps[index].x, ps[index].y, ps[index].z);
				index = fs[i].order[2];
				glVertex3f(ps[index].x, ps[index].y, ps[index].z);
			}
			glEnd();
		}
		glPopMatrix();
	}

	void show()
	{
		printf("pointnum:%d, facenum:%d, edgenum:%d\n", point_num, face_num, edge_num);
		for (int i = 0; i < point_num; i++)
		{
			printf("point%d:  %f, %f, %f\n", i, ps[i].x, ps[i].y, ps[i].z);
		}
		for (int i = 0; i < face_num; i++)
		{
			printf("face:%d: %d %d %d\n", i, fs[i].order[0], fs[i].order[1], fs[i].order[2]);
		}
	}

	void loop() {
		vector<vector<vector<int>>> edgeVertice(point_num,vector<vector<int>>(point_num,vector<int>(3,-1)));
		vector<Faces> newfaces;
		vector<Points> newpoints;
		int newfacesnum = 0;
		int origin_num = point_num;
		int vp,vq,vr;
		int a, b, c;
		float xx, yy, zz;
		//对每个面进行操作，生成新点、新面
		for (int i = 0; i < face_num; i++)
		{
			a = fs[i].order[0];
			b = fs[i].order[1];
			c = fs[i].order[2];
			int t;
			//令a<b<c
			if (a>b){t = a;a = b;b = t;}
			if (b>c){t = b;b = c;c = t;}
			if (a>b){t = a;a = b;b = t;}
			//如果该面的边ab尚未处理，则需要在该边上生成新的点;如果这条边已经处理过了，记录此边的另一个邻接面上的顶点索引
			if (edgeVertice[a][b][0] == -1) 
			{
				vq = point_num++; //边ab上的新点的索引
				ps.push_back(Points(-1, -1, -1));
				edgeVertice[a][b][0] = vq;//标记该点已处理
				edgeVertice[a][b][1] = c;//记录此边邻接的第一个面上顶点的索引
			}
			else
			{
				vq = edgeVertice[a][b][0];
				edgeVertice[a][b][2] = c;//记录此边邻接的第2个面上顶点的索引
			}
			//如果该面的边ac尚未处理，则需要在该边上生成新的点;如果这条边已经处理过了，记录此边的另一个邻接面上的顶点索引
			if (edgeVertice[a][c][0] == -1)
			{
				vp = point_num++; //边ac上的新点的索引
				ps.push_back(Points(-1, -1, -1));
				edgeVertice[a][c][0] = vp;//标记该点已处理
				edgeVertice[a][c][1] = b;//记录此边邻接的第一个面上顶点的索引
			}
			else
			{
				vp = edgeVertice[a][c][0];
				edgeVertice[a][c][2] = b;
			}
			//如果该面的边bc尚未处理，则需要在该边上生成新的点;如果这条边已经处理过了，记录此边的另一个邻接面上的顶点索引
			if (edgeVertice[b][c][0] == -1)
			{
				vr = point_num++; //边bc上的新点的索引
				ps.push_back(Points(-1, -1, -1));
				edgeVertice[b][c][0] = vr;//标记该点已处理
				edgeVertice[b][c][1] = a;//记录此边邻接的第一个面上顶点的索引
			}
			else
			{
				vr = edgeVertice[b][c][0];
				edgeVertice[b][c][2] = a;
			}

			//每个三角形面片生成四个新的三角形
			newfaces.push_back(Faces(3,vp,vq,vr));
			newfaces.push_back(Faces(3, b, vq, vr));
			newfaces.push_back(Faces(3, a, vq, vp));
			newfaces.push_back(Faces(3, c, vp, vr));
			newfacesnum += 4;
		}
		/*for (int i = 0; i < origin_num; i++)
		{
			for (int j = 0; j < origin_num; j++)
			{
				printf("%d  ", edgeVertice[i][j][0]);
				//printf("%d", edgeVertice[i][j][1]);
				//printf("%d\n", edgeVertice[i][j][2]);
			}
			printf("%d\n", i + 1);
		}*/
		//计算新点的坐标（E-顶点）
		for (int i = 0; i < origin_num - 1; i++)
		{
			for (int j = i + 1; j < origin_num; j++)
			{
				vp = edgeVertice[i][j][0];//索引
				if (vp != -1)
				{
					vq = edgeVertice[i][j][1];
					vr = edgeVertice[i][j][2];
					if (vr == -1)//边界E-顶点
					{
						xx = 0.5*(ps[i].x + ps[j].x);
						yy = 0.5*(ps[i].y + ps[j].y);
						zz = 0.5*(ps[i].z + ps[j].z);
						//printf("bj-E\n");
						ps[vp].x = xx;
						ps[vp].y = yy;
						ps[vp].z = zz;
					}
					else//内部E-顶点
					{
						xx = 0.375 * (ps[i].x + ps[j].x) + 0.125 * (ps[vq].x + ps[vr].x);
						yy = 0.375 * (ps[i].y + ps[j].y) + 0.125 * (ps[vq].y + ps[vr].y);
						zz = 0.375 * (ps[i].z + ps[j].z) + 0.125 * (ps[vq].z + ps[vr].z);
						//printf("nb-E\n");
						ps[vp].x = xx;
						ps[vp].y = yy;
						ps[vp].z = zz; 
					}
				}
			}
		}
		//计算原本每个点相邻的顶点
		vector<vector<int>>adjVertice(origin_num, vector<int>(1, -1));
		for (int i = 0; i < origin_num; i++)
		{
			for (int j = 0; j < origin_num; j++)
			{
				if ((i < j && edgeVertice[i][j][0] != -1) || (i > j && edgeVertice[j][i][0] != -1))
				{
					if (adjVertice[i][0] == -1) { adjVertice[i][0] = j; }
					else
					{
						adjVertice[i].push_back(j);
					}
				}
			}
			/*for (int j = 0; j < adjVertice[i].size(); j++)
				printf("%d  ", adjVertice[i][j]);
			printf("\n");*/
		}

		//计算旧点的坐标（V-顶点）
		newpoints = ps;
		int l;
		int vi,temp1,temp2;
		float beta;
		float sumx, sumy, sumz;
		vector<int> adjBoundaryVertices;//边界点集
		for (int i = 0; i < origin_num; i++)
		{
			l = adjVertice[i].size();
			adjBoundaryVertices.clear();
			for (int j = 0; j < l; j++)
			{
				vi = adjVertice[i][j];
				if ((i < vi && edgeVertice[i][vi][2] == -1) || (i > vi && edgeVertice[vi][i][2] == -1))//如果边i-vi是只与一个面邻接
				{
					adjBoundaryVertices.push_back(vi);
				}
			}
			if (adjBoundaryVertices.size() == 2)//边界V-顶点
			{
				temp1 = adjBoundaryVertices[0];
				temp2 = adjBoundaryVertices[1];
				//printf("bj-v\n");
				newpoints[i].x = 0.75 * ps[i].x + 0.125 * (ps[temp1].x + ps[temp2].x);
				newpoints[i].y = 0.75 * ps[i].y + 0.125 * (ps[temp1].y + ps[temp2].y);
				newpoints[i].x = 0.75 * ps[i].z + 0.125 * (ps[temp1].z + ps[temp2].z);
			}
			else//内部V-顶点
			{
				//printf("nb-v\n");
				if (l == 3) beta = (float)3 / 16;
				else beta = 3.0 / (8 * l);
				sumx = 0;
				sumy = 0;
				sumz = 0;
				for (int k = 0; k < l; k++)
				{
					sumx += ps[adjVertice[i][k]].x;
					sumy += ps[adjVertice[i][k]].y;
					sumz += ps[adjVertice[i][k]].z;
				}
				newpoints[i].x = (1 - l*beta)*ps[i].x + beta * sumx;
				newpoints[i].y = (1 - l*beta)*ps[i].y + beta * sumy;
				newpoints[i].z = (1 - l*beta)*ps[i].z + beta * sumz;
			}

		}
		ps = newpoints;
		fs = newfaces;
		face_num = newfacesnum;
		newfaces.clear();
		/*for (int i = 0; i < face_num; i++)
		{
			printf("%f  %f  %f \n", ps[fs[i].order[0]].x, ps[fs[i].order[0]].y, ps[fs[i].order[0]].z);
			printf("%f  %f  %f \n", ps[fs[i].order[1]].x, ps[fs[i].order[1]].y, ps[fs[i].order[1]].z);
			printf("%f  %f  %f \n", ps[fs[i].order[2]].x, ps[fs[i].order[2]].y, ps[fs[i].order[2]].z);
			printf("face%d\n", i);
		}*/
		//printf("end1oop");
	}

};
