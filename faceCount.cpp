#include "faceCount.h"
#include "denseMatrix.h"
#include "imageIO.h"

void SaveImg(unsigned char *pIn) 
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

unsigned ComputeAvg(unsigned char *pD, unsigned size, unsigned char *pMsk)
{
	unsigned sum = 0;
	unsigned count = 0;
	for (unsigned i = 0; i < size; i++) {
		if (pMsk[i] != 0) {
			sum += pD[i];
			count++;
		} else {}
	}
	MyAssert(count > 0);
	unsigned char avg = (unsigned char)(sum / count);
	return avg;
}

DATA ComputeSD(unsigned char *pD, unsigned size, unsigned avg, unsigned char *pMsk)
{
	DATA sd = 0;
	for (unsigned i = 0; i < size; i++) {
		if (pMsk[i] != 0) {
			int dis = (int)pD[i] - avg;
			sd += dis * dis;
		} else {}
	} 
	return sqrt(sd);
}

FaceNode::FaceNode(unsigned iW, unsigned iH, unsigned char *pMsk) 
	: m_dim(iW, iH)
	, m_unrefNum(0), m_dispNum(0)
	, m_bMale(true)
	, m_ageB(0), m_ageE(0), m_maxProb(0)
	, m_recL(0), m_recR(0), m_recB(0), m_recT(0)
	, m_pMsk(pMsk)
	, m_bSmile(false)
{
	unsigned size = iW * iH;
	m_pData = new unsigned char [size];

	for (unsigned i = 0; i < size; i++) {
		m_pData[i] = 0;
	}
	
	m_avg = 0;
	m_sd = 0;
}

FaceNode::FaceNode(FaceNode &fnIn) 
	: m_dim(fnIn.m_dim.m_x, fnIn.m_dim.m_y)
	, m_unrefNum(0), m_dispNum(0)
	, m_avg(fnIn.m_avg), m_sd(fnIn.m_sd)
	, m_bMale(fnIn.m_bMale)
	, m_ageB(fnIn.m_ageB), m_ageE(fnIn.m_ageE), m_maxProb(0)
	, m_recL(fnIn.m_recL), m_recR(fnIn.m_recR)
	, m_recB(fnIn.m_recB), m_recT(fnIn.m_recT)
	, m_bSmile(true)
{
	unsigned size = m_dim.m_x * m_dim.m_y;
	m_pData = new unsigned char [size];

	for (unsigned i = 0; i < size; i++) {
		m_pData[i] = fnIn.m_pData[i];
	}
}

FaceNode::~FaceNode()
{
	delete [] m_pData;
}

void FaceNode::CopyFrom(unsigned char *pIn, unsigned iW, unsigned iH, unsigned iC, unsigned char *pMsk)
{
	bool bDebug = false;

	MyAssert(iW == m_dim.m_x &&
			 iH == m_dim.m_y);
	MyAssert(iC == 1 || iC == 3);

	m_unrefNum = 0;
	m_dispNum = 0;

	unsigned size = iW * iH;
	if (iC == 1) {
		for (unsigned i = 0; i < size; i++) {
			m_pData[i] = pIn[i];
		}
	} else if (iC == 3) {
		unsigned ii = 0;
		for (unsigned i = 0; i < size; i++) {
			m_pData[i] = (unsigned char)(pIn[ii + 0] * 0.30F
									   + pIn[ii + 1] * 0.59F
									   + pIn[ii + 2] * 0.11F);
			ii += 3;
		}
	} else {
		MyAssert(0);
	}
	m_pMsk = pMsk;
	m_avg = ComputeAvg(m_pData, size, m_pMsk);
	m_sd =  ComputeSD(m_pData, size, m_avg, m_pMsk);
	if (bDebug) {
		cout << "avg: " << (unsigned)m_avg << endl;
		cout << "sd: " << m_sd << endl;
	} else {}
	//SaveImg(m_pData);
}

unsigned char FaceNode::GetAvg()
{
	return m_avg;
}

DATA FaceNode::GetSD()
{
	return m_sd;
}

unsigned char* FaceNode::GetData()
{
	return m_pData;
}

DATA FaceNode::Correlate(FaceNode &fnIn)
{
	Vect2D<unsigned> &dimIn = fnIn.m_dim;
	MyAssert(dimIn.m_x == m_dim.m_x &&
			 dimIn.m_y == m_dim.m_y);

	DATA sdIn = fnIn.m_sd;
	MyAssert(sdIn >= 1 && m_sd >= 1);

	DATA invSd = 1.F / (m_sd * sdIn);
	
	int corrSum = 0;
	unsigned size = dimIn.m_x * dimIn.m_y;
	unsigned avgIn = fnIn.m_avg;
	for (unsigned i = 0; i < size; i++) {
		if (m_pMsk[i] != 0) {
			int disNow = (int)m_pData[i] - m_avg;
			int disIn  = (int)fnIn.m_pData[i] - avgIn;
			corrSum += disNow * disIn;
		} else {}
	}
	//cout << corrSum << endl;
	DATA corr = corrSum * invSd;
	return corr;
}

void FaceNode::Update(FaceNode &fnIn, bool bNew) 
{
	float inScl = (bNew) ? 1.f : 0.1f;

	Vect2D<unsigned> &dimIn = fnIn.m_dim;
	MyAssert(dimIn.m_x == m_dim.m_x &&
			 dimIn.m_y == m_dim.m_y);

	unsigned size = dimIn.m_x * dimIn.m_y;
	for (unsigned i = 0; i < size; i++) {
		//m_pData[i] = (m_pData[i] * m_dispNum + fnIn.m_pData[i]) / (m_dispNum + 1);
		//m_pData[i] = (unsigned char)(m_pData[i] * 0.2f + fnIn.m_pData[i] * 0.8f); // / (m_dispNum + 1);
		//m_pData[i] = (unsigned char)(m_pData[i] * 0.5f + fnIn.m_pData[i] * 0.5f); // / (m_dispNum + 1);
		
		//m_pData[i] = (unsigned char)(m_pData[i] * 0.1f + fnIn.m_pData[i] * 0.9f); // / (m_dispNum + 1);
		m_pData[i] = (unsigned char)(m_pData[i] * (1.f - inScl) + fnIn.m_pData[i] * inScl); // / (m_dispNum + 1);
	}
	m_pMsk = fnIn.m_pMsk;
	m_dispNum++;
	m_avg = ComputeAvg(m_pData, size, m_pMsk);
	m_sd =  ComputeSD(m_pData, size, m_avg, m_pMsk);

	m_recL = fnIn.m_recL;
	m_recR = fnIn.m_recR;
	m_recB = fnIn.m_recB;
	m_recT = fnIn.m_recT;

	m_bSmile = fnIn.m_bSmile;

	//m_bMale = fnIn.m_bMale;
	//m_ageB = fnIn.m_ageB;
	//m_ageE = fnIn.m_ageE;
	//m_maxProb = fnIn.m_maxProb;
}

//*************************************************************************************************
//
//*************************************************************************************************

const unsigned FaceCount::LIST_LEN = 20;
const unsigned FaceCount::FACE_LEN = 100;
//const unsigned FaceCount::UNREF_MAX = 60;
const unsigned FaceCount::UNREF_MAX = 180;
const unsigned FaceCount::ID_MAX = UINT_MAX / 2;

FaceCount::FaceCount()
{
	m_pFList.clear();

	m_apUnuse.clear();
	for (unsigned a = 0; a < LIST_LEN; a++) {
		FaceNode *pFn = new FaceNode(FACE_LEN, FACE_LEN, 0);
		m_apUnuse.push_back(pFn);
	}

	bDebug = false;
}

FaceCount::~FaceCount()
{
	while (!m_pFList.empty()) {
		list<FaceNode *>::iterator it = m_pFList.begin();
		delete *it;
		m_pFList.pop_front();
	}

	for (unsigned a = 0; a < m_apUnuse.size(); a++) {
		delete m_apUnuse[a];
	}
}

FaceNode* FaceCount::GetBeginFace()
{
	m_itFace = m_pFList.begin();
		if (m_itFace == m_pFList.end()) {
		return 0;
	} else {}
	return *m_itFace;
}

FaceNode* FaceCount::GetNextFace()
{
	++m_itFace;
	if (m_itFace == m_pFList.end()) {
		return 0;
	} else {}
	return *m_itFace;
}

FaceNode* FaceCount::GetMatch(FaceNode &fnIn) 
{
	const DATA CORR_HIGH = 0.9F; //0.9F; //0.95F; 
	const DATA CORR_LOW  = 0.8F; //0.9F; //0.7F; //0.9F; //0.5F; //0.6F; //0.8F;

	DATA coMax = -1e10;
	list<FaceNode *>::iterator itMax;

	for (list<FaceNode *>::iterator it = m_pFList.begin(); it != m_pFList.end(); ++it) {
		if ((*it)->m_unrefNum == 0) {
			continue;
		} else {}

		DATA corr = (*it)->Correlate(fnIn);
		if (bDebug) {
			cout << (*it)->m_id << ": "
				 << (*it)->m_dispNum << " "
				 << (int)(*it)->GetAvg() << ", "
				 << (*it)->GetSD() << ", "
				 << corr << endl;
		} else {}

		if (corr >= CORR_HIGH) {
			FaceNode *pTmp = *it;
			m_pFList.erase(it);
			return pTmp;
		} else {}

		if (corr > coMax) {
			coMax = corr;
			itMax = it;
		} else {}	
	}

	if (coMax > CORR_LOW) {
		FaceNode *pTmp = *itMax;
		m_pFList.erase(itMax);
		return pTmp;
	} else {
		return 0;
	}
}

void FaceCount::Add(FaceNode *pFn)
{
	MyAssert(pFn->m_unrefNum == 0);

	list<FaceNode *> ::iterator it;
	for (it = m_pFList.begin(); it != m_pFList.end(); ++it) {
		if ((*it)->m_unrefNum > pFn->m_unrefNum) {
			break;
		} else if ((*it)->m_unrefNum == pFn->m_unrefNum &&
				   (*it)->m_unrefNum >= pFn->m_unrefNum) {
			break;
		} else {}
	}
	m_pFList.insert(it, pFn);

	MyAssert(m_pFList.size() <= LIST_LEN);
}

void FaceCount::NextFrame()
{
	for (list<FaceNode *> ::iterator it = m_pFList.begin(); it != m_pFList.end(); ++it) {
		(*it)->m_unrefNum++;

		if ((*it)->m_unrefNum >= UNREF_MAX) {
			list<FaceNode *> ::iterator it2 = it;
			--it;
			m_apUnuse.push_back(*it2);
			m_pFList.erase(it2);
		} else {}
	}
}

bool FaceCount::IsUsed(unsigned id) 
{
	for (list<FaceNode *> ::iterator it = m_pFList.begin(); it != m_pFList.end(); ++it) {
		if (id == (*it)->m_id) {
			return true;
		} else {}
	} 
	return false;
}

unsigned FaceCount::Renew(FaceNode **ppFnIn)
{
	FaceNode *pFnMatch = GetMatch(**ppFnIn);
	bool bMatch = (pFnMatch != 0) ? true : false;
	if (bDebug) {
		string bM = (pFnMatch) ? "y" : "n";
		cout << "matched: " << bM << endl;
		if (pFnMatch) {
			cout << "id: " << pFnMatch->m_id << endl;
		} else {}
	} else {}

	static unsigned newId = 0;
	if (!bMatch) {
		if (m_apUnuse.size() > 0) {
			pFnMatch = m_apUnuse[m_apUnuse.size() - 1];
			m_apUnuse.pop_back();
			if (bDebug) {
				cout << "take from" << endl;
			} else {}
		} else {
			list<FaceNode *> ::iterator it = m_pFList.end();
			--it;
			pFnMatch = *it;
			m_pFList.pop_back();
			if (bDebug) {
				cout << "empty" << endl;
			} else {}
		}
		pFnMatch->m_dispNum = 0;

		newId = (newId + 1) % ID_MAX;
		while (IsUsed(newId)) {
			newId = (newId + 1) % ID_MAX;
		}
		pFnMatch->m_id = newId;
	} else {}

	//(*ppFnIn)->m_dispNum = pFnMatch->m_dispNum + 1;
	//(*ppFnIn)->m_unrefNum = 0;
	//(*ppFnIn)->m_id = pFnMatch->m_id;
	//unsigned fId = pFnMatch->m_id;
	pFnMatch->Update(**ppFnIn, !bMatch);
	pFnMatch->m_unrefNum = 0;
	unsigned fId = pFnMatch->m_id;

	/*
	FaceNode *pTmp = *ppFnIn;
	*ppFnIn = pFnMatch;
	pFnMatch = pTmp;
	*/

	Add(pFnMatch);
	if (bDebug) {
		cout << "m_pFList num: " << m_pFList.size() << endl;
	} else {}

	return fId;
}