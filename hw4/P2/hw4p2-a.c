#include<stdio.h>
#include<stdlib.h>
#include<math.h>

typedef long INT32;
typedef unsigned short int INT16;
typedef unsigned char U_CHAR;

#define UCH(x)	((int) (x))
#define GET_2B(array,offset)  ((INT16) UCH(array[offset]) + \
			       (((INT16) UCH(array[offset+1])) << 8))
#define GET_4B(array,offset)  ((INT32) UCH(array[offset]) + \
			       (((INT32) UCH(array[offset+1])) << 8) + \
			       (((INT32) UCH(array[offset+2])) << 16) + \
			       (((INT32) UCH(array[offset+3])) << 24))
#define FREAD(file,buf,sizeofbuf)  \
  ((size_t) fread((void *) (buf), (size_t) 1, (size_t) (sizeofbuf), (file)))

#define Pi 3.1415926
#define Degree Pi*(5)/180

void set_2B(U_CHAR *array, int offset, INT16 value);
void set_4B(U_CHAR *array, int offset, INT32 value);


int main()
{
   FILE *input_file = 0 ;
   FILE *output_file = 0 ;
   
   U_CHAR bmpfileheader[14] = { 0 } ;
   U_CHAR bmpinfoheader[40] = { 0 } ;

   INT32 FileSize = 0 ;
   INT32 bfOffBits =0 ;
   INT32 headerSize =0 ;
   INT32 biWidth = 0 ;		
   INT32 biHeight = 0 ;
   INT16 biPlanes = 0 ;
   INT16 BitCount = 0 ;
   INT32 biCompression = 0 ;
   INT32 biImageSize = 0;
   INT32 biXPelsPerMeter = 0 ,biYPelsPerMeter = 0 ;
   INT32 biClrUsed = 0 ;
   INT32 biClrImp = 0 ;

   U_CHAR *data, *new_data, color_table[1024];
   int i, j, k;

   /* �}���ɮ� */
   if( ( input_file = fopen("Lines.bmp","rb") ) == NULL ){
      fprintf(stderr,"File can't open.\n");
      exit(0);
   }


   FREAD(input_file,bmpfileheader,14);
   FREAD(input_file,bmpinfoheader,40);

   if (GET_2B(bmpfileheader,0) == 0x4D42) /* 'BM' */
      fprintf(stdout,"BMP file.\n");
   else{
      fprintf(stdout,"Not bmp file.\n");
      exit(0);
   }

   FileSize           =   GET_4B(bmpfileheader,2);
   bfOffBits         =   GET_4B(bmpfileheader,10);
   headerSize      =   GET_4B(bmpinfoheader,0);
   biWidth           =   GET_4B(bmpinfoheader,4);
   biHeight          =   GET_4B(bmpinfoheader,8);
   biPlanes          =   GET_2B(bmpinfoheader,12);
   BitCount          =   GET_2B(bmpinfoheader,14);
   biCompression   =   GET_4B(bmpinfoheader,16);
   biImageSize      =   GET_4B(bmpinfoheader,20);
   biXPelsPerMeter =   GET_4B(bmpinfoheader,24);
   biYPelsPerMeter =   GET_4B(bmpinfoheader,28);
   biClrUsed         =   GET_4B(bmpinfoheader,32);
   biClrImp          =   GET_4B(bmpinfoheader,36);

   printf("FileSize = %ld \n"
	"DataOffset = %ld \n"
           "HeaderSize = %ld \n"
	"Width = %ld \n"
	"Height = %ld \n"
	"Planes = %d \n"
	"BitCount = %d \n"
	"Compression = %ld \n"
	"ImageSize = %ld \n"
	"XpixelsPerM = %ld \n"
	"YpixelsPerM = %ld \n"
	"ColorsUsed = %ld \n"
	"ColorsImportant = %ld \n",FileSize,bfOffBits,headerSize,biWidth,biHeight,biPlanes,
	BitCount,biCompression,biImageSize,biXPelsPerMeter,biYPelsPerMeter,biClrUsed,biClrImp);

   if (BitCount != 8) {
      fprintf(stderr,"Not a 8-bit file.\n");
      fclose (input_file);
      exit(0);
   }

   FREAD(input_file,color_table,1024);
   //

   data = (U_CHAR *)malloc( biWidth*biHeight );
   if (data == NULL) {
      fprintf(stderr,"Insufficient memory.\n");
      fclose (input_file);
      exit(0);
   }
   
   fseek(input_file,bfOffBits,SEEK_SET);
   FREAD(input_file,data,biWidth*biHeight);

   fclose(input_file);

   new_data = (U_CHAR *)malloc( biWidth*biHeight );
   if (new_data == NULL) {
      fprintf(stderr,"Insufficient memory.\n");
      fclose(input_file);
      exit(0);
   }

   // Process the file
   for (i=0; i < biHeight; i++)
   {
        k = i* ((biWidth*1 +3)/4 *4);
		for (j=0; j < biWidth; j++)
		{
            double midj = (i-biHeight/2);
            double midi = (j-biWidth/2);
            int newi = (midj*cos(Degree) - midi*sin(Degree)) + biHeight/2;
            int newj = (midj*sin(Degree) + midi*cos(Degree)) + biWidth/2;
            if(newi<0 || newj<0 || newi>biHeight || newj>biWidth)
              continue;
            int newk = newi*((biWidth +3)/4 *4) + newj;
            
            new_data[newk] = data[k];

            k = k+1;
		}
   }
   //mutiply (a) and original
   for (i=0; i < biHeight; i++)
   {
        k = i* ((biWidth*1 +3)/4 *4);
		for (j=0; j < biWidth; j++)
		{
      int gray = new_data[k] * data[k];
      if(gray > 255)
        gray = 255;
      else if(gray < 0)
        gray = 0;
      new_data[k] = gray;
      k = k+1;
    }
   }
   
   /* �}�ҷs�ɮ� */
   if( ( output_file = fopen("hw4p2-b.bmp","wb") ) == NULL ){
      fprintf(stderr,"Output file can't open.\n");
      exit(0);
   }

   fwrite(bmpfileheader, sizeof(bmpfileheader), 1, output_file);
   fwrite(bmpinfoheader, sizeof(bmpinfoheader), 1, output_file);

   fwrite(color_table, 1024, 1, output_file);
 
   fwrite(new_data, biWidth*biHeight, 1, output_file);
 
   fclose (output_file);
   
    puts("Press any key to continue");
    fgetc(stdin);
   return 0;
}

void set_2B(U_CHAR *array, int offset, INT16 value)
{
	array[offset] = (U_CHAR) (value % 256);
	array[offset+1] = (U_CHAR) (value >> 8);
}

void set_4B(U_CHAR *array, int offset, INT32 value)
{
	INT32 i;
	i = value;
	array[offset] = (U_CHAR) (i % 256);
	i = i >> 8;
	array[offset+1] = (U_CHAR) (i % 256);
	i = i >> 8;
	array[offset+2] = (U_CHAR) (i % 256);
	i = i >> 8;
	array[offset+3] = (U_CHAR) (i % 256);
}
