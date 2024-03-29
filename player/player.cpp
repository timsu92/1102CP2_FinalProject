#include<bits/stdc++.h>

using namespace std;
// #define DBG
#define DBGTIME
#define DBGFINAL

// 4的指數
const vector<unsigned long> FOUR{4, 16, 64, 256, 1024, 4096, 16384, 65536, 262144, 1048576, 4194304, 16777216, 67108864, 268435456, 1073741824, 4294967296, 17179869184, 68719476736, 274877906944, 1099511627776, 4398046511104, 17592186044416, 70368744177664, 281474976710656, 1125899906842624, 4503599627370496, 18014398509481984, 72057594037927936, 288230376151711744, 1152921504606846976, 4611686018427387904};

const short DROW[] = {-1, 1, 0, 0}; const short DCOL[] = {0, 0, -1, 1};

enum Direction{
	UP, DOWN, LEFT, RIGHT
};

struct RateAndScores{
	double r;
	int sa;
	int sb;

	void operator+=(const struct RateAndScores& other){
		r += other.r;
		sa = other.sa;
		sb = other.sb;
	}

	inline struct RateAndScores operator+(const struct RateAndScores &other) const{
		return RateAndScores{r + other.r, other.sa, other.sb};
	}
};

enum MapObjs{
	MINE = 'b',	//3 round
	MUSHROOM = 'm',	//+1
	PMUSHROOM = 'n', //-1
	STAR = 's', // *2
	PSTAR = 't', // /2
	WALL = 'x',
	PLAYER_A = 'A',
	PLAYER_B = 'B',
	PATH = '.'
};

class Map{
public:
	void read(){
		cin >> _height >> _width;
		char inp;
		for(unsigned short row=0 ; row < _height ; ++row){
			for(unsigned short col=0 ; col < _width ; ++col){
				cin >> inp;
				_data[row][col] = (enum MapObjs)inp;
				if(inp == PLAYER_A){
					playersAt.first = make_pair(row, col);
				}else if(inp == PLAYER_B){
					playersAt.second = make_pair(row, col);
				}
			}
		}
	}

	array<enum MapObjs, 20>& operator[](const unsigned short row){
		return _data[row];
	}

	array<enum MapObjs, 20>& at(const unsigned short row){
		return _data.at(row);
	}

	unsigned short height(){return _height;}
	unsigned short width(){return _width;}
	pair<pair<short, short>, pair<short, short>> playersAt;
	pair<short, short> myLocation = make_pair(5,5), oppoLocation = make_pair(5,5); // dummy, set in main
private:
	array<array<enum MapObjs, 20>, 20> _data;
	unsigned short _height;
	unsigned short _width;
};

/* Return true if the test is passed */
inline bool Required(const short &row, const short &col);

class Complex{
public:
	static struct RateAndScores collectObj(const pair<short, short> &playerAt,const pair<short, short> &movedPlayerAt, const pair<int, int> &scores, const unsigned short depth);
	static struct RateAndScores corner2Center(const pair<short, short> &playerAt, const pair<short, short> &movedPlayerAt, const pair<int, int> &scores, const unsigned short depth);

	RateAndScores all(const pair<short, short> &playerAt, const pair<short, short> &movedPlayerAt, const pair<int, int>&scores, const unsigned short depth) const{
		RateAndScores ret = {0, scores.first, scores.second};
		for(auto &i : _methods){
			auto next = (i)(playerAt, movedPlayerAt, scores, depth);
			if(next.r == -DBL_MAX){
				ret.r = -DBL_MAX;
				break;
			}else if(ret.r == DBL_MAX || next.r == DBL_MAX){
				ret.r = DBL_MAX;
				ret.sa = next.sa;
				ret.sb = next.sb;
			}else{
				ret += next;
			}
		}
		return ret;
	}
private:
	struct RateAndScores (*_methods[2])(const pair<short, short>&, const pair<short, short>&, const pair<int, int>&, const unsigned short) = {
		collectObj, corner2Center
	};
};

class Bot{
public:
	Bot(){
		benchmark();
	}

	const char* decide() const;
	double alpha_beta(const pair<short, short>&playerAt, const unsigned short depth, const bool isMax, const RateAndScores &parentRnS, double prunePivot) const;
	void benchmark();
private:
	unsigned short _maxDepth = 1; // changed later in benchmark
	const string _DIR_STR[4] = {"UP\n", "DOWN\n", "LEFT\n", "RIGHT\n"};
};

Map gameMap;
enum MapObjs WHOAMI;
unsigned short ROUND;
pair<int, int>SCORE;

int main(){
	ios::sync_with_stdio(false);
	cin.tie(0);
	srand(time(nullptr));
	cin >> ROUND;
	gameMap.read();
	cin >> SCORE.first >> SCORE.second;
	{
		char tmp;
		cin >> tmp;
		WHOAMI = (enum MapObjs)tmp;
		if(WHOAMI == PLAYER_A){
			gameMap.myLocation = gameMap.playersAt.first;
			gameMap.oppoLocation = gameMap.playersAt.second;
		}else{
			gameMap.myLocation = gameMap.playersAt.second;
			gameMap.oppoLocation = gameMap.playersAt.first;
		}
	}
	Bot bot;
	cout << bot.decide();
	return 0;
}

void Bot::benchmark(){
	const unsigned short &row = WHOAMI == PLAYER_A ? gameMap.playersAt.first.first : gameMap.playersAt.second.first,
		  &col = WHOAMI == PLAYER_A ? gameMap.playersAt.first.second : gameMap.playersAt.second.second;
	double average = 0;
	for(short i=0 ; i < 3 ; ++i){
		auto start_clock = clock();
		alpha_beta(make_pair(row, col), 0, true, RateAndScores{0, SCORE.first, SCORE.second}, DBL_MAX) * 0.8;
		average += (clock() - start_clock) * 1.0 / CLOCKS_PER_SEC;
	}
	average /= 3;

	short dirsOK = 4;
	for(short dir=0 ; dir < 4 ; ++dir){
		if(!Required(row + DROW[dir], col + DCOL[dir])){
			--dirsOK;
		}
	}
	if(dirsOK == 0){
		_maxDepth = 0;
#ifdef DBGTIME
		cerr << "[Bot::benchmark]running up to " << _maxDepth << " steps (" << dirsOK << " directions)\n";
#endif
		return;
	}else{
		const auto idx4 = lower_bound(FOUR.begin(), FOUR.end(), 0.9 / average * 4 / dirsOK);
		_maxDepth = min(static_cast<unsigned short>((idx4 - FOUR.begin()+1)), static_cast<unsigned short>(1001 - ROUND)); // 加一是因為第一層只有當前位置，沒有計算
	}
#ifdef DBGTIME
	cerr << "[Bot::benchmark]execution costs " << average << "s, which is " << 0.9/average << " executions\n";
	cerr << "[Bot::benchmark]running up to " << _maxDepth << " steps (" << dirsOK << " directions)\n";
#endif
}

double Bot::alpha_beta(const pair<short, short>&playerAt, const unsigned short depth, const bool isMax, const RateAndScores &parentRnS, double prunePivot) const{
	if(depth >= _maxDepth){
#ifdef DBG
		cerr << "[Bot::alpha_beta]_maxDepth reached\n";
#endif 
		return 0;
	}
	double diffRate = isMax ? -DBL_MAX : DBL_MAX;
	for(short dir = 0 ; dir < 4 && (isMax ? diffRate < prunePivot : diffRate > prunePivot) ; ++dir){
		const pair<short, short>movedPlayerAt = make_pair(playerAt.first + DROW[dir], playerAt.second + DCOL[dir]);
		if(!Required(movedPlayerAt.first, movedPlayerAt.second)){
			continue;
		}
		const enum MapObjs thisObj = gameMap[movedPlayerAt.first][movedPlayerAt.second];
		RateAndScores thisRnS = Complex().all(playerAt, movedPlayerAt, make_pair(parentRnS.sa, parentRnS.sb), depth+1);
		if(isMax){
			if(thisRnS.r == -DBL_MAX){
				continue;
			}else if(thisRnS.r == DBL_MAX){
				return DBL_MAX;
			}
		}else{
			if(thisRnS.r == DBL_MAX){
				continue;
			}else if(thisRnS.r == -DBL_MAX){
				return -DBL_MAX;
			}
		}
		// 刪除這格地圖上的物件
		gameMap[movedPlayerAt.first][movedPlayerAt.second] = PATH;
		double nextRate;
		if(thisObj == MINE){
			nextRate = alpha_beta(movedPlayerAt, depth+1, !isMax, parentRnS + thisRnS, diffRate) * 0.4096;
		}else{
			nextRate = alpha_beta(movedPlayerAt, depth+1, !isMax, parentRnS + thisRnS, diffRate) * 0.8;
		}
		// 還原這格地圖上的物件
		gameMap[movedPlayerAt.first][movedPlayerAt.second] = thisObj;
		if(isMax){
			if(nextRate == -DBL_MAX){
				continue;
			}else if(nextRate == DBL_MAX){
				return DBL_MAX;
			}else{
				diffRate = max(diffRate, thisRnS.r + nextRate);
			}
		}else{
			if(nextRate == DBL_MAX){
				continue;
			}else if(nextRate == -DBL_MAX){
				return -DBL_MAX;
			}else{
				diffRate = min(diffRate, thisRnS.r + nextRate);
			}
		}
	}
	return diffRate;
}

struct RateAndScores Complex::collectObj(const pair<short, short> &playerAt, const pair<short, short> &movedPlayerAt, const pair<int, int> &scores, const unsigned short depth){
	struct RateAndScores ret{0, scores.first, scores.second};
	int &myScore = WHOAMI == PLAYER_A ? ret.sa : ret.sb;
	switch(gameMap[movedPlayerAt.first][movedPlayerAt.second]){
		case MUSHROOM:
			ret.r = 1;
			++myScore;
			break;
		case PMUSHROOM:
			ret.r = -1;
			--myScore;
			break;
		case STAR:
			ret.r = myScore;
			myScore *= 2;
			break;
		case PSTAR:
			ret.r = -(int)(myScore/2);
			myScore /= 2;
			break;
		case MINE:
		case PATH:
		case PLAYER_A:
		case PLAYER_B:
		case WALL:
			break;
	}
	return ret;
}

const char* Bot::decide() const{
	if(_maxDepth == 0){ // 無路可走
		return "UP\n";
	}
	short maxDirIdx = 0;
	double maxRate = -DBL_MAX; // diffRate
	for(short dir = 0 ; dir < 4 ; ++dir){
		const pair<short, short> movedPlayerAt = make_pair(gameMap.myLocation.first + DROW[dir], gameMap.myLocation.second + DCOL[dir]);
		if(!Required(movedPlayerAt.first, movedPlayerAt.second)){
#ifdef DBGFINAL
			cerr << string(_DIR_STR[dir].begin(), _DIR_STR[dir].end()-1) << " -INF\t";
#endif
			continue;
		}
		const enum MapObjs thisObj = gameMap[movedPlayerAt.first][movedPlayerAt.second];
		double thisRate = Complex().all(gameMap.myLocation, movedPlayerAt, SCORE, 1).r;
		if(thisRate == -DBL_MAX){
#ifdef DBGFINAL
			cerr << string(_DIR_STR[dir].begin(), _DIR_STR[dir].end()-1) << " -INF\t";
#endif
			continue;
		}
		else if(thisRate == DBL_MAX){
#ifdef DBGFINAL
			cerr << string(_DIR_STR[dir].begin(), _DIR_STR[dir].end()-1) << " INF\t";
#endif
			maxDirIdx = dir;
			break;
		}
		gameMap[movedPlayerAt.first][movedPlayerAt.second] = PATH;
		double nextRate = alpha_beta(movedPlayerAt, 1, false, RateAndScores{0, SCORE.first, SCORE.second}, maxRate);
		gameMap[movedPlayerAt.first][movedPlayerAt.second] = thisObj;
		if(nextRate == -DBL_MAX){
#ifdef DBGFINAL
			cerr << string(_DIR_STR[dir].begin(), _DIR_STR[dir].end()-1) << " -INF\t";
#endif
			continue;
		}
		else if(nextRate == DBL_MAX){
			maxDirIdx = dir;
#ifdef DBGFINAL
			cerr << string(_DIR_STR[dir].begin(), _DIR_STR[dir].end()-1) << " INF\t";
#endif
			break;
		}
		else if(thisRate + nextRate > maxRate){
			maxRate = thisRate + nextRate;
			maxDirIdx = dir;
		}else if(thisRate + nextRate == maxRate){
			if(rand() % 2 == 0){
				maxRate = thisRate + nextRate;
				maxDirIdx = dir;
			}
		}
#ifdef DBGFINAL
		cerr << string(_DIR_STR[dir].begin(), _DIR_STR[dir].end()-1) << ' ' << thisRate + nextRate << '\t';
#endif 
	}
#ifdef DBGFINAL
	cerr << "\n\n";
#endif
	return _DIR_STR[maxDirIdx].c_str();
}

inline bool Required(const short &row, const short &col){
	return 0 <= row && row < gameMap.height() &&
		0 <= col && col < gameMap.width() &&
		gameMap[row][col] != WALL &&
		!((gameMap[row][col] == PLAYER_A || gameMap[row][col] == PLAYER_B) && gameMap[row][col] != WHOAMI);
}

struct RateAndScores Complex::corner2Center(const pair<short, short> &playerAt, const pair<short, short> &movedPlayerAt, const pair<int, int> &scores, const unsigned short depth){
	struct RateAndScores nearRnS = {0.13, scores.first, scores.second}, farRnS = {0, scores.first, scores.second};
	if(gameMap.myLocation.first < gameMap.height() * 0.2 || gameMap.myLocation.first > gameMap.height() * 0.7){
		if(pow(movedPlayerAt.first - gameMap.height()/2, 2) + pow(movedPlayerAt.second - gameMap.width()/2, 2) <
				pow(gameMap.myLocation.first - gameMap.height()/2, 2) + pow(gameMap.myLocation.second - gameMap.width()/2, 2)){
			return nearRnS;
		}else{
			return farRnS;
		}
	}else{
		if(gameMap.myLocation.second < gameMap.width() * 0.2 || gameMap.myLocation.second > gameMap.width() * 0.7){
			if(pow(movedPlayerAt.first - gameMap.height()/2, 2) + pow(movedPlayerAt.second - gameMap.width()/2, 2) <
					pow(gameMap.myLocation.first - gameMap.height()/2, 2) + pow(gameMap.myLocation.second - gameMap.width()/2, 2)){
				return nearRnS;
			}else{
				return farRnS;
			}
		}
	}
	return farRnS;
}
