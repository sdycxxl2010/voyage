#ifndef __HIST_CHECK_H__
#define __HIST_CHECK_H__

struct HitTestLine{
	struct HitTestLine*next;
	int x1;
	int x2;
};
class HistCheck
{
public:
	HistCheck();
	~HistCheck();


	HitTestLine**landHitTestData;
	int landHitTestDataLineNum;

	bool landHitTest(int x,int y);
	bool landHitTest(int x,int y,int r);
	void initLandHistTestData();
private:

};


#endif