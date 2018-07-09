
#pragma once

class CDrawMethod
{
	HWND m_dlgHWND; //the handle of dialog window
	CDC *m_pDC;     //the device context of dialog
	CDC m_dcMem;    //the memory dc, to contain all the drawing than flush onto screen
	CBitmap m_bmpMem;  //bitmap bind to memory dc--m_dcMem 
	Gdiplus::Graphics* m_pGraph; //point to the object of Gdiplus
	
private:
	void pickPoints(Gdiplus::Point *srcPoints, Gdiplus::Point *desPoints, int i1, int i2, int i3, int i4);
public:
	enum EShadowType{
		NONE = 1,
		SIMPLE = 4,
		COMPLEX = 9
	};

	CDrawMethod(HWND hwnd, CDC *pDC);
	~CDrawMethod();
	//call this func to init the member before any draw method
	int BeginDraw();
	//after all operate has been executed(drawn on memory DC), call this to draw on screen
	void DrawOnScreen();
	//call this func after all operate has done, free or release the resources, in case of memory leak
	int EndDraw();

	//putout the text follow the params onto the dc
	TEXTMETRIC PutOutTextA(char* lpsz, int X, int Y, unsigned long lColor, int nFontHeight, int nFontWidth = FW_NORMAL, EShadowType eShadow = NONE, unsigned long lShadowColor = 0, LPCWSTR lpFont = TEXT("Î¢ÈíÑÅºÚ"));
	TEXTMETRIC PutOutTextW(char* lpsz, int X, int Y, unsigned long lColor, int nFontHeight, int nFontWidth = FW_NORMAL, EShadowType eShadow = NONE, unsigned long lShadowColor = 0, LPCWSTR lpFont = TEXT("Î¢ÈíÑÅºÚ"));
	
	//draw rectangle frame
	void DrawRectFrame(CRect rect,int width, unsigned long lColor,byte alpha=255);
	//draw shadow of rectangle 
	//@param coeff: control the color gradiant rate
	void DrawRectShadow(CRect rect, unsigned long lColor, byte alpha, int width, float coeff=0.8);
	//fill inside of  rectangle 
	void FillRect(CRect rect, unsigned long lColor, byte alpha);
	
	void DrawEllipseFrame(CRect rect, int width, unsigned long lColor, byte alpha=255);
	void DrawEllipseShadow(CRect rect, unsigned long lColor, byte alpha, int width, float coeff=0.8);
	void FillEllipse(CRect rect, unsigned long lColor, byte alpha);
	
	void DrawPolygonFrame(Gdiplus::Point* arrPoints, int ctn, int width, unsigned long lColor, byte alpha=255);
	void FillPolygon(Gdiplus::Point* arrPoints, int ctn, unsigned long lColor, byte alpha);
	//draw round corner rectangle
	void DrawRoundRect(CRect rect, float arcSize, float lineWidth, unsigned long lColor, byte alpha, bool fillPath = false, unsigned long lColorFill = 0, byte alphaFill = 0);
	void DrawRoundRect(float x, float y, float Width, float Height, float arcSize, float lineWidth, unsigned long lColor, byte alpha, bool fillPath = false, unsigned long lColorFill = 0, byte alphaFill = 0);
	
	//method to draw cubic of area located by points array
	void FillCubicSurface(Gdiplus::Point* arrPoints, int ctn, unsigned long lColor, byte alpha = 155,unsigned long frameColor = 0);

	//draw image on path by method 1 to location by rect
	bool DrawImage(char* pStr, CRect rect);
	//draw png picture at path input to location by rect
	void DrawPNG(WCHAR* strPath, CRect rect);

	//save picture in memory DC to path as jpg file
	int SaveMemDCAsJPG(char* strPath);
	int SaveMemDCAsBMP(char* strPath);
	
	//return the memory bitmap member 
	CBitmap *GetMemBitmap();
	//return the memory DC member
	CDC *GetMemDC();
public:
	CRect m_rtClient;
};

