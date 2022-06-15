#include<bits/stdc++.h>

using namespace std;

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
class Req{
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

Map gameMap;
enum MapObjs WHOAMI;
unsigned short ROUND;
pair<int, int>SCORE;
time_t start_time;

int main(){
	ios::sync_with_stdio(false);
	cin.tie(0);
	start_time = time(nullptr);
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

bool Req::wall(const short &row, const short &col){
	return gameMap[row][col] != WALL;
}

bool Req::opponent(const short &row, const short &col){
	return !((gameMap[row][col] == PLAYER_A || gameMap[row][col] == PLAYER_B) && gameMap[row][col] != WHOAMI);
}

bool Req::outOfRange(const short &row, const short &col){
	return 0 <= row && row < gameMap.height() && 0 <= col && col < gameMap.width();
}
