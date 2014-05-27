#include "vglImage.h"
#include "vglDcmtkIo.h"


int main(int argc, char *argv[])
{
    char * opt_ifname = argv[argc-2]; // name of the input file
    char * opt_ofname = argv[argc-1]; // name of the output file

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
