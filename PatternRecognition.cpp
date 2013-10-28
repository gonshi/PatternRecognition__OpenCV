#include "cv.h"
#include "highgui.h"
#include<stdio.h>
#include<cxcore.h>
#include <iostream>
using namespace std;
double x,y,xsize,ysize,x2,y2;
int x_1,x_2,y_1,y_2,x_3,y_3,player[9][9],turn,stagepixel,start=0;  //player,turnは駒はどちらのプレイヤーのものかという情報,
												//startは開始時のみ扱う;
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

//ライブラリ読み込み
#pragma comment(lib,"cv.lib")
#pragma comment(lib,"cxcore.lib")
#pragma comment(lib,"highgui.lib")

//ラベリングクラス
//http://chihara.naist.jp/people/STAFF/imura/products/labeling
#include "Labeling.h"


//==============================================================================
//グローバル変数
//==============================================================================
IplImage *image;
short *dst;
LabelingBS labeling;




//------------------------------------------------------------------------------
//IplImageの任意ピクセルにアクセスするための関数
//------------------------------------------------------------------------------
inline unsigned char getPixel(IplImage *image, int x, int y, int index)
{
    return (uchar)image->imageData[y*image->widthStep + x*image->nChannels + index];
}


//------------------------------------------------------------------------------
//マウスコールバック
//------------------------------------------------------------------------------
void onMouse(int event, int x, int y, int flags)
{
    switch (event) {
        //クリック位置・元画像の値・ラベルナンバーを出力
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
//メイン
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
	
	// 駒情報の初期化
	for(j=0;j<=8;j++){
		for(k=0;k<=8;k++){
			masu[j][k]="      ";
			player[j][k]=2;
		}
	}
	masu[0][0]=masu[0][8]="(香車)";
	masu[0][1]=masu[0][7]="(桂馬)";
	masu[0][2]=masu[0][6]="(銀将)";
	masu[0][3]=masu[0][5]="(金将)";
	masu[0][4]="(玉将)";
	masu[1][1]="(飛車)";
	masu[1][7]="(角行)";
	for(i=0;i<9;i++)masu[2][i]="(歩兵)";
	for(i=0;i<9;i++){
		player[0][i]=1;
		player[2][i]=1;		//奥側をplayer1とする
	}
	player[1][1]=player[1][7]=1;


	masu[8][0]=masu[8][8]=" 香車 ";
	masu[8][1]=masu[8][7]=" 桂馬 ";
	masu[8][2]=masu[8][6]=" 銀将 ";
	masu[8][3]=masu[8][5]=" 金将 ";
	masu[8][4]=" 王将 ";
	masu[7][1]=" 角行 ";
	masu[7][7]=" 飛車 ";
	for(i=0;i<9;i++)masu[6][i]=" 歩兵 ";	
	for(i=0;i<9;i++){
		player[8][i]=0;
		player[6][i]=0;		//手前側をplayer0とする
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
	printf("  +------+------+------+------+------+------+------+------+------+\n\n\n\n");		//将棋盤の表示



	cvNamedWindow(windowNameCurrent,CV_WINDOW_AUTOSIZE);
	cvNamedWindow(windowNameResult,CV_WINDOW_AUTOSIZE);
	cvNamedWindow(windowNameBackground,CV_WINDOW_AUTOSIZE);

	
    char *window_name = "Labeling Sample";

    //cvNamedWindow(window_name);

    // コールバック関数をセット
    cvSetMouseCallback(window_name, (CvMouseCallback)onMouse);

    //画像の読み込み・二値化
    image = cvLoadImage("1.bmp", 0);  // 0: グレイスケールで読み込む
    //cvShowImage(window_name, image);
    //cvWaitKey(-1);
	cvSmooth (image, image, CV_GAUSSIAN, 7);
    //cvShowImage(window_name, image);
    //cvWaitKey(-1);
	
    cvThreshold(image, image, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU); 
    //cvShowImage(window_name, image);
    //cvWaitKey(-1);
	cvNot(image, image); //色の反転
    //cvShowImage(window_name, image);
    //cvWaitKey(-1);

    //ラベリング処理の結果保存用配列
    dst = new short[ image->width * image->height ];

    //ラベリング実行
    labeling.Exec((uchar *)image->imageData, dst, image->width, image->height, true, 10);
    //------------------------------------------------------------------------------
    //int Exec( SrcT *target, DstT *result, int target_width, int target_height,
    //          const bool is_sort_region, const int region_size_min )
    //引数:

    //  SrcT *target 入力バッファ
    //  DstT *result 出力バッファ
    //  int target_width バッファの横サイズ
    //  int target_height バッファの縦サイズ
    //  bool is_sort_region 連続領域を大きさの降順にソートするか
    //  int region_size_min 最小の領域サイズ(これ未満は無視する)
    //------------------------------------------------------------------------------


	
	RegionInfoBS *ri = labeling.GetResultRegionInfo( 0 ); //一番大きいラベルの情報を習得する

	stagepixel=ri->GetNumOfPixels();


	int size_x1, size_y1,size_x2, size_y2;

	

	ri->GetMin( size_x1, size_y1 );
	ri->GetMax( size_x2, size_y2 ); //左上の座標と右下の座標を取得

	//printf("%d %d,%d %d\n",size_x1, size_y1,size_x2, size_y2);
	x=(size_x2-size_x1)/29.4+size_x1;
	x2=size_x2-(size_x2-size_x1)/29.4;
	y=(size_y2-size_y1)/33.0+size_y1;
	y2=size_y2-(size_y2-size_y1)/33.0;
	xsize=(x2-x)/9;
	ysize=(y2-y)/9; //マスの縦幅と横幅を算出
	//printf("%lf\n",xsize);
	//printf("%lf\n",ysize);
	//printf("%lf\n",x);
	//printf("%lf\n",y);


    //画像を表示・キー入力待ち
    //cvShowImage(window_name, image);
    cvWaitKey(-1);

    
	for(i=1;i<=69;i++){//画像を繰り返し読み込む
		printf("%d手目\n\n",i);
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
	//終了処理
    delete dst;
    cvReleaseImage(&image);
    cvDestroyAllWindows();

}



//欠損領域を補完する

void interpolate(IplImage *polate,IplImage *temp){

	double min_val;

	IplImage* outImg = NULL;
	int minx,miny,maxx,maxy,pixel,i=0;
	float xcenter1,ycenter1,xcenter2,ycenter2;
	char windowNameResult[]="Result";
	//cvNamedWindow("test", CV_WINDOW_AUTOSIZE);
	//cvNamedWindow("test2", CV_WINDOW_AUTOSIZE);
	CvPoint minLocation;	// 相違度が最少になる場所



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

	RegionInfoBS *ri = labeling.GetResultRegionInfo( i ); //一番大きいラベルの情報を取得する
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
		if((maxy-miny)>ysize||(maxx-minx)>xsize){ //この条件を満たすとき，駒の移動をラベリングした結果，
													//２つの駒がくっついている状態とみなし，
													//駒が移動した場合と同様の処理を行う

			if((maxy-miny)>ysize&&(maxx-minx)>xsize){
				if(temp->imageData[temp->widthStep * ((int)ycenter1-(int)ysize/2) 
					+ (int)xcenter1-(int)xsize/2]==-1){//左ななめか右ななめかの判定
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
			}	//最初のターンのときに先行の手を把握する

			if(turn==0&&(y_1<3||y_2<3)){
				//printf("check....\n");
				IplImage *colorbackgroundImage1=NULL;
				colorbackgroundImage1=cvLoadImage(filename,1);
				if(player[y_1][x_1]!=turn){
					//ROI画像作成
					roiImg = cvCreateImage(cvSize(colorbackgroundImage->width,colorbackgroundImage->height),IPL_DEPTH_8U,1);
					roiImg = cvCloneImage(colorbackgroundImage);
					//ROI指定
					cvSetImageROI(roiImg,cvRect(xcenter1-(xsize/2.7),ycenter1-(ysize/2.7),(int)(xsize),(int)(ysize)));
					//cvShowImage("test", roiImg);					
					IplImage *differenceMapImage=cvCreateImage(cvSize(colorbackgroundImage1->width - (int)(xsize) + 1,
						colorbackgroundImage1->height - (int)(ysize) + 1),IPL_DEPTH_32F,1);
										//相違度マップ画像用IplImage
					//テンプレートマッチングを行う
					cvMatchTemplate(colorbackgroundImage1,roiImg,differenceMapImage,CV_TM_SQDIFF);

					//テンプレートが元画像のどの部分にあるのかという情報を得る
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
					//ROI画像作成
					roiImg = cvCreateImage(cvSize(colorbackgroundImage->width,colorbackgroundImage->height),IPL_DEPTH_8U,1);
					roiImg = cvCloneImage(colorbackgroundImage);
					//ROI指定
					cvSetImageROI(roiImg,cvRect(xcenter2-(xsize/2.7),ycenter2-(ysize/2.7),(int)(xsize),(int)(ysize)));
					//cvShowImage("test", roiImg);
					IplImage *differenceMapImage=cvCreateImage(cvSize(colorbackgroundImage1->width - (int)(xsize) + 1,
						colorbackgroundImage1->height - (int)(ysize) + 1),IPL_DEPTH_32F,1);
										//相違度マップ画像用IplImage
					//テンプレートマッチングを行う
					cvMatchTemplate(colorbackgroundImage1,roiImg,differenceMapImage,CV_TM_SQDIFF);

					//テンプレートが元画像のどの部分にあるのかという情報を得る
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
					//ROI画像作成
					roiImg = cvCreateImage(cvSize(colorbackgroundImage->width,colorbackgroundImage->height),IPL_DEPTH_8U,1);
					roiImg = cvCloneImage(colorbackgroundImage);
					//ROI指定
					cvSetImageROI(roiImg,cvRect(xcenter1-(xsize/2.7),ycenter1-(ysize/2.7),(int)(xsize),(int)(ysize)));
					//cvShowImage("test", roiImg);
					IplImage *differenceMapImage=cvCreateImage(cvSize(colorbackgroundImage1->width - (int)(xsize) + 1,
						colorbackgroundImage1->height - (int)(ysize) + 1),IPL_DEPTH_32F,1);
										//相違度マップ画像用IplImage
					//テンプレートマッチングを行う
					cvMatchTemplate(colorbackgroundImage1,roiImg,differenceMapImage,CV_TM_SQDIFF);

					//テンプレートが元画像のどの部分にあるのかという情報を得る
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
					//ROI画像作成
					roiImg = cvCreateImage(cvSize(colorbackgroundImage->width,colorbackgroundImage->height),IPL_DEPTH_8U,1);
					roiImg = cvCloneImage(colorbackgroundImage);
					//ROI指定
					cvSetImageROI(roiImg,cvRect(xcenter2-(xsize/2.7),ycenter2-(ysize/2.7),(int)(xsize),(int)(ysize)));
					//cvShowImage("test", roiImg);

					IplImage *differenceMapImage=cvCreateImage(cvSize(colorbackgroundImage1->width - (int)(xsize) + 1,
						colorbackgroundImage1->height - (int)(ysize) + 1),IPL_DEPTH_32F,1);
										//相違度マップ画像用IplImage
					//テンプレートマッチングを行う
					cvMatchTemplate(colorbackgroundImage1,roiImg,differenceMapImage,CV_TM_SQDIFF);

					//テンプレートが元画像のどの部分にあるのかという情報を得る
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
				if(masu[y_2][x_2]=="(玉将)") printf("Win Player1!!!\n");
				else if(masu[y_2][x_2]==" 王将 ") printf("Win Player2!!!\n");
				masu[y_2][x_2]=masu[y_1][x_1];
				masu[y_1][x_1]="      ";
				player[y_2][x_2]=turn;
				player[y_1][x_1]=2;
			}else{
				if(masu[y_1][x_1]=="(玉将)") printf("Win Player1!!!\n");
				else if(masu[y_1][x_1]==" 王将 ") printf("Win Player2!!!\n");
				masu[y_1][x_1]=masu[y_2][x_2];
				masu[y_2][x_2]="      ";
				player[y_2][x_2]=2;
				player[y_1][x_1]=turn;
			}
		}
		else{

			//ROI
			
			//ROI画像作成
			roiImg = cvCreateImage(cvSize(colorbackgroundImage->width,colorbackgroundImage->height),IPL_DEPTH_8U,1);
			roiImg = cvCloneImage(colorbackgroundImage);
			//ROI指定
			cvSetImageROI(roiImg,cvRect(xcenter1-(xsize/2.7),ycenter1-(ysize/2.7),(int)(xsize),(int)(ysize)));
			//cvShowImage("test", roiImg);
			//cvWaitKey(0);
			IplImage *differenceMapImage=cvCreateImage(cvSize(colorbasicgroundImage->width - (int)(xsize) + 1,
				colorbasicgroundImage->height - (int)(ysize) + 1),IPL_DEPTH_32F,1);
								//相違度マップ画像用IplImage
			//テンプレートマッチングを行う
			cvMatchTemplate(colorbasicgroundImage,roiImg,differenceMapImage,CV_TM_SQDIFF);

			//テンプレートが元画像のどの部分にあるのかという情報を得る
			cvMinMaxLoc(differenceMapImage,&min_val,NULL,&minLocation,NULL,NULL);
			
			cvResetImageROI(roiImg);

			x_3=(minLocation.x+xsize/2-x)/xsize;
			y_3=(minLocation.y+ysize/2-y)/ysize;
			x_1=(xcenter1-x)/xsize;
			y_1=(ycenter1-y)/ysize;
			
			
			if(x_3==0&&y_3==0||x_3==8&&y_3==0) masu[y_1][x_1]="(香車)";
			else if(x_3==1&&y_3==0||x_3==8&&y_3==0) masu[y_1][x_1]="(桂馬)";
			else if(x_3==2&&y_3==0||x_3==6&&y_3==0) masu[y_1][x_1]="(銀将)";
			else if(x_3==3&&y_3==0||x_3==5&&y_3==0) masu[y_1][x_1]="(金将)";
			else if(x_3==4&&y_3==0) masu[y_1][x_1]="(玉将)";
			else if(x_3==7&&y_3==1) masu[y_1][x_1]="(角行)";
			else if(x_3==1&&y_3==1) masu[y_1][x_1]="(飛車)";
			else if(y_3==2) masu[y_1][x_1]="(歩兵)";
			else if(x_3==0&&y_3==8||x_3==8&&y_3==8) masu[y_1][x_1]=" 香車 ";
			else if(x_3==1&&y_3==8||x_3==7&&y_3==8) masu[y_1][x_1]=" 桂馬 ";
			else if(x_3==2&&y_3==8||x_3==6&&y_3==8) masu[y_1][x_1]=" 銀将 ";
			else if(x_3==3&&y_3==8||x_3==5&&y_3==8) masu[y_1][x_1]=" 金将 ";
			else if(x_3==4&&y_3==8) masu[y_1][x_1]=" 王将 ";
			else if(x_3==7&&y_3==7) masu[y_1][x_1]=" 飛車 ";
			else if(x_3==1&&y_3==7) masu[y_1][x_1]=" 角行 ";
			else if(y_3==6) masu[y_1][x_1]=" 歩兵 ";
			else printf("error\n%d %d",x_3,y_3);

			player[y_1][x_1]=turn;
			
		}
	}

	if(labeling.GetNumOfResultRegions()>i){
		
		x_1=(xcenter1-x)/xsize;
		y_1=(ycenter1-y)/ysize;

		RegionInfoBS *ri = labeling.GetResultRegionInfo( i ); //2番目に大きいラベルの情報を取得する
		ri->GetCenter(xcenter2,ycenter2 );
		x_2=(xcenter2-x)/xsize;
		y_2=(ycenter2-y)/ysize;
		//printf("x=%lf y=%lf\n",xcenter2,ycenter2 );
		if(start==0){
			if(player[y_1][x_1]!=2)turn=player[y_1][x_1];
			else turn=player[y_2][x_2];
			start=1;
		}	//最初のターンのときに先行の手を把握する


		if(turn==0&&(y_1<3||y_2<3)){
			//printf("check....\n");
			IplImage *colorbackgroundImage1=NULL;
			colorbackgroundImage1=cvLoadImage(filename,1);
			if(player[y_1][x_1]!=turn){
				//ROI画像作成
				roiImg = cvCreateImage(cvSize(colorbackgroundImage->width,colorbackgroundImage->height),IPL_DEPTH_8U,1);
				roiImg = cvCloneImage(colorbackgroundImage);
				//ROI指定
				cvSetImageROI(roiImg,cvRect(xcenter1-(xsize/2.7),ycenter1-(ysize/2.7),(int)(xsize),(int)(ysize)));
				//cvShowImage("test", roiImg);

				IplImage *differenceMapImage=cvCreateImage(cvSize(colorbackgroundImage1->width - (int)(xsize) + 1,
					colorbackgroundImage1->height - (int)(ysize) + 1),IPL_DEPTH_32F,1);
									//相違度マップ画像用IplImage
				//テンプレートマッチングを行う
				cvMatchTemplate(colorbackgroundImage1,roiImg,differenceMapImage,CV_TM_SQDIFF);

				//テンプレートが元画像のどの部分にあるのかという情報を得る
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
				//ROI画像作成
				roiImg = cvCreateImage(cvSize(colorbackgroundImage->width,colorbackgroundImage->height),IPL_DEPTH_8U,1);
				roiImg = cvCloneImage(colorbackgroundImage);
				//ROI指定
				cvSetImageROI(roiImg,cvRect(xcenter2-(xsize/2.7),ycenter2-(ysize/2.7),(int)(xsize),(int)(ysize)));
				//cvShowImage("test", roiImg);

				IplImage *differenceMapImage=cvCreateImage(cvSize(colorbackgroundImage1->width - (int)(xsize) + 1,
					colorbackgroundImage1->height - (int)(ysize) + 1),IPL_DEPTH_32F,1);
									//相違度マップ画像用IplImage
				//テンプレートマッチングを行う
				cvMatchTemplate(colorbackgroundImage1,roiImg,differenceMapImage,CV_TM_SQDIFF);

				//テンプレートが元画像のどの部分にあるのかという情報を得る
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
				//ROI画像作成
				roiImg = cvCreateImage(cvSize(colorbackgroundImage->width,colorbackgroundImage->height),IPL_DEPTH_8U,1);
				roiImg = cvCloneImage(colorbackgroundImage);
				//ROI指定
				cvSetImageROI(roiImg,cvRect(xcenter1-(xsize/2.7),ycenter1-(ysize/2.7),(int)(xsize),(int)(ysize)));
				//cvShowImage("test", roiImg);

				IplImage *differenceMapImage=cvCreateImage(cvSize(colorbackgroundImage1->width - (int)(xsize) + 1,
					colorbackgroundImage1->height - (int)(ysize) + 1),IPL_DEPTH_32F,1);
									//相違度マップ画像用IplImage
				//テンプレートマッチングを行う
				cvMatchTemplate(colorbackgroundImage1,roiImg,differenceMapImage,CV_TM_SQDIFF);

				//テンプレートが元画像のどの部分にあるのかという情報を得る
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
				//ROI画像作成
				roiImg = cvCreateImage(cvSize(colorbackgroundImage->width,colorbackgroundImage->height),IPL_DEPTH_8U,1);
				roiImg = cvCloneImage(colorbackgroundImage);
				//ROI指定
				cvSetImageROI(roiImg,cvRect(xcenter2-(xsize/2.7),ycenter2-(ysize/2.7),(int)(xsize),(int)(ysize)));
				//cvShowImage("test", roiImg);

				IplImage *differenceMapImage=cvCreateImage(cvSize(colorbackgroundImage1->width - (int)(xsize) + 1,
					colorbackgroundImage1->height - (int)(ysize) + 1),IPL_DEPTH_32F,1);
									//相違度マップ画像用IplImage
				//テンプレートマッチングを行う
				cvMatchTemplate(colorbackgroundImage1,roiImg,differenceMapImage,CV_TM_SQDIFF);

				//テンプレートが元画像のどの部分にあるのかという情報を得る
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
			if(masu[y_2][x_2]=="(玉将)") printf("Win Player1!!!\n");
			else if(masu[y_2][x_2]==" 王将 ") printf("Win Player2!!!\n");
			masu[y_2][x_2]=masu[y_1][x_1];
			masu[y_1][x_1]="      ";
			player[y_2][x_2]=turn;
			player[y_1][x_1]=2;
		}else{
			if(masu[y_1][x_1]=="(玉将)") printf("Win Player1!!!\n");
			else if(masu[y_1][x_1]==" 王将 ") printf("Win Player2!!!\n");
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
		printf("縦,横,駒の種類,プレイヤー の順で入力してください.\n\n");
		printf("(駒の種類)\n歩兵 : 1 , 角行 : 2 , 飛車 : 3 , 香車 : 4 , 桂馬 : 5 ,\n銀将 : 6 , 金将 : 7 , 王将 : 8 ,");
		printf(" と金 : 9 , 成香 : 10 ,\n成桂 : 11 , 成銀 : 12 , 龍馬 : 13 , 龍王 : 14 , 空き : 0\n");

		scanf("%d %d %d %d",&y,&x,&koma,&p);
		if(x>9||y>9||x<1||y<1){
			printf("駒の位置が正しくありません\n");
		}
		else if(p==1){
			if(koma==1){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" 歩兵 ";
				print();
			}
			else if(koma==2){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" 角行 ";
				print();
			}
			else if(koma==3){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" 飛車 ";
				print();
			}
			else if(koma==4){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" 香車 ";
				print();
			}
			else if(koma==5){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" 桂馬 ";
				print();
			}
			else if(koma==6){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" 銀将 ";
				print();
			}
			else if(koma==7){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" 金将 ";
				print();
			}
			else if(koma==8){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" 王将 ";
				print();
			}
			else if(koma==9){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" と金 ";
				print();
			}
			else if(koma==10){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" 成香 ";
				print();
			}
			else if(koma==11){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" 成桂 ";
				print();
			}
			else if(koma==12){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" 成銀 ";
				print();
			}
			else if(koma==13){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" 龍馬 ";
				print();
			}
			else if(koma==14){
				player[y-1][x-1]=0;
				masu[y-1][x-1]=" 龍王 ";
				print();
			}
			else if(koma==0){
				player[y-1][x-1]=2;
				masu[y-1][x-1]="      ";
				print();
			}
			else printf("駒の入力情報が正しくありません\n");
		}
		else if(p==2){
			if(koma==1){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(歩兵)";
				print();
			}
			else if(koma==2){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(角行)";
				print();
			}
			else if(koma==3){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(飛車)";
				print();
			}
			else if(koma==4){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(香車)";
				print();
			}
			else if(koma==5){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(桂馬)";
				print();
			}
			else if(koma==6){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(銀将)";
				print();
			}
			else if(koma==7){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(金将)";
				print();
			}
			else if(koma==8){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(玉将)";
				print();
			}
			else if(koma==9){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(と金)";
				print();
			}
			else if(koma==10){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(成香)";
				print();
			}
			else if(koma==11){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(成桂)";
				print();
			}
			else if(koma==12){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(成銀)";
				print();
			}
			else if(koma==13){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(龍馬)";
				print();
			}
			else if(koma==14){
				player[y-1][x-1]=1;
				masu[y-1][x-1]="(龍王)";
				print();
			}
			else if(koma==0){
				player[y-1][x-1]=2;
				masu[y-1][x-1]="      ";
				print();
			}
			else printf("駒の入力情報が正しくありません\n");
		}else if(koma==0){
			player[y-1][x-1]=2;
			masu[y-1][x-1]="      ";
			print();
		}else printf("プレイヤーの入力情報が正しくありません\n");

		

		printf("訂正を続けますか(y/n)\n");
		scanf(" %c",&repeat);
		while(repeat!='y'&&repeat!='n'){
			printf("正しく入力してください\n");
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
		printf("  +------+------+------+------+------+------+------+------+------+\n\n\n\n");		//将棋盤の表示
}

void ura(int n){
	if(n==1){
		if(masu[y_1][x_1]==" 歩兵 ") masu[y_1][x_1]=" と金 ";
		else if(masu[y_1][x_1]==" 香車 ") masu[y_1][x_1]=" 成香 ";
		else if(masu[y_1][x_1]==" 桂馬 ") masu[y_1][x_1]=" 成桂 ";
		else if(masu[y_1][x_1]==" 銀将 ") masu[y_1][x_1]=" 成銀 ";
		else if(masu[y_1][x_1]==" 角行 ") masu[y_1][x_1]=" 龍馬 ";
		else if(masu[y_1][x_1]==" 飛車 ") masu[y_1][x_1]=" 龍王 ";
		else if(masu[y_1][x_1]=="(歩兵)") masu[y_1][x_1]="(と金)";
		else if(masu[y_1][x_1]=="(香車)") masu[y_1][x_1]="(成香)";
		else if(masu[y_1][x_1]=="(桂馬)") masu[y_1][x_1]="(成桂)";
		else if(masu[y_1][x_1]=="(銀将)") masu[y_1][x_1]="(成銀)";
		else if(masu[y_1][x_1]=="(角行)") masu[y_1][x_1]="(龍馬)";
		else if(masu[y_1][x_1]=="(飛車)") masu[y_1][x_1]="(龍王)";
	}
	else if(n==2){
		if(masu[y_2][x_2]==" 歩兵 ") masu[y_2][x_2]=" と金 ";
		else if(masu[y_2][x_2]==" 香車 ") masu[y_2][x_2]=" 成香 ";
		else if(masu[y_2][x_2]==" 桂馬 ") masu[y_2][x_2]=" 成桂 ";
		else if(masu[y_2][x_2]==" 銀将 ") masu[y_2][x_2]=" 成銀 ";
		else if(masu[y_2][x_2]==" 角行 ") masu[y_2][x_2]=" 龍馬 ";
		else if(masu[y_2][x_2]==" 飛車 ") masu[y_2][x_2]=" 龍王 ";
		else if(masu[y_2][x_2]=="(歩兵)") masu[y_2][x_2]="(と金)";
		else if(masu[y_2][x_2]=="(香車)") masu[y_2][x_2]="(成香)";
		else if(masu[y_2][x_2]=="(桂馬)") masu[y_2][x_2]="(成桂)";
		else if(masu[y_2][x_2]=="(銀将)") masu[y_2][x_2]="(成銀)";
		else if(masu[y_2][x_2]=="(角行)") masu[y_2][x_2]="(龍馬)";
		else if(masu[y_2][x_2]=="(飛車)") masu[y_2][x_2]="(龍王)";
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