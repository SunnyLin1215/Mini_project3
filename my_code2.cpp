#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include<set>
#include <cstdlib>
#include <ctime>
#include<math.h>
#include<utility>
using namespace std;

struct Point {
    int x, y;
};
int kuo[10][10] = {{500, -100, 70, 60, 60, 70, -100, 500}, { -100, -200, 10, 10, 10, 10, -200, -100},  {70, 10, 25, 15, 15, 25, 10, 70},{60, 10, 15, 5, 5, 15, 10, 60},{ 60, 10, 15, 5, 5, 15, 10, 60}, {70, 10, 25, 15, 15, 25, 10, 70}, {-100, -200, 10, 10, 10, 10, -200, -100}, {500, -100, 70, 60, 60, 70, -100, 500}};
int dir_x[10] = {-1, -1, -1, 0, 0, 1, 1, 1};    //x direction
int dir_y[10] = {-1, 0, 1, -1, 1, -1, 0, 1};     //y direction
int player, n_valid_spots;
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> board;
std::vector<Point> next_valid_spots;


class State{
public:
       int value, index;
       State(int value, int idx) : value(value), index(idx) { }
       bool operator< ( const State &rhs)const {
              return value < rhs.value;
       }
};
array<array<int, SIZE>, SIZE> get_new_board(int _play, const array<array<int, SIZE>, SIZE> b, Point p) {
       int opponent = (_play == 1) ? 2: 1;
       array<array<int, SIZE>, SIZE> new_board = b;
       new_board[p.x][p.y] = _play;
       for( int k = 0; k < 8; k++) {                // 8 directions
              int i = p.x + dir_x[k];
              int j = p.y + dir_y[k];
              int ok = 0;
              while( 1) {
                     if( i >= 0 && i  <= 7 && j >= 0 && j <= 7) {
                            if(b[i][j] == 0) {   //empty
                                   ok = 0;
                                   break;
                            }else if(b[i][j] == opponent) ok = 1;
                            else if( b[i][j] == _play) break;
                            i += dir_x[k];
                            j += dir_y[k];
                     }else {
                            ok = 0;
                            break;
                     }
              }
              if(ok) {            //update board
                     for( int xx = p.x + dir_x[k], yy = p.y + dir_y[k]; b[xx][yy] != _play; xx += dir_x[k], yy += dir_y[k]) {
                            new_board[xx][yy] = _play;
                     }
              }
       }

       return new_board;
}
array<array<int, SIZE>, SIZE> get_new_board2(int _play, array<array<int, SIZE>, SIZE> b, pair<Point, set<int>> p ){
       array<array<int, SIZE>, SIZE> new_board = b;
       set<int> dir = p.second;
       new_board[p.first.x][p.first.y] =  _play;
       for( auto it = dir.begin(); it != dir.end(); ++it) {                // 8 directions
              for( int i = (p.first).x + dir_x[*it], j = (p.first).y + dir_y[*it]; b[i][j] != _play; i += dir_x[*it], j += dir_y[*it]) {
                     new_board[i][j] = _play;
              }
       }
       return new_board;
}
vector<pair<Point, set<int>>> get_valid_spot(int _play, array<array<int, SIZE>, SIZE> b) {   //valid for _play
/*
       cout << "here\n" ;
       cout << _play << endl;
       for( int i = 0; i < 8; i++) {
              for( int j = 0; j < 8; j++) {
                     cout << b[i][j];
              }
              cout << endl;
       }
       cout << endl;*/
       vector<pair<Point, set<int>>> _valid;
       int opponent = (_play == 1) ? 2: 1;
       for(int i = 0; i < 8; i++) {
              for(int j = 0; j < 8; j++) {
                     if( b[i][j] != 0) continue;
                     set<int> v_direction;
                     int flag = 0;                        //Point is valid or not
                     for(int d = 0; d < 8; d++) {   //8 directions, find all valid direction
                            int xx = i + dir_x[d];
                            int yy = j + dir_y[d];
                            int ok = 0;
                            while( 1) {
                                   if(xx >= 0 && xx  <= 7 && yy >= 0 && yy <= 7) {
                                          if( b[xx][yy] == 0) {
                                                 ok = 0;
                                                 break;
                                          }else if( b[xx][yy] == opponent) ok = 1;             //opponent and player is different from the above
                                          else if( b[xx][yy] == _play) break;
                                          xx += dir_x[d];
                                          yy += dir_y[d];
                                   }else {
                                          ok = 0;
                                          break;
                                   }
                            }
                            if(ok) {
                                   flag = 1, v_direction.insert(d);
                            }
                     }
                     pair<Point, set<int>> n_point;
                     n_point.first.x = i;
                     n_point.first.y = j;
                     n_point.second = v_direction;
                     if(flag) _valid.insert(_valid.begin(), n_point);
              }
       }
       return _valid;
}
int get_value(array<array<int, SIZE>, SIZE> b) {
       int value = 0;
       int opponent = (player == 1) ? 2:1;
       for( int i = 0; i < 8; i++) {
              for( int j = 0; j < 8; j++) {
                     if( b[i][j] == player) {
                            int tmp = 0;
                            if( kuo[i][j] == -100 || kuo[i][j] == -200) {
                                   if( i <= 1 && j <= 1 && b[0][0] == player)  tmp = 100;
                                   else if( i >= 6 && j <= 1 && b[7][0] == player) tmp = 100;
                                   else if( i <= 1 && j >= 6  &&b[0][7] == player) tmp = 100;
                                   else if( i >= 6 && j >= 6 &&  b[7][7] == player) tmp = 100;
                            }
                            if(tmp == 100) value+=100;
                            else value += kuo[i][j];
                     }
              }
       }
       return value;
}
State func(int depth, int alpha, int beta, int idx, array<std::array<int, SIZE>, SIZE> B,  int now_player) {

       int next_player = (now_player == 1) ? 2 : 1;
       if(depth == 5) {
              int value = get_value(B);
              return State(value, idx);
       }
       else if(depth == 0) {         //max
              State S = State(-10000000, idx);
              for( int i = 0; i < n_valid_spots; i++) {
                     array<array<int, SIZE>, SIZE> new_B = get_new_board(now_player, B, next_valid_spots[i]);

                     State newState = func(1, alpha, beta, i, new_B, next_player);
                     S = max( S, newState);
                     alpha = max( alpha, S.value);
                     if( alpha >= beta ) break;
              }
              return S;
       }else if( now_player ==  player) {
              State S = State(-10000000, idx);
              vector<pair<Point, set<int>>> valid = get_valid_spot(now_player, B);
              int _size = valid.size();
              for(int i = 0; i < _size; i++) {
                     array<array<int, SIZE>, SIZE> new_B = get_new_board2(now_player, B, valid[i]);
                     State newState = func(depth + 1, alpha, beta, idx, new_B, next_player);
                     S = max( S, newState);
                     alpha = max( alpha, S.value);
                     if( alpha >= beta ) break;
              }
              return S;
       }else {
              State S = State(10000000, idx);
              vector<pair<Point, set<int>>> valid = get_valid_spot(now_player, B);
              int _size = valid.size();
              for(int i = 0; i < _size; i++) {
                     array<array<int, SIZE>, SIZE> new_B = get_new_board2(now_player, B, valid[i]);
                     State newState = func(depth + 1, alpha, beta, idx, new_B, next_player);
                     S = min( S, newState);
                     beta = min( beta, S.value);
                     if( beta <= alpha ) break;
              }
              return S;
       }
}
void read_board(ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}

void read_valid_spots(ifstream& fin) {
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back({x, y});
    }
}

void write_valid_spot(ofstream& fout) {
       n_valid_spots = next_valid_spots.size();


       State S = func(0, -10000000, 10000000, 0, board, player);
       /*
       int index = (rand() % n_valid_spots);
       Point p = next_valid_spots[index];
       //Remember to flush the output to ensure the last action is written to file.
       fout << p.x << " " << p.y << std::endl;
       fout.flush();*/

       if(S.index < n_valid_spots) {
              Point p = next_valid_spots[S.index];
              fout << p.x << " " << p.y << endl;
              fout.flush();
       }


}
int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
