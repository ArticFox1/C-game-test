//Libraries
#include "app_lib.h"

#include "platform.h"

#define APIENTRY
#include "glcorearb.h"

//Windows
#ifdef _WIN32
#include "win32_platform.ccp"
#endif

#include "gl_renderer.h"

int main() {

    platform_create_window(1200,720,"Sup gng");

    while (running)
    {
        // Update
        platform_update_window();

        //Assert(false,"The program doesn't like you :<");
    }
    
    return 0;
}