#ifndef AEX_GL_ERROR_H_
#define AEX_GL_ERROR_H_

#define check_gl_error() AEX::_check_gl_error(__FILE__, __LINE__)
namespace AEX
{
	void  _check_gl_error(const char * file, int line);
}
#endif