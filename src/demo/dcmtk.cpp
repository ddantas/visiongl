#include "vglImage.h"
#include "vglDcmtkFile.h"


int main(int argc, char *argv[])
{
    char * opt_ifname = argv[argc-2];//(char *) malloc(strlen(argv[argc-2]+1)); // nome de entrada do arquivo
    char * opt_ofname = argv[argc-1];//(char *) malloc(strlen(argv[argc-1]+1)); // nome de saída do arquivo

    //strcpy(opt_ifname, argv[argc-2]);
    //strcpy(opt_ofname, argv[argc-1]);
    
    VglImage imagevgl;

    imagevgl = vglLoadDicom(opt_ifname);
    if(argc == 3)
       vglSaveDicom(imagevgl, opt_ofname);
    else
       vglSaveDicomCompressed(imagevgl, opt_ofname);


  return 0;
}
