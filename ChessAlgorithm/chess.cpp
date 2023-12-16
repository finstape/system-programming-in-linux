#include <iostream>
#include <vector>
#include <random>
#include <map>

using namespace std;

struct Board {
    vector<vector<string>> BOARD;

    Board(int white_pawns, int white_bishops, int white_knights, int white_rooks, int black_pawns, int black_bishops,
          int black_knights, int black_rooks) {
        BOARD = vector<vector<string>>(8, vector<string>(8, "--"));
        BOARD[0][4] = "eg";
        BOARD[7][4] = "mg";
        for (int i = 0; i < white_pawns; i++) {
            BOARD[1][i] = "ep";
        }
        for (int i = 0; i < black_pawns; i++) {
            BOARD[6][i] = "mp";
        }
        for (int i = 0; i < white_bishops; i++) {
            BOARD[0][3 * i + 2] = "eb";
        }
        for (int i = 0; i < black_bishops; i++) {
            BOARD[7][3 * i + 2] = "mb";
        }
        for (int i = 0; i < white_knights; i++) {
            BOARD[0][5 * i + 1] = "ek";
        }
        for (int i = 0; i < black_knights; i++) {
            BOARD[7][5 * i + 1] = "mk";
        }
        for (int i = 0; i < white_rooks; i++) {
            BOARD[0][7 * i] = "er";
        }
        for (int i = 0; i < black_rooks; i++) {
            BOARD[7][7 * i] = "mr";
        }
    }
};

map<string, string> SYMBOL_DICT = {
        {"ep", "♙"},
        {"ek", "♘"},
        {"eb", "♗"},
        {"er", "♖"},
        {"eg", "♔"},
        {"mp", "♟"},
        {"mk", "♞"},
        {"mb", "♝"},
        {"mr", "♜"},
        {"mg", "♚"},
        {"--", " "}
};

map<string, int> SYMBOL_SCORE = {
        {"ep", 1},
        {"ek", 3},
        {"eb", 3},
        {"er", 5},
        {"mp", 1},
        {"mk", 3},
        {"mb", 3},
        {"mr", 5},
        {"--", 0}
};

/*void show_board(const Board &board) {
    cout << "a\t \tb\t \tc\t \td\t \te\t \tf\t \tg\t \th" << endl;
    for (int row_num = 0; row_num < 8; row_num++) {
        string row_str = "";
        for (const string &figure: board.BOARD[row_num]) {
            if (figure != "--") {
                row_str += SYMBOL_DICT[figure] + "\t|\t";
            } else {
                row_str += "-\t|\t";
            }
        }
        cout << row_str << 8 - row_num << endl;
    }
    cout << "=================================" << endl;
}*/

void show_board(const Board &board) {
    cout << "a  b  c  d  e  f  g  h" << endl;
    for (int row_num = 0; row_num < 8; row_num++) {
        string row_str;
        for (const string &figure: board.BOARD[row_num]) {
            if (figure != "--") {
                row_str += figure + "|";
            } else {
                row_str += "--|";
            }
        }
        cout << row_str << 8 - row_num << endl;
    }
    cout << "=========================" << endl;
}

Board rotate_board(const Board &board) {
    Board reversed_board = board;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            reversed_board.BOARD[i][j] = board.BOARD[7 - i][7 - j];
            if (reversed_board.BOARD[i][j][0] == 'e') {
                reversed_board.BOARD[i][j] = 'm' + reversed_board.BOARD[i][j].substr(1);
            } else if (reversed_board.BOARD[i][j][0] == 'm') {
                reversed_board.BOARD[i][j] = 'e' + reversed_board.BOARD[i][j].substr(1);
            }
        }
    }
    return reversed_board;
}

bool on_the_border(int coord) {
    return 0 <= coord && coord <= 7;
}

vector<vector<int>> make_moves(const Board &board, int x, int y) {
    vector<vector<int>> possible_moves;
    auto add_move = [&](int move_x, int move_y) {
        if (on_the_border(x + move_x) && on_the_border(y + move_y)) {
            possible_moves.push_back({x + move_x, y + move_y});
        }
    };
    if (board.BOARD[x][y] == "mp") {
        add_move(-1, 0);
        add_move(-1, 1);
        add_move(-1, -1);
    }
    if (board.BOARD[x][y] == "mk") {
        for (int mk_x: {-2, -1, 1, 2}) {
            for (int mk_y: {-2, -1, 1, 2}) {
                if (abs(mk_x) != abs(mk_y) && on_the_border(x + mk_x) && on_the_border(y + mk_y) &&
                    (board.BOARD[x + mk_x][y + mk_y][0] == 'e' || board.BOARD[x + mk_x][y + mk_y][0] == '-')) {
                    possible_moves.push_back({x + mk_x, y + mk_y});
                }
            }
        }
    }
    auto moves_on_line = [&](int add_x, int add_y) {
        int r_x = x + add_x;
        int r_y = y + add_y;
        while (on_the_border(r_x) && on_the_border(r_y) && board.BOARD[r_x][r_y] == "--") {
            possible_moves.push_back({r_x, r_y});
            r_x += add_x;
            r_y += add_y;
        }
        if (on_the_border(r_x) && on_the_border(r_y) && board.BOARD[r_x][r_y][0] == 'e') {
            possible_moves.push_back({r_x, r_y});
        }
    };
    if (board.BOARD[x][y] == "mr") {
        moves_on_line(1, 0);
        moves_on_line(-1, 0);
        moves_on_line(0, 1);
        moves_on_line(0, -1);
    }
    if (board.BOARD[x][y] == "mb") {
        moves_on_line(1, 1);
        moves_on_line(1, -1);
        moves_on_line(-1, 1);
        moves_on_line(-1, -1);
    }
    if (board.BOARD[x][y] == "mg") {
        for (int add_x = -1; add_x <= 1; add_x++) {
            for (int add_y = -1; add_y <= 1; add_y++) {
                if (abs(add_x) + abs(add_y) > 0 && on_the_border(x + add_x) && on_the_border(y + add_y) &&
                    (board.BOARD[x + add_x][y + add_y][0] == '-' || board.BOARD[x + add_x][y + add_y][0] == 'e')) {
                    possible_moves.push_back({x + add_x, y + add_y});
                }
            }
        }
    }
    return possible_moves;
}

bool is_check(const Board &board, char player) {
    int kingX = -1, kingY = -1;
    string kingSymbol = (player == 'e') ? "eg" : "mg";

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board.BOARD[i][j] == kingSymbol) {
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
            if (board.BOARD[i][j][0] == opponent) {
                vector<vector<int>> moves = make_moves(board, i, j);
                for (const auto &move: moves) {
                    if (move[0] == kingX && move[1] == kingY) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool is_checkmate(const Board &board) {
    int kingX = -1, kingY = -1;
    string kingSymbol = (board.BOARD[0][4][0] == 'e') ? "eg" : "mg";

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board.BOARD[i][j] == kingSymbol) {
                kingX = i;
                kingY = j;
                break;
            }
        }
        if (kingX != -1) {
            break;
        }
    }

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) {
                continue;
            }

            int newX = kingX + i;
            int newY = kingY + j;

            if (on_the_border(newX) && on_the_border(newY) && board.BOARD[newX][newY][0] == '-') {
                Board newBoard = board;
                newBoard.BOARD[newX][newY] = kingSymbol;
                newBoard.BOARD[kingX][kingY] = "--";

                if (!is_check(newBoard, kingSymbol[0])) {
                    return false;
                }
            }
        }
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board.BOARD[i][j][0] == kingSymbol[0]) {
                continue;
            }

            vector<vector<int>> moves = make_moves(board, i, j);
            for (const auto &move: moves) {
                Board newBoard = board;
                newBoard.BOARD[move[0]][move[1]] = board.BOARD[i][j];
                newBoard.BOARD[i][j] = "--";

                if (!is_check(newBoard, kingSymbol[0])) {
                    return false;
                }
            }
        }
    }
    return true;
}

Board generate_moves(Board board, int num_moves, bool noCapture, bool noCheckmate) {
    random_device rd;
    mt19937 gen(rd());

    for (int i = 0; i < num_moves; i++) {
        uniform_int_distribution<int> dist(0, 7);
        vector<int> cell = {dist(gen), dist(gen)};
        vector<vector<int>> possible_moves;

        while (make_moves(board, cell[0], cell[1]).empty()) {
            cell = {dist(gen), dist(gen)};
        }

        possible_moves = make_moves(board, cell[0], cell[1]);
        uniform_int_distribution<int> move_dist(0, int(possible_moves.size()) - 1);
        vector<int> chosen_move = possible_moves[move_dist(gen)];

        if (board.BOARD[cell[0]][cell[1]] == "mp" && chosen_move[0] == 0) {
            board.BOARD[chosen_move[0]][chosen_move[1]] = "mk";
        } else {
            if (noCapture && board.BOARD[chosen_move[0]][chosen_move[1]] != "--") {
                i--;
                continue;
            }

            board.BOARD[chosen_move[0]][chosen_move[1]] = board.BOARD[cell[0]][cell[1]];
        }

        if (board.BOARD[cell[0]][cell[1]] == "mp" && chosen_move[0] == 0) {
            board.BOARD[cell[0]][cell[1]] = "mk";
        } else {
            board.BOARD[cell[0]][cell[1]] = "--";
        }

        if (noCheckmate) {
            if (is_checkmate(board)) {
                i--;
                continue;
            }
        }
    }
    return rotate_board(board);
}

int main() {
    int white_pawns, white_bishops, white_knights, white_rooks;
    int black_pawns, black_bishops, black_knights, black_rooks;

    cout << "Enter the number of figures for black pieces (pawns bishops knights rooks): " << endl;
    cin >> white_pawns >> white_bishops >> white_knights >> white_rooks;
    if (white_pawns < 0 || white_pawns > 8 || white_bishops < 0 || white_bishops > 8 ||
        white_knights < 0 || white_knights > 8 || white_rooks < 0 || white_rooks > 8) {
        cout << "Error: Invalid number of figures for black pieces. Each type should be between 0 and 8." << endl;
        return 1;
    }

    cout << "Enter the number of figures for white pieces (pawns bishops knights rooks): " << endl;
    cin >> black_pawns >> black_bishops >> black_knights >> black_rooks;
    if (black_pawns < 0 || black_pawns > 8 || black_bishops < 0 || black_bishops > 8 ||
        black_knights < 0 || black_knights > 8 || black_rooks < 0 || black_rooks > 8) {
        cout << "Error: Invalid number of figures for white pieces. Each type should be between 0 and 8." << endl;
        return 1;
    }

    Board desk(white_pawns, white_bishops, white_knights, white_rooks, black_pawns, black_bishops, black_knights,
               black_rooks);
    for (int i = 0; i < 30; i++) {
        desk = generate_moves(desk, 1, true, true);
    }
    desk = rotate_board(desk);
    show_board(desk);
    return 0;
}