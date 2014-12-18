#ifndef _FACE_COUNT_H
#define _FACE_COUNT_H

#include "define.h"

#include <list>
#include <iostream>

using namespace std;

class FaceNode
{
public:
	FaceNode(unsigned iW, unsigned iH);
	FaceNode(FaceNode &fnIn);
	~FaceNode();

	void CopyFrom(unsigned char *pIn, unsigned iW, unsigned iH, unsigned iC);

	DATA Correlate(FaceNode &fnIn);
	void Update(FaceNode &fnIn);

	unsigned char GetAvg();
	DATA GetSD();

	unsigned char* GetData();

	unsigned m_unrefNum;
	unsigned m_dispNum;
	unsigned m_id;

	bool m_bMale;
	unsigned m_ageB;
	unsigned m_ageE;
	DATA m_maxProb;

	unsigned m_recL, m_recR;
	unsigned m_recB, m_recT;

private:
	Vect2D<unsigned> m_dim;
	unsigned char *m_pData;

	unsigned char m_avg;
	DATA m_sd;
};

class FaceCount
{
public:
	FaceCount();
	~FaceCount();

	void NextFrame();
	unsigned Renew(FaceNode **ppFnIn);

	FaceNode* GetBeginFace();
	FaceNode* GetNextFace();

	bool bDebug;

private:
	static const unsigned LIST_LEN;
	static const unsigned FACE_LEN;
	static const unsigned UNREF_MAX;
	static const unsigned ID_MAX;

	FaceNode* GetMatch(FaceNode &fnIn);
	void Add(FaceNode *pFn);
	bool IsUsed(unsigned id);

	list<FaceNode *> m_pFList;
	std::vector<FaceNode *> m_apUnuse;

	list<FaceNode *>::iterator m_itFace;
};

#endif