 /*
 *  Copyright (C) 2002-2013  The DOSBox Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#ifndef DOSBOX_VOODOO_VOGL_H
#define DOSBOX_VOODOO_VOGL_H

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_opengl_glext.h>

#undef glActiveTextureARB
#undef glMultiTexCoord4fARB
#undef glMultiTexCoord4fvARB
#undef glCreateShaderObjectARB
#undef glShaderSourceARB
#undef glCompileShaderARB
#undef glCreateProgramObjectARB
#undef glAttachObjectARB
#undef glLinkProgramARB
#undef glUseProgramObjectARB
#undef glUniform1iARB
#undef glUniform1fARB
#undef glUniform2fARB
#undef glUniform3fARB
#undef glUniform4fARB
#undef glGetUniformLocationARB
#undef glDetachObjectARB
#undef glDeleteObjectARB
#undef glGetObjectParameterivARB
#undef glGetInfoLogARB
#undef glBlendFuncSeparateEXT
#undef glGenerateMipmapEXT
#undef glGetAttribLocationARB
#undef glVertexAttrib1fARB

#ifndef GL_TEXTURE0_ARB
#define GL_TEXTURE0_ARB 0x84C0
#endif

typedef void (APIENTRYP PFNGLACTIVETEXTUREARBPROC) (GLenum texture);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4FARBPROC) (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
typedef void (APIENTRYP PFNGLMULTITEXCOORD4FVARBPROC) (GLenum target, const GLfloat *v);

extern PFNGLACTIVETEXTUREARBPROC v_glActiveTextureARB;
extern PFNGLMULTITEXCOORD4FARBPROC v_glMultiTexCoord4fARB;
extern PFNGLMULTITEXCOORD4FVARBPROC v_glMultiTexCoord4fvARB;
extern PFNGLCREATESHADEROBJECTARBPROC v_glCreateShaderObjectARB;
extern PFNGLSHADERSOURCEARBPROC v_glShaderSourceARB;
extern PFNGLCOMPILESHADERARBPROC v_glCompileShaderARB;
extern PFNGLCREATEPROGRAMOBJECTARBPROC v_glCreateProgramObjectARB;
extern PFNGLATTACHOBJECTARBPROC v_glAttachObjectARB;
extern PFNGLLINKPROGRAMARBPROC v_glLinkProgramARB;
extern PFNGLUSEPROGRAMOBJECTARBPROC v_glUseProgramObjectARB;
extern PFNGLUNIFORM1IARBPROC v_glUniform1iARB;
extern PFNGLUNIFORM1FARBPROC v_glUniform1fARB;
extern PFNGLUNIFORM2FARBPROC v_glUniform2fARB;
extern PFNGLUNIFORM3FARBPROC v_glUniform3fARB;
extern PFNGLUNIFORM4FARBPROC v_glUniform4fARB;
extern PFNGLGETUNIFORMLOCATIONARBPROC v_glGetUniformLocationARB;
extern PFNGLDETACHOBJECTARBPROC v_glDetachObjectARB;
extern PFNGLDELETEOBJECTARBPROC v_glDeleteObjectARB;
extern PFNGLGETOBJECTPARAMETERIVARBPROC v_glGetObjectParameterivARB;
extern PFNGLGETINFOLOGARBPROC v_glGetInfoLogARB;
extern PFNGLBLENDFUNCSEPARATEEXTPROC v_glBlendFuncSeparateEXT;
extern PFNGLGENERATEMIPMAPEXTPROC v_glGenerateMipmapEXT;
extern PFNGLGETATTRIBLOCATIONARBPROC v_glGetAttribLocationARB;
extern PFNGLVERTEXATTRIB1FARBPROC v_glVertexAttrib1fARB;

#define glActiveTextureARB v_glActiveTextureARB
#define glMultiTexCoord4fARB v_glMultiTexCoord4fARB
#define glMultiTexCoord4fvARB v_glMultiTexCoord4fvARB
#define glCreateShaderObjectARB v_glCreateShaderObjectARB
#define glShaderSourceARB v_glShaderSourceARB
#define glCompileShaderARB v_glCompileShaderARB
#define glCreateProgramObjectARB v_glCreateProgramObjectARB
#define glAttachObjectARB v_glAttachObjectARB
#define glLinkProgramARB v_glLinkProgramARB
#define glUseProgramObjectARB v_glUseProgramObjectARB
#define glUniform1iARB v_glUniform1iARB
#define glUniform1fARB v_glUniform1fARB
#define glUniform2fARB v_glUniform2fARB
#define glUniform3fARB v_glUniform3fARB
#define glUniform4fARB v_glUniform4fARB
#define glGetUniformLocationARB v_glGetUniformLocationARB
#define glDetachObjectARB v_glDetachObjectARB
#define glDeleteObjectARB v_glDeleteObjectARB
#define glGetObjectParameterivARB v_glGetObjectParameterivARB
#define glGetInfoLogARB v_glGetInfoLogARB
#define glBlendFuncSeparateEXT v_glBlendFuncSeparateEXT
#define glGenerateMipmapEXT v_glGenerateMipmapEXT
#define glGetAttribLocationARB v_glGetAttribLocationARB
#define glVertexAttrib1fARB v_glVertexAttrib1fARB


#define VOGL_ATLEAST_V20			0x00000001
#define VOGL_ATLEAST_V21			0x00000002
#define VOGL_ATLEAST_V30			0x00000004
#define VOGL_HAS_SHADERS			0x00000010
#define VOGL_HAS_STENCIL_BUFFER		0x00000100
#define VOGL_HAS_ALPHA_PLANE		0x00000200


static const GLuint ogl_sfactor[16] = {
	GL_ZERO,
	GL_SRC_ALPHA,
	GL_DST_COLOR,
	GL_DST_ALPHA,
	GL_ONE,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_ONE_MINUS_DST_COLOR,
	GL_ONE_MINUS_DST_ALPHA,
	GL_ZERO,
	GL_ZERO,
	GL_ZERO,
	GL_ZERO,
	GL_ZERO,
	GL_ZERO,
	GL_ZERO,
	GL_SRC_ALPHA_SATURATE
};

static const GLuint ogl_dfactor[16] = {
	GL_ZERO,
	GL_SRC_ALPHA,
	GL_SRC_COLOR,
	GL_DST_ALPHA,
	GL_ONE,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_ONE_MINUS_SRC_COLOR,
	GL_ONE_MINUS_DST_ALPHA,
	GL_ZERO,
	GL_ZERO,
	GL_ZERO,
	GL_ZERO,
	GL_ZERO,
	GL_ZERO,
	GL_ZERO,
	GL_SRC_COLOR /* A_COLORBEFOREFOG */
};


void VOGL_Reset(void);

bool VOGL_Initialize(void);

bool VOGL_CheckFeature(Bit32u feat);
void VOGL_FlagFeature(Bit32u feat);

void VOGL_BeginMode(INT32 new_mode);
void VOGL_ClearBeginMode(void);

void VOGL_SetDepthMode(Bit32s mode, Bit32s func);
void VOGL_SetAlphaMode(Bit32s enabled_mode,GLuint src_rgb_fac,GLuint dst_rgb_fac,
											GLuint src_alpha_fac,GLuint dst_alpha_fac);

void VOGL_SetDepthMaskMode(bool masked);
void VOGL_SetColorMaskMode(bool cmasked, bool amasked);

void VOGL_SetDrawMode(bool front_draw);
void VOGL_SetReadMode(bool front_read);

#endif
