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


#include <stdlib.h>
#include <math.h>
#include <map>

#include "dosbox.h"

#if C_OPENGL

#include "voodoo_types.h"
#include "voodoo_vogl.h"


PFNGLACTIVETEXTUREARBPROC v_glActiveTextureARB = NULL;
PFNGLMULTITEXCOORD4FARBPROC v_glMultiTexCoord4fARB = NULL;
PFNGLMULTITEXCOORD4FVARBPROC v_glMultiTexCoord4fvARB = NULL;
PFNGLCREATESHADEROBJECTARBPROC v_glCreateShaderObjectARB = NULL;
PFNGLSHADERSOURCEARBPROC v_glShaderSourceARB = NULL;
PFNGLCOMPILESHADERARBPROC v_glCompileShaderARB = NULL;
PFNGLCREATEPROGRAMOBJECTARBPROC v_glCreateProgramObjectARB = NULL;
PFNGLATTACHOBJECTARBPROC v_glAttachObjectARB = NULL;
PFNGLLINKPROGRAMARBPROC v_glLinkProgramARB = NULL;
PFNGLUSEPROGRAMOBJECTARBPROC v_glUseProgramObjectARB = NULL;
PFNGLUNIFORM1IARBPROC v_glUniform1iARB = NULL;
PFNGLUNIFORM1FARBPROC v_glUniform1fARB = NULL;
PFNGLUNIFORM2FARBPROC v_glUniform2fARB = NULL;
PFNGLUNIFORM3FARBPROC v_glUniform3fARB = NULL;
PFNGLUNIFORM4FARBPROC v_glUniform4fARB = NULL;
PFNGLGETUNIFORMLOCATIONARBPROC v_glGetUniformLocationARB = NULL;
PFNGLDETACHOBJECTARBPROC v_glDetachObjectARB = NULL;
PFNGLDELETEOBJECTARBPROC v_glDeleteObjectARB = NULL;
PFNGLGETOBJECTPARAMETERIVARBPROC v_glGetObjectParameterivARB = NULL;
PFNGLGETINFOLOGARBPROC v_glGetInfoLogARB = NULL;
PFNGLBLENDFUNCSEPARATEEXTPROC v_glBlendFuncSeparateEXT = NULL;
PFNGLGENERATEMIPMAPEXTPROC v_glGenerateMipmapEXT = NULL;
PFNGLGETATTRIBLOCATIONARBPROC v_glGetAttribLocationARB = NULL;
PFNGLVERTEXATTRIB1FARBPROC v_glVertexAttrib1fARB = NULL;


static Bit32s opengl_version = -1;

static bool has_shaders = false;
static bool has_stencil = false;
static bool has_alpha = false;


static INT32 current_begin_mode = -1;

static Bit32s current_depth_mode = -1;
static Bit32s current_depth_func = -1;

static Bit32s current_alpha_enabled = -1;
static Bit32s current_src_rgb_fac = -1;
static Bit32s current_dst_rgb_fac = -1;
static Bit32s current_src_alpha_fac = -1;
static Bit32s current_dst_alpha_fac = -1;

static bool depth_masked = false;
static bool color_masked = false;
static bool alpha_masked = false;

// buffer read/write defaults are back-buffer for double buffered contexts
static bool draw_to_front_buffer = false;
static bool read_from_front_buffer = false;


void VOGL_Reset(void) {
	opengl_version = -1;
	has_shaders = false;
	has_stencil = false;
	has_alpha = false;

	current_depth_mode=-1;
	current_depth_func=-1;

	current_alpha_enabled=-1;
	current_src_rgb_fac=-1;
	current_dst_rgb_fac=-1;
	current_src_alpha_fac=-1;
	current_dst_alpha_fac=-1;

	depth_masked = false;
	color_masked = false;
	alpha_masked = false;

	draw_to_front_buffer = false;
	read_from_front_buffer = false;
}


void VOGL_InitVersion(void) {
	opengl_version = -1;

	char gl_verstr[16];
	const GLubyte* gl_verstr_ub = glGetString(GL_VERSION);
	strncpy(gl_verstr, (const char*)gl_verstr_ub, 16);
	gl_verstr[15] = 0;
	char* gl_ver_minor = strchr(gl_verstr, '.');
	if (gl_ver_minor != NULL) {
		gl_ver_minor++;
		char* skip = strchr(gl_ver_minor, '.');
		if (skip != NULL) *skip = 0;
	}

	int ogl_ver = 100;
	if (gl_verstr[0] != 0) {
		int major = 1;
		int minor = 0;
		if (strchr(gl_verstr, '.') != NULL) {
			if (sscanf(gl_verstr,"%d.%d", &major, &minor) != 2) major = 0;
		} else {
			if (sscanf(gl_verstr, "%d", &major) != 1) major = 0;
		}
		if (major > 0) {
			ogl_ver = major*100;
			if (minor >= 0) {
				if (minor < 10) ogl_ver += minor*10;
				else ogl_ver += minor;
			}
		}
	}

	if (ogl_ver > 0) opengl_version = ogl_ver;
}

void VOGL_ClearShaderFunctions(void) {
	v_glShaderSourceARB = NULL;
	v_glCompileShaderARB = NULL;
	v_glCreateProgramObjectARB = NULL;
	v_glAttachObjectARB = NULL;
	v_glLinkProgramARB = NULL;
	v_glUseProgramObjectARB = NULL;
	v_glUniform1iARB = NULL;
	v_glUniform1fARB = NULL;
	v_glUniform2fARB = NULL;
	v_glUniform3fARB = NULL;
	v_glUniform4fARB = NULL;
	v_glGetUniformLocationARB = NULL;
	v_glDetachObjectARB = NULL;
	v_glDeleteObjectARB  = NULL;
	v_glGetObjectParameterivARB = NULL;
	v_glGetInfoLogARB = NULL;
}

bool VOGL_Initialize(void) {
	VOGL_ClearShaderFunctions();
	
	VOGL_InitVersion();

	v_glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)SDL_GL_GetProcAddress("glActiveTextureARB");
	if (!v_glActiveTextureARB) {
		LOG_MSG("opengl: glActiveTextureARB extension not supported");
		return false;
	}

	v_glMultiTexCoord4fARB = (PFNGLMULTITEXCOORD4FARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord4fARB");
	if (!v_glMultiTexCoord4fARB) {
		LOG_MSG("opengl: glMultiTexCoord4fARB extension not supported");
		return false;
	}

	v_glMultiTexCoord4fvARB = (PFNGLMULTITEXCOORD4FVARBPROC)SDL_GL_GetProcAddress("glMultiTexCoord4fvARB");
	if (!v_glMultiTexCoord4fvARB) {
		LOG_MSG("opengl: glMultiTexCoord4fvARB extension not supported");
		return false;
	}

	v_glBlendFuncSeparateEXT = (PFNGLBLENDFUNCSEPARATEEXTPROC)SDL_GL_GetProcAddress("glBlendFuncSeparateEXT");
	if (!v_glBlendFuncSeparateEXT) {
		LOG_MSG("opengl: glBlendFuncSeparateEXT extension not supported");
		return false;
	}

	v_glGenerateMipmapEXT = (PFNGLGENERATEMIPMAPEXTPROC)SDL_GL_GetProcAddress("glGenerateMipmapEXT");
	if (!v_glGenerateMipmapEXT) {
		LOG_MSG("opengl: glGenerateMipmapEXT extension not supported");
		return false;
	}

	if (VOGL_CheckFeature(VOGL_ATLEAST_V20)) {
		const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
		if (strstr(extensions, "GL_ARB_shader_objects") && strstr(extensions, "GL_ARB_vertex_shader") &&
			strstr(extensions, "GL_ARB_fragment_shader")) {

			v_glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)SDL_GL_GetProcAddress("glCreateShaderObjectARB");
			if (!v_glCreateShaderObjectARB) {
				LOG_MSG("opengl: shader extensions not supported. Using fixed pipeline");
			} else {
				v_glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)SDL_GL_GetProcAddress("glShaderSourceARB");
				if (!v_glShaderSourceARB) LOG_MSG("opengl: glShaderSourceARB extension not supported");

				v_glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)SDL_GL_GetProcAddress("glCompileShaderARB");
				if (!v_glCompileShaderARB) LOG_MSG("opengl: glCompileShaderARB extension not supported");

				v_glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)SDL_GL_GetProcAddress("glCreateProgramObjectARB");
				if (!v_glCreateProgramObjectARB) LOG_MSG("opengl: glCreateProgramObjectARB extension not supported");

				v_glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)SDL_GL_GetProcAddress("glAttachObjectARB");
				if (!v_glAttachObjectARB) LOG_MSG("opengl: glAttachObjectARB extension not supported");

				v_glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)SDL_GL_GetProcAddress("glLinkProgramARB");
				if (!v_glLinkProgramARB) LOG_MSG("opengl: glLinkProgramARB extension not supported");

				v_glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)SDL_GL_GetProcAddress("glUseProgramObjectARB");
				if (!v_glUseProgramObjectARB) LOG_MSG("opengl: glUseProgramObjectARB extension not supported");

				v_glUniform1iARB = (PFNGLUNIFORM1IARBPROC)SDL_GL_GetProcAddress("glUniform1iARB");
				if (!v_glUniform1iARB) LOG_MSG("opengl: glUniform1iARB extension not supported");

				v_glUniform1fARB = (PFNGLUNIFORM1FARBPROC)SDL_GL_GetProcAddress("glUniform1fARB");
				if (!v_glUniform1fARB) LOG_MSG("opengl: glUniform1fARB extension not supported");

				v_glUniform2fARB = (PFNGLUNIFORM2FARBPROC)SDL_GL_GetProcAddress("glUniform2fARB");
				if (!v_glUniform2fARB) LOG_MSG("opengl: glUniform2fARB extension not supported");

				v_glUniform3fARB = (PFNGLUNIFORM3FARBPROC)SDL_GL_GetProcAddress("glUniform3fARB");
				if (!v_glUniform3fARB) LOG_MSG("opengl: glUniform3fARB extension not supported");

				v_glUniform4fARB = (PFNGLUNIFORM4FARBPROC)SDL_GL_GetProcAddress("glUniform4fARB");
				if (!v_glUniform4fARB) LOG_MSG("opengl: glUniform4fARB extension not supported");

				v_glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)SDL_GL_GetProcAddress("glGetUniformLocationARB");
				if (!v_glGetUniformLocationARB) LOG_MSG("opengl: glGetUniformLocationARB extension not supported");

				v_glDetachObjectARB = (PFNGLDETACHOBJECTARBPROC)SDL_GL_GetProcAddress("glDetachObjectARB");
				if (!v_glDetachObjectARB) LOG_MSG("opengl: glDetachObjectARB extension not supported");

				v_glDeleteObjectARB  = (PFNGLDELETEOBJECTARBPROC)SDL_GL_GetProcAddress("glDeleteObjectARB");
				if (!v_glDeleteObjectARB) LOG_MSG("opengl: glDeleteObjectARB extension not supported");

				v_glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)SDL_GL_GetProcAddress("glGetObjectParameterivARB");
				if (!v_glGetObjectParameterivARB) LOG_MSG("opengl: glGetObjectParameterivARB extension not supported");

				v_glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)SDL_GL_GetProcAddress("glGetInfoLogARB");
				if (!v_glGetInfoLogARB) LOG_MSG("opengl: glGetInfoLogARB extension not supported");

				v_glGetAttribLocationARB = (PFNGLGETATTRIBLOCATIONARBPROC)SDL_GL_GetProcAddress("glGetAttribLocationARB");
				if (!v_glGetAttribLocationARB) LOG_MSG("opengl: glGetAttribLocationARB extension not supported");
			
				v_glVertexAttrib1fARB = (PFNGLVERTEXATTRIB1FARBPROC)SDL_GL_GetProcAddress("glVertexAttrib1fARB");
				if (!v_glVertexAttrib1fARB) LOG_MSG("opengl: glVertexAttrib1fARB extension not supported");

				if (v_glShaderSourceARB && v_glCompileShaderARB && v_glCreateProgramObjectARB &&
					v_glAttachObjectARB && v_glLinkProgramARB && v_glUseProgramObjectARB &&
					v_glUniform1iARB && v_glUniform1fARB && v_glUniform2fARB && v_glUniform3fARB &&
					v_glUniform4fARB && v_glGetUniformLocationARB && v_glDetachObjectARB &&
					v_glDeleteObjectARB && v_glGetObjectParameterivARB && v_glGetInfoLogARB) {
						VOGL_FlagFeature(VOGL_HAS_SHADERS);
//						LOG_MSG("opengl: shader functionality enabled");
				} else {
					VOGL_ClearShaderFunctions();
				}
			}
		}
	}

	return true;
}


bool VOGL_CheckFeature(Bit32u feat) {
	switch (feat) {
		case VOGL_ATLEAST_V20:
			if (opengl_version >= 200) return true;
			break;
		case VOGL_ATLEAST_V21:
			if (opengl_version >= 210) return true;
			break;
		case VOGL_ATLEAST_V30:
			if (opengl_version >= 300) return true;
			break;
		case VOGL_HAS_SHADERS:
			if (has_shaders) return true;
			break;
		case VOGL_HAS_STENCIL_BUFFER:
			if (has_stencil) return true;
			break;
		case VOGL_HAS_ALPHA_PLANE:
			if (has_alpha) return true;
			break;
		default:
			LOG_MSG("opengl: unknown feature queried: %x",feat);
			break;
	}

	return false;
}

void VOGL_FlagFeature(Bit32u feat) {
	switch (feat) {
		case VOGL_HAS_SHADERS:
			has_shaders = true;
			break;
		case VOGL_HAS_STENCIL_BUFFER:
			has_stencil = true;
			break;
		case VOGL_HAS_ALPHA_PLANE:
			has_alpha = true;
			break;
		default:
			LOG_MSG("opengl: unknown feature: %x",feat);
			break;
	}
}


void VOGL_BeginMode(INT32 new_mode) {
	if (current_begin_mode > -1) {
		if (new_mode != current_begin_mode) {
			glEnd();
			if (new_mode > -1) glBegin(new_mode);
			current_begin_mode = new_mode;
		}
	} else {
		if (new_mode > -1) {
			glBegin(new_mode);
			current_begin_mode = new_mode;
		}
	}
}

void VOGL_ClearBeginMode(void) {
	if (current_begin_mode > -1) {
		glEnd();
		current_begin_mode = -1;
	}
}


void VOGL_SetDepthMode(Bit32s mode, Bit32s func) {
	if (current_depth_mode!=mode) {
		if (mode!=0) {
			VOGL_ClearBeginMode();
			glEnable(GL_DEPTH_TEST);
			current_depth_mode=1;
			if (current_depth_func!=func) {
				glDepthFunc(GL_NEVER+func);
				current_depth_func=func;
			}
		} else {
			VOGL_ClearBeginMode();
			glDisable(GL_DEPTH_TEST);
			current_depth_mode=0;
		}
	} else {
		if ((mode!=0) && (current_depth_func!=func)) {
			VOGL_ClearBeginMode();
			glDepthFunc(GL_NEVER+func);
			current_depth_func=func;
		}
	}
}


void VOGL_SetAlphaMode(Bit32s enabled_mode,GLuint src_rgb_fac,GLuint dst_rgb_fac,
											GLuint src_alpha_fac,GLuint dst_alpha_fac) {
	if (current_alpha_enabled!=enabled_mode) {
		VOGL_ClearBeginMode();
		if (enabled_mode!=0) {
			glEnable(GL_BLEND);
			current_alpha_enabled=1;
			if ((current_src_rgb_fac!=(Bit32s)src_rgb_fac) || (current_dst_rgb_fac!=(Bit32s)dst_rgb_fac) ||
				(current_src_alpha_fac!=(Bit32s)src_alpha_fac) || (current_dst_alpha_fac!=(Bit32s)dst_alpha_fac)) {
				glBlendFuncSeparateEXT(src_rgb_fac, dst_rgb_fac, src_alpha_fac, dst_alpha_fac);
				current_src_rgb_fac=(Bit32s)src_rgb_fac;
				current_dst_rgb_fac=(Bit32s)dst_rgb_fac;
				current_src_alpha_fac=(Bit32s)src_alpha_fac;
				current_dst_alpha_fac=(Bit32s)dst_alpha_fac;
			}
		} else {
			glDisable(GL_BLEND);
			current_alpha_enabled=0;
		}
	} else {
		if (current_alpha_enabled!=0) {
			if ((current_src_rgb_fac!=(Bit32s)src_rgb_fac) || (current_dst_rgb_fac!=(Bit32s)dst_rgb_fac) ||
				(current_src_alpha_fac!=(Bit32s)src_alpha_fac) || (current_dst_alpha_fac!=(Bit32s)dst_alpha_fac)) {
				VOGL_ClearBeginMode();
				glBlendFuncSeparateEXT(src_rgb_fac, dst_rgb_fac, src_alpha_fac, dst_alpha_fac);
				current_src_rgb_fac=(Bit32s)src_rgb_fac;
				current_dst_rgb_fac=(Bit32s)dst_rgb_fac;
				current_src_alpha_fac=(Bit32s)src_alpha_fac;
				current_dst_alpha_fac=(Bit32s)dst_alpha_fac;
			}
		}
	}
}


void VOGL_SetDepthMaskMode(bool masked) {
	if (depth_masked!=masked) {
		VOGL_ClearBeginMode();
		if (masked) {
			glDepthMask(GL_TRUE);
			depth_masked=true;
		} else {
			glDepthMask(GL_FALSE);
			depth_masked=false;
		}
	}
}


void VOGL_SetColorMaskMode(bool cmasked, bool amasked) {
	if ((color_masked!=cmasked) || (alpha_masked!=amasked)) {
		color_masked=cmasked;
		alpha_masked=amasked;
		GLboolean cm = (color_masked ? GL_TRUE : GL_FALSE);
		GLboolean am = (alpha_masked ? GL_TRUE : GL_FALSE);
		glColorMask(cm,cm,cm,am);
	}
}


void VOGL_SetDrawMode(bool front_draw) {
	if (draw_to_front_buffer!=front_draw) {
		VOGL_ClearBeginMode();
		if (front_draw) glDrawBuffer(GL_FRONT);
		else glDrawBuffer(GL_BACK);
		draw_to_front_buffer=front_draw;
	}
}


void VOGL_SetReadMode(bool front_read) {
	VOGL_ClearBeginMode();

	if (read_from_front_buffer!=front_read) {
		if (front_read) glReadBuffer(GL_FRONT);
		else glReadBuffer(GL_BACK);
		read_from_front_buffer=front_read;
	}
}

#endif
