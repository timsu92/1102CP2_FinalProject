#include<bits/stdc++.h>

using namespace std;

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
	MINE = 'b',	// 3 round
	MUSHROOM = 'm',	// +1
	PMUSHROOM = 'n', // -1
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

	inline enum MapObjs& operator[](const pair<short, short> &location){
		return _data[location.first][location.second];
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
	// static struct RateAndScores corner2Center(const pair<short, short> &playerAt, const pair<short, short> &movedPlayerAt, const pair<int, int> &scores, const unsigned short depth);

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
	struct RateAndScores (*_methods[1])(const pair<short, short>&, const pair<short, short>&, const pair<int, int>&, const unsigned short) = {
		collectObj 
	};
};

class Bot{
public:
	Bot(){
		_computePlayerHalfDistance();
		benchmark();
	}

	double dfs(const pair<short, short> &playerAt, const unsigned short depth, const RateAndScores &parentRnS) const;
	const string decide() const;
	void benchmark();
private:
	unsigned short _maxDepth = 1; // changed later in benchmark	
	const string _DIR_STR[4] = {"UP\n", "DOWN\n", "LEFT\n", "RIGHT\n"};
	unsigned short _playerHalfDistance;
	void _computePlayerHalfDistance();
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
	double average = 0;
	for(short i=0 ; i < 3 ; ++i){
		auto start_clock = clock();
		dfs(gameMap.myLocation, 0, RateAndScores{0, SCORE.first, SCORE.second}) * 0.8;
		average += (clock() - start_clock);
	}
	average /= 3 * CLOCKS_PER_SEC;

	short dirsOK = 4;
	for(short dir=0 ; dir < 4 ; ++dir){
		if(!Required(gameMap.myLocation.first + DROW[dir], gameMap.myLocation.second + DCOL[dir])){
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
		const auto idx4 = lower_bound(FOUR.begin(), FOUR.end(), (0.92 - ((double)clock() / CLOCKS_PER_SEC)) / average * 4 / dirsOK);
		_maxDepth = min(static_cast<unsigned short>((idx4 - FOUR.begin()+1)), static_cast<unsigned short>(1001 - ROUND)); // 加一是因為第一層只有當前位置，沒有計算
	}
#ifdef DBGTIME
	cerr << "[Bot::benchmark]execution costs " << average << "s, which is " << 0.9/average << " executions\n";
	cerr << "[Bot::benchmark]running up to " << _maxDepth << " steps (" << dirsOK << " directions)\n";
#endif
}

struct RateAndScores Complex::collectObj(const pair<short, short> &playerAt, const pair<short, short> &movedPlayerAt, const pair<int, int> &scores, const unsigned short depth){
	struct RateAndScores ret{0, scores.first, scores.second};
	int &myScore = WHOAMI == PLAYER_A ? ret.sa : ret.sb;
	switch(gameMap[movedPlayerAt]){
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

inline bool Required(const short &row, const short &col){
	return 0 <= row && row < gameMap.height() &&
		0 <= col && col < gameMap.width() &&
		gameMap[row][col] != WALL &&
		!((gameMap[row][col] == PLAYER_A || gameMap[row][col] == PLAYER_B) && gameMap[row][col] != WHOAMI);
}

double Bot::dfs(const pair<short, short> &playerAt, const unsigned short depth, const RateAndScores &parentRnS) const{
	if(depth >= _maxDepth){
		return 0;
	}
	double diffRate = -DBL_MAX;
	for(short dir = 0 ; dir < 4 ; ++dir){
		const pair<short, short>movedPlayerAt = make_pair(playerAt.first + DROW[dir], playerAt.second + DCOL[dir]);
		if(!Required(movedPlayerAt.first, movedPlayerAt.second)){
			continue;
		}
		const enum MapObjs thisObj = gameMap[movedPlayerAt];
		RateAndScores thisRnS = Complex().all(playerAt, movedPlayerAt, make_pair(parentRnS.sa, parentRnS.sb), depth+1);
		if(thisRnS.r == -DBL_MAX){
			continue;
		}else if(thisRnS.r == DBL_MAX){
			return DBL_MAX;
		}
		// 刪除這格地圖上的物件
		gameMap[movedPlayerAt] = PATH;
		double nextRate;
		if(thisObj == MINE){
			if(depth+3 < _playerHalfDistance){
				nextRate = dfs(movedPlayerAt, depth+1, parentRnS + thisRnS);
			}else{
				nextRate = dfs(movedPlayerAt, depth+1, parentRnS + thisRnS) * pow(0.8, depth + 4 - _playerHalfDistance);
			}
		}else{
			if(depth < _playerHalfDistance){
				nextRate = dfs(movedPlayerAt, depth+1, parentRnS + thisRnS);
			}else{
				nextRate = dfs(movedPlayerAt, depth+1, parentRnS + thisRnS) * 0.8;
			}
		}
		// 還原這格地圖上的物件
		gameMap[movedPlayerAt] = thisObj;
		if(nextRate == -DBL_MAX){
			continue;
		}else if(nextRate == DBL_MAX){
			return DBL_MAX;
		}else{
			diffRate = max(diffRate, thisRnS.r + nextRate);
		}
	}
	return diffRate;
}

const string Bot::decide() const{
	if(_maxDepth == 0){ // 無路可走
		return "UP\n";
	}
	short maxDirIdx = 0;
	double maxRate = -DBL_MAX; // diffRate
	for(short dir=0 ; dir < 4 ; ++dir){
		const pair<short, short> movedPlayerAt = make_pair(gameMap.myLocation.first + DROW[dir], gameMap.myLocation.second + DCOL[dir]);
		if(!Required(movedPlayerAt.first, movedPlayerAt.second)){
			continue;
		}
		const enum MapObjs thisObj = gameMap[movedPlayerAt];
		double thisRate = Complex().all(gameMap.myLocation, movedPlayerAt, SCORE, 1).r;
		if(thisRate == -DBL_MAX){continue;}
		else if(thisRate == DBL_MAX){
			maxDirIdx = dir;
			break;
		}
		gameMap[movedPlayerAt] = PATH;
		double nextRate = dfs(movedPlayerAt, 1, RateAndScores{0, SCORE.first, SCORE.second});
		gameMap[movedPlayerAt] = thisObj;
		if(nextRate == -DBL_MAX){continue;}
		else if(nextRate == DBL_MAX){
			maxDirIdx = dir;
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
	}
	return _DIR_STR[maxDirIdx];
}

// struct RateAndScores Complex::corner2Center(const pair<short, short> &playerAt, const pair<short, short> &movedPlayerAt, const pair<int, int> &scores, const unsigned short depth){
	// struct RateAndScores nearRnS = {0.1, scores.first, scores.second}, farRnS = {0, scores.first, scores.second};
	// if(gameMap.myLocation.first < gameMap.height() * 0.2 || gameMap.myLocation.first > gameMap.height() * 0.8){
		// if(pow(movedPlayerAt.first - gameMap.height()/2, 2) + pow(movedPlayerAt.second - gameMap.width()/2, 2) <
				// pow(gameMap.myLocation.first - gameMap.height()/2, 2) + pow(gameMap.myLocation.second - gameMap.width()/2, 2)){
			// return nearRnS;
		// }else{
			// return farRnS;
		// }
	// }else{
		// if(gameMap.myLocation.second < gameMap.width() * 0.2 || gameMap.myLocation.second > gameMap.width() * 0.8){
			// if(pow(movedPlayerAt.first - gameMap.height()/2, 2) + pow(movedPlayerAt.second - gameMap.width()/2, 2) <
					// pow(gameMap.myLocation.first - gameMap.height()/2, 2) + pow(gameMap.myLocation.second - gameMap.width()/2, 2)){
				// return nearRnS;
			// }else{
				// return farRnS;
			// }
		// }
	// }
	// return farRnS;
// }

void Bot::_computePlayerHalfDistance(){
	queue<pair<unsigned short, pair<short, short>>> q({make_pair(0, gameMap.oppoLocation)});
	while(!q.empty()){
		for(short dir=0 ; dir < 4 ; ++dir){
			const pair<short, short> movedPlayerAt = make_pair(q.front().second.first + DROW[dir], q.front().second.second + DCOL[dir]);
			if(movedPlayerAt.first < 0 || movedPlayerAt.first >= gameMap.height() || movedPlayerAt.second < 0 || movedPlayerAt.second >= gameMap.width() ||
					gameMap[movedPlayerAt] == WALL){
				continue;
			}
			if(gameMap[movedPlayerAt] == WHOAMI){
				_playerHalfDistance = (q.front().first + 1) / 2;
				return;
			}
			q.push(make_pair(q.front().first + 1, movedPlayerAt));
		}
		q.pop();
	}
	_playerHalfDistance = USHRT_MAX;
}
