/*
 * ShaderHelper.cpp
 *
 *  Created on: 28.07.2008
 *      Author: ralph
 */

#include "ShaderHelper.h"
#include "../dataset/DatasetHelper.h"
#include "../dataset/surface.h"

ShaderHelper::ShaderHelper(DatasetHelper* dh)
:   m_dh(dh)
{
	m_dh->printDebug(_T("initializing texture shader"), 1);
	m_textureShader = new Shader(wxT("anatomy"));
	m_textureShader->bind();
	if (m_dh->GLError()) m_dh->printGLError(wxT("setup anatomy shader"));

	m_dh->printDebug(_T("initializing mesh shader"), 1);
	m_meshShader = new Shader(wxT("mesh"));
	m_meshShader->bind();
	if (m_dh->GLError()) m_dh->printGLError(wxT("setup mesh shader"));

	m_dh->printDebug(_T("initializing fiber shader"), 1);
	m_fiberShader = new Shader(wxT("fibers"));
	m_fiberShader->bind();
	if (m_dh->GLError()) m_dh->printGLError(wxT("setup fiber shader"));

	m_dh->printDebug(_T("initializing fake tube shader"), 1);
	m_fakeTubeShader = new Shader(wxT("fake-tubes"));
	m_fakeTubeShader->bind();
	if (m_dh->GLError()) m_dh->printGLError(wxT("setup fake tube shader"));

	m_dh->printDebug(_T("initializing spline surface shader"), 1);
	m_splineSurfShader = new Shader(wxT("splineSurf"));
	m_splineSurfShader->bind();
	if (m_dh->GLError()) m_dh->printGLError(wxT("setup spline surface shader"));

	m_dh->printDebug(_T("initializing vector shader"), 1);
	m_vectorShader = new Shader(wxT("vectors"));
	m_vectorShader->bind();
	if (m_dh->GLError()) m_dh->printGLError(wxT("setup vectors shader"));

	m_dh->printDebug(_T("initializing legend shader"), 1);
	m_legendShader = new Shader(wxT("legend"));
	m_legendShader->bind();
	if (m_dh->GLError()) m_dh->printGLError(wxT("setup legend shader"));

}

ShaderHelper::~ShaderHelper() {
	m_dh->printDebug(_T("execute shader helper destructor\n"), 0);
	delete m_textureShader;
	delete m_meshShader;
	delete m_fiberShader;
	delete m_splineSurfShader;
	delete m_fakeTubeShader;
	m_dh->printDebug(_T("shader helper destructor done\n"), 0);
}


void ShaderHelper::setTextureShaderVars()
{
	GLint* tex = new GLint[10];
	GLint* show = new GLint[10];
	GLfloat* threshold = new GLfloat[10];
	GLfloat* alpha = new GLfloat[10];
	GLint* type = new GLint[10];
	int c = 0;
	for (int i = 0 ; i < m_dh->mainFrame->m_listCtrl->GetItemCount() ; ++i)
	{
		DatasetInfo* info = (DatasetInfo*)m_dh->mainFrame->m_listCtrl->GetItemData(i);
		if(info->getType() < Mesh_)
		{
			tex[c] = c;
			show[c] = info->getShow();
			threshold[c] = info->getThreshold();
			alpha[c] = info->getAlpha();
			type[c] = info->getType();
			++c;
		}
		if ( c == 10 ) break;
	}

	m_textureShader->setUniArrayInt("texes", tex, c);
	m_textureShader->setUniArrayInt("show", show, c);
	m_textureShader->setUniArrayInt("type", type, c);
	m_textureShader->setUniArrayFloat("threshold", threshold, c);
	m_textureShader->setUniArrayFloat("alpha", alpha, c);
}

void ShaderHelper::setMeshShaderVars()
{
	m_meshShader->setUniInt("dimX", m_dh->columns);
	m_meshShader->setUniInt("dimY", m_dh->rows);
	m_meshShader->setUniInt("dimZ", m_dh->frames);
	m_meshShader->setUniFloat("voxX", m_dh->xVoxel);
	m_meshShader->setUniFloat("voxY", m_dh->yVoxel);
	m_meshShader->setUniFloat("voxZ", m_dh->zVoxel);
	m_meshShader->setUniInt("blendTex", m_dh->blendTexOnMesh);
	m_meshShader->setUniFloat("cutX", m_dh->xSlize + 0.5f);
	m_meshShader->setUniFloat("cutY", m_dh->ySlize + 0.5f);
	m_meshShader->setUniFloat("cutZ", m_dh->zSlize + 0.5f);
	m_meshShader->setUniInt("sector", m_dh->quadrant);

	GLint* tex = new GLint[10];
	GLint* show = new GLint[10];
	float* threshold = new float[10];
	GLfloat* alpha = new GLfloat[10];
	GLint* type = new GLint[10];
	int c = 0;
	GLuint cutTex = 0;
	for (int i = 0 ; i < m_dh->mainFrame->m_listCtrl->GetItemCount() ; ++i)
	{
		DatasetInfo* info = (DatasetInfo*)m_dh->mainFrame->m_listCtrl->GetItemData(i);
		if(info->getType() < Mesh_) {
			tex[c] = c;
			show[c] = info->getShow();
			threshold[c] = info->getThreshold();
			alpha[c] = info->getAlpha();
			type[c] = info->getType();
			++c;
		}
		else if (info->getType() == Surface_)
		{
			Surface* s = (Surface*)m_dh->mainFrame->m_listCtrl->GetItemData(i);
			cutTex = s->getCutTex();
		}
		if ( c == 9 ) break;
	}

	if (cutTex != 0 && m_dh->surface_loaded)
	{
		glActiveTexture(GL_TEXTURE0 + c);
		glBindTexture(GL_TEXTURE_2D, cutTex);
		tex[c] = c;
		show[c] = false;
		threshold[c] = 0;
		type[c] = 5;
		m_meshShader->setUniInt("cutTex", c);
		++c;
	}

	m_meshShader->setUniArrayInt("texes", tex, c);
	m_meshShader->setUniArrayInt("show", show, c);
	m_meshShader->setUniArrayInt("type", type, c);
	m_meshShader->setUniArrayFloat("threshold", threshold, c);
	m_meshShader->setUniArrayFloat("alpha", alpha, c);
}

void ShaderHelper::setFiberShaderVars()
{
	m_fiberShader->setUniInt("dimX", m_dh->columns);
	m_fiberShader->setUniInt("dimY", m_dh->rows);
	m_fiberShader->setUniInt("dimZ", m_dh->frames);
	m_fiberShader->setUniFloat("voxX", m_dh->xVoxel);
    m_fiberShader->setUniFloat("voxY", m_dh->yVoxel);
    m_fiberShader->setUniFloat("voxZ", m_dh->zVoxel);

	int tex = 0;
	int show = 0;
	float threshold = 0;
	int type = 0;

	int c = 0;
	for (int i = 0 ; i < m_dh->mainFrame->m_listCtrl->GetItemCount() ; ++i)
	{
		DatasetInfo* info = (DatasetInfo*)m_dh->mainFrame->m_listCtrl->GetItemData(i);
		if(info->getType() < Mesh_)
		{
			if( (info->getType() == Overlay) && info->getShow() ) {
				tex = c;
				show = info->getShow();
				threshold = info->getThreshold();
				type = info->getType();
				break;
			}
			++c;
		}
		if ( c == 10 ) break;
	}

	m_fiberShader->setUniInt("tex", tex);
	m_fiberShader->setUniInt("type", type);
	m_fiberShader->setUniFloat("threshold", threshold);
}

void ShaderHelper::setSplineSurfaceShaderVars()
{
	m_splineSurfShader->setUniInt("dimX", m_dh->columns);
	m_splineSurfShader->setUniInt("dimY", m_dh->rows);
	m_splineSurfShader->setUniInt("dimZ", m_dh->frames);
	m_splineSurfShader->setUniFloat("voxX", m_dh->xVoxel);
	m_splineSurfShader->setUniFloat("voxY", m_dh->yVoxel);
	m_splineSurfShader->setUniFloat("voxZ", m_dh->zVoxel);

	GLint* tex = new GLint[10];
	GLint* show = new GLint[10];
	float* threshold = new float[10];
	GLfloat* alpha = new GLfloat[10];
	GLint* type = new GLint[10];
	int c = 0;
	for (int i = 0 ; i < m_dh->mainFrame->m_listCtrl->GetItemCount() ; ++i)
	{
		DatasetInfo* info = (DatasetInfo*)m_dh->mainFrame->m_listCtrl->GetItemData(i);
		if(info->getType() < Mesh_) {
			tex[c] = c;
			show[c] = info->getShow();
			threshold[c] = info->getThreshold();
			alpha[c] = info->getAlpha();
			type[c] = info->getType();
			++c;
		}
		if ( c == 10 ) break;
	}

	m_splineSurfShader->setUniArrayInt("texes", tex, c);
	m_splineSurfShader->setUniArrayInt("show", show, c);
	m_splineSurfShader->setUniArrayInt("type", type, c);
	m_splineSurfShader->setUniArrayFloat("threshold", threshold, c);
	m_splineSurfShader->setUniArrayFloat("alpha", alpha, c);
}

