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

	bool isValid(const unsigned short row, const unsigned short col);

	unsigned short height(){return _height;}
	unsigned short width(){return _width;}
	pair<pair<unsigned short, unsigned short>, pair<unsigned short, unsigned short>> playersAt;
private:
	array<array<enum MapObjs, 20>, 20> _data;
	unsigned short _height;
	unsigned short _width;
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

bool Map::isValid(const unsigned short row, const unsigned short col){
	return !(_data[row][col]==WALL
			|| ((_data[row][col]==PLAYER_A || _data[row][col]==PLAYER_B) && _data[row][col] != WHOAMI)
			|| 0 > row || row >= _height
			|| 0 > col || col >= _width);
}
