#include <vermilion.h>

#ifdef _M_X64
  #ifdef _DEBUG
    #pragma comment (lib, "vermilion64_d.lib")
  #else
    #pragma comment (lib, "vermilion64.lib")
  #endif /* _DEBUG */
#else
  #ifdef _DEBUG
   #pragma comment (lib, "vermilion32_d.lib")
  #else
    #pragma comment (lib, "vermilion32.lib")
  #endif /* _DEBUG */
#endif /* _M_X64 */

#include <GL3/gl3.h>
#include <GL3/gl3w.h>

extern "C" void vglLoadDDS(const char* filename, vglImageData* image);

void main(void)
{
    vglImageData image;

    vglLoadDDS("C:/Vermilion-Book/trunk/Code/media/test.dds", &image);
}
