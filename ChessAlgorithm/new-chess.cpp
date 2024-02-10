#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>
#include <map>

using namespace std;

struct Move {
    int from_x, from_y, to_x, to_y, score;

    Move(int from_x, int from_y, int to_x, int to_y, int score) : from_x(from_x), from_y(from_y), to_x(to_x),
                                                                  to_y(to_y), score(score) {}
};

map<string, int> SYMBOL_SCORE = {
        {"ep", 1},
        {"ek", 3},
        {"eb", 3},
        {"er", 5},
        {"--", 0}
};

class Board {
private:
    vector<vector<string>> BOARD;

public:
    Board(int white_pawns, int white_bishops, int white_knights, int white_rooks, int black_pawns, int black_bishops,
          int black_knights, int black_rooks) {
        BOARD = vector<vector<string>>(8, vector<string>(8, "--"));
        BOARD[0][4] = "eg";
        BOARD[7][4] = "mg";
        srand(time(nullptr));

        placeFigures(white_pawns, 6, "mp");
        placeFigures(black_pawns, 1, "ep");
        placeFigures(white_bishops, 7, "mb");
        placeFigures(black_bishops, 0, "eb");
        placeFigures(white_knights, 7, "mk");
        placeFigures(black_knights, 0, "ek");
        placeFigures(white_rooks, 7, "mr");
        placeFigures(black_rooks, 0, "er");
    }

    Board(const vector<vector<string>> &initial_board) : BOARD(initial_board) {}

    Board rotate_board() const {
        Board reversed_board = *this;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                reversed_board.BOARD[i][j] = BOARD[7 - i][7 - j];
                if (reversed_board.BOARD[i][j][0] == 'e') {
                    reversed_board.BOARD[i][j] = 'm' + reversed_board.BOARD[i][j].substr(1);
                } else if (reversed_board.BOARD[i][j][0] == 'm') {
                    reversed_board.BOARD[i][j] = 'e' + reversed_board.BOARD[i][j].substr(1);
                }
            }
        }
        return reversed_board;
    }

    const vector<string> &operator[](int index) const {
        return BOARD.at(index);
    }

    vector<string> &operator[](int index) {
        return BOARD.at(index);
    }

    Board &operator=(const Board &other) {
        if (this != &other) {
            BOARD = other.BOARD;
        }
        return *this;
    }

    friend ostream &operator<<(ostream &os, const Board &board) {
        os << "a  b  c  d  e  f  g  h" << endl;
        for (int row_num = 0; row_num < 8; row_num++) {
            string row_str;
            for (const string &figure: board.BOARD[row_num]) {
                row_str += (figure != "--") ? figure + "|" : "--|";
            }
            os << row_str << 8 - row_num << endl;
        }
        os << "=========================" << endl;
        return os;
    }

private:
    void placeFigures(int num_figures, int row, const string &figure) {
        while (num_figures > 0) {
            int j = rand() % 8;
            int step = 1, start_pos = 0;;

            switch (figure[1]) {
                case 'p':
                    break;
                case 'b':
                    j %= 2;
                    step = 3;
                    start_pos = 2;
                    break;
                case 'k':
                    j %= 2;
                    step = 5;
                    start_pos = 1;
                    break;
                case 'r':
                    j %= 2;
                    step = 7;
                    break;
            }

            if (BOARD[row][start_pos + j * step] == "--") {
                BOARD[row][start_pos + j * step] = figure;
                num_figures--;
            }
        }
    }
};

bool is_check(const Board &board, char player);

bool on_the_border(int coord) {
    return 0 <= coord && coord <= 7;
}

vector<vector<int> > make_moves(const Board &board, int x, int y, bool check_check) {
    vector<vector<int> > moves;
    char ENEMY = (board[x][y][0] == 'm') ? 'e' : 'm';

    auto moves_on_line = [&](int add_x, int add_y) {
        int r_x = x + add_x;
        int r_y = y + add_y;
        while (on_the_border(r_x) && on_the_border(r_y) && board[r_x][r_y][0] == '-') {
            moves.push_back({r_x, r_y});
            r_x += add_x;
            r_y += add_y;
        }
        if (on_the_border(r_x) && on_the_border(r_y) && board[r_x][r_y][0] == ENEMY) {
            moves.push_back({r_x, r_y});
        }
    };

    switch (board[x][y][1]) {
        case 'p':
            if (board[x][y][0] == 'm') {
                if (x == 6 && board[x - 2][y][0] == '-' && board[x - 1][y][0] == '-') {
                    moves.push_back({x - 2, y});
                }
                if (on_the_border(x - 1) && board[x - 1][y][0] == '-') {
                    moves.push_back({x - 1, y});
                }
                if (on_the_border(x - 1) && on_the_border(y - 1) && board[x - 1][y - 1][0] == ENEMY) {
                    moves.push_back({x - 1, y - 1});
                }
                if (on_the_border(x - 1) && on_the_border(y + 1) && board[x - 1][y + 1][0] == ENEMY) {
                    moves.push_back({x - 1, y + 1});
                }
            } else {
                if (x == 1 && board[x + 2][y][0] == '-' && board[x + 1][y][0] == '-') {
                    moves.push_back({x + 2, y});
                }
                if (on_the_border(x + 1) && board[x + 1][y][0] == '-') {
                    moves.push_back({x + 1, y});
                }
                if (on_the_border(x + 1) && on_the_border(y + 1) && board[x + 1][y + 1][0] == ENEMY) {
                    moves.push_back({x + 1, y + 1});
                }
                if (on_the_border(x + 1) && on_the_border(y - 1) && board[x + 1][y - 1][0] == ENEMY) {
                    moves.push_back({x + 1, y - 1});
                }
            }
            break;
        case 'b':
            moves_on_line(1, 1);
            moves_on_line(1, -1);
            moves_on_line(-1, 1);
            moves_on_line(-1, -1);
            break;
        case 'k':
            for (int mk_x: {-2, -1, 1, 2}) {
                for (int mk_y: {-2, -1, 1, 2}) {
                    if (abs(mk_x) != abs(mk_y) && on_the_border(x + mk_x) && on_the_border(y + mk_y) &&
                        (board[x + mk_x][y + mk_y][0] == ENEMY || board[x + mk_x][y + mk_y][0] == '-')) {
                        moves.push_back({x + mk_x, y + mk_y});
                    }
                }
            }
            break;
        case 'r':
            moves_on_line(1, 0);
            moves_on_line(-1, 0);
            moves_on_line(0, 1);
            moves_on_line(0, -1);
            break;
        case 'g':
            for (int add_x = -1; add_x <= 1; add_x++) {
                for (int add_y = -1; add_y <= 1; add_y++) {
                    if (abs(add_x) + abs(add_y) > 0 && on_the_border(x + add_x) && on_the_border(y + add_y) &&
                        (board[x + add_x][y + add_y][0] == '-' || board[x + add_x][y + add_y][0] == ENEMY)) {

                        bool valid_move = true;
                        string ENEMY_G = string(1, ENEMY) + "g";
                        for (int i = 0; i < 8; i++) {
                            for (int j = 0; j < 8; j++) {
                                if (board[i][j] == ENEMY_G) {
                                    if (abs(i - (x + add_x)) < 2 && abs(j - (y + add_y)) < 2) {
                                        valid_move = false;
                                        break;
                                    }
                                }
                            }
                            if (!valid_move) {
                                break;
                            }
                        }

                        if (valid_move) {
                            moves.push_back({x + add_x, y + add_y});
                        }
                    }
                }
            }
            break;
    }

    if (!check_check) {
        return moves;
    }

    vector<vector<int> > filtered_moves;
    for (const auto &move_coords: moves) {
        Board temp_board = board;
        temp_board[move_coords[0]][move_coords[1]] = temp_board[x][y];
        temp_board[x][y] = "--";
        if (!is_check(temp_board, board[x][y][0])) {
            filtered_moves.push_back(move_coords);
        }
    }
    return filtered_moves;
}

bool is_check(const Board &board, char player) {
    int kingX = -1, kingY = -1;
    string kingSymbol = (player == 'e') ? "eg" : "mg";

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] == kingSymbol) {
                kingX = i;
                kingY = j;
                break;
            }
        }
        if (kingX != -1) {
            break;
        }
    }

    char opponent = (player == 'e') ? 'm' : 'e';
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j][0] == opponent) {
                vector<vector<int>> moves = make_moves(board, i, j, false);
                for (const auto &move_coords: moves) {
                    if (move_coords[0] == kingX && move_coords[1] == kingY) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool is_checkmate(const Board &board) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j][0] == 'e') {
                vector<vector<int>> moves = make_moves(board, i, j, false);
                for (const auto &move: moves) {
                    Board newBoard = board;
                    newBoard[move[0]][move[1]] = board[i][j];
                    newBoard[i][j] = "--";

                    if (!is_check(newBoard, 'e')) {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

Board get_random_board(const Board &board) {
    Board temp_board = board;
    for (int _ = 0; _ < 30; _++) {
        vector<vector<int> > figures;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (temp_board[i][j][0] == 'm') {
                    figures.push_back({i, j});
                }
            }
        }

        mt19937 rng(random_device{}());
        shuffle(figures.begin(), figures.end(), rng);

        bool is_moved = false;
        for (auto figure_coords: figures) {
            vector<vector<int> > moves = make_moves(temp_board, figure_coords[0], figure_coords[1], true);
            shuffle(moves.begin(), moves.end(), rng);

            for (auto move_coords: moves) {
                if ((move_coords[0] != 0 or temp_board[figure_coords[0]][figure_coords[1]][1] != 'p') &&
                    temp_board[move_coords[0]][move_coords[1]][0] != 'e') {

                    Board TEMP_BOARD = temp_board;
                    TEMP_BOARD[move_coords[0]][move_coords[1]] = TEMP_BOARD[figure_coords[0]][figure_coords[1]];
                    TEMP_BOARD[figure_coords[0]][figure_coords[1]] = "--";

                    if (!is_check(TEMP_BOARD, 'e') && !is_check(TEMP_BOARD, 'm')) {
                        is_moved = true;
                        temp_board[move_coords[0]][move_coords[1]] = temp_board[figure_coords[0]][figure_coords[1]];
                        temp_board[figure_coords[0]][figure_coords[1]] = "--";
                        break;
                    }
                }
            }

            if (is_moved) {
                break;
            }
        }

        if (!is_moved) {
            return get_random_board(board);
        } else {
            temp_board = temp_board.rotate_board();
        }
    }
    return temp_board;
}

int calculate_score(const Board &board, int from_x, int from_y, int to_x, int to_y) {
    int base_score = SYMBOL_SCORE[board[to_x][to_y]];

    Board temp_board = board;
    if (temp_board[from_x][from_y] == "mp" && to_x == 0) {
        temp_board[to_x][to_y] = "mk";
    } else {
        temp_board[to_x][to_y] = temp_board[from_x][from_y];
    }
    temp_board[from_x][from_y] = "--";

    if (is_check(temp_board, 'e')) {
        if (is_checkmate(temp_board)) {
            base_score = 1000;
        } else {
            base_score = 100;
        }
    }

    return base_score;
}

void find_best_moves_recursive(const Board &board, int num_moves, vector<Move> &current_moves,
                               vector<vector<Move>> &all_moves) {
    if (num_moves == 0) {
        all_moves.push_back(current_moves);
        return;
    }

    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (board[x][y][0] == 'm') {
                vector<vector<int>> piece_moves = make_moves(board, x, y, true);
                for (const auto &move: piece_moves) {
                    int score = calculate_score(board, x, y, move[0], move[1]);

                    Board temp_board = board;
                    if (temp_board[x][y] == "mp" && move[0] == 0) {
                        temp_board[move[0]][move[1]] = "mk";
                    } else {
                        temp_board[move[0]][move[1]] = temp_board[x][y];
                    }
                    temp_board[x][y] = "--";

                    current_moves.emplace_back(x, y, move[0], move[1], score);

                    if (score >= 100) {
                        find_best_moves_recursive(temp_board, 0, current_moves, all_moves);
                    } else {
                        find_best_moves_recursive(temp_board, num_moves - 1, current_moves, all_moves);
                    }

                    current_moves.pop_back();
                }
            }
        }
    }
}

int main() {
    int white_pawns, white_bishops, white_knights, white_rooks;
    int black_pawns, black_bishops, black_knights, black_rooks;

    cout << "Enter the number of figures for white pieces (pawns bishops knights rooks): " << endl;
    cin >> white_pawns >> white_bishops >> white_knights >> white_rooks;
    if (white_pawns < 0 || white_pawns > 8 || white_bishops < 0 || white_bishops > 2 || white_knights < 0 ||
        white_knights > 2 || white_rooks < 0 || white_rooks > 2) {
        cout << "Error: Invalid number of figures for white pieces." << endl;
        return 1;
    }

    cout << "Enter the number of figures for black pieces (pawns bishops knights rooks): " << endl;
    cin >> black_pawns >> black_bishops >> black_knights >> black_rooks;
    if (black_pawns < 0 || black_pawns > 8 || black_bishops < 0 || black_bishops > 2 || black_knights < 0 ||
        black_knights > 2 || black_rooks < 0 || black_rooks > 2) {
        cout << "Error: Invalid number of figures for black pieces." << endl;
        return 1;
    }

    Board board(white_pawns, white_bishops, white_knights, white_rooks, black_pawns, black_bishops, black_knights,
                black_rooks);
    board = get_random_board(board);
    cout << board << endl;

    vector<Move> current_moves;
    vector<vector<Move>> all_moves;
    find_best_moves_recursive(board, 3, current_moves, all_moves);
    sort(all_moves.begin(), all_moves.end(),
         [](const vector<Move> &a, const vector<Move> &b) {
             int score_a = accumulate(a.begin(), a.end(), 0,
                                      [](int sum, const Move &move) { return sum + move.score; });
             int score_b = accumulate(b.begin(), b.end(), 0,
                                      [](int sum, const Move &move) { return sum + move.score; });
             if (score_a != score_b) {
                 return score_a > score_b;
             }
             return a.size() < b.size();
         });

    for (int i = 0; i < min(3, static_cast<int>(all_moves.size())); ++i) {
        const auto &move_sequence = all_moves[i];

        cout << "Move sequence (Total Score: "
             << accumulate(move_sequence.begin(), move_sequence.end(), 0,
                           [](int sum, const Move &move) { return sum + move.score; })
             << "):" << endl;

        Board board_after_moves = board;
        for (const auto &move: move_sequence) {
            cout << "Move: " << board_after_moves[move.from_x][move.from_y]
                 << " from " << static_cast<char>('a' + move.from_y) << 8 - move.from_x
                 << " to " << static_cast<char>('a' + move.to_y) << 8 - move.to_x
                 << " (Score: " << move.score << ")" << endl;

            board_after_moves[move.to_x][move.to_y] = board_after_moves[move.from_x][move.from_y];
            board_after_moves[move.from_x][move.from_y] = "--";
            cout << board_after_moves;
        }
        cout << endl;
    }
    return 0;
}