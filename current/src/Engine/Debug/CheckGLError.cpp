
#include <sstream>
#include "CheckGLError.h"
#include "..\Graphics\AEXGL.h"

namespace AEX
{
	void  _check_gl_error(const char * file, int line)
	{
		GLenum err(glGetError());

		while (err != GL_NO_ERROR) {
			std::string error;

			switch (err) {
			case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
			case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
			case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
			case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
			}

			// format error message
			std::stringstream errorMsg;
			errorMsg << "GL_" << error.c_str() << " - " << file << ":" << line << std::endl;

			// send to VS outpput
			OutputDebugString(errorMsg.str().c_str());

			// repeat
			err = glGetError();
		}
	}
}
