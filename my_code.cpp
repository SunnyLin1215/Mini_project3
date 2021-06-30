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
struct cmp{
       bool operator() (const vector<int> &lhs, const vector<int> &rhs)const {
              if( lhs[1] == rhs[1]) return lhs[0] < rhs[0];
              return lhs[1] > rhs[1];
       }
};
struct cmp2{
       bool operator() (const vector<int> &lhs, const vector<int> &rhs)const {
              if( lhs[1] == rhs[1]) return lhs[0] < rhs[0];
              return lhs[1] < rhs[1];
       }
};
int kuo[10][10] = {{700, -400, 70, 60, 60, 70, -400, 700}, { -400, -500, 10, 10, 10, 10, -500, -400},  {70, 10, 25, 15, 15, 25, 10, 70},{60, 10, 15, 5, 5, 15, 10, 60},{ 60, 10, 15, 5, 5, 15, 10, 60}, {70, 10, 25, 15, 15, 25, 10, 70}, {-400, -500, 10, 10, 10, 10, -500, -400}, {700, -400, 70, 60, 60, 70, -400, 700}};
int dir_x[10] = {-1, -1, -1, 0, 0, 1, 1, 1};    //x direction
int dir_y[10] = {-1, 0, 1, -1, 1, -1, 0, 1};     //y direction
int player, n_valid_spots;
int flag[70] = {0};
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
       float m_kuo = 0.0, o_kuo = 0.0, me = 0.0, oppo = 0.0, m_corner = 0.0, o_corner = 0.0;
       int opponent = (player == 1) ? 2:1;
       for( int i = 0; i < 8; i++) {
              for( int j = 0; j < 8; j++) {
                     if( b[i][j] == player) {
                            if( (i == 0|| i == 7) && (j == 0 || j == 7)) m_corner++;
                            me++;
                            int tmp = 0;
                            if( kuo[i][j] == -400 || kuo[i][j] == -500) {
                                   if( i <= 1 && j <= 1 && b[0][0] == player)  tmp = 100;
                                   else if( i >= 6 && j <= 1 && b[7][0] == player) tmp = 100;
                                   else if( i <= 1 && j >= 6  &&b[0][7] == player) tmp = 100;
                                   else if( i >= 6 && j >= 6 &&  b[7][7] == player) tmp = 100;
                            }
                            if(tmp == 100) m_kuo+=100;
                            else m_kuo += kuo[i][j];
                     }else if( b[i][j] == opponent) {
                            if( (i == 0|| i == 7) && (j == 0 || j == 7)) o_corner++;
                            oppo++;
                            int tmp = 0;
                            if( kuo[i][j] == -400 || kuo[i][j] == -500) {
                                   if( i <= 1 && j <= 1 && b[0][0] == opponent)  tmp = 100;
                                   else if( i >= 6 && j <= 1 && b[7][0] == opponent) tmp = 100;
                                   else if( i <= 1 && j >= 6  &&b[0][7] == opponent) tmp = 100;
                                   else if( i >= 6 && j >= 6 &&  b[7][7] == opponent) tmp = 100;
                            }
                            if(tmp == 100) o_kuo+=100;
                            else o_kuo += kuo[i][j];
                     }
              }
       }
        int m_valid = (get_valid_spot( player, b)).size();
       int o_valid = (get_valid_spot( opponent, b)).size();
       float num = (me + oppo == 0)? 0 : ((me - oppo)/(me + oppo));
       float kkuo = (m_kuo + o_kuo == 0) ? 0 : ((m_kuo - o_kuo)/(m_kuo + o_kuo));
       float valid = (m_valid + o_valid == 0) ? 0 : ((m_valid - o_valid)/(m_valid + o_valid));
       float corner = (m_corner + o_corner == 0) ? 0 : ((m_corner - o_corner)/(m_corner + o_corner));

       int ans = 50 * num + 100* kkuo + 500* valid + 600* corner;
       return ans;
}
bool all_filled_B( array<std::array<int, SIZE>, SIZE> B) {
       for( int i = 0; i < 8; i++) {
              for( int j = 0; j < 8; j++) {
                     if(B[i][j] == 0) return false;
              }
       }
       return true;
}
int* get_new_valid(int* f, vector<pair<Point, set<int>>> valid, array<std::array<int, SIZE>, SIZE> B, int _play) {
       set<vector<int>, cmp> point;            //idx, value
       int _s = valid.size();
       for( int k = 0; k < _s; k++) {
              f[k] = 0;
              int i = valid[k].first.x;
              int j = valid[k].first.y;
              int value = kuo[i][j];
              if( kuo[i][j] == -400 || kuo[i][j] == -500) {
                     if( i <= 1 && j <= 1 && B[0][0] == _play)  value = 100;
                     else if( i >= 6 && j <= 1 && B[7][0] == _play) value = 100;
                     else if( i <= 1 && j >= 6  && B[0][7] == _play) value = 100;
                     else if( i >= 6 && j >= 6 &&  B[7][7] == _play) value = 100;
              }
              point.insert({k, value});
       }

       if( _s > 4) {
              auto it = point.begin();
              for( int i = 0; i < 4; i++) {
                     f[(*it)[0]] = 1;
                     ++it;
              }
       }else {
              for( int i = 0; i < _s + 5; i++) f[i] = 1;
       }
       return f;
}
int* get_new_valid_min(int* f, vector<pair<Point, set<int>>> valid, array<std::array<int, SIZE>, SIZE> B, int _play) {
       set<vector<int>, cmp2> point;            //idx, value
       int _s = valid.size();
       for( int k = 0; k < _s; k++) {
              f[k] = 0;
              int i = valid[k].first.x;
              int j = valid[k].first.y;
              int value = kuo[i][j];
              if( kuo[i][j] == -400 || kuo[i][j] == -500) {
                     if( i <= 1 && j <= 1 && B[0][0] == _play)  value = 100;
                     else if( i >= 6 && j <= 1 && B[7][0] == _play) value = 100;
                     else if( i <= 1 && j >= 6  && B[0][7] == _play) value = 100;
                     else if( i >= 6 && j >= 6 &&  B[7][7] == _play) value = 100;
              }
              point.insert({k, value});
       }

       if( _s > 4) {
              auto it = point.begin();
              for( int i = 0; i < 4; i++) {
                     f[(*it)[0]] = 1;
                     ++it;
              }
       }else {
              for( int i = 0; i < _s + 5; i++) f[i] = 1;
       }
       return f;
}
State func(int depth, int alpha, int beta, int idx, array<std::array<int, SIZE>, SIZE> B,  int now_player) {
       int next_player = (now_player == 1) ? 2 : 1;
       if(depth == 6 || all_filled_B(B)) {
              int value = get_value(B);
              return State(value, idx);
       }
       else if(depth == 0) {         //max
              State S = State(-10000000, idx);
              for( int i = 0; i < n_valid_spots; i++) {
                     if(flag[i] != 1) continue;
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
              if(_size ==  0)  {
                      int value = get_value(B);
                     return State(value, idx);
              }
              //int* _flag = new int[70];
              //_flag = get_new_valid( _flag, valid, B, now_player);
              for(int i = 0; i < _size; i++) {
                     //if( _flag[i] == 0) continue;
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
              if(_size ==  0)  {
                      int value = get_value(B);
                     return State(value, idx);
              }

              //int* _flag = new int[70];
              //_flag = get_new_valid_min( _flag, valid, B, now_player);
              for(int i = 0; i < _size; i++) {
                     //if(_flag[i] == 0) continue;
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

       set<vector<int>, cmp> point;
       for( int k = 0; k < n_valid_spots; k++) {
              flag[k] = 0;
              int i = next_valid_spots[k].x;
              int j = next_valid_spots[k].y;
              int value = kuo[i][j];
              if( kuo[i][j] == -400 || kuo[i][j] == -500) {
                     if( i <= 1 && j <= 1 && board[0][0] == player)  value = 100;
                     else if( i >= 6 && j <= 1 && board[7][0] == player) value = 100;
                     else if( i <= 1 && j >= 6  &&board[0][7] == player) value = 100;
                     else if( i >= 6 && j >= 6 &&  board[7][7] == player) value = 100;
              }
              point.insert({k, value});
       }

       if( n_valid_spots > 4) {
              auto it = point.begin();
              for( int i = 0; i < 4; i++) {
                     flag[(*it)[0]] = 1;
                     ++it;
              }
       }else {
              for( int i = 0; i < n_valid_spots + 5; i++) flag[i] = 1;
       }

       Point p = next_valid_spots[(*(point.begin()))[0]];
       fout << p.x << " " << p.y << endl;
       fout.flush();

       if(kuo[p.x][p.y] == 700) return;
       State S = func(0, -10000000, 10000000, -1, board, player);

       //cout << S.index <<endl;
       if(S.index < n_valid_spots && S.index >= 0) {
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
