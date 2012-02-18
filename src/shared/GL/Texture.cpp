#include "screen.h"




void *LoadDIBitmap(char	*filename,	/* I - File to load */
             BITMAPINFO	**info)		/* O - Bitmap information */
{
  FILE			*fp;		/* Open file pointer */
  void			*bits;		/* Bitmap pixel bits */
  long			bitsize,	/* Size of bitmap */
			infosize;	/* Size of header information */
  BITMAPFILEHEADER	header;		/* File header */


 /*
  * Try opening the file; use "rb" mode to read this *binary* file.
  */

  if ((fp = fopen(filename, "rb")) == NULL)
    return (NULL);

 /*
  * Read the file header and any following bitmap information...
  */

  if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1)
  {
   /*
    * Couldn't read the file header - return NULL...
    */

    fclose(fp);
    return (NULL);
  };

  if (header.bfType != 'MB')	/* Check for BM reversed... */
  {
   /*
    * Not a bitmap file - return NULL...
    */

    fclose(fp);
    return (NULL);
  };

  infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);
  if ((*info = (BITMAPINFO *)malloc(infosize)) == NULL)
  {
   /*
    * Couldn't allocate memory for bitmap info - return NULL...
    */

    fclose(fp);
    return (NULL);
  };

  if (fread(*info, 1, infosize, fp) < infosize)
  {
   /*
    * Couldn't read the bitmap header - return NULL...
    */

    free(*info);
    fclose(fp);
    return (NULL);
  };

 /*
  * Now that we have all the header info read in, allocate memory for the
  * bitmap and read *it* in...
  */

  if ((bitsize = (*info)->bmiHeader.biSizeImage) == 0)
    bitsize = ((*info)->bmiHeader.biWidth *
               (*info)->bmiHeader.biBitCount + 7) / 8 *
  	      abs((*info)->bmiHeader.biHeight);

  if ((bits = malloc(bitsize)) == NULL)
  {
   /*
    * Couldn't allocate memory - return NULL!
    */

    free(*info);
    fclose(fp);
    return (NULL);
  };

  if (fread(bits, 1, bitsize, fp) < bitsize)
  {
   /*
    * Couldn't read bitmap - free memory and return NULL!
    */

    free(*info);
    free(bits);
    fclose(fp);
    return (NULL);
  };

 /*
  * OK, everything went fine - return the allocated bitmap...
  */

  fclose(fp);
  return (bits);
}


GLubyte *ConvertRGB(BITMAPINFO *info,  void *bits)
{
	int		i, j,			/* Looping vars */
  			bitsize,		/* Total size of bitmap */
			width;			/* Aligned width of bitmap */
	GLubyte	*newbits;		/* New RGB bits */
	GLubyte	*from, *to;		/* RGB looping vars */



/*
 * Allocate memory for the RGB bitmap...
 */

	width   = 3 * info->bmiHeader.biWidth;
	width   = (width + 3) & ~3;	
	bitsize = width * info->bmiHeader.biHeight;
	if ((newbits = (GLubyte *)calloc(bitsize, 1)) == NULL)
		return (NULL);

 /*
  * Copy the original bitmap to the new array, converting as necessary.
  */

	switch (info->bmiHeader.biCompression)
	{
		case BI_RGB :
		if (info->bmiHeader.biBitCount == 24)
		{
			 /*
			  * Swap red & blue in a 24-bit image...
			  */

			for (i = 0; i < info->bmiHeader.biHeight; i ++)
				for (j = 0, from = ((GLubyte *)bits) + i * width,
						 to = newbits + i * width;
				 j < info->bmiHeader.biWidth;
				 j ++, from += 3, to += 3)
				{
					  to[0] = from[2];
					  to[1] = from[1];
					  to[2] = from[0];
				};
		};
		break;
		case BI_RLE4 :
		case BI_RLE8 :
		case BI_BITFIELDS :
			break;
	};

	return (newbits);
}



S3DLIBRESULT Texture::LoadTexture(Screen3D& Screen, char *filename, char* alphamask)
{


	//clear up any memory if this texture was used before (ie, we're reloading a different texture).
	Destroy();



	//load the bitmap
		BITMAPINFO	*info;				/* Bitmap information */
	void		*bits;				/* Bitmap pixel bits */
	GLubyte	    *rgb;				/* Bitmap RGB pixels */


	//load in texture
	bits = LoadDIBitmap(filename, &info);
	if (bits == NULL)
		return S3DLIB_ERR;

	rgb = ConvertRGB(info, bits);
	if (rgb == NULL)
	{
	    free(info);
	    free(bits);

	    return S3DLIB_ERR;
	};

	
 
	//define texture image
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);	/* Force 4-byte alignment */
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

	//generate an ID for the bitmap
	glGenTextures(1, &TextureSurface);

	//load the texture into opengl
	glBindTexture(GL_TEXTURE_2D, TextureSurface);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, info->bmiHeader.biWidth, info->bmiHeader.biHeight, GL_RGB, GL_UNSIGNED_BYTE, rgb);





	strcpy(TextureFile, filename);
	Initialized = true;
		
	return S3DLIB_OK;
}


void Texture::Destroy() 
{ 
	if(Initialized)
		glDeleteTextures(1, &TextureSurface);

	Initialized = false;


}





