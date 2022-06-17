#include<bits/stdc++.h>

using namespace std;

// 4的指數
const vector<unsigned long> FOUR{4, 16, 64, 256, 1024, 4096, 16384, 65536, 262144, 1048576, 4194304, 16777216, 67108864, 268435456, 1073741824, 4294967296, 17179869184, 68719476736, 274877906944, 1099511627776, 4398046511104, 17592186044416, 70368744177664, 281474976710656, 1125899906842624, 4503599627370496, 18014398509481984, 72057594037927936, 288230376151711744, 1152921504606846976, 4611686018427387904};

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
	pair<pair<unsigned short, unsigned short>, pair<unsigned short, unsigned short>> playersAt;
private:
	array<array<enum MapObjs, 20>, 20> _data;
	unsigned short _height;
	unsigned short _width;
};

/* Return true if the test is passed */
class Required{
public:
	static bool wall(const short &row, const short &col);
	static bool opponent(const short &row, const short &col);
	static bool outOfRange(const short &row, const short &col);

	bool all(const short &row, const short &col){
		auto j = _methods[0];
		for(auto &i: _methods){
			if(!(i)(row, col))
				return false;
		}
		return true;
	}
private:
	bool (*_methods[3])(const short&, const short&) = {wall, opponent, outOfRange};
};

class Complex{
public:
	int all(const short &row, const short &col, const bool &getMax){
		int ret = getMax ? INT_MIN : INT_MAX;
		for(auto &i : _methods){
			ret = getMax ? max((i)(row, col), ret) : min((i)(row, col), ret);
			if(ret == INT_MAX && getMax){
				return ret;
			}else if(ret == INT_MIN && !getMax){
				return ret;
			}
		}
		return ret;
	}
private:
	int (*_methods[0])(const short&, const short&) = {};
};

class Bot{
public:
	Bot(){
		benchmark();
	}

	void benchmark();
private:
	unsigned short _maxDepth;
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
	return 0;
}

bool Required::wall(const short &row, const short &col){
	return gameMap[row][col] != WALL;
}

bool Required::opponent(const short &row, const short &col){
	return !((gameMap[row][col] == PLAYER_A || gameMap[row][col] == PLAYER_B) && gameMap[row][col] != WHOAMI);
}

bool Required::outOfRange(const short &row, const short &col){
	return 0 <= row && row < gameMap.height() && 0 <= col && col < gameMap.width();
}

void Bot::benchmark(){
	unsigned short row, col;
	do{
		row = rand() % gameMap.height();
		col = rand() % gameMap.width();
	}while(gameMap[row][col] == WALL);
	double tries[3];
	for(short i=0 ; i < 3 ; ++i){
		auto start_clock = clock();
		Required().all(row, col);
		Complex().all(row, col, true);
		tries[i] = (clock() - start_clock) * 1.0 / CLOCKS_PER_SEC;
	}

	const double average = (tries[0] + tries[1] + tries[2]) / 3;
	const auto idx4 = lower_bound(FOUR.begin(), FOUR.end(), 0.9 / average);
	_maxDepth = idx4 - FOUR.begin();
}
