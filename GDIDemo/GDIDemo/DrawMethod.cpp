#include "stdafx.h"
#include "DrawMethod.h"
#include <math.h>

CDrawMethod::CDrawMethod(HWND hwnd, CDC *pDC) : m_dlgHWND(hwnd),m_pDC(pDC){

}

CDrawMethod::~CDrawMethod()
{

}

int CDrawMethod::BeginDraw(){
	GetClientRect(m_dlgHWND, &m_rtClient);
	m_dcMem.CreateCompatibleDC(m_pDC);
	m_bmpMem.CreateCompatibleBitmap(m_pDC, m_rtClient.Width(), m_rtClient.Height());
	m_dcMem.SelectObject(&m_bmpMem);
	m_pGraph = new Gdiplus::Graphics(m_dcMem.m_hDC);
	m_pGraph->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);//抗锯齿   
	return 0;
}

void CDrawMethod::DrawOnScreen(){
	m_pDC->BitBlt(0, 0, m_rtClient.Width(), m_rtClient.Height(), &m_dcMem, 0, 0, SRCCOPY);//将内存DC上的图象拷贝到前台
}

int CDrawMethod::EndDraw(){
	m_pGraph->ReleaseHDC(m_pDC->GetSafeHdc());
	m_bmpMem.DeleteObject();
	m_dcMem.DeleteDC();
	delete m_pGraph;
	return 0;
}

TEXTMETRIC CDrawMethod::PutOutTextA(char* lpsz, int X, int Y, unsigned long lColor, int nFontHeight, int nFontWidth, EShadowType eShadow, unsigned long lShadowColor, LPCWSTR lpFont){
	HFONT hFont,hOldFont;	//定义两种字体句柄
	TEXTMETRIC tm;			//定义一个TEXTMETRIC结构，用以记录字体信息

	hFont = CreateFont		//创建自定义字体
		(
		nFontHeight,				//字体的高度
		0,				//由系统根据高宽比选取字体最佳宽度值
		0,				//文本的倾斜度为0，表示水平
		0,				//字体的倾斜度为0
		nFontWidth,		//字体的粗度，FW_HEAVY为最粗
		0,				//非斜体字
		0,				//无下划线
		0,				//无删除线
		GB2312_CHARSET,	//表示所用的字符集为ANSI_CHARSET
		OUT_DEFAULT_PRECIS,	//输出精度为缺省精度
		CLIP_DEFAULT_PRECIS,	//剪裁精度为缺省精度
		DEFAULT_QUALITY,		//输出质量为缺省值
		DEFAULT_PITCH | FF_DONTCARE,//字间距和字体系列使用缺省值
		lpFont				//字体名称
		);
	SetTextColor(m_dcMem, lShadowColor);	//设置文本颜色
	SetBkMode(m_dcMem, TRANSPARENT);
	hOldFont = (HFONT)SelectObject(m_dcMem, hFont);		//将自定义字体选入设备环境
	GetTextMetrics(m_dcMem, &tm);		//获取字体的信息，并写入tm结构中

	switch (eShadow){
	case NONE:
		break;
	case SIMPLE:
		TextOutA(m_dcMem, X - 1, Y, (LPCSTR)lpsz, strlen(lpsz));
		TextOutA(m_dcMem, X + 1, Y, (LPCSTR)lpsz, strlen(lpsz));
		TextOutA(m_dcMem, X, Y - 1, (LPCSTR)lpsz, strlen(lpsz));
		TextOutA(m_dcMem, X, Y + 1, (LPCSTR)lpsz, strlen(lpsz));
		break;
	case COMPLEX:
		TextOutA(m_dcMem, X - 1, Y - 1, (LPCSTR)lpsz, strlen(lpsz));
		TextOutA(m_dcMem, X - 1, Y, (LPCSTR)lpsz, strlen(lpsz));
		TextOutA(m_dcMem, X - 1, Y + 1, (LPCSTR)lpsz, strlen(lpsz));
		TextOutA(m_dcMem, X, Y - 1, (LPCSTR)lpsz, strlen(lpsz));
		TextOutA(m_dcMem, X, Y + 1, (LPCSTR)lpsz, strlen(lpsz));
		TextOutA(m_dcMem, X + 1, Y - 1, (LPCSTR)lpsz, strlen(lpsz));
		TextOutA(m_dcMem, X + 1, Y, (LPCSTR)lpsz, strlen(lpsz));
		TextOutA(m_dcMem, X + 1, Y + 1, (LPCSTR)lpsz, strlen(lpsz));
		break;
	}


	SetTextColor(m_dcMem, lColor);	//设置文本颜色
	GetTextMetrics(m_dcMem, &tm);		//获取字体的信息，并写入tm结构中

	TextOutA(m_dcMem, X, Y, (LPCSTR)lpsz, strlen(lpsz));	//使用当前字体输出文本
	Y = Y + tm.tmHeight + tm.tmExternalLeading;//计算换行时下一行文本的输出坐标
	
	(HFONT)SelectObject(m_dcMem, hOldFont);
	DeleteObject(hFont);		//删除自定义字体句柄
	DeleteObject(hOldFont);
	return tm;
}

TEXTMETRIC CDrawMethod::PutOutTextW(char* lpsz, int X, int Y, unsigned long lColor, int nFontHeight, int nFontWidth, EShadowType eShadow, unsigned long lShadowColor, LPCWSTR lpFont){
	HFONT hFont,hOldFont;// , hShadow;	//定义两种字体句柄
	TEXTMETRIC tm;			//定义一个TEXTMETRIC结构，用以记录字体信息
	hFont = CreateFont		//创建自定义字体
		(
		nFontHeight,				//字体的高度
		0,				//由系统根据高宽比选取字体最佳宽度值
		0,				//文本的倾斜度为0，表示水平
		0,				//字体的倾斜度为0
		nFontWidth,		//字体的粗度，FW_HEAVY为最粗
		0,				//非斜体字
		0,				//无下划线
		0,				//无删除线
		GB2312_CHARSET,	//表示所用的字符集为ANSI_CHARSET
		OUT_DEFAULT_PRECIS,	//输出精度为缺省精度
		CLIP_DEFAULT_PRECIS,	//剪裁精度为缺省精度
		DEFAULT_QUALITY,		//输出质量为缺省值
		DEFAULT_PITCH | FF_DONTCARE,//字间距和字体系列使用缺省值
		lpFont				//字体名称
		);

	SetBkMode(m_dcMem, TRANSPARENT);
	SetTextColor(m_dcMem, lShadowColor);
	hOldFont =(HFONT) SelectObject(m_dcMem, hFont);
	GetTextMetrics(m_dcMem, &tm);

	switch (eShadow){
	case NONE:
		break;
	case SIMPLE:		
		TextOutW(m_dcMem, X - 1, Y, (LPCWSTR)lpsz, wcslen((const wchar_t*)lpsz));	
		TextOutW(m_dcMem, X + 1, Y, (LPCWSTR)lpsz, wcslen((const wchar_t*)lpsz));	
		TextOutW(m_dcMem, X , Y - 1, (LPCWSTR)lpsz, wcslen((const wchar_t*)lpsz));	
		TextOutW(m_dcMem, X, Y + 1, (LPCWSTR)lpsz, wcslen((const wchar_t*)lpsz));	
		break;
	case COMPLEX:
		TextOutW(m_dcMem, X - 1, Y - 1, (LPCWSTR)lpsz, wcslen((const wchar_t*)lpsz));
		TextOutW(m_dcMem, X - 1, Y, (LPCWSTR)lpsz, wcslen((const wchar_t*)lpsz));
		TextOutW(m_dcMem, X - 1, Y + 1, (LPCWSTR)lpsz, wcslen((const wchar_t*)lpsz));
		TextOutW(m_dcMem, X , Y - 1, (LPCWSTR)lpsz, wcslen((const wchar_t*)lpsz));
		TextOutW(m_dcMem, X, Y + 1, (LPCWSTR)lpsz, wcslen((const wchar_t*)lpsz));
		TextOutW(m_dcMem, X + 1, Y - 1, (LPCWSTR)lpsz, wcslen((const wchar_t*)lpsz));
		TextOutW(m_dcMem, X + 1, Y, (LPCWSTR)lpsz, wcslen((const wchar_t*)lpsz));
		TextOutW(m_dcMem, X + 1, Y + 1, (LPCWSTR)lpsz, wcslen((const wchar_t*)lpsz));
		break;
	}

	SetTextColor(m_dcMem, lColor);	//设置文本颜色
	GetTextMetrics(m_dcMem, &tm);		//获取字体的信息，并写入tm结构中

	TextOutW(m_dcMem, X, Y, (LPCWSTR)lpsz, wcslen((const wchar_t*)lpsz));	//使用当前字体输出文本
	Y = Y + tm.tmHeight + tm.tmExternalLeading;//计算换行时下一行文本的输出坐标

	SelectObject(m_dcMem, hOldFont);
	DeleteObject(hFont);		//删除自定义字体句柄
	DeleteObject(hOldFont);
	return tm;
}

void CDrawMethod::DrawRectFrame(CRect rect, int width, unsigned long lColor,byte alpha){
	//Method with GDI
	/*
	CPen pen(PS_SOLID, width, lColor);
	CPen* oldPen = m_dcMem.SelectObject(&pen);
	m_dcMem.SelectStockObject(NULL_BRUSH); //select null brush to fill the rect
	m_dcMem.Rectangle(&rect);
	m_dcMem.SelectObject(oldPen);
	DeleteObject(pen);
	*/
	//Method with GDI+, it support alpha channel
	Gdiplus::Color color(alpha,GetRValue(lColor),GetGValue(lColor),GetBValue(lColor));
	Gdiplus::Pen pen(color,width);
	m_pGraph->DrawRectangle(&pen,rect.left,rect.top,rect.Width(),rect.Height());
}

void  CDrawMethod::DrawRectShadow(CRect rect, unsigned long lColor, byte alpha, int width, float coeff){
	int i;
	for(i=0;i<width;i++){
		rect.left -= 1;
		rect.right += 1;
		rect.top -= 1;
		rect.bottom +=1;
		alpha = alpha * coeff;
		DrawRectFrame(rect,1,lColor,alpha);
	}
}

void CDrawMethod::FillRect(CRect rect, unsigned long lColor, byte alpha){
	Gdiplus::SolidBrush brushBk(Gdiplus::Color(alpha, GetRValue(lColor), GetGValue(lColor), GetBValue(lColor)));
	m_pGraph->FillRectangle(&brushBk, Gdiplus::Rect(rect.left, rect.top, rect.Width(),rect.Height()));
}

void CDrawMethod::DrawEllipseFrame(CRect rect, int width, unsigned long lColor, byte alpha){
	Gdiplus::Pen pen(Gdiplus::Color(alpha, GetRValue(lColor), GetGValue(lColor), GetBValue(lColor)), width);
	m_pGraph->DrawEllipse(&pen, rect.left,rect.top,rect.Width(),rect.Height());
}

void CDrawMethod::DrawEllipseShadow(CRect rect, unsigned long lColor, byte alpha, int width, float coeff){
	int i;
	for(i=0;i<width;i++){
		rect.left -= 1;
		rect.right += 1;
		rect.top -= 1;
		rect.bottom +=1;
		alpha = alpha * coeff;
		DrawEllipseFrame(rect,1,lColor,alpha);
	}
}

void CDrawMethod::FillEllipse(CRect rect, unsigned long lColor, byte alpha){
	Gdiplus::SolidBrush brushBk(Gdiplus::Color(alpha, GetRValue(lColor), GetGValue(lColor), GetBValue(lColor)));
	m_pGraph->FillEllipse(&brushBk, Gdiplus::Rect(rect.left, rect.top, rect.Width(), rect.Height()));
}

void CDrawMethod::DrawPolygonFrame(Gdiplus::Point* arrPoints, int ctn, int width, unsigned long lColor, byte alpha){
	Gdiplus::Pen pen(Gdiplus::Color(alpha, GetRValue(lColor), GetGValue(lColor), GetBValue(lColor)), width);
	m_pGraph->DrawPolygon(&pen, arrPoints, ctn);
}

void CDrawMethod::FillPolygon(Gdiplus::Point* arrPoints,int ctn, unsigned long lColor, byte alpha){
	Gdiplus::SolidBrush brushBk(Gdiplus::Color(alpha, GetRValue(lColor), GetGValue(lColor), GetBValue(lColor)));
	m_pGraph->FillPolygon(&brushBk, arrPoints,ctn);
}

void CDrawMethod::DrawRoundRect(CRect rect, float arcSize, float lineWidth, unsigned long lColor, byte alpha,  bool fillPath, unsigned long lColorFill, byte alphaFill)
{
	DrawRoundRect(rect.left, rect.top, rect.Width(), rect.Height(), arcSize,lineWidth, lColor, alpha, fillPath, lColorFill, alphaFill);
}

void CDrawMethod::DrawRoundRect(float x, float y, float Width, float Height, float arcSize, float lineWidth, unsigned long lColor, byte alpha, bool fillPath, unsigned long lColorFill, byte alphaFill)
{
	// 小矩形的半宽（hew）和半高（heh）  
	float hew = Width / arcSize / 2;
	float heh = Height / arcSize / 2;

	// 圆角修正  
	if (fabs(hew - heh)>10)
	{
		hew = heh = hew>heh ? heh : hew;
	}

	// 绘图路径  
	Gdiplus::GraphicsPath roundRectPath;

	// 保存绘图路径  
	roundRectPath.AddLine(x + hew, y, x + Width - hew, y);  // 顶部横线  
	roundRectPath.AddArc(x + Width - 2 * hew, y, 2 * hew, 2 * heh, 270, 90); // 右上圆角  

	roundRectPath.AddLine(x + Width, y + heh, x + Width, y + Height - heh);  // 右侧竖线  
	roundRectPath.AddArc(x + Width - 2 * hew, y + Height - 2 * heh, 2 * hew, 2 * heh, 0, 90); // 右下圆角  

	roundRectPath.AddLine(x + Width - hew, y + Height, x + hew, y + Height);  // 底部横线  
	roundRectPath.AddArc(x, y + Height - 2 * heh, 2 * hew, 2 * heh, 90, 90); // 左下圆角  

	roundRectPath.AddLine(x, y + Height - heh, x, y + heh);  // 左侧竖线  
	roundRectPath.AddArc(x, y, 2 * hew, 2 * heh, 180, 90); // 左上圆角  

	//创建画笔  
	Gdiplus::Pen pen(Gdiplus::Color(alpha, GetRValue(lColor), GetGValue(lColor), GetBValue(lColor)), lineWidth);

	// 绘制矩形  
	m_pGraph->DrawPath(&pen, &roundRectPath);

	// 是否填充  
	if (!fillPath)
	{
		return;
	}

	// 创建画刷  
	Gdiplus::SolidBrush brush(Gdiplus::Color(alphaFill, GetRValue(lColorFill), GetGValue(lColorFill), GetBValue(lColorFill)));

	// 填充  
	m_pGraph->FillPath(&brush, &roundRectPath);
}

void CDrawMethod::pickPoints(Gdiplus::Point *srcPoints, Gdiplus::Point *desPoints, int i1, int i2, int i3, int i4){
	desPoints[0] = srcPoints[i1];
	desPoints[1] = srcPoints[i2];
	desPoints[2] = srcPoints[i3];
	desPoints[3] = srcPoints[i4];
}

void CDrawMethod::FillCubicSurface(Gdiplus::Point* arrPoints, int ctn, unsigned long lColor, byte alpha, unsigned long frameColor){
	if (ctn < 8){
		return;
	}
	Gdiplus::Point ptList[4];
	bool isLeft = arrPoints[3].X>arrPoints[0].X;
	pickPoints(arrPoints, ptList, 0, 1, 2, 3);
	FillPolygon(ptList, 4, lColor, 100);
	DrawPolygonFrame(ptList, 4, 2, frameColor, 200);

	pickPoints(arrPoints, ptList, 3, 2, 6, 7);
	FillPolygon(ptList, 4, lColor, alpha);
	DrawPolygonFrame(ptList, 4, 2, frameColor, 200);

	pickPoints(arrPoints, ptList, 2, 1, 5, 6);
	FillPolygon(ptList, 4, lColor, alpha);
	DrawPolygonFrame(ptList, 4, 2, frameColor, 200);

	pickPoints(arrPoints, ptList, 3, 0, 4, 7);
	FillPolygon(ptList, 4, lColor, alpha);
	DrawPolygonFrame(ptList, 4, 2, frameColor, 200);

	pickPoints(arrPoints, ptList, 0, 1, 5, 4);
	FillPolygon(ptList, 4, lColor, alpha);
	DrawPolygonFrame(ptList, 4, 2, frameColor, 200);

	pickPoints(arrPoints, ptList, 4, 5, 6, 7);
	FillPolygon(ptList, 4, lColor, alpha);
	DrawPolygonFrame(ptList, 4, 2, frameColor, 200);
}

bool  CDrawMethod::DrawImage(char* pStr, CRect rect){
	CImage mImage;
	CString strCsPath(pStr);
	if (mImage.Load((LPCTSTR)strCsPath) == S_OK)
	{
		mImage.Draw(m_dcMem, rect);
		mImage.Destroy();
		return true;
	}
	mImage.Destroy();
	return false;
}

void CDrawMethod::DrawPNG(WCHAR* strPath, CRect rect){
	Gdiplus::Bitmap *bmp = Gdiplus::Bitmap::FromFile(strPath);
	m_pGraph->DrawImage(bmp, rect.left,rect.top,rect.Width(),rect.Height());
	DeleteObject(&bmp);
	delete bmp;
}

CBitmap * CDrawMethod::GetMemBitmap(){
	return &m_bmpMem;
}

CDC * CDrawMethod::GetMemDC(){
	return &m_dcMem;
}

unsigned long BMP2JPEG(char* pUnZipData, unsigned long ulUnZipDataLen, char** pBuffer)
{
	unsigned long ulBufferLen = 0;
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, ulUnZipDataLen);
	void* pData = GlobalLock(hGlobal);
	memcpy(pData, pUnZipData, ulUnZipDataLen);
	GlobalUnlock(hGlobal);
	IStream* pStream = NULL;
	if (CreateStreamOnHGlobal(hGlobal, TRUE, &pStream) == S_OK)
	{
		CImage image;
		if (SUCCEEDED(image.Load(pStream)))
		{
			IStream* pOutStream = NULL;
			if (CreateStreamOnHGlobal(NULL, TRUE, &pOutStream) == S_OK)
			{
				image.Save(pOutStream, Gdiplus::ImageFormatJPEG);
				HGLOBAL hOutGlobal = NULL;
				GetHGlobalFromStream(pOutStream, &hOutGlobal);
				LPBYTE pBits = (LPBYTE)GlobalLock(hOutGlobal);
				ulBufferLen = (DWORD)GlobalSize(pBits);
				*pBuffer = new char[ulBufferLen];
				memcpy(*pBuffer, pBits, ulBufferLen);

				GlobalUnlock(hOutGlobal);
				pOutStream->Release();
			}
		}
		pStream->Release();
	}
	GlobalFree(hGlobal);
	return ulBufferLen;
}

int  CDrawMethod::SaveMemDCAsJPG(char* strPath){
	CImage img;
	BITMAP bmp;
	m_bmpMem.GetBitmap(&bmp);//获得位图信息
	img.Create(bmp.bmWidth, bmp.bmHeight, 32);
	::BitBlt(img.GetDC(), 0, 0, bmp.bmWidth, bmp.bmHeight, m_dcMem.GetSafeHdc(), 0, 0, SRCCOPY);
	CString strCSPath(strPath);
	img.Save((LPCTSTR)strCSPath);
	img.ReleaseDC();
	img.Destroy();
	return 0;
}

int  CDrawMethod::SaveMemDCAsBMP(char* strPath){
	//以下代码保存memDC中的位图到文件
	BITMAP bmp;
	m_bmpMem.GetBitmap(&bmp);//获得位图信息
	FILE *fp = fopen(strPath, "wb");
	BITMAPINFOHEADER bih = {0};//位图信息头
	bih.biBitCount = bmp.bmBitsPixel;//每个像素字节大小
	bih.biCompression = BI_RGB;
	bih.biHeight = bmp.bmHeight;//高度
	bih.biPlanes = 1;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biSizeImage = bmp.bmWidthBytes * bmp.bmHeight;//图像数据大小
	bih.biWidth = bmp.bmWidth;//宽度
	BITMAPFILEHEADER bfh = {0};//位图文件头
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);//到位图数据的偏移量
	bfh.bfSize = bfh.bfOffBits + bmp.bmWidthBytes * bmp.bmHeight;//文件总的大小
	bfh.bfType = (WORD)0x4d42;
	fwrite(&bfh, 1, sizeof(BITMAPFILEHEADER), fp);//写入位图文件头
	fwrite(&bih, 1, sizeof(BITMAPINFOHEADER), fp);//写入位图信息头
	byte * p = new byte[bmp.bmWidthBytes * bmp.bmHeight];//申请内存保存位图数据
	GetDIBits(m_dcMem.m_hDC, (HBITMAP)m_bmpMem.m_hObject, 0,bmp.bmHeight, p,
				(LPBITMAPINFO)&bih, DIB_RGB_COLORS);//获取位图数据
	fwrite(p, 1, bmp.bmWidthBytes * bmp.bmHeight, fp);//写入位图数据
	delete [] p;
	fclose(fp);
	return 0;
}