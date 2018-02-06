#include "SafeAreaCheck.h"

#include "cocos2d.h"
USING_NS_CC;
SafeAreaCheck::SafeAreaCheck()
{
	landHitTestData = 0;
	landHitTestDataLineNum = 0;
	initLandHistTestData();
}

SafeAreaCheck::~SafeAreaCheck()
{
	delete [] landHitTestData;
}

bool SafeAreaCheck::landHitTest(int x,int y){
	y = 36000-y;
	//log("safe check %d", y);
	if(y < landHitTestDataLineNum && y >= 0){
		SafeTestLine*head = landHitTestData[y];
		while(head){
			if(head->x1 <= x && head->x2 >= x){
				return true;
			}
			head = head->next;
		}
		return false;
	}else{
		return true;
	}
}

bool SafeAreaCheck::landHitTest(int x,int y,int r){
	if(landHitTest(x,y+r)){
		return true;
	}

	if(landHitTest(x,y-r)){
		return true;
	}

	if(landHitTest(x-r,y)){
		return true;
	}

	if(landHitTest(x+r,y)){
		return true;
	}
	// sqrt(2)/2 = 0.7071

	if(landHitTest(x+r*0.7071,y+r*0.7071)){
		return true;
	}

	if(landHitTest(x+r*0.7071,y-r*0.7071)){
		return true;
	}

	if(landHitTest(x-r*0.7071,y+r*0.7071)){
		return true;
	}

	if(landHitTest(x-r*0.7071,y-r*0.7071)){
		return true;
	}
	return false;
}
void SafeAreaCheck::initLandHistTestData(){
	//log("safe point init.");
	std::string path = CCFileUtils::getInstance()->fullPathForFilename("config/safearea");
	ssize_t size = 0;
	unsigned char*charData = FileUtils::getInstance()->getFileData(path,"rb",&size);
	if(charData){
		unsigned char*start = charData;
		unsigned char*end = start + size;
		int lineNum = 0;
		while(start < end && *start != '\n' && *start != '\r'){
			lineNum = *start-0x30 + lineNum*10;
			start++;
		}
		landHitTestDataLineNum = lineNum;
		landHitTestData = new SafeTestLine*[lineNum];
		memset(landHitTestData,0,sizeof(SafeTestLine*)*lineNum);

		while(start < end && (*start == '\n' || *start == '\r'))
			start++;

		while(start < end){
			int line = 0;
			while(start < end && *start != ':'){
				line = *start-0x30 + line*10;
				start++;
			}
			//while((*start == '\n' || *start == '\r') && start < end)
			start++;
			if(line<lineNum){
				int value = 0;
				int odds = 0;
				SafeTestLine*last = 0;
				while(start<end && *start !='\n' && *start != '\r'){
					if(*start == ' '){
						if(odds%2 == 0){
							SafeTestLine*newPoints = new SafeTestLine;
							newPoints->next = 0;
							if(!landHitTestData[line]){
								landHitTestData[line] = newPoints;
								last = newPoints;
							}else{
								last->next = newPoints;
								last = newPoints;
							}
							last->x1 = value;
							//log("safe point: x1 %d", last->x1);
						}else if(odds %2 == 1){
							last->x2 = value;
							//log("safe point: x2 %d", last->x2);
						}
						value = 0;
						start ++;
						odds ++;

						if(start>= end || *start =='\n' || *start == '\r'){
							break;
						}
					}
					value = *start-0x30 + value*10;
					start ++;
				}
			}

			while(start < end && (*start == '\n' || *start == '\r'))
				start++;
		}
		free(charData);
	}else{
		//log("safe point fail.");
	}

	// 	Data data = FileUtils::getInstance()->getDataFromFile(path);
	// 	unsigned char*charData = data->getBytes();
	// 	int size = data->getContentSize();
}
