#ifdef __DCMTK__

#include <vglDcmtkFile.h>
#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dcmtk/dcmdata/dctk.h"          /* for various dcmdata headers */
#include "dcmtk/dcmdata/dcrledrg.h"      /* for DcmRLEDecoderRegistration */
#include "dcmtk/dcmimgle/dcmimage.h"     /* for DicomImage */
#include "dcmtk/dcmimage/diregist.h"     /* include to support color images */

#include "dcmtk/dcmjpeg/djdecode.h"      /* for dcmjpeg decoders */
#include "dcmtk/dcmjpeg/djencode.h"      /* for dcmjpeg encoders */
#include "dcmtk/dcmjpeg/djrplol.h"       /* for DJ_RPLossless */
#include "dcmtk/dcmjpeg/djrploss.h"      /* for DJ_RPLossy */
#include "dcmtk/dcmjpeg/dipijpeg.h"      /* for dcmimage JPEG plugin */


//========= Métodos para converter depth =============//

int convertDepthDcmToVgl(int dcmDepth)
{
  if(dcmDepth <= 8)
     return CV_8U;
  else
     if(dcmDepth <= 16)
        return CV_16U;
     else
       if(dcmDepth <= 32)
	 return CV_32S;
}

int convertDepthVglToDcm(int vglDepth)
{
  if(vglDepth == CV_8U)
     return 8;
  else
     if(vglDepth <= CV_16U)
       return 16;
     else
       if(vglDepth <= CV_32S)
	 return 32;
}

//========= Método para carregar a imagem ============//

VglImage vglLoadDicom(char* opt_ifname)
{
    E_FileReadMode      opt_readMode = ERM_autoDetect;    /* default: fileformat or dataset */
     /* Outros valores para opt_readMode:
      - para file opt_readMode = ERM_fileOnly;
      - para dataset opt_readMode = ERM_dataset;
     */
    E_TransferSyntax    opt_transferSyntax = EXS_Unknown; /* default: xfer syntax recognition */
    /* Valores possiveis para opt_transferSyntax:
       opt_transferSyntax = EXS_Unknown;
       opt_transferSyntax = EXS_LittleEndianExplicit;
       opt_transferSyntax = EXS_BigEndianExplicit;
       opt_transferSyntax = EXS_LittleEndianImplicit
    */
    unsigned long       opt_compatibilityMode = CIF_MayDetachPixelData | CIF_TakeOverExternalDataset;
    /* Outros tipos de valores que podem ser adicionados à opt_compatibilityMode:
       - para aceitar acr-nema opt_compatibilityMode |= CIF_AcrNemaCompatibility;
       - para aceitar palettes opt_compatibilityMode |= CIF_WrongPaletteAttributeTags;
       - para checar lut-depth opt_compatibilityMode |= CIF_CheckLutBitDepth;
       - para ignorar mlut-depth opt_compatibilityMode |= CIF_IgnoreModalityLutBitDepth;
    */
    OFCmdUnsignedInt    opt_frame = 1;                    /* default: first frame */
    OFCmdUnsignedInt    opt_frameCount = 0;               /* default: all frames */
    int                 opt_multiFrame = 1;               /* default: multiframes // opt_multifFrame = 0; no multiframes */
  

    // JPEG parameters
    E_DecompressionColorSpaceConversion opt_decompCSconversion = EDC_photometricInterpretation; // conv photometric
    /* Outros valores para opt_decompCSconversion:
       - para conv lossy opt_decompCSconversion = EDC_lossyOnly;
       - para conv guess opt_decompCSconversion = EDC_guess;
       - para conv guess lossy opt_decompCSconversion = EDC_guessLossyOnly;
       - para conv always opt_decompCSconversion = EDC_always;
       - para conv never opt_decompCSconversion = EDC_never;
    */
    OFCmdUnsignedInt    opt_fileBits = 0;

    // register JPEG decompression codecs
    DJDecoderRegistration::registerCodecs(opt_decompCSconversion);

    DcmFileFormat *dfile = new DcmFileFormat();
    OFCondition cond = dfile->loadFile(opt_ifname, opt_transferSyntax, EGL_withoutGL, DCM_MaxReadLength, opt_readMode);

    DcmDataset *dataset = dfile->getDataset();
    E_TransferSyntax xfer = dataset->getOriginalXfer();
    Sint32 frameCount;


    if (dataset->findAndGetSint32(DCM_NumberOfFrames, frameCount).bad())
       frameCount = 1;
    if ((opt_frameCount == 0) || ((opt_frame == 1) && (opt_frameCount == OFstatic_cast(Uint32, frameCount))))
       // since we process all frames anyway, decompress the complete pixel data (if required)
       opt_compatibilityMode |= CIF_DecompressCompletePixelData;
    if (frameCount > 1)
       // use partial read access to pixel data (only in case of multiple frames)
       opt_compatibilityMode |= CIF_UsePartialAccessToPixelData;


    DicomImage *di = new DicomImage(dfile, xfer, opt_compatibilityMode, opt_frame - 1, opt_frameCount);

    unsigned int fcount = OFstatic_cast(unsigned int, ((opt_frameCount > 0) && (opt_frameCount <= di->getFrameCount())) ? opt_frameCount : di->getFrameCount());
  
    VglImage imagevgl;

    imagevgl.filename = (char *) malloc(strlen(opt_ifname));
    imagevgl.filename = opt_ifname;
    imagevgl.shape[0] = di->getWidth();           // width
    imagevgl.shape[1] = di->getHeight();          // height
    imagevgl.shape[2] = di->getFrameCount();      // number of Frames
    int depth = (di->getDepth()/8)*8;             // bits of bits of image
    imagevgl.depth = convertDepthDcmToVgl(depth); // depth of image
        
    if(di->isMonochrome())
       imagevgl.nChannels = 1;
    else
       imagevgl.nChannels = 3;
    
    int nPixels = imagevgl.shape[0]*imagevgl.shape[1]*imagevgl.nChannels;
    int nBytes = nPixels*(di->getDepth()/8);
    int totalBytes = nBytes*imagevgl.shape[2];
	
    int j = 0;
    imagevgl.ndarray = (void *) malloc(totalBytes);//ok16nBytes);
    
    for (int frame = 0; frame < fcount; frame++)
    {
      void *pixelData = (void *)(di->getOutputData(depth, frame));
      memcpy((imagevgl.ndarray)+j, pixelData, nBytes);  
      j += nPixels; 
    }
        
    delete di;

    // deregister JPEG decompression codecs
    DJDecoderRegistration::cleanup();

    return imagevgl; 
}


//=========== Método para salvar imagem sem compressão ============//


int vglSaveDicom(VglImage imagevgl, char* opt_ofname)
{ 
    E_FileReadMode      opt_readMode = ERM_autoDetect;    /* default: fileformat or dataset */
    /* Outros valores para opt_readMode:
      - para file opt_readMode = ERM_fileOnly;
      - para dataset opt_readMode = ERM_dataset;
    */
    E_TransferSyntax    opt_transferSyntax = EXS_Unknown; /* default: xfer syntax recognition */
    /* Valores possiveis para opt_transferSyntax:
       opt_transferSyntax = EXS_Unknown;
       opt_transferSyntax = EXS_LittleEndianExplicit;
       opt_transferSyntax = EXS_BigEndianExplicit;
       opt_transferSyntax = EXS_LittleEndianImplicit
    */
  
    DcmFileFormat *dfile = new DcmFileFormat();
    OFCondition cond = dfile->loadFile(imagevgl.filename, opt_transferSyntax, EGL_withoutGL, DCM_MaxReadLength, opt_readMode);
    DcmDataset *dataset = dfile->getDataset();
    int nPixels = imagevgl.shape[0]*imagevgl.shape[1]*imagevgl.nChannels;
    int dcmDepth = convertDepthVglToDcm(imagevgl.depth); 
    int nBytes = nPixels*(dcmDepth/8);
    int totalBytes = nBytes*imagevgl.shape[2];
    
    if(dcmDepth == 8)
       dataset->putAndInsertUint8Array(DCM_PixelData, (Uint8 *)imagevgl.ndarray,totalBytes);
    else
      if(dcmDepth == 16)
	 dataset->putAndInsertUint16Array(DCM_PixelData, (Uint16 *)imagevgl.ndarray, totalBytes);
        
    if(imagevgl.nChannels == 3)
       dataset->putAndInsertString(DCM_PhotometricInterpretation, "RGB"); 
    
    dfile->saveFile(opt_ofname, EXS_LittleEndianExplicit);
    
    return 0;
}


//========= Método para salvar imagem com compressão ==========//


int vglSaveDicomCompressed(VglImage imagevgl, char* opt_ofname)
{
#define OFFIS_CONSOLE_APPLICATION "dcmcjpeg"

    static OFLogger dcmcjpegLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

#ifdef HAVE_GUSI_H
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

    E_FileReadMode opt_readMode = ERM_autoDetect; // default: fileformat or dataset
    /* Outros valores para opt_readMode:
      - para file opt_readMode = ERM_fileOnly;
      - para dataset opt_readMode = ERM_dataset;
    */
    E_TransferSyntax opt_ixfer = EXS_Unknown;
    /* Outros valores para opt_ixfer:
         opt_ixfer = EXS_Unknown;
         opt_ixfer = EXS_LittleEndianExplicit;
         opt_ixfer = EXS_BigEndianExplicit;
         opt_ixfer = EXS_LittleEndianImplicit;	 
    */
    E_GrpLenEncoding opt_oglenc = EGL_recalcGL; // group length recalc
    /* Outros valores para opt_oglenc:
       - para group length create opt_oglenc = EGL_withGL;
       - para group length remove opt_oglenc = EGL_withoutGL;
    */
    E_EncodingType opt_oenctype = EET_ExplicitLength; // length explicit
    /* Outro valor, para length undefined opt_oenctype = EET_UndefinedLength; */
    E_PaddingEncoding opt_opadenc = EPD_noChange; // padding retain
    /* Outros valores para opt_opadenc:
       - para padding off opt_opadenc = EPD_withoutPadding;
       - para padding create opt_opadenc = EPD_withPadding;
    */
    OFCmdUnsignedInt opt_filepad = 0;
    OFCmdUnsignedInt opt_itempad = 0;
    OFBool opt_acceptWrongPaletteTags = OFFalse;
    OFBool opt_acrNemaCompatibility = OFFalse;

    // JPEG options
    E_TransferSyntax opt_oxfer =  EXS_JPEGProcess1; // valor para  modo baseline
    /* Outros valores para opt_oxfer:
      - para lossless-sv1 opt_oxfer = EXS_JPEGProcess14SV1TransferSyntax;
      - para lossless opt_oxfer = EXS_JPEGProcess14TransferSyntax;
      - para extended opt_oxfer = EXS_JPEGProcess2_4TransferSyntax;
      - para spectral opt_oxfer = EXS_JPEGProcess6_8TransferSyntax;
      - para progressive opt_oxfer = EXS_JPEGProcess10_12TransferSyntax;
    */ 
    OFCmdUnsignedInt opt_selection_value = 6;
    OFCmdUnsignedInt opt_point_transform = 0;
    OFCmdUnsignedInt opt_quality = 90;
    OFBool           opt_huffmanOptimize = OFTrue;
    OFCmdUnsignedInt opt_smoothing = 0;
    int              opt_compressedBits = 0; // 0=auto, 8/12/16=force
    E_CompressionColorSpaceConversion opt_compCSconversion = ECC_lossyRGB; // cor rgb
    /* Outros valores para opt_compCSconversion:
       - para cor ybr opt_compCSconversion = ECC_lossyYCbCr;
       - para cor monochrome opt_compCSconversion = ECC_monochrome;
    */
    E_DecompressionColorSpaceConversion opt_decompCSconversion = EDC_photometricInterpretation; // conv photometric
    /* Outros valores para opt_decompCSconversion:
       - para conv lossy opt_decompCSconversion = EDC_lossyOnly;
       - para conv guess opt_decompCSconversion = EDC_guess;
       - para conv guess lossy opt_decompCSconversion = EDC_guessLossyOnly;
       - para conv always opt_decompCSconversion = EDC_always;
       - para conv never opt_decompCSconversion = EDC_never;   
    */
    E_SubSampling    opt_sampleFactors = ESS_444; 
    /* Outros valores para opt_sampleFactors: ESS_422; ESS_411*/
    OFBool           opt_useYBR422 = OFFalse;
    OFCmdUnsignedInt opt_fragmentSize = 0; // 0=unlimited
    OFBool           opt_createOffsetTable = OFTrue;
    int              opt_windowType = 0;  /* default: no windowing; 1=Wi, 2=Wl, 3=Wm, 4=Wh, 5=Ww, 6=Wn, 7=Wr */
    OFCmdUnsignedInt opt_windowParameter = 0;
    OFCmdFloat       opt_windowCenter=0.0, opt_windowWidth=0.0;
    E_UIDCreation    opt_uidcreation = EUC_default; // uid default
    /* Outros valores para opt_uidcreation:
       - para uid always opt_uidcreation = EUC_always;
       - para uid never opt_uidcreation = EUC_never;
    */
    OFBool           opt_secondarycapture = OFFalse;
    OFCmdUnsignedInt opt_roiLeft = 0, opt_roiTop = 0, opt_roiWidth = 0, opt_roiHeight = 0;
    OFBool           opt_usePixelValues = OFTrue;
    OFBool           opt_useModalityRescale = OFFalse;
    OFBool           opt_trueLossless = OFTrue;
    OFBool           lossless = OFTrue;  /* see opt_oxfer */


    // check for JPEG lossless output transfer syntaxes
    lossless = (opt_oxfer == EXS_JPEGProcess14SV1) || (opt_oxfer == EXS_JPEGProcess14);

    // disable true lossless mode since we are not encoding with JPEG lossless
    if (!lossless) opt_trueLossless = OFFalse;

    // register global decompression codecs
    DJDecoderRegistration::registerCodecs(
      opt_decompCSconversion,
      opt_uidcreation);

    // register global compression codecs
    DJEncoderRegistration::registerCodecs(
      opt_compCSconversion,
      opt_uidcreation,
      opt_huffmanOptimize,
      OFstatic_cast(int, opt_smoothing),
      opt_compressedBits,
      OFstatic_cast(Uint32, opt_fragmentSize),
      opt_createOffsetTable,
      opt_sampleFactors,
      opt_useYBR422,
      opt_secondarycapture,
      opt_windowType,
      opt_windowParameter,
      opt_windowCenter,
      opt_windowWidth,
      opt_roiLeft,
      opt_roiTop,
      opt_roiWidth,
      opt_roiHeight,
      opt_usePixelValues,
      opt_useModalityRescale,
      opt_acceptWrongPaletteTags,
      opt_acrNemaCompatibility,
      opt_trueLossless);

    DcmFileFormat fileformat;
    OFCondition error = fileformat.loadFile(imagevgl.filename, opt_ixfer, EGL_noChange, DCM_MaxReadLength, opt_readMode);

    DcmDataset *dataset = fileformat.getDataset();
    
    int nPixels = imagevgl.shape[0]*imagevgl.shape[1]*imagevgl.nChannels;
    int dcmDepth = convertDepthVglToDcm(imagevgl.depth); 
    int nBytes = nPixels*(dcmDepth/8);
    int totalBytes = nBytes*imagevgl.shape[2];
    
    if(dcmDepth == 8)
       dataset->putAndInsertUint8Array(DCM_PixelData, (Uint8 *)imagevgl.ndarray,totalBytes);
    else
      if(dcmDepth == 16)
	 dataset->putAndInsertUint16Array(DCM_PixelData, (Uint16 *)imagevgl.ndarray, totalBytes);

    if(imagevgl.nChannels == 3)
       dataset->putAndInsertString(DCM_PhotometricInterpretation, "RGB");

    DcmXfer original_xfer(dataset->getOriginalXfer());

    OFString sopClass;
    if (fileformat.getMetaInfo()->findAndGetOFString(DCM_MediaStorageSOPClassUID, sopClass).good())
    {
        /* check for DICOMDIR files */
        if (sopClass == UID_MediaStorageDirectoryStorage)
        {
            OFLOG_FATAL(dcmcjpegLogger, "DICOMDIR files (Media Storage Directory Storage SOP Class) cannot be compressed!");
	    return 1;
        }
    }

    DcmXfer opt_oxferSyn(opt_oxfer);

    // create representation parameters for lossy and lossless
    DJ_RPLossless rp_lossless(OFstatic_cast(int, opt_selection_value), OFstatic_cast(int, opt_point_transform));
    DJ_RPLossy rp_lossy(OFstatic_cast(int, opt_quality));

    const DcmRepresentationParameter *rp = &rp_lossy;
    if (lossless)
       rp = &rp_lossless;

    dataset->chooseRepresentation(opt_oxfer, rp);

    fileformat.saveFile(opt_ofname, opt_oxfer, opt_oenctype, opt_oglenc,
            opt_opadenc, OFstatic_cast(Uint32, opt_filepad), OFstatic_cast(Uint32, opt_itempad), EWM_updateMeta);

    DJDecoderRegistration::cleanup();
    DJEncoderRegistration::cleanup();

    return 0;
}

#endif
