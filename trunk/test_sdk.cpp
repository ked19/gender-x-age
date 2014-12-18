#include "test_sdk.h"

/*
bool bDebug2 = false;

HANDLE mainHandle2 = 0; 

unsigned bufImg_num2 = 3;
unsigned bufImgFnt_no2 = 0;
unsigned bufImgBck_idx2 = 0;
unsigned sleepS2 = 1000; //250;



void WINAPI GetRawDataCallback2( DWORD UserParam, DWORD dwdataType, BYTE* buf, DWORD len )
{
	// TYPE
	// 1. MPEG4
	// 2. AUDIO PCM
	// 3. AUDIO PCM must with time stamp
	// 4. MJPEG
	// 5. H.264
	// 6. AUDIO G711 mu-law
	// 7. Audio G711 a law
	if(dwdataType==1 || dwdataType==4 || dwdataType==5){
	}
	//cout << len ;
}

void WINAPI GetImage_callback2(DWORD UserParam, LPVOID b2, LPBITMAPINFO bmpinfo, LPBYTE buf, DWORD len, DWORD dwWidth, DWORD dwHeight )
{
	cout << "len:" << len << endl;
	cout << "dwWidth:" << dwWidth << endl;
	cout << "dwHeight: " << dwHeight << endl;

	static char aNo[10];

	MyAssert(bufImg_num2 >= 1);
	unsigned maxFntNo = bufImg_num2;
	string imgName;
	if (bufImgFnt_no2 < maxFntNo) {
		imgName = string("fnt_") + itoa(bufImgFnt_no2, aNo, 10);
		bufImgFnt_no2++;
	} else if (bufImgFnt_no2 == maxFntNo) {
		imgName = string("bck_") + itoa(bufImgBck_idx2, aNo, 10);
		bufImgBck_idx2 = (bufImgBck_idx2 + 1) % bufImg_num2;

		if (bufImgBck_idx2 == 0) {
			//pCalculate_litBttn->value(1);
		} else {}
	} else {
		MyAssert(0);
	}
	imgName += ".pgm"; //".bmp";

	static Layer lyrIn(dwWidth, dwHeight, 3);
	unsigned count = 0;
	for (unsigned y = 0; y < dwHeight; y++) {
		unsigned yy = dwHeight - 1 - y;
		for (unsigned x = 0; x < dwWidth; x++) {
			for (unsigned c = 0; c < 3; c++) {
				lyrIn.CellRef(x, yy, c) = (DATA)buf[count++];
			} // c
		} // x
	} // y
	static Mtx *pMtxIn = MyImg(lyrIn).ConvertGray()->GetMtx(0);
	imgIO.Write(imgName, MyImg(*pMtxIn));

	Sleep(sleepS2);

	cout << "*******************************" << endl;
}

void CloseConnect2()
{
	if (bDebug2) {
		cout << "CloseConnect" << endl;
	} else {}

	if (mainHandle2) {
		KStop(mainHandle2);
		KStopStreaming(mainHandle2);
		KDisconnect(mainHandle2);
		KCloseInterface(mainHandle2);
		mainHandle2 = 0;
	} else {}
}

void ResetCompute2()
{
	bufImgFnt_no2 = 0;
	bufImgBck_idx2 = 0;
	//pCalculate_litBttn->value(0);
}

void test_CSDK()
{


	//**********************************************
	// configuration
	//**********************************************
	MEDIA_CONNECTION_CONFIG2 mcc;
	//if (!pMcc) {
	//	pMcc = new MEDIA_CONNECTION_CONFIG2;
	//} else {}





	memset((void*)&mcc, 0, sizeof(MEDIA_CONNECTION_CONFIG2));
strcpy(mcc.UniCastIP,"172.16.15.243");
	sprintf(mcc.MultiCastIP, "%d.%d.%d.%d\0", 228,5, 6,1 );
	mcc.ContactType = CONTACT_TYPE_UNICAST_WOC_PREVIEW;

	mcc.RegisterPort = 6000;
	mcc.ControlPort = 6001;
	mcc.StreamingPort = 6002;
	mcc.HTTPPort = 80;
	mcc.ConnectTimeOut = 1;
	mcc.MultiCastPort = 5000 ;
	mcc.RTSPPort = 7070 ;
sprintf(mcc.UserID,"admin");
sprintf(mcc.Password,"123456");
	mcc.SearchPortC2S = 6005;
	mcc.SearchPortS2C = 6006;

	mcc.TCPVideoStreamID = 0;
	mcc.RTPVideoTrackNumber = 1;
	mcc.RTPAudioTrackNumber = 0;

	//**********************************************
	// conection
	//**********************************************
	//CloseConnect2();

	mainHandle2 = KOpenInterface();
	DWORD UserParam = 0;
	if (mainHandle2) {
		if (mcc.ContactType != CONTACT_TYPE_PLAYBACK &&
			mcc.ContactType != CONTACT_TYPE_PLAYBACK_AVI) {
			KSetTCPType(mainHandle2, 0);
		} else {}

		bool bMc2 = KSetMediaConfig2(mainHandle2, &mcc);
		MyAssert(bMc2);

		ResetCompute2();
		KSetRawDataCallback(mainHandle2, UserParam, GetRawDataCallback2 );
		KSetImageCallback3 (mainHandle2, UserParam, GetImage_callback2);

		if (KConnect(mainHandle2)) {
			if (KStartStreaming(mainHandle2)) {
				KEnableDisplayTime(mainHandle2, true);
				KPlay(mainHandle2);
			} else {
				if (bDebug2) {
					MyAssert(0);
				} else {}
			} 
		} else {} 
	} else {
		if (bDebug2) {
			MyAssert(0);
		} else {}
	} // mainHandle

	while(1);
}
*/



void WINAPI GetRawDataCallback2( DWORD UserParam, DWORD dwdataType, BYTE* buf, DWORD len )
{
// TYPE
// 1. MPEG4
// 2. AUDIO PCM
// 3. AUDIO PCM must with time stamp
// 4. MJPEG
// 5. H.264
// 6. AUDIO G711 mu-law
// 7. Audio G711 a law
if(dwdataType==1 || dwdataType==4 || dwdataType==5){

}

}


void WINAPI GetImageDataCallback(DWORD UserParam, LPVOID b2, LPBITMAPINFO bmpinfo, LPBYTE buf, DWORD len, DWORD dwWidth, DWORD dwHeight )
{
	unsigned skipNum = 30;
	static unsigned sNo = 0;
	if (sNo < skipNum) {
		sNo++;
		return;
	} else {
		sNo = 0;
	}

	cout << "len:" << len << endl;
	cout << "dwWidth:" << dwWidth << endl;
	cout << "dwHeight: " << dwHeight << endl;

	static Layer lyrIn(dwWidth, dwHeight, 3);
	unsigned count = 0;
	for (unsigned y = 0; y < dwHeight; y++) {
		for (unsigned x = 0; x < dwWidth; x++) {
			//for (unsigned c = 0; c < 3; c++) {
			//	lyrIn.CellRef(x, y, c) = (DATA)buf[count++];
			//} // c
			lyrIn.CellRef(x, y, 2) = (DATA)buf[count++];
			lyrIn.CellRef(x, y, 1) = (DATA)buf[count++];
			lyrIn.CellRef(x, y, 0) = (DATA)buf[count++];
		} // x
	} // y
	imgIO.Write("in.jpg", MyImg(lyrIn));

	cout << "*******************************" << endl;

	cout << "getchar: ";
	//getchar();
}


void test_CSDK()
{
	MEDIA_CONNECTION_CONFIG2 mcc ;
	HANDLE h = KOpenInterface();
	DWORD UserParam = 0;

	memset((void*)&mcc,0,sizeof(MEDIA_CONNECTION_CONFIG2));
	strcpy(mcc.UniCastIP,"172.16.15.243");
	//strcpy(mcc.UniCastIP,"172.16.15.208");

	sprintf( mcc.MultiCastIP, "%d.%d.%d.%d\0", 228,5, 6,1 );

	mcc.ContactType =CONTACT_TYPE_UNICAST_WOC_PREVIEW;

	mcc.RegisterPort = 6000 ;
	mcc.ControlPort = 6001 ;
	mcc.StreamingPort = 6002 ;
	mcc.HTTPPort = 80 ;
	mcc.ConnectTimeOut = 1 ;
	mcc.MultiCastPort = 5000 ;
	mcc.RTSPPort = 7070 ;
	//sprintf(mcc.UserID,"Admin");
	//sprintf(mcc.Password,"ivs123456");
	sprintf(mcc.UserID,"admin");
	sprintf(mcc.Password,"123456");
	mcc.SearchPortC2S = 6005;
	mcc.SearchPortS2C = 6006;

	mcc.TCPVideoStreamID = 0;
	mcc.RTPVideoTrackNumber =  1;
	mcc.RTPAudioTrackNumber = 0;



	if(h){
		if( mcc.ContactType != CONTACT_TYPE_PLAYBACK && mcc.ContactType != CONTACT_TYPE_PLAYBACK_AVI  ) {
			KSetTCPType( h, 0 );
		}

		if(KSetMediaConfig2( h, &mcc )==false) { 
			int aa = 0 ;
		}

		KSetRawDataCallback( h,UserParam , GetRawDataCallback2 );

		KSetImageCallback3(  h, UserParam, GetImageDataCallback );

		if( KConnect( h ) ) {
			if( KStartStreaming( h ) ) {             
                KEnableDisplayTime(h,true);
				KPlay( h );               
			}
		}

		while(1){
			//Sleep(1000);
		}

		if(h) {
			KStop(h);
			KStopStreaming(h);
			KDisconnect(h);
			KCloseInterface(h);
		}
	}
}
