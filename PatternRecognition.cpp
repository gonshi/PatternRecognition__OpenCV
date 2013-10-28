#include "cv.h"
#include "highgui.h"
#include<stdio.h>
#include<cxcore.h>
#include <iostream>
using namespace std;
double x,y,xsize,ysize,x2,y2;
int x_1,x_2,y_1,y_2,x_3,y_3,player[9][9],turn,stagepixel,start=0;  //player,turn�͋�͂ǂ���̃v���C���[�̂��̂��Ƃ������,
												//start�͊J�n���݈̂���;
char *masu[9][9],filename[20],filename2[20];

IplImage *colorbasicgroundImage=NULL;



#define THRESHOLD 40
#define THRESHOLD_MAX_VALUE 255
#define SCALE (1.0/255.0)

CvSize imageSize;
void interpolate(IplImage *polate,IplImage *temp);
void current(void);
void print(void);
void ura(int n);
IplImage* roiImg = NULL;
IplImage* roiImg1 = NULL;
unsigned int Get_Ssd(IplImage *imgA,IplImage *imgB);

//���C�u�����ǂݍ���
#pragma comment(lib,"cv.lib")
#pragma comment(lib,"cxcore.lib")
#pragma comment(lib,"highgui.lib")

//���x�����O�N���X
//http://chihara.naist.jp/people/STAFF/imura/products/labeling
#include "Labeling.h"


//==============================================================================
//�O���[�o���ϐ�
//==============================================================================
IplImage *image;
short *dst;
LabelingBS labeling;




//------------------------------------------------------------------------------
//IplImage�̔C�Ӄs�N�Z���ɃA�N�Z�X���邽�߂̊֐�
//------------------------------------------------------------------------------
inline unsigned char getPixel(IplImage *image, int x, int y, int index)
{
    return (uchar)image->imageData[y*image->widthStep + x*image->nChannels + index];
}


//------------------------------------------------------------------------------
//�}�E�X�R�[���o�b�N
//------------------------------------------------------------------------------
void onMouse(int event, int x, int y, int flags)
{
    switch (event) {
        //�N���b�N�ʒu�E���摜�̒l�E���x���i���o�[���o��
        case CV_EVENT_LBUTTONUP:
            cout << " x: " << x << ", y: " << y << endl;
            cout << "\tValue: " << (int)getPixel(image, x, y, 0) << endl;
            cout << "\tLabel: " << dst[y*image->width+x] << endl;
            break;
        default:
            break;
    }
}

//------------------------------------------------------------------------------
//���C��
//------------------------------------------------------------------------------
int main()
{ 
	int i,j,k;
	char key;

	char windowNameCurrent[]="Current";
	char windowNameResult[]="Result";
	char windowNameBackground[]="Background";

	colorbasicgroundImage=cvLoadImage("1.bmp",1);

	IplImage *frameImage=NULL;
	IplImage *frameImage2=NULL;
	IplImage *currentImage=NULL;
	IplImage *resultImage=NULL;
	IplImage *temp=NULL;
	IplImage *backgroundImage=NULL;
	
	// ����̏�����
	for(j=0;j<=8;j++){
		for(k=0;k<=8;k++){
			masu[j][k]="      ";
			player[j][k]=2;
		}
	}
	masu[0][0]=masu[0][8]="(����)";
	masu[0][1]=masu[0][7]="(�j�n)";
	masu[0][2]=masu[0][6]="(�⏫)";
	masu[0][3]=masu[0][5]="(����)";
	masu[0][4]="(�ʏ�)";
	masu[1][1]="(���)";
	masu[1][7]="(�p�s)";
	for(i=0;i<9;i++)masu[2][i]="(����)";
	for(i=0;i<9;i++){
		player[0][i]=1;
		player[2][i]=1;		//������player1�Ƃ���
	}
	player[1][1]=player[1][7]=1;


	masu[8][0]=masu[8][8]=" ���� ";
	masu[8][1]=masu[8][7]=" �j�n ";
	masu[8][2]=masu[8][6]=" �⏫ ";
	masu[8][3]=masu[8][5]=" ���� ";
	masu[8][4]=" ���� ";
	masu[7][1]=" �p�s ";
	masu[7][7]=" ��� ";
	for(i=0;i<9;i++)masu[6][i]=" ���� ";	
	for(i=0;i<9;i++){
		player[8][i]=0;
		player[6][i]=0;		//��O����player0�Ƃ���
	}
	player[7][1]=player[7][7]=0;
	//
	printf("  +------+------+------+------+------+------+------+------+------+\n  ");
	for(j=0;j<9;j++)printf("|%s",masu[0][j]);printf("|\n");
	printf("  +------+------+------+------+------+------+------+------+------+\n  ");
	for(j=0;j<9;j++)printf("|%s",masu[1][j]);printf("|\n");
	printf("  +------+------+------+------+------+------+------+------+------+\n  ");
	for(j=0;j<9;j++)printf("|%s",masu[2][j]);printf("|\n");
	printf("  +------+------+------+------+------+------+------+------+------+\n  ");
	for(j=0;j<9;j++)printf("|%s",masu[3][j]);printf("|\n");
	printf("  +------+------+------+------+------+------+------+------+------+\n  ");
	for(j=0;j<9;j++)printf("|%s",masu[4][j]);printf("|\n");
	printf("  +------+------+------+------+------+------+------+------+------+\n  ");
	for(j=0;j<9;j++)printf("|%s",masu[5][j]);printf("|\n");
	printf("  +------+------+------+------+------+------+------+------+------+\n  ");
	for(j=0;j<9;j++)printf("|%s",masu[6][j]);printf("|\n");
	printf("  +------+------+------+------+------+------+------+------+------+\n  ");
	for(j=0;j<9;j++)printf("|%s",masu[7][j]);printf("|\n");
	printf("  +------+------+------+------+------+------+------+------+------+\n  ");
	for(j=0;j<9;j++)printf("|%s",masu[8][j]);printf("|\n");
	printf("  +------+------+------+------+------+------+------+------+------+\n\n\n\n");		//�����Ղ̕\��



	cvNamedWindow(windowNameCurrent,CV_WINDOW_AUTOSIZE);
	cvNamedWindow(windowNameResult,CV_WINDOW_AUTOSIZE);
	cvNamedWindow(windowNameBackground,CV_WINDOW_AUTOSIZE);

	
    char *window_name = "Labeling Sample";

    //cvNamedWindow(window_name);

    // �R�[���o�b�N�֐����Z�b�g
    cvSetMouseCallback(window_name, (CvMouseCallback)onMouse);

    //�摜�̓ǂݍ��݁E��l��
    image = cvLoadImage("1.bmp", 0);  // 0: �O���C�X�P�[���œǂݍ���
    //cvShowImage(window_name, image);
    //cvWaitKey(-1);
	cvSmooth (image, image, CV_GAUSSIAN, 7);
    //cvShowImage(window_name, image);
    //cvWaitKey(-1);
	
    cvThreshold(image, image, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU); 
    //cvShowImage(window_name, image);
    //cvWaitKey(-1);
	cvNot(image, image); //�F�̔��]
    //cvShowImage(window_name, image);
    //cvWaitKey(-1);

    //���x�����O�����̌��ʕۑ��p�z��
    dst = new short[ image->width * image->height ];

    //���x�����O���s
    labeling.Exec((uchar *)image->imageData, dst, image->width, image->height, true, 10);
    //------------------------------------------------------------------------------
    //int Exec( SrcT *target, DstT *result, int target_width, int target_height,
    //          const bool is_sort_region, const int region_size_min )
    //����:

    //  SrcT *target ���̓o�b�t�@
    //  DstT *result �o�̓o�b�t�@
    //  int target_width �o�b�t�@�̉��T�C�Y
    //  int target_height �o�b�t�@�̏c�T�C�Y
    //  bool is_sort_region �A���̈��傫���̍~���Ƀ\�[�g���邩
    //  int region_size_min �ŏ��̗̈�T�C�Y(���ꖢ���͖�������)
    //------------------------------------------------------------------------------


	
	RegionInfoBS *ri = labeling.GetResultRegionInfo( 0 ); //��ԑ傫�����x���̏����K������

	stagepixel=ri->GetNumOfPixels();


	int size_x1, size_y1,size_x2, size_y2;

	

	ri->GetMin( size_x1, size_y1 );
	ri->GetMax( size_x2, size_y2 ); //����̍��W�ƉE���̍��W���擾

	//printf("%d %d,%d %d\n",size_x1, size_y1,size_x2, size_y2);
	x=(size_x2-size_x1)/29.4+size_x1;
	x2=size_x2-(size_x2-size_x1)/29.4;
	y=(size_y2-size_y1)/33.0+size_y1;
	y2=size_y2-(size_y2-size_y1)/33.0;
	xsize=(x2-x)/9;
	ysize=(y2-y)/9; //�}�X�̏c���Ɖ������Z�o
	//printf("%lf\n",xsize);
	//printf("%lf\n",ysize);
	//printf("%lf\n",x);
	//printf("%lf\n",y);


    //�摜��\���E�L�[���͑҂�
    //cvShowImage(window_name, image);
    cvWaitKey(-1);

    
	for(i=1;i<=69;i++){//�摜���J��Ԃ��ǂݍ���
		printf("%d���\n\n",i);
		sprintf(filename,"%d.bmp",i);
		frameImage=cvLoadImage(filename,0);
		backgroundImage=cvCloneImage(frameImage);
		imageSize=cvSize(frameImage->width,frameImage->height);

		
		sprintf(filename2,"%d.bmp",i+1);
		frameImage2=cvLoadImage(filename2,0);
		currentImage=cvCloneImage(frameImage2);
		temp=cvCreateImage(imageSize,IPL_DEPTH_8U,1);
		resultImage=cvCreateImage(imageSize,IPL_DEPTH_8U,1);
		cvAbsDiff(frameImage,frameImage2,resultImage);
		//cvShowImage(windowNameResult,resultImage);
		//cvWaitKey(0);
		cvThreshold(resultImage,resultImage,THRESHOLD,THRESHOLD_MAX_VALUE,CV_THRESH_BINARY);
		cvShowImage(windowNameCurrent,currentImage);
		cvShowImage(windowNameResult,resultImage);
		cvShowImage(windowNameBackground,backgroundImage);
		//cvWaitKey(0);
		interpolate(resultImage,temp);	
		
		print();

		key=cvWaitKey(0);
		if(key=='c') current();
		else if(key=='t') turn++;
	}

	


	cvReleaseImage(&backgroundImage);
	cvReleaseImage(&currentImage);
	cvReleaseImage(&resultImage);
	cvDestroyWindow(windowNameCurrent);
	cvDestroyWindow(windowNameResult);
	cvDestroyWindow(windowNameBackground);

	return 0;
	//�I������
    delete dst;
    cvReleaseImage(&image);
    cvDestroyAllWindows();

}



//�����̈��⊮����

void interpolate(IplImage *polate,IplImage *temp){

	double min_val;

	IplImage* outImg = NULL;
	int minx,miny,maxx,maxy,pixel,i=0;
	float xcenter1,ycenter1,xcenter2,ycenter2;
	char windowNameResult[]="Result";
	//cvNamedWindow("test", CV_WINDOW_AUTOSIZE);
	//cvNamedWindow("test2", CV_WINDOW_AUTOSIZE);
	CvPoint minLocation;	// ����x���ŏ��ɂȂ�ꏊ



	IplImage *colorbackgroundImage=NULL;
	colorbackgroundImage=cvLoadImage(filename2,1);

	
	
	cvDilate(polate,temp,NULL,2);
	//cvShowImage(windowNameResult, temp);
    //cvWaitKey(-1);

	cvErode(temp,polate,NULL,2);
	cvErode(polate,temp,NULL,1);
	//cvShowImage(windowNameResult, temp);
    //cvWaitKey(-1);

	
	turn=turn%2;
	cvShowImage(windowNameResult,temp);
	

	labeling.Exec((uchar *)temp->imageData, dst, temp->width, temp->height, true, 30);

	RegionInfoBS *ri = labeling.GetResultRegionInfo( i ); //��ԑ傫�����x���̏����擾����
	ri->GetCenter(xcenter1,ycenter1 );
	//printf("x=%lf y=%lf\n",xcenter1,ycenter1 );
	while(xcenter1<x||xcenter1>x2||ycenter1<y||ycenter1>y2){
		//printf("out\n");
		i++;
		RegionInfoBS *ri = labeling.GetResultRegionInfo( i );
		ri->GetCenter(xcenter1,ycenter1 );
	}
	i++;

	if(labeling.GetNumOfResultRegions()==i){

		ri->GetMin( minx, miny );
		ri->GetMax( maxx, maxy );
		if((maxy-miny)>ysize||(maxx-minx)>xsize){ //���̏����𖞂����Ƃ��C��̈ړ������x�����O�������ʁC
													//�Q�̋�������Ă����ԂƂ݂Ȃ��C
													//��ړ������ꍇ�Ɠ��l�̏������s��

			if((maxy-miny)>ysize&&(maxx-minx)>xsize){
				if(temp->imageData[temp->widthStep * ((int)ycenter1-(int)ysize/2) 
					+ (int)xcenter1-(int)xsize/2]==-1){//���ȂȂ߂��E�ȂȂ߂��̔���
						xcenter2=xcenter1+xsize/2;
						ycenter2=ycenter1+ysize/2;
						xcenter1-=xsize/2;
						ycenter1-=ysize/2;
					}else{
						xcenter2=xcenter1+xsize/2;
						ycenter2=ycenter1-ysize/2;
						xcenter1-=xsize/2;
						ycenter1+=ysize/2;
					}
			}
			else if((maxy-miny)>ysize){
				ycenter2=ycenter1-ysize/2;
				xcenter2=xcenter1;
				ycenter1+=ysize/2;
			}
			else if((maxx-minx)>xsize){
				ycenter2=ycenter1;
				xcenter2=xcenter1+xsize/2;
				xcenter1-=xsize/2;
			}

			x_1=(xcenter1-x)/xsize;
			y_1=(ycenter1-y)/ysize;
			x_2=(xcenter2-x)/xsize;
			y_2=(ycenter2-y)/ysize;
				
			if(start==0){
				if(player[y_1][x_1]!=2)turn=player[y_1][x_1];
				else turn=player[y_2][x_2];
				start=1;
			}	//�ŏ��̃^�[���̂Ƃ��ɐ�s�̎��c������

			if(turn==0&&(y_1<3||y_2<3)){
				//printf("check....\n");
				IplImage *colorbackgroundImage1=NULL;
				colorbackgroundImage1=cvLoadImage(filename,1);
				if(player[y_1][x_1]!=turn){
					//ROI�摜�쐬
					roiImg = cvCreateImage(cvSize(colorbackgroundImage->width,colorbackgroundImage->height),IPL_DEPTH_8U,1);
					roiImg = cvCloneImage(colorbackgroundImage);
					//ROI�w��
					cvSetImageROI(roiImg,cvRect(xcenter1-(xsize/2.7),ycenter1-(ysize/2.7),(int)(xsize),(int)(ysize)));
					//cvShowImage("test", roiImg);					
					IplImage *differenceMapImage=cvCreateImage(cvSize(colorbackgroundImage1->width - (int)(xsize) + 1,
						colorbackgroundImage1->height - (int)(ysize) + 1),IPL_DEPTH_32F,1);
										//����x�}�b�v�摜�pIplImage
					//�e���v���[�g�}�b�`���O���s��
					cvMatchTemplate(colorbackgroundImage1,roiImg,differenceMapImage,CV_TM_SQDIFF);

					//�e���v���[�g�����摜�̂ǂ̕����ɂ���̂��Ƃ������𓾂�
					cvMinMaxLoc(differenceMapImage,&min_val,NULL,&minLocation,NULL,NULL);

					roiImg1 = cvCreateImage(cvSize(colorbackgroundImage->width,colorbackgroundImage->height),IPL_DEPTH_8U,1);
					roiImg1 = cvCloneImage(colorbackgroundImage1);
				cvSetImageROI(roiImg1,cvRect(minLocation.x,minLocation.y,(int)(xsize),(int)(ysize)));
				//cvShowImage("test2", roiImg1);

					x_3=(minLocation.x+xsize/2-x)/xsize;
					y_3=(minLocation.y+ysize/2-y)/ysize;

					if(masu[y_3][x_3]==masu[y_2][x_2]){
						if(Get_Ssd(roiImg,roiImg1)>3000000)
						ura(2);
					}
					else if(masu[y_3][x_3]!=masu[y_2][x_2])
						ura(2);
					cvResetImageROI(roiImg);
					cvResetImageROI(roiImg1);
				}
				else if(player[y_2][x_2]!=turn){
					//ROI�摜�쐬
					roiImg = cvCreateImage(cvSize(colorbackgroundImage->width,colorbackgroundImage->height),IPL_DEPTH_8U,1);
					roiImg = cvCloneImage(colorbackgroundImage);
					//ROI�w��
					cvSetImageROI(roiImg,cvRect(xcenter2-(xsize/2.7),ycenter2-(ysize/2.7),(int)(xsize),(int)(ysize)));
					//cvShowImage("test", roiImg);
					IplImage *differenceMapImage=cvCreateImage(cvSize(colorbackgroundImage1->width - (int)(xsize) + 1,
						colorbackgroundImage1->height - (int)(ysize) + 1),IPL_DEPTH_32F,1);
										//����x�}�b�v�摜�pIplImage
					//�e���v���[�g�}�b�`���O���s��
					cvMatchTemplate(colorbackgroundImage1,roiImg,differenceMapImage,CV_TM_SQDIFF);

					//�e���v���[�g�����摜�̂ǂ̕����ɂ���̂��Ƃ������𓾂�
					cvMinMaxLoc(differenceMapImage,&min_val,NULL,&minLocation,NULL,NULL);

					roiImg1 = cvCreateImage(cvSize(colorbackgroundImage->width,colorbackgroundImage->height),IPL_DEPTH_8U,1);
					roiImg1 = cvCloneImage(colorbackgroundImage1);

				cvSetImageROI(roiImg1,cvRect(minLocation.x,minLocation.y,(int)(xsize),(int)(ysize)));
				//cvShowImage("test2", roiImg1);
					cvResetImageROI(roiImg);

					x_3=(minLocation.x+xsize/2-x)/xsize;
					y_3=(minLocation.y+ysize/2-y)/ysize;


					if(masu[y_3][x_3]==masu[y_1][x_1]){
						if(Get_Ssd(roiImg,roiImg1)>3000000)
						ura(1);
					}
					else if(masu[y_3][x_3]!=masu[y_1][x_1])
						ura(1);
					cvResetImageROI(roiImg);
					cvResetImageROI(roiImg1);
				}
			}
			else if(turn==1&&(y_1>5||y_2>5)){
				//printf("check....\n");
				IplImage *colorbackgroundImage1=NULL;
				colorbackgroundImage1=cvLoadImage(filename,1);

				if(player[y_1][x_1]!=turn){
					//ROI�摜�쐬
					roiImg = cvCreateImage(cvSize(colorbackgroundImage->width,colorbackgroundImage->height),IPL_DEPTH_8U,1);
					roiImg = cvCloneImage(colorbackgroundImage);
					//ROI�w��
					cvSetImageROI(roiImg,cvRect(xcenter1-(xsize/2.7),ycenter1-(ysize/2.7),(int)(xsize),(int)(ysize)));
					//cvShowImage("test", roiImg);
					IplImage *differenceMapImage=cvCreateImage(cvSize(colorbackgroundImage1->width - (int)(xsize) + 1,
						colorbackgroundImage1->height - (int)(ysize) + 1),IPL_DEPTH_32F,1);
										//����x�}�b�v�摜�pIplImage
					//�e���v���[�g�}�b�`���O���s��
					cvMatchTemplate(colorbackgroundImage1,roiImg,differenceMapImage,CV_TM_SQDIFF);

					//�e���v���[�g�����摜�̂ǂ̕����ɂ���̂��Ƃ������𓾂�
					cvMinMaxLoc(differenceMapImage,&min_val,NULL,&minLocation,NULL,NULL);

					roiImg1 = cvCreateImage(cvSize(colorbackgroundImage->width,colorbackgroundImage->height),IPL_DEPTH_8U,1);
					roiImg1 = cvCloneImage(colorbackgroundImage1);
				cvSetImageROI(roiImg1,cvRect(minLocation.x,minLocation.y,(int)(xsize),(int)(ysize)));
				//cvShowImage("test2", roiImg1);
					
					
					x_3=(minLocation.x+xsize/2-x)/xsize;
					y_3=(minLocation.y+ysize/2-y)/ysize;

					if(masu[y_3][x_3]==masu[y_2][x_2]){
						if(Get_Ssd(roiImg,roiImg1)>3000000)
						ura(2);
					}
					else if(masu[y_3][x_3]!=masu[y_2][x_2])
						ura(2);
					cvResetImageROI(roiImg);
					cvResetImageROI(roiImg1);

				}
				else if(player[y_2][x_2]!=turn){
					//ROI�摜�쐬
					roiImg = cvCreateImage(cvSize(colorbackgroundImage->width,colorbackgroundImage->height),IPL_DEPTH_8U,1);
					roiImg = cvCloneImage(colorbackgroundImage);
					//ROI�w��
					cvSetImageROI(roiImg,cvRect(xcenter2-(xsize/2.7),ycenter2-(ysize/2.7),(int)(xsize),(int)(ysize)));
					//cvShowImage("test", roiImg);

					IplImage *differenceMapImage=cvCreateImage(cvSize(colorbackgroundImage1->width - (int)(xsize) + 1,
						colorbackgroundImage1->height - (int)(ysize) + 1),IPL_DEPTH_32F,1);
										//����x�}�b�v�摜�pIplImage
					//�e���v���[�g�}�b�`���O���s��
					cvMatchTemplate(colorbackgroundImage1,roiImg,differenceMapImage,CV_TM_SQDIFF);

					//�e���v���[�g�����摜�̂ǂ̕����ɂ���̂��Ƃ������𓾂�
					cvMinMaxLoc(differenceMapImage,&min_val,NULL,&minLocation,NULL,NULL);

					roiImg1 = cvCreateImage(cvSize(colorbackgroundImage->width,colorbackgroundImage->height),IPL_DEPTH_8U,1);
					roiImg1 = cvCloneImage(colorbackgroundImage1);
				cvSetImageROI(roiImg1,cvRect(minLocation.x,minLocation.y,(int)(xsize),(int)(ysize)));
				//cvShowImage("test2", roiImg1);

					x_3=(minLocation.x+xsize/2-x)/xsize;
					y_3=(minLocation.y+ysize/2-y)/ysize;

					if(masu[y_3][x_3]==masu[y_1][x_1]){
						if(Get_Ssd(roiImg,roiImg1)>3000000)
						ura(1);
					}
					else if(masu[y_3][x_3]!=masu[y_1][x_1])
						ura(1);
					cvResetImageROI(roiImg);
					cvResetImageROI(roiImg1);

				}
			}
			

			if(player[y_1][x_1]==turn){
				if(masu[y_2][x_2]=="(�ʏ�)") printf("Win Player1!!!\n");
				else if(masu[y_2][x_2]==" ���� ") printf("Win Player2!!!\n");
				masu[y_2][x_2]=masu[y_1][x_1];
				masu[y_1][x_1]="      ";
				player[y_2][x_2]=turn;
				player[y_1][x_1]=2;
			}else{
				if(masu[y_1][x_1]=="(�ʏ�)") printf("Win Player1!!!\n");
				else if(masu[y_1][x_1]==" ���� ") printf("Win Player2!!!\n");
				masu[y_1][x_1]=masu[y_2][x_2];
				masu[y_2][x_2]="      ";
				player[y_2][x_2]=2;
				player[y_1][x_1]=turn;
			}
		}
		else{

			//ROI
			
			//ROI�摜�쐬
			roiImg = cvCreateImage(cvSize(colorbackgroundImage->width,colorbackgroundImage->height),IPL_DEPTH_8U,1);
			roiImg = cvCloneImage(colorbackgroundImage);
			//ROI�w��
			cvSetImageROI(roiImg,cvRect(xcenter1-(xsize/2.7),ycenter1-(ysize/2.7),(int)(xsize),(int)(ysize)));
			//cvShowImage("test", roiImg);
			//cvWaitKey(0);
			IplImage *differenceMapImage=cvCreateImage(cvSize(colorbasicgroundImage->width - (int)(xsize) + 1,
				colorbasicgroundImage->height - (int)(ysize) + 1),IPL_DEPTH_32F,1);
								//����x�}�b�v�摜�pIplImage
			//�e���v���[�g�}�b�`���O���s��
			cvMatchTemplate(colorbasicgroundImage,roiImg,differenceMapImage,CV_TM_SQDIFF);

			//�e���v���[�g�����摜�̂ǂ̕����ɂ���̂��Ƃ������𓾂�
			cvMinMaxLoc(differenceMapImage,&min_val,NULL,&minLocation,NULL,NULL);
			
			cvResetImageROI(roiImg);

			x_3=(minLocation.x+xsize/2-x)/xsize;
			y_3=(minLocation.y+ysize/2-y)/ysize;
			x_1=(xcenter1-x)/xsize;
			y_1=(ycenter1-y)/ysize;
			
			
			if(x_3==0&&y_3==0||x_3==8&&y_3==0) masu[y_1][x_1]="(����)";
			else if(x_3==1&&y_3==0||x_3==8&&y_3==0) masu[y_1][x_1]="(�j�n)";
			else if(x_3==2&&y_3==0||x_3==6&&y_3==0) masu[y_1][x_1]="(�⏫)";
			else if(x_3==3&&y_3==0||x_3==5&&y_3==0) masu[y_1][x_1]="(����)";
			else if(x_3==4&&y_3==0) masu[y_1][x_1]="(�ʏ�)";
			else if(x_3==7&&y_3==1) masu[y_1][x_1]="(�p�s)";
			else if(x_3==1&&y_3==1) masu[y_1][x_1]="(���)";
			else if(y_3==2) masu[y_1][x_1]="(����)";
			else if(x_3==0&&y_3==8||x_3==8&&y_3==8) masu[y_1][x_1]=" ���� ";
			else if(x_3==1&&y_3==8||x_3==7&&y_3==8) masu[y_1][x_1]=" �j�n ";
			else if(x_3==2&&y_3==8||x_3==6&&y_3==8) masu[y_1][x_1]=" �⏫ ";
			else if(x_3==3&&y_3==8||x_3==5&&y_3==8) masu[y_1][x_1]=" ���� ";
			else if(x_3==4&&y_3==8) masu[y_1][x_1]=" ���� ";
			else if(x_3==7&&y_3==7) masu[y_1][x_1]=" ��� ";
			else if(x_3==1&&y_3==7) masu[y_1][x_1]=" �p�s ";
			else if(y_3==6) masu[y_1][x_1]=" ���� ";
			else printf("error\n%d %d",x_3,y_3);

			player[y_1][x_1]=turn;
			
		}
	}

	if(labeling.GetNumOfResultRegions()>i){
		
		x_1=(xcenter1-x)/xsize;
		y_1=(ycenter1-y)/ysize;

		RegionInfoBS *ri = labeling.GetResultRegionInfo( i ); //2�Ԗڂɑ傫�����x���̏����擾����
		ri->GetCenter(xcenter2,ycenter2 );
		x_2=(xcenter2-x)/xsize;
		y_2=(ycenter2-y)/ysize;
		//printf("x=%lf y=%lf\n",xcenter2,ycenter2 );
		if(start==0){
			if(player[y_1][x_1]!=2)turn=player[y_1][x_1];
			else turn=player[y_2][x_2];
			start=1;
		}	//�ŏ��̃^�[���̂Ƃ��ɐ�s�̎��c������


		if(turn==0&&(y_1<3||y_2<3)){
			//printf("check....\n");
			IplImage *colorbackgroundImage1=NULL;
			colorbackgroundImage1=cvLoadImage(filename,1);
			if(player[y_1][x_1]!=turn){
				//ROI�摜�쐬
				roiImg = cvCreateImage(cvSize(colorbackgroundImage->width,colorbackgroundImage->height),IPL_DEPTH_8U,1);
				roiImg = cvCloneImage(colorbackgroundImage);
				//ROI�w��
				cvSetImageROI(roiImg,cvRect(xcenter1-(xsize/2.7),ycenter1-(ysize/2.7),(int)(xsize),(int)(ysize)));
				//cvShowImage("test", roiImg);

				IplImage *differenceMapImage=cvCreateImage(cvSize(colorbackgroundImage1->width - (int)(xsize) + 1,
					colorbackgroundImage1->height - (int)(ysize) + 1),IPL_DEPTH_32F,1);
									//����x�}�b�v�摜�pIplImage
				//�e���v���[�g�}�b�`���O���s��
				cvMatchTemplate(colorbackgroundImage1,roiImg,differenceMapImage,CV_TM_SQDIFF);

				//�e���v���[�g�����摜�̂ǂ̕����ɂ���̂��Ƃ������𓾂�
				cvMinMaxLoc(differenceMapImage,&min_val,NULL,&minLocation,NULL,NULL);

				roiImg1 = cvCreateImage(cvSize(colorbackgroundImage->width,colorbackgroundImage->height),IPL_DEPTH_8U,1);
				roiImg1 = cvCloneImage(colorbackgroundImage1);
				cvSetImageROI(roiImg1,cvRect(minLocation.x,minLocation.y,(int)(xsize),(int)(ysize)));
				//cvShowImage("test2", roiImg1);

				x_3=(minLocation.x+xsize/2-x)/xsize;
				y_3=(minLocation.y+ysize/2-y)/ysize;

				if(masu[y_3][x_3]==masu[y_2][x_2]){
						if(Get_Ssd(roiImg,roiImg1)>3000000)
						ura(2);
					}
				else if(masu[y_3][x_3]!=masu[y_2][x_2])
					ura(2);
				cvResetImageROI(roiImg);
				cvResetImageROI(roiImg1);
			}
			else if(player[y_2][x_2]!=turn){
				//ROI�摜�쐬
				roiImg = cvCreateImage(cvSize(colorbackgroundImage->width,colorbackgroundImage->height),IPL_DEPTH_8U,1);
				roiImg = cvCloneImage(colorbackgroundImage);
				//ROI�w��
				cvSetImageROI(roiImg,cvRect(xcenter2-(xsize/2.7),ycenter2-(ysize/2.7),(int)(xsize),(int)(ysize)));
				//cvShowImage("test", roiImg);

				IplImage *differenceMapImage=cvCreateImage(cvSize(colorbackgroundImage1->width - (int)(xsize) + 1,
					colorbackgroundImage1->height - (int)(ysize) + 1),IPL_DEPTH_32F,1);
									//����x�}�b�v�摜�pIplImage
				//�e���v���[�g�}�b�`���O���s��
				cvMatchTemplate(colorbackgroundImage1,roiImg,differenceMapImage,CV_TM_SQDIFF);

				//�e���v���[�g�����摜�̂ǂ̕����ɂ���̂��Ƃ������𓾂�
				cvMinMaxLoc(differenceMapImage,&min_val,NULL,&minLocation,NULL,NULL);

				roiImg1 = cvCreateImage(cvSize(colorbackgroundImage->width,colorbackgroundImage->height),IPL_DEPTH_8U,1);
				roiImg1 = cvCloneImage(colorbackgroundImage1);
				cvSetImageROI(roiImg1,cvRect(minLocation.x,minLocation.y,(int)(xsize),(int)(ysize)));
				//cvShowImage("test2", roiImg1);
				

				x_3=(minLocation.x+xsize/2-x)/xsize;
				y_3=(minLocation.y+ysize/2-y)/ysize;

				if(masu[y_3][x_3]==masu[y_1][x_1]){
						if(Get_Ssd(roiImg,roiImg1)>3000000)
						ura(1);
					}
				else if(masu[y_3][x_3]!=masu[y_1][x_1])
					ura(1);
				cvResetImageROI(roiImg);
				cvResetImageROI(roiImg1);
			}
		}
		else if(turn==1&&(y_1>5||y_2>5)){
			//printf("check....\n");
			IplImage *colorbackgroundImage1=NULL;
			colorbackgroundImage1=cvLoadImage(filename,1);

			if(player[y_1][x_1]!=turn){
				//ROI�摜�쐬
				roiImg = cvCreateImage(cvSize(colorbackgroundImage->width,colorbackgroundImage->height),IPL_DEPTH_8U,1);
				roiImg = cvCloneImage(colorbackgroundImage);
				//ROI�w��
				cvSetImageROI(roiImg,cvRect(xcenter1-(xsize/2.7),ycenter1-(ysize/2.7),(int)(xsize),(int)(ysize)));
				//cvShowImage("test", roiImg);

				IplImage *differenceMapImage=cvCreateImage(cvSize(colorbackgroundImage1->width - (int)(xsize) + 1,
					colorbackgroundImage1->height - (int)(ysize) + 1),IPL_DEPTH_32F,1);
									//����x�}�b�v�摜�pIplImage
				//�e���v���[�g�}�b�`���O���s��
				cvMatchTemplate(colorbackgroundImage1,roiImg,differenceMapImage,CV_TM_SQDIFF);

				//�e���v���[�g�����摜�̂ǂ̕����ɂ���̂��Ƃ������𓾂�
				cvMinMaxLoc(differenceMapImage,&min_val,NULL,&minLocation,NULL,NULL);

				roiImg1 = cvCreateImage(cvSize(colorbackgroundImage->width,colorbackgroundImage->height),IPL_DEPTH_8U,1);
				roiImg1 = cvCloneImage(colorbackgroundImage1);
				cvSetImageROI(roiImg1,cvRect(minLocation.x,minLocation.y,(int)(xsize),(int)(ysize)));
				//cvShowImage("test2", roiImg1);


				x_3=(minLocation.x+xsize/2-x)/xsize;
				y_3=(minLocation.y+ysize/2-y)/ysize;

				if(masu[y_3][x_3]==masu[y_2][x_2]){
						if(Get_Ssd(roiImg,roiImg1)>3000000)
						ura(2);
					}
				else if(masu[y_3][x_3]!=masu[y_2][x_2])
					ura(2);
				cvResetImageROI(roiImg);
				cvResetImageROI(roiImg1);

			}
			else if(player[y_2][x_2]!=turn){
				//ROI�摜�쐬
				roiImg = cvCreateImage(cvSize(colorbackgroundImage->width,colorbackgroundImage->height),IPL_DEPTH_8U,1);
				roiImg = cvCloneImage(colorbackgroundImage);
				//ROI�w��
				cvSetImageROI(roiImg,cvRect(xcenter2-(xsize/2.7),ycenter2-(ysize/2.7),(int)(xsize),(int)(ysize)));
				//cvShowImage("test", roiImg);

				IplImage *differenceMapImage=cvCreateImage(cvSize(colorbackgroundImage1->width - (int)(xsize) + 1,
					colorbackgroundImage1->height - (int)(ysize) + 1),IPL_DEPTH_32F,1);
									//����x�}�b�v�摜�pIplImage
				//�e���v���[�g�}�b�`���O���s��
				cvMatchTemplate(colorbackgroundImage1,roiImg,differenceMapImage,CV_TM_SQDIFF);

				//�e���v���[�g�����摜�̂ǂ̕����ɂ���̂��Ƃ������𓾂�
				cvMinMaxLoc(differenceMapImage,&min_val,NULL,&minLocation,NULL,NULL);

				roiImg1 = cvCreateImage(cvSize(colorbackgroundImage->width,colorbackgroundImage->height),IPL_DEPTH_8U,1);
				roiImg1 = cvCloneImage(colorbackgroundImage1);
				cvSetImageROI(roiImg1,cvRect(minLocation.x,minLocation.y,(int)(xsize),(int)(ysize)));
				//cvShowImage("test2", roiImg1);


				x_3=(minLocation.x+xsize/2-x)/xsize;
				y_3=(minLocation.y+ysize/2-y)/ysize;

				if(masu[y_3][x_3]==masu[y_1][x_1]){
						if(Get_Ssd(roiImg,roiImg1)>3000000)
						ura(1);
					}
				else if(masu[y_3][x_3]!=masu[y_1][x_1])
					ura(1);
				cvResetImageROI(roiImg);
				cvResetImageROI(roiImg1);

			}
		}
		if(player[y_1][x_1]==turn){
			if(masu[y_2][x_2]=="(�ʏ�)") printf("Win Player1!!!\n");
			else if(masu[y_2][x_2]==" ���� ") printf("Win Player2!!!\n");
			masu[y_2][x_2]=masu[y_1][x_1];
			masu[y_1][x_1]="      ";
			player[y_2][x_2]=turn;
			player[y_1][x_1]=2;
		}else{
			if(masu[y_1][x_1]=="(�ʏ�)") printf("Win Player1!!!\n");
			else if(masu[y_1][x_1]==" ���� ") printf("Win Player2!!!\n");
			masu[y_1][x_1]=masu[y_2][x_2];
			masu[y_2][x_2]="      ";
			player[y_2][x_2]=2;
			player[y_1][x_1]=turn;
		}
	}
	
	turn++;

}

void current(void){
	char repeat;
	int x,y,koma,p;
	
	while(1){
		printf("�c,��,��̎��,�v���C���[ �̏��œ��͂��Ă�������.\n\n");
		printf("(��̎��)\n���� : 1 , �p�s : 2 , ��� : 3 , ���� : 4 , �j�n : 5 ,\n�⏫ : 6 , ���� : 7 , ���� : 8 ,");
		printf(" �Ƌ� : 9 , ���� : 10 ,\n���j : 11 , ���� : 12 , ���n : 13 , ���� : 14 , �� : 0\n");

		scanf("%d %d %d %d",&y,&x,&koma,&p);
		if(x>9||y>9||x<1||y<1){
			printf("��̈ʒu������������܂���\n");
		}
		else if(p==1){
			if(koma==1){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" ���� ";
				print();
			}
			else if(koma==2){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" �p�s ";
				print();
			}
			else if(koma==3){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" ��� ";
				print();
			}
			else if(koma==4){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" ���� ";
				print();
			}
			else if(koma==5){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" �j�n ";
				print();
			}
			else if(koma==6){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" �⏫ ";
				print();
			}
			else if(koma==7){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" ���� ";
				print();
			}
			else if(koma==8){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" ���� ";
				print();
			}
			else if(koma==9){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" �Ƌ� ";
				print();
			}
			else if(koma==10){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" ���� ";
				print();
			}
			else if(koma==11){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" ���j ";
				print();
			}
			else if(koma==12){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" ���� ";
				print();
			}
			else if(koma==13){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" ���n ";
				print();
			}
			else if(koma==14){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" ���� ";
				print();
			}
			else if(koma==0){
				player[y-1][x-1]=2;
				masu[y-1][x-1]="      ";
				print();
			}
			else printf("��̓��͏�񂪐���������܂���\n");
		}
		else if(p==2){
			if(koma==1){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(����)";
				print();
			}
			else if(koma==2){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(�p�s)";
				print();
			}
			else if(koma==3){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(���)";
				print();
			}
			else if(koma==4){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(����)";
				print();
			}
			else if(koma==5){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(�j�n)";
				print();
			}
			else if(koma==6){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(�⏫)";
				print();
			}
			else if(koma==7){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(����)";
				print();
			}
			else if(koma==8){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(�ʏ�)";
				print();
			}
			else if(koma==9){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(�Ƌ�)";
				print();
			}
			else if(koma==10){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(����)";
				print();
			}
			else if(koma==11){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(���j)";
				print();
			}
			else if(koma==12){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(����)";
				print();
			}
			else if(koma==13){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(���n)";
				print();
			}
			else if(koma==14){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(����)";
				print();
			}
			else if(koma==0){
				player[y-1][x-1]=2;
				masu[y-1][x-1]="      ";
				print();
			}
			else printf("��̓��͏�񂪐���������܂���\n");
		}else if(koma==0){
			player[y-1][x-1]=2;
			masu[y-1][x-1]="      ";
			print();
		}else printf("�v���C���[�̓��͏�񂪐���������܂���\n");

		

		printf("�����𑱂��܂���(y/n)\n");
		scanf(" %c",&repeat);
		while(repeat!='y'&&repeat!='n'){
			printf("���������͂��Ă�������\n");
			scanf(" %c",&repeat);
		}
		if(repeat=='n') {
			cvWaitKey(0);
			break;
		}
	}

}

void print(void)
{
	int j;
		printf("  +------+------+------+------+------+------+------+------+------+\n  ");
		for(j=0;j<9;j++)printf("|%s",masu[0][j]);printf("|\n");
		printf("  +------+------+------+------+------+------+------+------+------+\n  ");
		for(j=0;j<9;j++)printf("|%s",masu[1][j]);printf("|\n");
		printf("  +------+------+------+------+------+------+------+------+------+\n  ");
		for(j=0;j<9;j++)printf("|%s",masu[2][j]);printf("|\n");
		printf("  +------+------+------+------+------+------+------+------+------+\n  ");
		for(j=0;j<9;j++)printf("|%s",masu[3][j]);printf("|\n");
		printf("  +------+------+------+------+------+------+------+------+------+\n  ");
		for(j=0;j<9;j++)printf("|%s",masu[4][j]);printf("|\n");
		printf("  +------+------+------+------+------+------+------+------+------+\n  ");
		for(j=0;j<9;j++)printf("|%s",masu[5][j]);printf("|\n");
		printf("  +------+------+------+------+------+------+------+------+------+\n  ");
		for(j=0;j<9;j++)printf("|%s",masu[6][j]);printf("|\n");
		printf("  +------+------+------+------+------+------+------+------+------+\n  ");
		for(j=0;j<9;j++)printf("|%s",masu[7][j]);printf("|\n");
		printf("  +------+------+------+------+------+------+------+------+------+\n  ");
		for(j=0;j<9;j++)printf("|%s",masu[8][j]);printf("|\n");
		printf("  +------+------+------+------+------+------+------+------+------+\n\n\n\n");		//�����Ղ̕\��
}

void ura(int n){
	if(n==1){
		if(masu[y_1][x_1]==" ���� ") masu[y_1][x_1]=" �Ƌ� ";
		else if(masu[y_1][x_1]==" ���� ") masu[y_1][x_1]=" ���� ";
		else if(masu[y_1][x_1]==" �j�n ") masu[y_1][x_1]=" ���j ";
		else if(masu[y_1][x_1]==" �⏫ ") masu[y_1][x_1]=" ���� ";
		else if(masu[y_1][x_1]==" �p�s ") masu[y_1][x_1]=" ���n ";
		else if(masu[y_1][x_1]==" ��� ") masu[y_1][x_1]=" ���� ";
		else if(masu[y_1][x_1]=="(����)") masu[y_1][x_1]="(�Ƌ�)";
		else if(masu[y_1][x_1]=="(����)") masu[y_1][x_1]="(����)";
		else if(masu[y_1][x_1]=="(�j�n)") masu[y_1][x_1]="(���j)";
		else if(masu[y_1][x_1]=="(�⏫)") masu[y_1][x_1]="(����)";
		else if(masu[y_1][x_1]=="(�p�s)") masu[y_1][x_1]="(���n)";
		else if(masu[y_1][x_1]=="(���)") masu[y_1][x_1]="(����)";
	}
	else if(n==2){
		if(masu[y_2][x_2]==" ���� ") masu[y_2][x_2]=" �Ƌ� ";
		else if(masu[y_2][x_2]==" ���� ") masu[y_2][x_2]=" ���� ";
		else if(masu[y_2][x_2]==" �j�n ") masu[y_2][x_2]=" ���j ";
		else if(masu[y_2][x_2]==" �⏫ ") masu[y_2][x_2]=" ���� ";
		else if(masu[y_2][x_2]==" �p�s ") masu[y_2][x_2]=" ���n ";
		else if(masu[y_2][x_2]==" ��� ") masu[y_2][x_2]=" ���� ";
		else if(masu[y_2][x_2]=="(����)") masu[y_2][x_2]="(�Ƌ�)";
		else if(masu[y_2][x_2]=="(����)") masu[y_2][x_2]="(����)";
		else if(masu[y_2][x_2]=="(�j�n)") masu[y_2][x_2]="(���j)";
		else if(masu[y_2][x_2]=="(�⏫)") masu[y_2][x_2]="(����)";
		else if(masu[y_2][x_2]=="(�p�s)") masu[y_2][x_2]="(���n)";
		else if(masu[y_2][x_2]=="(���)") masu[y_2][x_2]="(����)";
	}
	//printf("%d\n",Get_Ssd(roiImg,roiImg1));
}

unsigned int Get_Ssd(IplImage *imgA,IplImage *imgB)
{

	int tmpsize;

	IplImage *tmpA = cvCreateImage(cvGetSize(imgA),IPL_DEPTH_64F,imgA->nChannels);
	cvScale(imgA,tmpA);

	IplImage *tmpB = cvCreateImage(cvGetSize(imgB),IPL_DEPTH_64F,imgB->nChannels);
	cvScale(imgB,tmpB);

	IplImage *tmp = cvCloneImage(tmpA);
	cvSub(tmpA,tmpB,tmp);
	cvPow(tmp,tmp,2);

	CvScalar SumData = cvScalarAll( 0 );
	SumData = cvSum(tmp);
	
	tmpsize=imgA->width*imgA->height;

	cvReleaseImage(&tmpA);
	cvReleaseImage(&tmpB);
	cvReleaseImage(&tmp);
	
	return cvRound(SumData.val[0] + SumData.val[1] + SumData.val[2] + SumData.val[3]);
}