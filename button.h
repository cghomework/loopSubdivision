#define BMP_Header_Length 54  //图像数据在内存块中的偏移量  

/*bitmap字体
GLUT_BITMAP_8_BY_13
GLUT_BITMAP_9_BY_15
GLUT_BITMAP_TIMES_ROMAN_10 字体：TIMES_ROMAN 大小：10
GLUT_BITMAP_TIMES_ROMAN_24 字体：TIMES_ROMAN 大小：24
GLUT_BITMAP_HELVETICA_10
GLUT_BITMAP_HELVETICA_12
GLUT_BITMAP_HELVETICA_18
*/

class button {
private:
	// 函数power_of_two用于判断一个整数是不是2的整数次幂    
	int power_of_two(int n) {
		if (n <= 0)
			return 0;
		return (n & (n - 1)) == 0;
	}
	void showTex() {
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, _tex);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(x, y);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(x + w, y);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(x + w, y + h);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(x, y + h);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
		glPopAttrib();
	}
	void showText() {
		setColor(colorText);
		glRasterPos2d(x + 5, y + h - 18);
		for (char *c = _text; *c != '\0'; c++) {
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
		}
	}
	void showBorder() {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		setColor(colorBorder);
		glBegin(GL_LINE_STRIP & GL_LINE_LOOP);
		glVertex2f(x, y);
		glVertex2f(x + w, y);
		glVertex2f(x + w, y + h);
		glVertex2f(x, y + h);
		glEnd();
	}
	void showInside() {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		setColor(colorInside);
		glBegin(GL_POLYGON);
		glVertex3f(x, y, 0);
		glVertex3f(x + w, y, 0);
		glVertex3f(x + w, y + h, 0);
		glVertex3f(x, y + h, 0);
		glEnd();
	}
	void setColor(double *color) {
		glColor4dv(color);
	}
public:
	float x, y;
	float w, h;
	double colorInside[4];
	double colorBorder[4];
	double colorHoverInside[4];
	double colorHoverBorder[4];
	double colorText[4];
	int solid;
	int border;
	int mouseIn;
	GLuint _tex;
	int tex;
	char *_text;
	button(float _x = 0, float _y = 0, float _w = 125, float _h = 25) :
		x(_x), y(_y), w(_w), h(_h), solid(1), border(0), mouseIn(0), tex(0), _text(NULL) {
		for (int i = 0; i<3; i++) {
			colorBorder[i] = 0.5;
			colorInside[i] = 1.0;
			colorHoverBorder[i] = 0.5;
			colorHoverInside[i] = 1.0;
			colorText[i] = 0.0;
		}
		colorBorder[3] = 1.0;
		colorInside[3] = 1.0;
		colorHoverBorder[3] = 1.0;
		colorHoverInside[3] = 1.0;
		colorText[3] = 1.0;
	}
	void setPosition(int _x, int _y) {
		x = _x;
		y = _y;
	}
	void setSize(int _w, int _h) {
		w = _w;
		h = _h;
	}
	void setColorInside(double c1, double c2, double c3, double c4 = 1.0) {
		colorInside[0] = c1;
		colorInside[1] = c2;
		colorInside[2] = c3;
		colorInside[3] = c4;
	}
	void setColorBorder(double c1, double c2, double c3, double c4 = 1.0) {
		colorBorder[0] = c1;
		colorBorder[1] = c2;
		colorBorder[2] = c3;
		colorBorder[3] = c4;
	}
	void setText(const char *text) {
		if (text == NULL) {
			_text = NULL;
			return;
		}
		_text = new char[strlen(text)];
		strcpy(_text, text);
	}
	void setBorder(int x = 1) {
		border = x;
	}
	void setSolid(int x = 1) {
		solid = x;
	}
	void setTex(int x = 0) {
		tex = x;
	}
	void show() {
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glPushMatrix();
		if (solid) {
			if (tex) showTex();
			else showInside();
		}
		if (border) showBorder();
		if (_text != NULL)	showText();
		glPopMatrix();
		glPopAttrib();
	}
	void mouseHover() {
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glPushMatrix();
		/*
		setColor(colorHoverInside);
		glBegin(GL_POLYGON);
		glVertex2f(x,y);
		glVertex2f(x+w,y);
		glVertex2f(x+w,y+h);
		glVertex2f(x,y+h);
		glEnd();
		*/

		setColor(colorHoverBorder);
		glBegin(GL_LINE_STRIP & GL_LINE_LOOP);
		glVertex2f(x, y);
		glVertex2f(x + w, y);
		glVertex2f(x + w, y + h);
		glVertex2f(x, y + h);
		glEnd();
		//		glFlush();
		glPopMatrix();
		glPopAttrib();
	}
	void mouseUnhover() {
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glPushMatrix();
		setColor(colorInside);
		glBegin(GL_POLYGON);
		glVertex2f(x, y);
		glVertex2f(x + w, y);
		glVertex2f(x + w, y + h);
		glVertex2f(x, y + h);
		glEnd();
		setColor(colorBorder);
		glBegin(GL_LINE_STRIP & GL_LINE_LOOP);
		glVertex2f(x, y);
		glVertex2f(x + w, y);
		glVertex2f(x + w, y + h);
		glVertex2f(x, y + h);
		glEnd();
		//		glFlush();
		glPopMatrix();
		glPopAttrib();
	}
	int setTexImage(const char* file_name) {
		GLint width, height, total_bytes;
		GLubyte* pixels = 0;
		GLuint last_texture_ID = 0, texture_ID = 0;

		// 打开文件，如果失败，返回    
		FILE* pFile = fopen(file_name, "rb");
		if (pFile == 0)
			return 2;

		// 读取文件中图象的宽度和高度    
		fseek(pFile, 0x0012, SEEK_SET);
		fread(&width, 4, 1, pFile);
		fread(&height, 4, 1, pFile);
		fseek(pFile, BMP_Header_Length, SEEK_SET);

		// 计算每行像素所占字节数，并根据此数据计算总像素字节数    
		{
			GLint line_bytes = width * 3;
			while (line_bytes % 4 != 0)
				++line_bytes;
			total_bytes = line_bytes * height;
		}

		// 根据总像素字节数分配内存    
		pixels = (GLubyte*)malloc(total_bytes);
		if (pixels == 0)
		{
			fclose(pFile);
			return 3;
		}

		// 读取像素数据    
		if (fread(pixels, total_bytes, 1, pFile) <= 0)
		{
			free(pixels);
			fclose(pFile);
			return 4;
		}

		// 对就旧版本的兼容，如果图象的宽度和高度不是的整数次方，则需要进行缩放    
		// 若图像宽高超过了OpenGL规定的最大值，也缩放    
		/*{
		GLint max;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
		if( !power_of_two(width)
		|| !power_of_two(height)
		|| width > max
		|| height > max )
		{
		const GLint new_width = 256;
		const GLint new_height = 256; // 规定缩放后新的大小为边长的正方形
		GLint new_line_bytes, new_total_bytes;
		GLubyte* new_pixels = 0;

		// 计算每行需要的字节数和总字节数
		new_line_bytes = new_width * 3;
		while( new_line_bytes % 4 != 0 )
		++new_line_bytes;
		new_total_bytes = new_line_bytes * new_height;

		// 分配内存
		new_pixels = (GLubyte*)malloc(new_total_bytes);
		if( new_pixels == 0 )
		{
		free(pixels);
		fclose(pFile);
		return 5;
		}

		// 进行像素缩放
		gluScaleImage(GL_RGB,
		width, height, GL_UNSIGNED_BYTE, pixels,
		new_width, new_height, GL_UNSIGNED_BYTE, new_pixels);

		// 释放原来的像素数据，把pixels指向新的像素数据，并重新设置width和height
		free(pixels);
		pixels = new_pixels;
		width = new_width;
		height = new_height;
		}
		}*/

		// 分配一个新的纹理编号    
		glGenTextures(1, &texture_ID);
		if (texture_ID == 0)
		{
			free(pixels);
			fclose(pFile);
			return 6;
		}

		// 绑定新的纹理，载入纹理并设置纹理参数    
		// 在绑定前，先获得原来绑定的纹理编号，以便在最后进行恢复    
		GLint lastTextureID = last_texture_ID;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTextureID);
		glBindTexture(GL_TEXTURE_2D, texture_ID);
		/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); */

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		/* glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels); */

		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, 0x80E0, GL_UNSIGNED_BYTE, pixels);

		glBindTexture(GL_TEXTURE_2D, lastTextureID);  //恢复之前的纹理绑定    
		free(pixels);
		_tex = texture_ID;
		return 1;
	}
};