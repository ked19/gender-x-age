#include "main.h"

void EqualHis(unsigned char *pIn, unsigned char *pMsk)
{
	static Mtx mIn(100, 100);
	static Mtx mMsk(100, 100);
	unsigned loc = 0;
	for (unsigned y = 0; y < 100; y++) {
		for (unsigned x = 0; x < 100; x++) {
			mIn.CellRef(x, y) = pIn[loc];
			mMsk.CellRef(x, y) = pMsk[loc];
			loc++;
		}
	}
	mtxOp.hisEqual.Gen(mIn, 255, &mMsk);
	//mtxOp.hisEqual.Gen(mIn, 256, &mMsk);
	unsigned loc2 = 0;
	for (unsigned y = 0; y < 100; y++) {
		for (unsigned x = 0; x < 100; x++) {
			pIn[loc2++] = mIn.CellRef(x, y);
		}
	}
}

/*
std::vector<rectangle> DetectFace( bool bDebug)
{
	//if (!pMtxGray) {
	if (!pArrGray) {
		std::vector<rectangle> fTmp;
		fTmp.clear();
		return fTmp;
	} else {}

	static frontal_face_detector fd = get_frontal_face_detector();
	static array2d<unsigned char> arrGray(txH, txW);
	unsigned loc = 0;
	for (unsigned y = 0; y < txH; y++) {
		for (unsigned x = 0; x < txW; x++) {
			//arrGray[y][x] = (unsigned char)pMtxGray->CellVal(x, y);
			arrGray[y][x] = pArrGray[loc++];
		}
	}
	//pyramid_up(arrGray);

	std::vector<rectangle> fRect = fd(arrGray);
	return fRect;

	/*
	static shape_predictor sp;
	static bool bSpInit = false;
	if (!bSpInit) {
		cout << "load lamdmarks" << endl;
		bSpInit = true;
		deserialize("shape_predictor_68_face_landmarks.dat") >> sp;
	} else {}

	arrFace.clear();
	arrFFea.clear();
	unsigned fNum = fRect.size();
	if (fNum > 0) {
		gMutex.lock();
		bDetect = true;
		gMutex.unlock();
	} else {
		return;
	}

	for (unsigned f = 0; f < fNum; f++) {
		FF vTmp; 
		vTmp.m_0 = (DATA)fRect[f].left();
		vTmp.m_1 = (DATA)(txH - 1 - fRect[f].bottom()); 
		vTmp.m_2 = (DATA)(vTmp.m_0 + fRect[f].width()); 
		vTmp.m_3 = (DATA)(vTmp.m_1 + fRect[f].height()); 
		arrFace.push_back(vTmp);

		full_object_detection shape = sp(arrGray, fRect[f]);
		MyAssert(shape.num_parts() == 68);
		Fea fTmp;
		for (unsigned fea = 0; fea < shape.num_parts(); fea++) {
			point pTmp = shape.part(fea);
			fTmp.m_x[fea] = pTmp.x();
			fTmp.m_y[fea] = txH - 1 - pTmp.y();
		}
		arrFFea.push_back(fTmp);

		cout << fRect[f].bottom() << " " << shape.part(0).y() << " " << all_parts_in_rect(shape) << endl;
	}
	pTarget_win->redraw();

	gMutex.lock();
	bDetect = false;
	gMutex.unlock();
}
*/

void SaveImg2(unsigned char *pIn) 
{
	static Mtx mtxOut(100, 100);
	unsigned loc = 0;
	for (unsigned y = 0; y < 100; y++) {
		for (unsigned x = 0; x < 100; x++) {
			mtxOut.CellRef(x, y) = pIn[loc++];
		}
	}
	imgIO.Write("out.jpg", MyImg(mtxOut));
}

void main() 
{
	/*
	bool b = true;
	bool c = (b == true);
	cout << c << endl;
	getchar();
	return;
	*/

	bool bDebug = true;
	//FaceFeature ff;
	HeadPose hp;
	for (unsigned n = 0; n < NORM_NUM; n++) {
		hp.GetMemory(&apNorm[n], 1, normW, normH, normC); 
	}
	unsigned char *pMsk = new unsigned char [normW * normH];
	//hp.GetMask(pWW);
	//SaveImg2(pWW);

	MyFace myf;

	const unsigned CFACE_LEN = 50;
	const unsigned FCACHE_NUM = 20;
	//FaceCount fcount;
	std::vector<FaceNode *> aFIn;
	aFIn.clear();
	for (unsigned f = 0; f < FCACHE_NUM; f++) {
		FaceNode *pFn = new FaceNode(100, 100);
		aFIn.push_back(pFn);
	}

	GenderAge ga;

	//*******************************************
	// window
	//*******************************************
	ifstream ifs;
	ifs.open("version.txt");
	if (ifs.fail()) {
		MyAssert(0);
	} else {}
	
	string sLine;
	getline(ifs, sLine);
	string title = "Face recognition " + sLine;
	
	getline(ifs, sLine);
	bool bSetDefault = (sLine.compare("set default") == 0) ?
		true : false;

	fl_message_icon()->hide();

	pMain_win = make_window();
	pMain_win->show();
	pMain_win->label(title.c_str());
	if (!bSetDefault) {
		pIP0_txtIn->value("");
		pIP1_txtIn->value("");
		pIP2_txtIn->value("");
		pIP3_txtIn->value("");
		pAccount_txtIn->value("");
		pPasswd_txtIn->value("");
	} else {}

	pTarget_win->show();

	//*******************************************
	// heatmap
	//*******************************************
	Mtx mtxHpM(1280, 720);
	Mtx mtxHpF(1280, 720);
	mtxOp.zero.Gen(mtxHpM);
	mtxOp.zero.Gen(mtxHpF);

	//*******************************************
	// looping
	//*******************************************
	//while(Fl::wait(0.05)) {
	while(Fl::wait(0)) {
		if (!Fl::first_window()) {
			break;
		} else {}

#ifdef SURV
#else
		int bVi = GetImage_vi();
		if (bVi != 0) {
			continue;
		} else {}
#endif

		if (!bTx) {
			continue;
		} else {}

#ifdef SURV
		gMutex.lock();
		bool bContinue = (condition != 1) ? true : false;
		gMutex.unlock();
		if (bContinue) {
			continue;
		} else {}
#else
#endif

		static unsigned count = 0;
		static time_t tBeg;
		static time_t tEnd;
		if (count == 0) {
			tBeg = time(0);
		} else {}
		count++;
		if (count >= 50) {
			tEnd = time(0);
			//time_t tDiff = tEnd - tBeg;
			cout << "********************************************************************************" << endl;
			cout << "processing time: " << difftime(tEnd, tBeg) / 49.f * 1000 << "ms" << endl;
			cout << "********************************************************************************" << endl;

			count = 0;
		} else {}

		std::vector<rectangle> fRect; // = DetectFace(false); //true);	
		//myf.Detect(fRect, pArrGray, txW, txH, 1, txW, txH, true);
		
		
		const unsigned SK_SCL = 8;
		static Mtx mtxSkin(txW / SK_SCL, txH / SK_SCL);
		static Mtx mtxLab (txW / SK_SCL, txH / SK_SCL);
		myf.Detect(fRect, pArrGray, pArrRgb, mtxSkin, mtxLab, SK_SCL, txW, txH);
		

		/*
		arrFace.clear();
		for (unsigned f = 0; f < fRect.size(); f++) {
			//fRect[f].bottom() = txH - 1 - fRect[f].bottom();
			//fRect[f].top() = txH - 1 - fRect[f].top();

			if (bDebug) {
				FF vTmp; 
				vTmp.m_0 = (DATA)fRect[f].left();
				vTmp.m_1 = (DATA)(txH - 1 - fRect[f].bottom()); 
				vTmp.m_2 = (DATA)fRect[f].right();; 
				vTmp.m_3 = (DATA)(txH - 1 - fRect[f].top()); 
				arrFace.push_back(vTmp);
			} else {}
		}
		//continue;
		*/

		/*
		arrFFea.clear();
		for (unsigned f = 0; f < fRect.size(); f++) {
			FFeaLoc feaLoc;
			ff.Detect(feaLoc, pArrRgb, txW, txH, 3, fRect[f].left(), fRect[f].right(), fRect[f].bottom(), fRect[f].top());

			Fea ffTmp;
			for (unsigned fea = 0; fea < FFeaLoc::FEA_NUM; fea++) {
				ffTmp.m_x[fea] = feaLoc.m_x[fea];
				ffTmp.m_y[fea] = txH - 1 - feaLoc.m_y[fea];
			}
			arrFFea.push_back(ffTmp);
		}
		*/
		static bool bMsk = false;
		if (!bMsk) {
			hp.GetMask(pMsk);
			bMsk = true;

			ga.SetMsk(pMsk, normW, normH);
		} else {}
		unsigned fNum = (fRect.size() < NORM_NUM) ? fRect.size() : NORM_NUM;
		for (unsigned f = 0; f < fNum; f++) {
			hp.Normalize(apNorm[f], normC, pArrRgb, txW, txH, 3, fRect[f].left(), fRect[f].right(), fRect[f].bottom(), fRect[f].top(), false); //true);
			//hp.Normalize(apNorm[f], pArrGray, txW, txH, 1, fRect[f].left(), fRect[f].right(), fRect[f].bottom(), fRect[f].top(), false); //true);
			EqualHis(apNorm[f], pMsk);
			//mtxOp.hisEqual.Gen(apNorm[f], 10000, pMsk);
		}
		
		MyAssert(fRect.size() <= FCACHE_NUM);
		for (unsigned f = 0; f < fNum; f++) {
			//bool bM;
			//unsigned aB, aE;
			//ga.Detect(bM, aB, aE, apNorm[f], 100, 100, 1, false);

			aFIn[f]->CopyFrom(apNorm[f], 100, 100, normC);
			//aFIn[f]->m_bMale = bM;
			//aFIn[f]->m_ageB = aB;
			//aFIn[f]->m_ageE = aE;
			aFIn[f]->m_recL = fRect[f].left();
			aFIn[f]->m_recR = fRect[f].right();
			aFIn[f]->m_recB = fRect[f].bottom();
			aFIn[f]->m_recT = fRect[f].top();
		}
	
		fcount.bDebug = true;
		fcount.NextFrame();
		//aFId.clear();
		for (unsigned f = 0; f < fRect.size(); f++) {
			unsigned id = fcount.Renew(&aFIn[f]);
			//aFId.push_back(id);
		}
		
		FaceNode *pFn = fcount.GetBeginFace();
		while(pFn) {
			if (pFn->m_unrefNum > 0) {
				pFn = fcount.GetNextFace();
				continue;
			} else {}

			bool bM;
			unsigned aB, aE;
			DATA maxProb = ga.Detect(bM, aB, aE, pFn->GetData(), 100, 100, 1, false);
			//cout << pFn->m_maxProb << " " << maxProb << endl;
			if (maxProb > pFn->m_maxProb) {
				pFn->m_bMale = bM;
				pFn->m_ageB = aB;
				pFn->m_ageE = aE;
				pFn->m_maxProb = maxProb;
			} else {}
			//pFn->m_bMale = bM;

			//***********************************
			// heatmap
			//***********************************
			//cout << "aaaaaaaaaa: " << pFn->m_recB << " " << pFn->m_recT << endl;
			if (pFn->m_bMale) {
				for (unsigned y = pFn->m_recT; y <= pFn->m_recB; y++) {
					for (unsigned x = pFn->m_recL; x <= pFn->m_recR; x++) {
						mtxHpM.CellRef(x, y) += 1.F;
					}
				}
			} else {
				for (unsigned y = pFn->m_recT; y <= pFn->m_recB; y++) {
					for (unsigned x = pFn->m_recL; x <= pFn->m_recR; x++) {
						mtxHpF.CellRef(x, y) += 1.F;
					}
				}
			}

			pFn = fcount.GetNextFace();
		}

		pTarget_win->redraw();

#ifdef SURV
		gMutex.lock();
		condition = 0;
		gMutex.unlock();
#else
#endif

		DATA vMax, vMin;
		mtxOp.rng.Gen(vMin, vMax, mtxHpM);
		if (vMax < 1.F) {
			vMax = 1.F;
		} else {}
		mtxOp.mul.Gen(mtxHpM, 255.F / vMax);
		imgIO.Write("hpM.jpg", MyImg(mtxHpM));
		mtxOp.mul.Gen(mtxHpM, vMax / 255.F);

		mtxOp.rng.Gen(vMin, vMax, mtxHpF);
		if (vMax < 1.F) {
			vMax = 1.F;
		} else {}
		mtxOp.mul.Gen(mtxHpF, 255.F / vMax);
		imgIO.Write("hpF.jpg", MyImg(mtxHpF));
		mtxOp.mul.Gen(mtxHpF, vMax / 255.F);
	}

	//Fl::run();
	CloseConnect();	
}