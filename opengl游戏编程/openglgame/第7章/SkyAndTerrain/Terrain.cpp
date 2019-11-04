//========================================================
/**
*  @file      Terrain.cpp
*
*  项目描述： 构造天空和地面
*  文件描述:  地形类 
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2006-12-06	
*  修改日期： 2006-12-10
*
*/     
//========================================================

#include "Terrain.h"
#include "CBMPLoader.h"

/** 当前CTerrain指针 */
CTerrain* CTerrain::m_pTerrain = NULL;


/** 多重纹理函数指针 */
PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2fARB = NULL;
PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB = NULL;

/** 构造函数 */
CTerrain::CTerrain():m_bDetail(true),m_DetailScale(128)
{
	/** 设置地形大小 */
	setSize(MAP_WIDTH,CELL_WIDTH);
	
	/** 为地形高程分配内存,并初始化 */
	m_pHeightMap = new BYTE[m_nWidth * m_nWidth];
	for(int i=0; i<m_nWidth * m_nWidth; i++)
		m_pHeightMap[i] = 0;

	m_pTerrain = this;
		
}

/** 析构函数 */
CTerrain::~CTerrain()
{
	/** 删除内存和指针 */
	if(m_pHeightMap)
	{
		delete[] m_pHeightMap;
	    m_pHeightMap = 0;
	}
	
	/** 删除纹理对象及其占用内存 */	
	for(int i=0; i<2; i++)
	{
		m_texture[i].FreeImage();
	    glDeleteTextures(1,&m_texture[i].ID);
	}

	
}

/** 初始化雾效 */
void CTerrain::initFog()
{
	float fogColor[4] = { 0.8f,0.8f,0.8f,1.0f };
	
	glEnable(GL_FOG);

	glFogi(GL_FOG_MODE,GL_EXP);             /** 设置雾效的模式 */
	glFogfv(GL_FOG_COLOR,fogColor);         /** 指定雾的颜色 */ 
    glFogf(GL_FOG_DENSITY,0.001f);          /** 设置雾的浓度 */
	glFogf(GL_FOG_START,1.0);               /** 设置线性雾的开始位置 */
	glFogf(GL_FOG_END,1000.0);              /** 设置线性雾的结束位置 */
	glHint(GL_FOG_HINT,GL_DONT_CARE);       /** 规定雾化效果的质量 */ 

}


/** 初始化地形 */
bool CTerrain::init()
{
	glActiveTextureARB		= (PFNGLACTIVETEXTUREARBPROC)		wglGetProcAddress("glActiveTextureARB");
	glMultiTexCoord2fARB	= (PFNGLMULTITEXCOORD2FARBPROC)		wglGetProcAddress("glMultiTexCoord2fARB");

	if(!glActiveTextureARB || !glMultiTexCoord2fARB)
	{
		//输出错误信息
		MessageBox(NULL, "不支持多重纹理！", "错误", MB_OK);
		exit(0);
		//setDetail(false);
	}

	/** 载入高度文件 */
	loadRawFile("data/terrain.raw");

	/** 载入纹理 */
	loadTexture();

	/** 初始化雾效 */
    initFog();
	
	return true;
}

/** 设置地形大小 */
void CTerrain::setSize(unsigned  int width, unsigned  int cell)
{
	m_nWidth = width;
	m_nCellWidth = cell; 
}

/** 获得高度图中高度值 */
int CTerrain::getHeight(int x,int y)
{
	if(!m_pHeightMap)
		return 0;
	int xx = x % m_nWidth;
	int yy = y % m_nWidth;
    /** 返回当前高度 */
	return m_pHeightMap[(xx + yy * m_nWidth)];
}

/** 获得地面高度 */
float CTerrain::getAveHeight(float x,float z)
{
	float CameraX, CameraZ;

	CameraX = x / m_nCellWidth;
	CameraZ = z / m_nCellWidth;

	/** 计算高程坐标(Col0, Row0)，(Col1, Row1) */
	int col0 = int(CameraX);
	int row0 = int(CameraZ);
	unsigned int col1 = col0 + 1;
	unsigned int row1 = row0 + 1;

	/** 确保单元坐标不超过高程以外 */
	if (col1 > m_nWidth) col1 = 0;
	if (row1 > m_nWidth) row1 = 0;

	/** 获取单元的四个角的高度 */
	float h00 = (float)(m_pHeightMap[col0*m_nCellWidth + row0*m_nCellWidth*m_nWidth]);
	float h01 = (float)(m_pHeightMap[col1*m_nCellWidth + row0*m_nCellWidth*m_nWidth]);
	float h11 = (float)(m_pHeightMap[col1*m_nCellWidth + row1*m_nCellWidth*m_nWidth]);
	float h10 = (float)(m_pHeightMap[col0*m_nCellWidth + row1*m_nCellWidth*m_nWidth]);

	/** 计算机摄像机相对于单元格的位置 */
	float tx = CameraX - int(CameraX);
	float ty = CameraZ - int(CameraZ);

	/** 进行双线性插值得到地面高度 */
	float txty = tx * ty;

	float final_height	= h00 * (1.0f - ty - tx + txty)
						+ h01 * (tx - txty)
						+ h11 * txty
						+ h10 * (ty - txty);
	return final_height;
}

/** 载入高度图 */
bool CTerrain::loadRawFile(const char* fileName)
{
	FILE *pFile = NULL;

	/** 打开文件 */
	pFile = fopen( fileName, "rb" );

	/** 错误检查 */
	if ( pFile == NULL )	
	{
		/** 输出错误信息，并返回false */
		MessageBox(NULL, "打开高度图文件失败！", "错误", MB_OK);
		exit(0);
	}

	/** 读取高度图文件 */
	fread( m_pHeightMap, 1, m_nWidth*m_nWidth, pFile );

	/** 获取错误代码 */
	int result = ferror( pFile );

	/** 检查错误代码 */
	if (result)
	{
		MessageBox(NULL, "无法获取高度数据！", "错误", MB_OK);
		exit(0);
	}
    
	/** 关闭文件，成功返回 */
	fclose(pFile);
	return true;
}

/** 设置纹理坐标 */
void CTerrain::setTexCoord(float x,float z)
{
	
	float u =  (float)x / (float)m_nWidth;
	float v = -(float)z / (float)m_nWidth;
	
	///设置地面纹理的纹理坐标
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, u, v);

	///设置细节纹理的纹理坐标
	glMultiTexCoord2fARB(GL_TEXTURE1_ARB, u, v);
}


/** 载入地面纹理 */
bool CTerrain::loadTexture()
{
	char* fileName[] = {"data/terrain.bmp","data/detail.bmp"};	
	for(int i=0; i < 2; i++)
	{
		if(!m_texture[i].LoadBitmap(fileName[i]) )                    /**< 载入位图文件 */
		{
			MessageBox(NULL,"装载位图文件失败！","错误",MB_OK);       /**< 如果载入失败则弹出对话框 */
			exit(0);
		}
		glGenTextures(1, &m_texture[i].ID);                            /**< 生成一个纹理对象名称 */
		
			
		glBindTexture(GL_TEXTURE_2D, m_texture[i].ID);                 /**< 创建纹理对象 */
		
		/** 控制滤波 */
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		
		/** 创建纹理 */
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, m_texture[i].imageWidth,
						m_texture[i].imageHeight, GL_RGB, GL_UNSIGNED_BYTE,
						m_texture[i].image);
	}
  	
	return true;

}

/** 渲染地形 */
void CTerrain::render()
{
		
	int X = 0, Y = 0;						
	float x, y, z;							
	bool bSwitchSides = false;

	/** 检查高度图是否有效 */
	if(!m_pHeightMap) 
		return;					

	/** 绑定纹理 */
	
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texture[0].ID);
	
	/** 渲染细节纹理 */
	if(m_bDetail)
	{
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glEnable(GL_TEXTURE_2D);

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 2);

		glBindTexture(GL_TEXTURE_2D, m_texture[1].ID);
		
		/** 变换纹理矩阵 */
		glMatrixMode(GL_TEXTURE);
		    glLoadIdentity();
			glScalef((float)m_DetailScale, (float)m_DetailScale, 1);
		glMatrixMode(GL_MODELVIEW);
	}

	/** 绘制三角形带 */
	glBegin( GL_TRIANGLE_STRIP );			

	/** 从行(X)开始循环 */
	for ( X = 0; X <= m_nWidth; X += m_nCellWidth )
	{
		/** 检查该列是否需要从相反顺序绘制 */
		if(bSwitchSides)
		{	
			/** 绘制地形的一列 */
			for ( Y = m_nWidth; Y >= 0; Y -= m_nCellWidth )
			{
				/** 顶点(X, Y, Z) */		
				x = X;							
				y = getHeight( X, Y );	
				z = Y;							

				/** 指定纹理坐标,并发送顶点 */
				//setFogCoord(m_FogDepth, (float)y);
				setTexCoord( (float)x, (float)z );
				glVertex3f(x, y, z);		

				/** 顶点(X + m_nCellWidth, Y, Z) */		
				x = X + m_nCellWidth; 
				y = getHeight( X + m_nCellWidth, Y ); 
				z = Y;

				/** 指定纹理坐标,并发送顶点 */
				//setFogCoord(m_FogDepth, (float)y);
				setTexCoord( (float)x, (float)z );
				glVertex3f(x, y, z);			
			}
		}
		else
		{	
			/** 绘制地形的一列 */
			for ( Y = 0; Y <= m_nWidth; Y += m_nCellWidth )
			{
				/** 顶点(X + m_nCellWidth, Y, Z) */	
				x = X + m_nCellWidth; 
				y = getHeight( X + m_nCellWidth, Y ); 
				z = Y;

				/** 指定纹理坐标,并发送顶点 */
				//setFogCoord(m_FogDepth, (float)y);
				setTexCoord( (float)x, (float)z );
				glVertex3f(x, y, z);

				/** 顶点 (X, Y, Z) */		
				x = X;							
				y = getHeight( X, Y );	
				z = Y;							

				/** 指定纹理坐标,并发送顶点 */
				//setFogCoord(m_FogDepth, (float)y);
				setTexCoord( (float)x, (float)z );
				glVertex3f(x, y, z);		
			}
		}
			
		/** 变换开关 */
		bSwitchSides = !bSwitchSides;
	}

	/** 绘制结束 */
	glEnd();

	///关闭纹理单元1
	glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);

	//关闭纹理单元0
	glActiveTextureARB(GL_TEXTURE0_ARB);		
    glDisable(GL_TEXTURE_2D);

}

