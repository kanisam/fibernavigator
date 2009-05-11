#ifndef THESCENE_H_
#define THESCENE_H_

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../gui/ArcBall.h"
#include "../dataset/datasetInfo.h"
#include "../dataset/DatasetHelper.h"

//#include "lic/FgeImageSpaceLIC.h"

#include "wx/glcanvas.h"

enum {
	axial = 1,
	coronal,
	sagittal,
	mainView
};

class DatasetHelper;
class FgeImageSpaceLIC;

class TheScene {

public:
	TheScene(DatasetHelper*);
	~TheScene();

	void initGL(int);

	void bindTextures();

	void setMainGLContext(wxGLContext* context) {m_mainGLContext = context;};
	wxGLContext* getMainGLContext() {return m_mainGLContext;};

	void renderScene();
	void renderNavView(int);

	void drawSphere(float, float, float, float);

private:
    DatasetHelper* m_dh;
	wxGLContext* m_mainGLContext;

	void lightsOn();
	void lightsOff();

	void renderSlizes();
	void renderMesh();
	void renderFibers();
	void renderFakeTubes();
	void renderSplineSurface();

	void drawVectors();
	void drawColorMapLegend();

	void drawSelectionBoxes();
	void drawPoints();
};

#endif /*THESCENE_H_*/
