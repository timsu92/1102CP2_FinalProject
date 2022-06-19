#include<bits/stdc++.h>

using namespace std;
#define DBG

// 4的指數
const vector<unsigned long> FOUR{4, 16, 64, 256, 1024, 4096, 16384, 65536, 262144, 1048576, 4194304, 16777216, 67108864, 268435456, 1073741824, 4294967296, 17179869184, 68719476736, 274877906944, 1099511627776, 4398046511104, 17592186044416, 70368744177664, 281474976710656, 1125899906842624, 4503599627370496, 18014398509481984, 72057594037927936, 288230376151711744, 1152921504606846976, 4611686018427387904};

const short DROW[] = {-1, 1, 0, 0}; const short DCOL[] = {0, 0, -1, 1};

enum Direction{
	UP, DOWN, LEFT, RIGHT
};

struct RateAndScores{
	int r;
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
private:
	array<array<enum MapObjs, 20>, 20> _data;
	unsigned short _height;
	unsigned short _width;
};

/* Return true if the test is passed */
class Required{
public:
	static inline bool wall(const short &row, const short &col);
	static inline bool opponent(const short &row, const short &col);
	static inline bool outOfRange(const short &row, const short &col);

	bool all(const short &row, const short &col) const{
		for(auto &i: _methods){
			if(!(i)(row, col))
				return false;
		}
		return true;
	}
private:
	bool (*_methods[3])(const short&, const short&) = {outOfRange, wall, opponent};
};

class Complex{
public:
	static inline struct RateAndScores evenDownVote(const short &row, const short &col, const pair<int, int> &scores, const unsigned short depth){
		return RateAndScores{depth % 2 ? 0 : -1, scores.first, scores.second};
	}

	static struct RateAndScores collectObj(const short &row, const short &col, const pair<int, int> &scores, const unsigned short depth);

	RateAndScores all(const short &row, const short &col, const pair<int, int>&scores, const unsigned short depth) const{
		RateAndScores ret = {0, scores.first, scores.second};
		for(auto &i : _methods){
			ret += (i)(row, col, scores, depth);
		}
		return ret;
	}
private:
	struct RateAndScores (*_methods[2])(const short&, const short&, const pair<int, int>&, const unsigned short) = {
		evenDownVote, collectObj
	};
};

class Bot{
public:
	Bot(){
		benchmark();
	}

	const char* decide() const;
	int alpha_beta(const pair<short, short>&playerAt, const unsigned short depth, const bool isMax, const RateAndScores &parentRnS, int prunePivot) const;
	void benchmark();
private:
	unsigned short _maxDepth;
	const char* _DIR_STR[4] = {"UP\n", "DOWN\n", "LEFT\n", "RIGHT\n"};
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
	}
	Bot bot;
	cout << bot.decide();
	return 0;
}

inline bool Required::wall(const short &row, const short &col){
	return gameMap[row][col] != WALL;
}

inline bool Required::opponent(const short &row, const short &col){
	return !((gameMap[row][col] == PLAYER_A || gameMap[row][col] == PLAYER_B) && gameMap[row][col] != WHOAMI);
}

inline bool Required::outOfRange(const short &row, const short &col){
	return 0 <= row && row < gameMap.height() && 0 <= col && col < gameMap.width();
}

void Bot::benchmark(){
	unsigned short row, col;
	do{
		row = rand() % gameMap.height();
		col = rand() % gameMap.width();
	}while(gameMap[row][col] == WALL || gameMap[row][col] == PLAYER_A || gameMap[row][col] == PLAYER_B);
	double average = 0;
	for(short i=0 ; i < 3 ; ++i){
		auto start_clock = clock();
		Required().all(row, col);
		Complex().all(row, col, make_pair(0, 0), 1);
		average += (clock() - start_clock) * 1.0 / CLOCKS_PER_SEC;
	}
	average /= 3;

	const auto idx4 = lower_bound(FOUR.begin(), FOUR.end(), 0.9 / average);
	_maxDepth = min(static_cast<unsigned short>(idx4 - FOUR.begin()+1), static_cast<unsigned short>(1001 - ROUND)); // 加一是因為第一層只有當前位置，沒有計算
#ifdef DBG
	cerr << "[Bot::benchmark]execution costs " << average << " s, which is " << 0.9/average << " executions\n";
	cerr << "[Bot::benchmark]running up to " << _maxDepth << " steps\n";
#endif
}

int Bot::alpha_beta(const pair<short, short>&playerAt, const unsigned short depth, const bool isMax, const RateAndScores &parentRnS, int prunePivot) const{
	if(depth >= _maxDepth){
#ifdef DBG
		cerr << "[Bot::alpha_beta]_maxDepth reached\n";
#endif 
		return 0;
	}
	int diffRate = isMax ? INT_MIN : INT_MAX;
	for(short dir = 0 ; dir < 4 && (isMax ? diffRate < prunePivot : diffRate > prunePivot) ; ++dir){
		const pair<short, short>movedPlayerAt = make_pair(playerAt.first + DROW[dir], playerAt.second + DCOL[dir]);
#ifdef DBG
		cerr << "[Bot::alpha_beta]player moved ";
		switch((enum Direction)dir){
			case UP: cerr << "up"; break;
			case DOWN: cerr << "down"; break;
			case LEFT: cerr << "left"; break;
			case RIGHT: cerr << "right"; break;
		}
		cerr << " to (" << movedPlayerAt.first << ',' << movedPlayerAt.second << ")\n";
#endif
		if(!Required().all(movedPlayerAt.first, movedPlayerAt.second)){
#ifdef DBG
			cerr << "[Bot::alpha_beta]Required not passed\n";
#endif 
			continue;
		}
		const enum MapObjs thisObj = gameMap.at(movedPlayerAt.first).at(movedPlayerAt.second);
#ifdef DBG
		cerr << "[Bot::alpha_beta]Required passed\n"
			"[Bot::alpha_beta]thisObj is " << (char)thisObj << endl;
#endif
		RateAndScores thisRnS = Complex().all(movedPlayerAt.first, movedPlayerAt.second, make_pair(parentRnS.sa, parentRnS.sb), depth+1);
#ifdef DBG
		cerr << "[Bot::alpha_beta]thisRnS(Complex)={" << thisRnS.r << ',' << thisRnS.sa << ',' << thisRnS.sb << "}\n";
#endif
		if(isMax){
			if(thisRnS.r == INT_MIN){
				continue;
			}else if(thisRnS.r == INT_MAX){
				return INT_MAX;
			}
		}else{
			if(thisRnS.r == INT_MAX){
				continue;
			}else if(thisRnS.r == INT_MIN){
				return INT_MIN;
			}
		}
		// 刪除這格地圖上的物件
		gameMap[movedPlayerAt.first][movedPlayerAt.second] = PATH;
#ifdef DBG
		cerr << "[Bot::alpha_beta]call alpha_beta by playerAt=(" << movedPlayerAt.first << ',' << movedPlayerAt.second << "), depth=" << depth+1 << ", " << (isMax ? "min" : "max") << ", RnS={" << (parentRnS + thisRnS).r << ',' << (parentRnS + thisRnS).sa << ',' << (parentRnS + thisRnS).sb << ", prunePivot=" << diffRate << endl;
#endif
		int nextRate = alpha_beta(movedPlayerAt, depth+1, !isMax, parentRnS + thisRnS, diffRate);
#ifdef DBG
		cerr << "[Bot::alpha_beta]back to playerAt (" << movedPlayerAt.first << ',' << movedPlayerAt.second << "), depth=" << depth << endl <<
			"[Bot::alpha_beta]nextRate=" << nextRate << endl;
#endif
		// 還原這格地圖上的物件
		gameMap[movedPlayerAt.first][movedPlayerAt.second] = thisObj;
		if(isMax){
			if(nextRate == INT_MIN){
				continue;
			}else if(nextRate == INT_MAX){
				return INT_MAX;
			}else{
#ifdef DBG
				if(thisRnS.r + nextRate > diffRate){
					cerr << "[Bot::alpha_beta]diffRate changed to " << thisRnS.r + nextRate << endl;
				}else{
					cerr << "[Bot::alpha_beta]diffRate remain " << diffRate << endl;
				}
#endif
				diffRate = max(diffRate, thisRnS.r + nextRate);
			}
		}else{
			if(nextRate == INT_MAX){
				continue;
			}else if(nextRate == INT_MIN){
				return INT_MIN;
			}else{
#ifdef DBG
				if(thisRnS.r + nextRate < diffRate){
					cerr << "[Bot::alpha_beta]diffRate changed to " << thisRnS.r + nextRate << endl;
				}else{
					cerr << "[Bot::alpha_beta]diffRate remain " << diffRate << endl;
				}
#endif
				diffRate = min(diffRate, thisRnS.r + nextRate);
			}
		}
	}
#ifdef DBG
	cerr << "[Bot::alpha_beta]diffRate return as " << diffRate << endl;
#endif
	return diffRate;
}

struct RateAndScores Complex::collectObj(const short &row, const short &col, const pair<int, int> &scores, const unsigned short depth){
	struct RateAndScores ret{0, scores.first, scores.second};
	int &myScore = WHOAMI == PLAYER_A ? ret.sa : ret.sb;
	switch(gameMap[row][col]){
		case PATH:
			break;
		case MINE:
			ret.r = -2;
			break;
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
			ret.r = -(myScore/2);
			myScore /= 2;
		case PLAYER_A:
		case PLAYER_B:
#ifdef DBG
			if((gameMap[row][col] == PLAYER_A || gameMap[row][col] == PLAYER_B) && gameMap[row][col] != WHOAMI){
				cerr << "[Complex::collectObj]found another player!\n";
				ret.r = INT_MIN;
			}
#endif
			break;
		case WALL:
#ifdef DBG
			cerr << "[Complex::collectObj]found WALL!\n";
			ret.r = INT_MIN;
#endif
			break;
	}
	return ret;
}

const char* Bot::decide() const{
	short maxDirIdx = 0;
	int maxRate = INT_MIN; // diffRate
	const pair<short, short> &myLocation = WHOAMI == PLAYER_A ? gameMap.playersAt.first : gameMap.playersAt.second;
	for(short dir = 0 ; dir < 4 ; ++dir){
		const pair<short, short> movedPlayerAt = make_pair(myLocation.first + DROW[dir], myLocation.second + DCOL[dir]);
#ifdef DBG
		cerr << "[Bot::decide]player moved to (" << movedPlayerAt.first << ',' << movedPlayerAt.second << ")\n";
#endif
		if(!Required().all(movedPlayerAt.first, movedPlayerAt.second)){
#ifdef DBG
			cerr << "[Bot::decide]Required not passed";
#endif
			continue;
		}
		const enum MapObjs thisObj = gameMap[movedPlayerAt.first][movedPlayerAt.second];
#ifdef DBG
		cerr << "[Bot::decide]Required passed\n"
			"[Bot::decide]thisObj is " << (char)thisObj << endl;
#endif
		int thisRate = Complex().all(movedPlayerAt.first, movedPlayerAt.second, SCORE, 1).r;
#ifdef DBG
		cerr << "[Bot::decide]thisRate=" << thisRate << endl;
#endif
		if(thisRate == INT_MIN){continue;}
		else if(thisRate == INT_MAX){
			maxDirIdx = dir;
			break;
		}
		gameMap[movedPlayerAt.first][movedPlayerAt.second] = PATH;
#ifdef DBG
		cerr << "[Bot::decide]call alpha_beta by playerAt (" << movedPlayerAt.first << ',' << movedPlayerAt.second << "), depth=1, min, RnS={0," << SCORE.first << ',' << SCORE.second << "}, prunePivot=" << maxRate << endl;
#endif
		int nextRate = alpha_beta(movedPlayerAt, 1, false, RateAndScores{0, SCORE.first, SCORE.second}, maxRate);
		gameMap[movedPlayerAt.first][movedPlayerAt.second] = thisObj;
#ifdef DBG
		cerr << "[Bot::decide]nextRate=" << nextRate << endl;
#endif
		if(nextRate == INT_MIN){continue;}
		else if(nextRate == INT_MAX){
			maxDirIdx = dir;
			break;
		}
		else if(thisRate + nextRate > maxRate){
			maxRate = thisRate + nextRate;
			maxDirIdx = dir;
		}
	}
	return _DIR_STR[maxDirIdx];
}
