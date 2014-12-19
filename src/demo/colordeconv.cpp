#include "vglDeconv.h"

int main(int argc, char *argv[])
{
  char* usage = (char*) "\n\
Usage: demo_colordeconv <input file> option <output file> [values]\n\
\n\
Where:\n\
  <input file>: image to be deconvoluted\n\
  <output file>: name to save the three deconvoluted images. Must have a %d.\n\
  option:\n\
     1  -> H&E\n\
     2  -> H&E 2\n\
     3  -> H DAB\n\
     4  -> Feulgen Ligh tGreen\n\
     5  -> Giemsa\n\
     6  -> FastRed FastBlue DAB\n\
     7  -> Methyl Green DAB\n\
     8  -> H&E DAB\n\
     9  -> H AEC\n\
     10 -> Azan-Mallory\n\
     11 -> Masson Trichrome\n\
     12 -> Alcian Blue & H\n\
     13 -> H PAS\n\
     14 -> RGB\n\
     15 -> CMY\n\
  [values]: used for insert your own values, used only if option = 0\n\
    Example of [values] use:\n\
      0.0 0.345 0.21 0.12 0.32 0.55 0.99 1.0 0.45\n\
    Where:\n\
      the 3 first values corresponds to the values RGB of the first color\n\
      the 3 following values corresponds to the values RGB of the second color\n\
      the 3 last values corresponds to the values RGB of the third color\n\
\n";
  if(argc < 4)
  {
    printf("%s", usage);
    exit(1);
  }

  
  char *inFilename = argv[1]; // name of input file
  int imgIFirst = 0; // number of the first frame
  int imgILast  = 0; // number of the last frame
  int option = atoi(argv[2]); // type of colors
  char *outFilename = argv[3]; // name of output file  
  double *mInitial = (double *) malloc(9*sizeof(double));
  
  if((option == 0) && (argc != 13))
  {
    printf("%s", usage);
    exit(1);
  }

  switch(option)
  {
     case 1: /* H&E*/
       // col 0 = GL Hematoxylin
       mInitial[0] = 0.644211; //MODx[0]
       mInitial[3] = 0.716556; //MODy[0]
       mInitial[6] = 0.266844; //MODz[0]
       // col 1 = GL Eosine
       mInitial[1] = 0.092789; //MODx[1]
       mInitial[4] = 0.954111; //MODy[1]
       mInitial[7] = 0.283111; //MODz[1]
       // col 2 = None
       mInitial[2] = 0.0;      //MODx[2]
       mInitial[5] = 0.0;      //MODy[2]
       mInitial[8] = 0.0;      //MODz[2] 
       break;

     case 2:  /* H&E 2 */ 
       // col 0 = GL Hematoxylin
       mInitial[0] = 0.49015734; //MODx[0]
       mInitial[3] = 0.76897085; //MODy[0]
       mInitial[6] = 0.41040173; //MODz[0]
       // col 1 = GL Eosin
       mInitial[1] = 0.04615336; //MODx[1]
       mInitial[4] = 0.8420684; //MODy[1]
       mInitial[7] = 0.5373925; //MODz[1]
       // col 2 = None
       mInitial[2] = 0.0;      //MODx[2]
       mInitial[5] = 0.0;      //MODy[2]
       mInitial[8] = 0.0;      //MODz[2]
       break;
  
     case 3: /* H DAB */ 
       // col 0 = Hematoxylin
       mInitial[0] = 0.650; //MODx[0]
       mInitial[3] = 0.704; //MODy[0]
       mInitial[6] = 0.286; //MODz[0]
       // col 1 = DAB (3,3-Diamino-Benzidine-Tetrahydrochloride)
       mInitial[1] = 0.268; //MODx[1]
       mInitial[4] = 0.270; //MODy[1]
       mInitial[7] = 0.776; //MODz[1]
       // col 2 = None
       mInitial[2] = 0.0;      //MODx[2]
       mInitial[5] = 0.0;      //MODy[2]
       mInitial[8] = 0.0;      //MODz[2]
       break;
       
     case 4: /* Feulgen Ligh tGreen */ 
       // col 0 = Feulgen
       mInitial[0] = 0.46420921; //MODx[0]
       mInitial[3] = 0.83008335; //MODy[0]
       mInitial[6] = 0.30827187; //MODz[0]
       // col 1 = Light Green
       mInitial[1] = 0.94705542; //MODx[1]
       mInitial[4] = 0.25373821; //MODy[1]
       mInitial[7] = 0.19650764; //MODz[1]
       // col 2 = None
       mInitial[2] = 0.0;      //MODx[2]
       mInitial[5] = 0.0;      //MODy[2]
       mInitial[8] = 0.0;      //MODz[2]
       break;

     case 5: /* Giemsa */ 
       // col 0 = GL Methylene Blue and Eosin
       mInitial[0] = 0.834750233; //MODx[0]
       mInitial[3] = 0.513556283; //MODy[0]
       mInitial[6] = 0.196330403; //MODz[0]
       // col 1 = GL Eosin
       mInitial[1] = 0.092789; //MODx[1]
       mInitial[4] = 0.954111; //MODy[1]
       mInitial[7] = 0.283111; //MODz[1]
       // col 2 = None
       mInitial[2] = 0.0;      //MODx[2]
       mInitial[5] = 0.0;      //MODy[2]
       mInitial[8] = 0.0;      //MODz[2]
       break;

     case 6: /* FastRed FastBlue DAB */ 
       // col 0 = Fast Red
       mInitial[0] = 0.21393921; //MODx[0]
       mInitial[3] = 0.85112669; //MODy[0]
       mInitial[6] = 0.47794022; //MODz[0]
       // col 1 = Fast Blue
       mInitial[1] = 0.74890292; //MODx[1]
       mInitial[4] = 0.60624161; //MODy[1]
       mInitial[7] = 0.26731082; //MODz[1]
       // col 2 = DAB
       mInitial[2] = 0.268;      //MODx[2]
       mInitial[5] = 0.570;      //MODy[2]
       mInitial[8] = 0.776;      //MODz[2]
       break;

     case 7: /* Methyl Green DAB */ 
       // col 0 = GL Methyl Green
       mInitial[0] = 0.98003; //MODx[0]
       mInitial[3] = 0.144316; //MODy[0]
       mInitial[6] = 0.133146; //MODz[0]
       // col 1 = DAB
       mInitial[1] = 0.268; //MODx[1]
       mInitial[4] = 0.570; //MODy[1]
       mInitial[7] = 0.776; //MODz[1]
       // col 2 = None
       mInitial[2] = 0.0;      //MODx[2]
       mInitial[5] = 0.0;      //MODy[2]
       mInitial[8] = 0.0;      //MODz[2]
       break;

     case 8: /* H&E DAB */ 
       // col 0 = Hematoxylin
       mInitial[0] = 0.650; //MODx[0]
       mInitial[3] = 0.704; //MODy[0]
       mInitial[6] = 0.286; //MODz[0]
       // col 1 = Eosin
       mInitial[1] = 0.072; //MODx[1]
       mInitial[4] = 0.990; //MODy[1]
       mInitial[7] = 0.105; //MODz[1]
       // col 2 = DAB
       mInitial[2] = 0.268;      //MODx[2]
       mInitial[5] = 0.570;      //MODy[2]
       mInitial[8] = 0.776;      //MODz[2]
       break;

     case 9: /* H AEC */ 
       // col 0 = Hematoxylin
       mInitial[0] = 0.650; //MODx[0]
       mInitial[3] = 0.704; //MODy[0]
       mInitial[6] = 0.286; //MODz[0]
       // col 1 = AEC (3-Amino-9-Ethylcabazole)
       mInitial[1] = 0.2743; //MODx[1]
       mInitial[4] = 0.6796; //MODy[1]
       mInitial[7] = 0.6803; //MODz[1]
       // col 2 = None
       mInitial[2] = 0.0;      //MODx[2]
       mInitial[5] = 0.0;      //MODy[2]
       mInitial[8] = 0.0;      //MODz[2]
       break;

     case 10: /* Azan-Mallory */
       // AZAN (Azocarmine and Aniline Blue)
       // col 0 = GL Blue (Anilline Blue)
       mInitial[0] = 0.853033; //MODx[0]
       mInitial[3] = 0.508733; //MODy[0]
       mInitial[6] = 0.112656; //MODz[0]
       // col 1 = GL Red (Azocarmine)
       mInitial[1] = 0.09289875; //MODx[1]
       mInitial[4] = 0.8662008; //MODy[1]
       mInitial[7] = 0.49098468; //MODz[1]
       // col 2 = GL Orange (Orange-G)
       mInitial[2] = 0.10732849;      //MODx[2]
       mInitial[5] = 0.36765403;      //MODy[2]
       mInitial[8] = 0.9237484;      //MODz[2]
       break;

     case 11: /* Masson Trichrome */ 
       // col 0 = GL Methyl Blue
       mInitial[0] = 0.7995107; //MODx[0]
       mInitial[3] = 0.5913521; //MODy[0]
       mInitial[6] = 0.10528667; //MODz[0]
       // col 1 = GL Ponceau Fuchsin (aproximated)
       mInitial[1] = 0.09997159; //MODx[1]
       mInitial[4] = 0.73738605; //MODy[1]
       mInitial[7] = 0.6680326; //MODz[1]
       // col 2 = None
       mInitial[2] = 0.0;      //MODx[2]
       mInitial[5] = 0.0;      //MODy[2]
       mInitial[8] = 0.0;      //MODz[2]
       break;

     case 12: /* Alcian Blue & H */ 
       // col 0 = Alcian Blue
       mInitial[0] = 0.874622; //MODx[0]
       mInitial[3] = 0.457711; //MODy[0]
       mInitial[6] = 0.158256; //MODz[0]
       // col 1 = Hematoxylin after PAS matrix
       mInitial[1] = 0.552556; //MODx[1]
       mInitial[4] = 0.7544; //MODy[1]
       mInitial[7] = 0.353744; //MODz[1]
       // col 2 = None
       mInitial[2] = 0.0;      //MODx[2]
       mInitial[5] = 0.0;      //MODy[2]
       mInitial[8] = 0.0;      //MODz[2]
       break;

     case 13: /* H PAS */ 
       // col 0 = Hematoxylin
       mInitial[0] = 0.644211; //MODx[0]
       mInitial[3] = 0.716556; //MODy[0]
       mInitial[6] = 0.266844; //MODz[0]
       // col 1 = PAS
       mInitial[1] = 0.175411; //MODx[1]
       mInitial[4] = 0.972178; //MODy[1]
       mInitial[7] = 0.154589; //MODz[1]
       // col 2 = None
       mInitial[2] = 0.0;      //MODx[2]
       mInitial[5] = 0.0;      //MODy[2]
       mInitial[8] = 0.0;      //MODz[2]
       break;

     case 14: /* RGB */ 
       // col 0 = R
       mInitial[0] = 0.0; //MODx[0]
       mInitial[3] = 1.0; //MODy[0]
       mInitial[6] = 1.0; //MODz[0]
       // col 1 = G
       mInitial[1] = 1.0; //MODx[1]
       mInitial[4] = 0.0; //MODy[1]
       mInitial[7] = 1.0; //MODz[1]
       // col 2 = B
       mInitial[2] = 1.0;      //MODx[2]
       mInitial[5] = 1.0;      //MODy[2]
       mInitial[8] = 0.0;      //MODz[2]
       break;
       
     case 15: /* CMY */ 
       // col 0 = C
       mInitial[0] = 1.0; //MODx[0]
       mInitial[3] = 0.0; //MODy[0]
       mInitial[6] = 0.0; //MODz[0]
       // col 1 = M
       mInitial[1] = 0.0; //MODx[1]
       mInitial[4] = 1.0; //MODy[1]
       mInitial[7] = 0.0; //MODz[1]
       // col 2 = Y
       mInitial[2] = 0.0;      //MODx[2]
       mInitial[5] = 0.0;      //MODy[2]
       mInitial[8] = 1.0;      //MODz[2]
       break;

     default:
       int k = 4;
       // col 0 = color 1
       sscanf(argv[k+0], "%lf", &mInitial[0]);
       sscanf(argv[k+1], "%lf", &mInitial[3]);
       sscanf(argv[k+2], "%lf", &mInitial[6]);
       // col 1 = color 2
       sscanf(argv[k+3], "%lf", &mInitial[1]);
       sscanf(argv[k+4], "%lf", &mInitial[4]);
       sscanf(argv[k+5], "%lf", &mInitial[7]);
       // col 2 = color 3
       sscanf(argv[k+6], "%lf", &mInitial[2]);
       sscanf(argv[k+7], "%lf", &mInitial[5]);
       sscanf(argv[k+8], "%lf", &mInitial[8]);
  }

  VglImage *imgvgl, *imgvgldeconv;
  imgvgl = vglLoad3dImage(inFilename, imgIFirst, imgILast);
  //vglPrintImageInfo(imagevgl);
  printf("\n\nwidth = %d\nheigth = %d\nnChannels = %d\ndepth = %d\nframes = %d\n\n ", imgvgl->shape[0], imgvgl->shape[1], imgvgl->nChannels, imgvgl->depth, imgvgl->shape[2]);
  
  imgvgldeconv = vglColorDeconv(imgvgl, mInitial);
  char outname[strlen(outFilename)+10];
  sprintf(outname, outFilename, 4);
  vglSave3dImage(imgvgldeconv, outname, imgIFirst, imgILast);
  
  vglSaveColorDeconv(imgvgldeconv, mInitial, outFilename);

  printf("\n");
  printf("Results saved to %s\n", outFilename);
      
  return 0;
}
