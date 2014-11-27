#ifdef __DCMTK__

#include <vglDcmtkIo.h>
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


/** \brief Convert depth from dcm's format to vgl's format.
  */

int convertDepthDcmtkToVgl(int dcmDepth)
{
  if(dcmDepth <= 8)
     return IPL_DEPTH_8U;
  else if(dcmDepth <= 16)
     return IPL_DEPTH_16U;
  else if(dcmDepth <= 32)
     return IPL_DEPTH_32S;
}

/** \brief Convert depth from vlg's format to dcm's format.
  */

int convertDepthVglToDcmtk(int vglDepth)
{
  if(vglDepth == IPL_DEPTH_8U)
     return 8;
  else if(vglDepth <= IPL_DEPTH_16U)
     return 16;
  else if(vglDepth <= IPL_DEPTH_32S)
     return 32;
}

/** Function for create DICOM Header with DCMTK library
  */

int vglCreateHeaderDcmtk(VglImage* imagevgl, DcmFileFormat *fileformat)
{
    
    DcmDataset *dataset = fileformat->getDataset();
    
    char* columns = (char*)malloc(10);
    char* rows    = (char*)malloc(10);
    char* frames  = (char*)malloc(10);
    char* depth   = (char*)malloc(10);
    char* highbit = (char*)malloc(10);
    int i, dcmDepth =  convertDepthDcmtkToVgl(imagevgl->depth);
    int hbit = dcmDepth-1;
    i = sprintf(columns, "%d", imagevgl->shape[VGL_WIDTH]); // width/columns
    i = sprintf(rows, "%d", imagevgl->shape[VGL_HEIGHT]);    // height/rows
    i = sprintf(frames, "%d", imagevgl->shape[VGL_LENGTH]);  // number of Frames
    i = sprintf(depth, "%d", dcmDepth);
    i = sprintf(highbit, "%d", hbit);

    char* samplesPerPixel;
    char* photometricInterpretation;
    if(imagevgl->nChannels == 1)
    {
      samplesPerPixel = (char*) "1";
      photometricInterpretation = (char*) "MONOCHROME2";
    }
    else
    {
      samplesPerPixel = (char*) "3";
      photometricInterpretation = (char*) "RGB";
    }   
    
    dataset->putAndInsertString(DCM_SamplesPerPixel, samplesPerPixel);
    dataset->putAndInsertString(DCM_PhotometricInterpretation, photometricInterpretation);
    dataset->putAndInsertString(DCM_Rows, rows);
    dataset->putAndInsertString(DCM_Columns, columns);
    dataset->putAndInsertString(DCM_NumberOfFrames, frames);
    dataset->putAndInsertString(DCM_BitsAllocated, depth);
    dataset->putAndInsertString(DCM_BitsStored, depth);
    dataset->putAndInsertString(DCM_HighBit, highbit);
    dataset->putAndInsertString(DCM_PixelRepresentation, "0000H");
    dataset->putAndInsertString(DCM_PlanarConfiguration, "0"); 
    
  return 0;
}


/** Function for loading DICOM images with DCMTK library
  */

VglImage* vglDcmtkLoadDicom(char* inFilename)
{
    E_FileReadMode      opt_readMode = ERM_autoDetect;    /* default: fileformat or dataset */
     /* Other values to opt_readMode:
      - to file, opt_readMode = ERM_fileOnly;
      - to dataset, opt_readMode = ERM_dataset;
     */
    E_TransferSyntax    opt_transferSyntax = EXS_Unknown; /* default: xfer syntax recognition */
    /* Others possible values to opt_transferSyntax:
       opt_transferSyntax = EXS_Unknown;
       opt_transferSyntax = EXS_LittleEndianExplicit;
       opt_transferSyntax = EXS_BigEndianExplicit;
       opt_transferSyntax = EXS_LittleEndianImplicit
    */
    unsigned long       opt_compatibilityMode = CIF_MayDetachPixelData | CIF_TakeOverExternalDataset;
    /* Others types of values what can be added to opt_compatibilityMode:
       - to accept acr-nema opt_compatibilityMode |= CIF_AcrNemaCompatibility;
       - to accept palettes opt_compatibilityMode |= CIF_WrongPaletteAttributeTags;
       - to check lut-depth opt_compatibilityMode |= CIF_CheckLutBitDepth;
       - to ignore ignorar mlut-depth opt_compatibilityMode |= CIF_IgnoreModalityLutBitDepth;
    */
    OFCmdUnsignedInt    opt_frame = 1;                    /* default: first frame */
    OFCmdUnsignedInt    opt_frameCount = 0;               /* default: all frames */
    int                 opt_multiFrame = 1;               /* default: multiframes // opt_multifFrame = 0; no multiframes */
  

    // JPEG parameters
    E_DecompressionColorSpaceConversion opt_decompCSconversion = EDC_photometricInterpretation; // conv photometric
    /* Other values to opt_decompCSconversion:
       - to conv lossy, opt_decompCSconversion = EDC_lossyOnly;
       - to conv guess, opt_decompCSconversion = EDC_guess;
       - to conv guess lossy, opt_decompCSconversion = EDC_guessLossyOnly;
       - to conv always, opt_decompCSconversion = EDC_always;
       - to conv never, opt_decompCSconversion = EDC_never;
    */
    OFCmdUnsignedInt    opt_fileBits = 0;

    // register JPEG decompression codecs
    DJDecoderRegistration::registerCodecs(opt_decompCSconversion);

    DcmFileFormat *dfile = new DcmFileFormat();
    OFCondition cond = dfile->loadFile(inFilename, opt_transferSyntax, EGL_withoutGL, DCM_MaxReadLength, opt_readMode);

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

    VglImage* imagevgl; 

    int width  = di->getWidth();
    int height = di->getHeight();
    int layers = di->getFrameCount();
    int depth  = (di->getDepth()/8)*8;             // bits per pixel
    int iplDepth = convertDepthDcmtkToVgl(depth);  // depth \in {IPL_DEPTH_8U, ...}
    char* filename = (char *) malloc(strlen(inFilename)+1);
    strcpy(filename, inFilename);
    
    int nChannels = 1;
    if(!di->isMonochrome())
       nChannels = 3;

    imagevgl = vglCreate3dImage(cvSize(width,height), iplDepth, nChannels, layers);
    imagevgl->filename = filename;

    int pixelsPerFrame = imagevgl->shape[VGL_WIDTH]*imagevgl->shape[VGL_HEIGHT]*imagevgl->nChannels;
    int bytesPerFrame = pixelsPerFrame*(depth/8);
    int totalBytes = bytesPerFrame*imagevgl->shape[VGL_LENGTH];

    printf("%s:%s: dims = [%d, %d, %d], nchannels = %d, bytes/pix = %d, totalBytes = %d\n", __FILE__, __FUNCTION__, width, height, layers, nChannels, depth/8, totalBytes);
	
    //imagevgl->ndarray = (void *) malloc(totalBytes);
    
    int j = 0;
    for (int frame = 0; frame < fcount; frame++)
    {
      void *pixelData = (void *)(di->getOutputData(depth, frame));
      memcpy(((char*)imagevgl->ndarray)+j, pixelData, bytesPerFrame);  
      j += bytesPerFrame; 
    }

    delete di;

    // deregister JPEG decompression codecs
    DJDecoderRegistration::cleanup();

    vglSetContext(imagevgl, VGL_RAM_CONTEXT);
    return imagevgl; 
}


/** Function for saving DICOM images with DCMTK library
  */

int vglDcmtkSaveDicom(VglImage* imagevgl, char* opt_ofname, int compress)
{
    if ( (imagevgl->nChannels != 1) && (imagevgl->nChannels != 3) )
    {
        fprintf(stderr, "%s: %s: Error: image has %d channels but only 1 or 3 channels supported. Use vglImage4to3Channels function before saving\n", __FILE__, __FUNCTION__);
        return 1;
    }

#define OFFIS_CONSOLE_APPLICATION "dcmcjpeg"

    static OFLogger dcmcjpegLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

#ifdef HAVE_GUSI_H
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

    E_FileReadMode opt_readMode = ERM_autoDetect; // default: fileformat or dataset
    /* Other values to opt_readMode:
      - to file, opt_readMode = ERM_fileOnly;
      - to dataset, opt_readMode = ERM_dataset;
    */
    E_TransferSyntax opt_ixfer = EXS_Unknown;
    /* Other values to opt_ixfer:
         opt_ixfer = EXS_Unknown;
         opt_ixfer = EXS_LittleEndianExplicit;
         opt_ixfer = EXS_BigEndianExplicit;
         opt_ixfer = EXS_LittleEndianImplicit;	 
    */
    E_GrpLenEncoding opt_oglenc = EGL_recalcGL; // group length recalc
    /* Other values to opt_oglenc:
       - to group length create, opt_oglenc = EGL_withGL;
       - to group length remove, opt_oglenc = EGL_withoutGL;
    */
    E_EncodingType opt_oenctype = EET_ExplicitLength; // length explicit
    /* Other value, to length undefined, opt_oenctype = EET_UndefinedLength; */
    E_PaddingEncoding opt_opadenc = EPD_noChange; // padding retain
    /* Other values to opt_opadenc:
       - to padding off, opt_opadenc = EPD_withoutPadding;
       - to padding create, opt_opadenc = EPD_withPadding;
    */
    OFCmdUnsignedInt opt_filepad = 0;
    OFCmdUnsignedInt opt_itempad = 0;
    OFBool opt_acceptWrongPaletteTags = OFFalse;
    OFBool opt_acrNemaCompatibility = OFFalse;
    
    // JPEG options
    E_TransferSyntax opt_oxfer =  EXS_JPEGProcess1; // value to baseline (lossy) mode
    /* Other values to opt_oxfer:
       - to lossless, selection value 1, opt_oxfer = EXS_JPEGProcess14SV1;
       - to lossless with any selection value lossless, opt_oxfer = EXS_JPEGProcess14;
       - to extended sequential (lossy, 8/12 bit), opt_oxfer = EXS_JPEGProcess2_4;
       - to spectral selection, non-hierarchical (lossy, 8/12 bit), opt_oxfer = EXS_JPEGProcess6_8;
       - to full progression, non-hierarchical (lossy, 8/12 bit), opt_oxfer = EXS_JPEGProcess10_12;
       - for more values access the file: dcxfer.h
    */ 
    OFCmdUnsignedInt opt_selection_value = 6;
    OFCmdUnsignedInt opt_point_transform = 0;
    OFCmdUnsignedInt opt_quality = 90;
    OFBool           opt_huffmanOptimize = OFTrue;
    OFCmdUnsignedInt opt_smoothing = 0;
    int              opt_compressedBits = 0; // 0=auto, 8/12/16=force
    E_CompressionColorSpaceConversion opt_compCSconversion = ECC_lossyRGB; // cor rgb
    /* Other values to opt_compCSconversion:
       - for color ybr, opt_compCSconversion = ECC_lossyYCbCr;
       - for color monochrome, opt_compCSconversion = ECC_monochrome;
    */
    E_DecompressionColorSpaceConversion opt_decompCSconversion = EDC_photometricInterpretation; // conv photometric
    /* Other values to opt_decompCSconversion:
       - to conv lossy, opt_decompCSconversion = EDC_lossyOnly;
       - to conv guess, opt_decompCSconversion = EDC_guess;
       - to conv guess lossy, opt_decompCSconversion = EDC_guessLossyOnly;
       - to conv always, opt_decompCSconversion = EDC_always;
       - to conv never, opt_decompCSconversion = EDC_never;   
    */
    E_SubSampling    opt_sampleFactors = ESS_444; 
    /* Other values to opt_sampleFactors: ESS_422; ESS_411*/
    OFBool           opt_useYBR422 = OFFalse;
    OFCmdUnsignedInt opt_fragmentSize = 0; // 0=unlimited
    OFBool           opt_createOffsetTable = OFTrue;
    int              opt_windowType = 0;  /* default: no windowing; 1=Wi, 2=Wl, 3=Wm, 4=Wh, 5=Ww, 6=Wn, 7=Wr */
    OFCmdUnsignedInt opt_windowParameter = 0;
    OFCmdFloat       opt_windowCenter=0.0, opt_windowWidth=0.0;
    E_UIDCreation    opt_uidcreation = EUC_default; // uid default
    /* Other values to opt_uidcreation:
       - to uid always, opt_uidcreation = EUC_always;
       - to uid never, opt_uidcreation = EUC_never;
    */
    OFBool           opt_secondarycapture = OFFalse;
    OFCmdUnsignedInt opt_roiLeft = 0, opt_roiTop = 0, opt_roiWidth = 0, opt_roiHeight = 0;
    OFBool           opt_usePixelValues = OFTrue;
    OFBool           opt_useModalityRescale = OFFalse;
    OFBool           opt_trueLossless = OFTrue;
    OFBool           lossless = OFTrue;  /* see opt_oxfer */
    

    if(compress == 1)
    {
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
    }

    DcmFileFormat *fileformat = new DcmFileFormat();
    if(!imagevgl->filename)
      int r = vglCreateHeaderDcmtk(imagevgl, fileformat);
    else
      OFCondition error = fileformat->loadFile(imagevgl->filename, opt_ixfer, EGL_noChange, DCM_MaxReadLength, opt_readMode);

    DcmDataset *dataset = fileformat->getDataset();
    
    int nPixels = imagevgl->shape[VGL_WIDTH]*imagevgl->shape[VGL_HEIGHT]*imagevgl->nChannels;
    int dcmDepth = convertDepthVglToDcmtk(imagevgl->depth); 
    int totalPixels = imagevgl->shape[VGL_WIDTH]*imagevgl->shape[VGL_HEIGHT]*imagevgl->shape[VGL_LENGTH]*imagevgl->nChannels;
    
    if(dcmDepth == 8)
       dataset->putAndInsertUint8Array(DCM_PixelData, (Uint8 *)imagevgl->ndarray, totalPixels);
    else
      if(dcmDepth == 16)
	 dataset->putAndInsertUint16Array(DCM_PixelData, (Uint16 *)imagevgl->ndarray, totalPixels);

    if(imagevgl->nChannels == 3)
       dataset->putAndInsertString(DCM_PhotometricInterpretation, "RGB");

    if(compress == 1)
    {
      DcmXfer original_xfer(dataset->getOriginalXfer());

      OFString sopClass;
      if (fileformat->getMetaInfo()->findAndGetOFString(DCM_MediaStorageSOPClassUID, sopClass).good())
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
      if(lossless)
	rp = &rp_lossless;

      dataset->chooseRepresentation(opt_oxfer, rp);

      fileformat->saveFile(opt_ofname, opt_oxfer, opt_oenctype, opt_oglenc,
            opt_opadenc, OFstatic_cast(Uint32, opt_filepad), OFstatic_cast(Uint32, opt_itempad), EWM_updateMeta);

      DJDecoderRegistration::cleanup();
      DJEncoderRegistration::cleanup();
    }
    else
      fileformat->saveFile(opt_ofname, EXS_LittleEndianExplicit);

    return 0;
}

/** Function for saving uncompressed DICOM images with DCMTK library
  */

int vglDcmtkSaveDicomUncompressed(VglImage* imagevgl, char* outfilename)
{
    if ( (imagevgl->nChannels != 1) && (imagevgl->nChannels != 3) )
    {
        fprintf(stderr, "%s: %s: Error: image has %d channels but only 1 or 3 channels supported. Use vglImage4to3Channels function before saving\n", __FILE__, __FUNCTION__);
        return 1;
    }

    int compressionMode = 0;
    int r = vglDcmtkSaveDicom(imagevgl, outfilename, compressionMode);
    return r;
}


/** Function for saving compressed DICOM images with DCMTK library
  */


int vglDcmtkSaveDicomCompressed(VglImage* imagevgl, char* outfilename)
{
    if ( (imagevgl->nChannels != 1) && (imagevgl->nChannels != 3) )
    {
        fprintf(stderr, "%s: %s: Error: image has %d channels but only 1 or 3 channels supported. Use vglImage4to3Channels function before saving\n", __FILE__, __FUNCTION__);
        return 1;
    }
    int compressionMode = 1;
    int r = vglDcmtkSaveDicom(imagevgl, outfilename, compressionMode);
    return r;
}

#endif
