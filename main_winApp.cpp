#include "main_winApp.h"

bool bDebug = true;

#ifdef SURV
HANDLE mainHandle = 0; 
#else
videoInput vi;
int device0 = 0;
#endif

bool bTx = false;
unsigned char *pTxBuf = 0;
unsigned txW = 0;
unsigned txH = 0;
unsigned txLen = 0;
unsigned txCh = 3;
bool bInitGL = false;

//Mtx *pMtxGray = 0;
unsigned char *pArrGray = 0;
unsigned char *pArrRgb = 0;
unsigned char *apNorm[NORM_NUM] = {0};
unsigned normW;
unsigned normH;
unsigned normC;

//std::vector<FF> arrFace;
//std::vector<Fea> arrFFea;
unsigned condition = 0;
std::mutex gMutex;

//std::vector<unsigned> aFId;
FaceCount fcount;

void WINAPI GetRawDataCallback( DWORD UserParam, DWORD dwdataType, BYTE* buf, DWORD len )
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

#ifdef SURV
void WINAPI GetImage_callback(DWORD UserParam, LPVOID b2, LPBITMAPINFO bmpinfo, LPBYTE buf, DWORD inLen, DWORD dwWidth, DWORD dwHeight )
{	
	
	gMutex.lock();
	bool bReturn = (condition != 0) ? true : false;
	gMutex.unlock();
	if (bReturn) {
		return;
	} else {}
	

	unsigned len = dwWidth * dwHeight * txCh;
	unsigned lenGray = dwWidth * dwHeight;
	MyAssert(len == inLen);

	if (!pTxBuf || len > txLen) {
		delete []pTxBuf;
		pTxBuf = new unsigned char[len]; 

		delete pArrGray;
		pArrGray = new unsigned char [dwWidth * dwHeight];
		cout << dwWidth << "x" << dwHeight << endl;
	} else {}

	//static Layer *pLyrIn = 0;
	if (txW != dwWidth || txH != dwHeight) {
		txW = dwWidth;
		txH = dwHeight;
		txLen = len;
		bInitGL = false;
	} else {}

	for (unsigned loc = 0; loc < len; loc += txCh) {
		pTxBuf[loc + 0] = buf[loc + 2];
		pTxBuf[loc + 1] = buf[loc + 1];
		pTxBuf[loc + 2] = buf[loc + 0];
	}
	//memcpy(pTxBuf, buf, len * sizeof(unsigned char));
	pArrRgb = pTxBuf;

	unsigned loc = 0;
	unsigned locG = 0;
	for (unsigned y = 0; y < txH; y++) {
		for (unsigned x = 0; x < txW; x++) {
			//pMtxGray->CellRef(x, y) = pTxBuf[loc + 0] * 0.30F +
			//						  pTxBuf[loc + 1] * 0.59F +
			//						  pTxBuf[loc + 2] * 0.11F;
			pArrGray[locG] = pTxBuf[loc + 0] * 0.30F +
							 pTxBuf[loc + 1] * 0.59F +
							 pTxBuf[loc + 2] * 0.11F;
			locG++;
			loc += txCh;
		}
	}

	//pTarget_win->redraw();
	gMutex.lock();
	condition = 1;
	gMutex.unlock();
}
#else
int GetImage_vi()
{	
	if (!vi.isFrameNew(device0)) {
		return -1;
	} else {}

	unsigned viW = vi.getWidth(device0);
	unsigned viH = vi.getHeight(device0);

	unsigned len = vi.getSize(device0);
	unsigned lenGray = viW * viH;

	if (!pTxBuf || len > txLen) {
		delete []pTxBuf;
		pTxBuf = new unsigned char[len]; 
		pArrRgb = pTxBuf;

		//delete pMtxGray;
		//pMtxGray = new Mtx(viW, viH);
		delete []pArrGray;
		pArrGray = new unsigned char [lenGray];
	} else {}

	//static Layer *pLyrIn = 0;
	if (txW != viW || txH != viH) {
		txW = viW;
		txH = viH;
		txLen = len;
		bInitGL = false;
	} else {}

	vi.getPixels(device0, pTxBuf, true, true);
	
	unsigned loc = 0;
	unsigned locG = 0;
	for (unsigned y = 0; y < txH; y++) {
		for (unsigned x = 0; x < txW; x++) {
			//pMtxGray->CellRef(x, y) = pTxBuf[loc + 0] * 0.30F +
			//						  pTxBuf[loc + 1] * 0.59F +
			//						  pTxBuf[loc + 2] * 0.11F;
			pArrGray[locG] = pTxBuf[loc + 0] * 0.30F +
							 pTxBuf[loc + 1] * 0.59F +
							 pTxBuf[loc + 2] * 0.11F;
			locG++;
			loc += txCh;
		}
	}

	//pTarget_win->redraw();
	return 0;
}
#endif

#ifdef SURV
void CloseConnect()
{
	if (bDebug) {
		cout << "CloseConnect" << endl;
	} else {}

	if (mainHandle) {
		KStop(mainHandle);
		KStopStreaming(mainHandle);
		KDisconnect(mainHandle);
		KCloseInterface(mainHandle);
		mainHandle = 0;
	} else {}
}
#else
void CloseConnect()
{
	if (bDebug) {
		cout << "CloseConnect" << endl;
	} else {}

	vi.stopDevice(device0);
}
#endif

void ResetCompute()
{
	pCalculate_litBttn->value(0);
	pCalculate_litBttn->deactivate();
}

#ifdef SURV
void RunConnect(Fl_Button *w, void *v) 
{
	//**********************************************
	// configuration
	//**********************************************
	MEDIA_CONNECTION_CONFIG2 *pMcc = 0;
	if (!pMcc) {
		pMcc = new MEDIA_CONNECTION_CONFIG2;
	} else {}

	string ipS0 = pIP0_txtIn->value();	
	string ipS1 = pIP1_txtIn->value();
	string ipS2 = pIP2_txtIn->value();
	string ipS3 = pIP3_txtIn->value();
	string ipS = ipS0 + "." + ipS1 + "." +ipS2 + "." + ipS3;
	string portS = pPort_txtIn->value();	int portNo = atoi(portS.c_str());
	string accountS = pAccount_txtIn->value();
	string passwdS = pPasswd_txtIn->value();

	if (bDebug) {
		cout << "ipS: " << ipS << endl
			 << "portNo: " << portNo << endl
			 << "accountS: " << accountS << endl
			 << "passwdS: " << passwdS << endl
			 << endl;
	} else {}

	memset((void*)pMcc, 0, sizeof(MEDIA_CONNECTION_CONFIG2));
	strcpy(pMcc->UniCastIP, ipS.c_str());
	sprintf(pMcc->MultiCastIP, "%d.%d.%d.%d\0", 228,5, 6,1 );
	pMcc->ContactType = CONTACT_TYPE_UNICAST_WOC_PREVIEW;

	pMcc->RegisterPort = 6000;
	pMcc->ControlPort = 6001;
	pMcc->StreamingPort = 6002;
	pMcc->HTTPPort = portNo;
	pMcc->ConnectTimeOut = 1;
	pMcc->MultiCastPort = 5000 ;
	pMcc->RTSPPort = 7070 ;
	strcpy(pMcc->UserID, accountS.c_str());
	strcpy(pMcc->Password, passwdS.c_str());
	pMcc->SearchPortC2S = 6005;
	pMcc->SearchPortS2C = 6006;

	pMcc->TCPVideoStreamID = 0;
	pMcc->RTPVideoTrackNumber = 1;
	pMcc->RTPAudioTrackNumber = 0;

	//**********************************************
	// conection
	//**********************************************
	CloseConnect();

	mainHandle = KOpenInterface();
	MEDIA_PORT_INFO portInfo;	
	bool bb = KGetPortInfoByHTTP(mainHandle, &portInfo, pMcc->UniCastIP, pMcc->HTTPPort, pMcc->UserID, pMcc->Password);		
	if (!bb) {
		//txtBuf.append("Connection fail\n");
		fl_alert("Connection fail");
		CloseConnect();
		return;
	} else {}

	DWORD UserParam = 0;	
	if (mainHandle) {
		if (pMcc->ContactType != CONTACT_TYPE_PLAYBACK &&
			pMcc->ContactType != CONTACT_TYPE_PLAYBACK_AVI) {
			KSetTCPType(mainHandle, 0);
		} else {}

		bool bMc2 = KSetMediaConfig2(mainHandle, pMcc);
		MyAssert(bMc2);

		ResetCompute();
		KSetRawDataCallback(mainHandle, UserParam, GetRawDataCallback);
		KSetImageCallback3 (mainHandle, UserParam, GetImage_callback);

		if (KConnect(mainHandle)) {
			if (KStartStreaming(mainHandle)) {
				KEnableDisplayTime(mainHandle, true);
				KPlay(mainHandle);
			} else {
				//txtBuf.append("Connection fail\n");
				fl_alert("Connection fail");
				CloseConnect();
				return;
			} 
		} else {
			//txtBuf.append("Connection fail\n");
			fl_alert("Connect fail");
			CloseConnect();
			return;
		} 
	} else {
		//if (bDebug) {
			MyAssert(0);
		//} else {}
	} // mainHandle

	pClose_litBttn->value(1);
	
	bTx = true;
	pClose_litBttn->activate();
	pConnect_bttn->deactivate();
}
#else
void RunConnect(Fl_Button *w, void *v) 
{
	CloseConnect();

	vi.setIdealFramerate(device0, 30);
	vi.setupDevice(device0, 1280, 720);
	//vi.setupDevice(device0, 320, 240);
	
	int vw = vi.getWidth(device0);
	int vh = vi.getHeight(device0);
	int vs = vi.getSize(device0);
	
	pClose_litBttn->value(1);
	
	bTx = true;
	pClose_litBttn->activate();
	pConnect_bttn->deactivate();
}
#endif

void ScaleDraw(Mtx &mtxOut, Mtx &mtxIn) 
{
	Vect2D<unsigned> dimOut = mtxOut.GetDim();
	Vect2D<unsigned> dimIn  = mtxIn.GetDim();

	DATA xR = (DATA)dimOut.m_x / dimIn.m_x;
	DATA yR = (DATA)dimOut.m_y / dimIn.m_y;
	DATA sR = (xR < yR) ? xR : yR;
	if (sR >= 1.F) {
		sR = 1.F;
	} else {}

	unsigned xLen = (unsigned)(dimIn.m_x * sR);
	unsigned yLen = (unsigned)(dimIn.m_y * sR);
	unsigned xOff = (dimOut.m_x - xLen) / 2;
	unsigned yOff = (dimOut.m_y - yLen) / 2;

	mtxOp.one.Gen(mtxOut);
	mtxOp.mul.Gen(mtxOut, 100.F);
	Mtx mtxScl(mtxOut, xOff, yOff, xLen, yLen);
	mtxOp.scaleDim.Gen(mtxScl, mtxIn);
}
void ComputeScore(Fl_Light_Button *w, void *v)
{
}

void Disconnect(Fl_Light_Button *w, void *v)
{
	ResetCompute();

	CloseConnect();
	pClose_litBttn->value(0);
	pClose_litBttn->deactivate();
	pConnect_bttn->activate();
	bTx = false;
}

//***********************************************
// MyGlWindow
//***********************************************
MyGlWindow::MyGlWindow(int x, int y, int w, int h, const char *l)
	: Fl_Gl_Window(x, y, w, h, l)
{}

int MyGlWindow::handle(int event)
{
	//redraw();
	return 0;
}

void MyGlWindow::draw()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluOrtho2D(0, w(), 0, h());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glViewport(0, 0, w(), h());

	glClearColor(0.9f, 0.9f, 0.9f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//if (pClose_litBttn->value() == 0) {
	if (!bTx) {
		glFlush();
		return;
	} else {}

	//*******************************************
	// draw
	//*******************************************

	static unsigned xOff = 0; 
	static unsigned yOff = 0;
	static unsigned xLen = 0;
	static unsigned yLen = 0;

	static GLuint txName;
	//static bool bInit = false;
	stringstream ss;
	string idS;
	string abS, aeS;
	if (!bInitGL) {
		bInitGL = true;

		glEnable(GL_TEXTURE_2D);
		glGenTextures(1, &txName);
		glBindTexture(GL_TEXTURE_2D, txName);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, txW, txH, 0, GL_BGR, GL_UNSIGNED_BYTE, pTxBuf);		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, txW, txH, 0, GL_RGB, GL_UNSIGNED_BYTE, pTxBuf);		

		DATA xR = (DATA)w() / txW;
		DATA yR = (DATA)h() / txH;
		DATA mR = (xR < yR) ? xR : yR;
		xLen = (unsigned)(txW * mR);
		yLen = (unsigned)(txH * mR);
		xOff = (w() - xLen) / 2;
		yOff = (h() - yLen) / 2;
	} else {		
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, txName);
		//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, txW, txH, GL_BGR, GL_UNSIGNED_BYTE, pTxBuf);		
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, txW, txH, GL_RGB, GL_UNSIGNED_BYTE, pTxBuf);		
	}

	static bool bInitNorm = false;
	static GLuint aTxNorm[NORM_NUM];
	if (apNorm[0] != 0) {
		if (!bInitNorm) {
			bInitNorm = true;
			//getchar();
			MyAssert(normC == 1 || normC == 3);

			glEnable(GL_TEXTURE_2D);
			for (unsigned n = 0; n < NORM_NUM; n++) {
				glGenTextures(1, &aTxNorm[n]);
				glBindTexture(GL_TEXTURE_2D, aTxNorm[n]);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //GL_NEAREST);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
				if (normC == 1) {
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, normW, normH, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, apNorm[n]);				
				} else if (normC == 3) {
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, normW, normH, 0, GL_RGB, GL_UNSIGNED_BYTE, apNorm[n]);	
				} else {
					MyAssert(0);
				}
			}
		} else {}
		//for (unsigned n = 0; n < NORM_NUM; n++) {
		FaceNode *pFn4 = fcount.GetBeginFace();
		unsigned txNo4 = 0;
		while(pFn4) {
			if (pFn4->m_unrefNum > 0) {
				pFn4 = fcount.GetNextFace();
				continue;
			} else {}

			glBindTexture(GL_TEXTURE_2D, aTxNorm[txNo4++]);
			//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, DIM_FACE.m_x, DIM_FACE.m_y, GL_RGB, GL_UNSIGNED_BYTE, m_pTxBuf);
			if (normC == 1) {
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, normW, normH, GL_LUMINANCE, GL_UNSIGNED_BYTE, pFn4->GetData());	

			} else if (normC == 3) {
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, normW, normH, GL_RGB, GL_UNSIGNED_BYTE, pFn4->GetData());	
			} else {
				MyAssert(0);
			}
			pFn4 = fcount.GetNextFace();
		}
	} else {}

	glViewport(xOff, yOff, xLen, yLen);
	glMatrixMode(GL_MODELVIEW);
	gluOrtho2D(0, txW - 1, 0, txH - 1);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txName);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 1.f);			glVertex2f(0,		0);			//glVertex2f(-1.f,	-1.f);			
		glTexCoord2f(1.f, 1.f);			glVertex2f(txW - 1, 0);			//glVertex2f(1.f,		-1.f);			
		glTexCoord2f(1.f, 0);			glVertex2f(txW - 1,	txH - 1);	//glVertex2f(1.f,		1.f);		
		glTexCoord2f(0, 0);				glVertex2f(0,		txH - 1);	//glVertex2f(-1.f,	1.f);			
	glEnd();
	glDisable(GL_TEXTURE_2D);

	/*
	//cout << arrFace.size() << endl;
	glColor4f(0, 1.f, 0, 1.f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	for (unsigned fa = 0; fa < arrFace.size(); fa++) {
		glVertex2f(arrFace[fa].m_0, arrFace[fa].m_1);
		glVertex2f(arrFace[fa].m_2, arrFace[fa].m_1);
		glVertex2f(arrFace[fa].m_2, arrFace[fa].m_3);
		glVertex2f(arrFace[fa].m_0, arrFace[fa].m_3);
	}
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	*/

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	FaceNode *pFn = fcount.GetBeginFace();
	while(pFn) {
		if (pFn->m_unrefNum > 0) {
			pFn = fcount.GetNextFace();
			continue;
		} else {}

		if (pFn->m_bMale) {
			glColor4f(0, 1.f, 0, 1.f);
		} else {
			glColor4f(1.f, 0, 0, 1.f);
		}

		glVertex2f(pFn->m_recL, txH - 1 - pFn->m_recB);
		glVertex2f(pFn->m_recR, txH - 1 - pFn->m_recB);
		glVertex2f(pFn->m_recR, txH - 1 - pFn->m_recT);
		glVertex2f(pFn->m_recL, txH - 1 - pFn->m_recT);
	
		pFn = fcount.GetNextFace();
	}
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDisable(GL_DEPTH_TEST);
	glLineWidth(2.f);
	FaceNode *pFn2 = fcount.GetBeginFace();
	while(pFn2) {
		if (pFn2->m_unrefNum > 0) {
			pFn2 = fcount.GetNextFace();
			continue;
		} else {}

		ss.str("");			ss.clear();		ss << pFn2->m_id;		ss >> idS;
		ss.str("");			ss.clear();		ss << pFn2->m_ageB;		ss >> abS;
		ss.str("");			ss.clear();		ss << pFn2->m_ageE;		ss >> aeS;

		glPushMatrix();
		glColor3f(1.f, 1.f, 1.f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBegin(GL_QUADS);
			glVertex2f(pFn2->m_recL,			 txH - 1 - pFn2->m_recB - 50.f);
			glVertex2f(pFn2->m_recL + 150.f, txH - 1 - pFn2->m_recB - 50.f);
			glVertex2f(pFn2->m_recL + 150.f, txH - 1 - pFn2->m_recB);
			glVertex2f(pFn2->m_recL,			 txH - 1 - pFn2->m_recB);
		glEnd();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(pFn2->m_recL, txH - 1 - pFn2->m_recB - 50.f, 0);
		glScalef(0.25f, 0.25f, 0.25f);

		glColor3f(0.8f, 0.4f, 0);
		for (unsigned s = 0; s < idS.size(); s++) {
			glutStrokeCharacter(GLUT_STROKE_ROMAN, idS.c_str()[s]);
		}
		glutStrokeCharacter(GLUT_STROKE_ROMAN, ':');

		glColor3f(0.04f, 0.15f, 0.89f);
		for (unsigned s = 0; s < abS.size(); s++) {
			glutStrokeCharacter(GLUT_STROKE_ROMAN, abS.c_str()[s]);
		}
		glutStrokeCharacter(GLUT_STROKE_ROMAN, '-');
		for (unsigned s = 0; s < aeS.size(); s++) {
			glutStrokeCharacter(GLUT_STROKE_ROMAN, aeS.c_str()[s]);
		}
		glPopMatrix();

		pFn2 = fcount.GetNextFace();
	}
	glEnable(GL_DEPTH_TEST);

	/*
	glColor4f(1.f, 0, 0, 1.f);
	glPointSize(3.0);
	glBegin(GL_POINTS);
	for (unsigned fa = 0; fa < arrFFea.size(); fa++) {
		for (unsigned fea = 0; fea < 68; fea++) {
			glVertex2f(arrFFea[fa].m_x[fea], arrFFea[fa].m_y[fea]);
		}
	}
	glEnd();
	*/

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	FaceNode *pFn3 = fcount.GetBeginFace();
	unsigned txNo3 = 0;
	while(pFn3) {
		if (pFn3->m_unrefNum > 0) {
			pFn3 = fcount.GetNextFace();
			continue;
		} else {}

		unsigned nLen = 100;
		
		glPushMatrix();
		glTranslatef(pFn3->m_recR, txH - 1 - pFn3->m_recB, 0);

		glBindTexture(GL_TEXTURE_2D, aTxNorm[txNo3++]);
		glBegin(GL_QUADS);
			glTexCoord2f(0,		1.f - 1.f);			glVertex2f(0,		0);					
			glTexCoord2f(1.f,	1.f - 1.f);			glVertex2f(nLen,	0);					
			glTexCoord2f(1.f,	1.f - 0);			glVertex2f(nLen,	nLen);		
			glTexCoord2f(0,		1.f - 0);			glVertex2f(0,		nLen);				
		glEnd();
		glPopMatrix();
				
		pFn3 = fcount.GetNextFace();
	}
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	glFlush();
}
