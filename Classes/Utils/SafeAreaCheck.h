#ifndef __SAFE_AREA_CHECK_H__
#define __SAFE_AREA_CHECK_H__

struct SafeTestLine{
	struct SafeTestLine*next;
	int x1;
	int x2;
};
class SafeAreaCheck
{
public:
	SafeAreaCheck();
	~SafeAreaCheck();


	SafeTestLine**landHitTestData;
	int landHitTestDataLineNum;

	bool landHitTest(int x,int y);
	bool landHitTest(int x,int y,int r);
	void initLandHistTestData();
private:

};


#endif
