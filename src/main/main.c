#include "../xfenetre/xfenetre.h"
#include "../pave_serpent/pave_serpent.h"
#include "../animation/animation.h"
#include "../log/log.h"

int main(int argc, char *argv[])
{
    enable_log(argc, argv);

    /* choix xfenetre, pave_serpent, animation */
    if (argc > 2)
    {
        if (strcmp("--xfenetre", argv[2]) == 0) xfenetre_main();
        else if (strcmp("--pave_serpent", argv[2]) == 0) pave_serpent_main();
        else if (strcmp("--animation", argv[2]) == 0) animation_main();
    }
    else {
        fprintf(stdout,
                "app arguments:\n"
                "\t--xfenetre\n"
                "\t--pave_serpent\n"
                "\t--animation\n");
    }
    
    return 0;
}

