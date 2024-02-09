#include <iostream>
#include <vector>
#include <random>
#include <map>
#include <algorithm>

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

    Board(const Board &other) {
        BOARD = other.BOARD;
    }

    Board &operator=(const Board &other) {
        BOARD = other.BOARD;
        return *this;
    }
};

struct Move {
    int from_x, from_y, to_x, to_y, score;

    Move(int from_x, int from_y, int to_x, int to_y, int score) : from_x(from_x), from_y(from_y), to_x(to_x),
                                                                  to_y(to_y), score(score) {}
};

bool is_check(const Board &board, char player);

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
        if (on_the_border(x - 1) && board.BOARD[x - 1][y] == "--") {
            possible_moves.push_back({x - 1, y});
        }
        if (on_the_border(x - 1) && on_the_border(y - 1) && board.BOARD[x - 1][y - 1][0] == 'e') {
            possible_moves.push_back({x - 1, y - 1});
        }
        if (on_the_border(x - 1) && on_the_border(y + 1) && board.BOARD[x - 1][y + 1][0] == 'e') {
            possible_moves.push_back({x - 1, y + 1});
        }
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

                    bool valid_move = true;
                    for (int i = 0; i < 8; i++) {
                        for (int j = 0; j < 8; j++) {
                            if (board.BOARD[i][j] == "eg") {
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
                        possible_moves.push_back({x + add_x, y + add_y});
                    }
                }
            }
        }
    }
    vector<vector<int>> filtered_moves;
    for (const auto &move: possible_moves) {
        Board temp_board = board;
        temp_board.BOARD[move[0]][move[1]] = temp_board.BOARD[x][y];
        temp_board.BOARD[x][y] = "--";

        if (!is_check(temp_board, temp_board.BOARD[move[0]][move[1]][0])) {
            filtered_moves.push_back(move);
        }
    }

    return filtered_moves;
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

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board.BOARD[i][j] == "eg") {
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

            if (on_the_border(newX) && on_the_border(newY) &&
                (board.BOARD[newX][newY][0] == '-' || board.BOARD[newX][newY][0] == 'm')) {
                Board newBoard = board;
                newBoard.BOARD[newX][newY] = "eg";
                newBoard.BOARD[kingX][kingY] = "--";

                if (!is_check(newBoard, 'e')) {
                    return false;
                }
            }
        }
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board.BOARD[i][j][0] == 'e') {
                vector<vector<int>> moves = make_moves(board, i, j);
                for (const auto &move: moves) {
                    Board newBoard = board;
                    newBoard.BOARD[move[0]][move[1]] = board.BOARD[i][j];
                    newBoard.BOARD[i][j] = "--";

                    if (!is_check(newBoard, 'e')) {
                        return false;
                    }
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

        if (noCapture && board.BOARD[chosen_move[0]][chosen_move[1]] != "--") {
            i--;
            continue;
        }

        if (board.BOARD[cell[0]][cell[1]][0] == board.BOARD[chosen_move[0]][chosen_move[1]][0]) {
            i--;
            continue;
        }

        if (board.BOARD[cell[0]][cell[1]] == "mp" && chosen_move[0] == 0) {
            board.BOARD[chosen_move[0]][chosen_move[1]] = "mk";
        } else {
            board.BOARD[chosen_move[0]][chosen_move[1]] = board.BOARD[cell[0]][cell[1]];
        }

        if (board.BOARD[cell[0]][cell[1]] == "mp" && chosen_move[0] == 0) {
            board.BOARD[cell[0]][cell[1]] = "mk";
        } else {
            board.BOARD[cell[0]][cell[1]] = "--";
        }

        if (noCheckmate) {
            if (is_checkmate(board) ||
                is_check(board, (board.BOARD[chosen_move[0]][chosen_move[1]][0] == 'e' ? 'm' : 'e'))) {
                i--;
                continue;
            }
        }
    }
    return rotate_board(board);
}

int calculate_score(const Board &board, int from_x, int from_y, int to_x, int to_y) {
    int base_score = SYMBOL_SCORE[board.BOARD[from_x][from_y]];

    if (board.BOARD[to_x][to_y] != "--") {
        base_score += SYMBOL_SCORE[board.BOARD[to_x][to_y]];
    }

    Board temp_board = board;
    temp_board.BOARD[to_x][to_y] = temp_board.BOARD[from_x][from_y];
    temp_board.BOARD[from_x][from_y] = "--";

    if (is_check(temp_board, 'e')) {
        base_score += 100;
    }

    if (is_checkmate(temp_board)) {
        base_score += 1000;
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
            if (board.BOARD[x][y][0] == 'm') {
                vector<vector<int>> piece_moves = make_moves(board, x, y);
                for (const auto &move: piece_moves) {
                    //cout << move[0] << " " << move[1] << " " << x << " " << y << endl;
                    int score = calculate_score(board, x, y, move[0], move[1]);

                    Board temp_board = board;
                    temp_board.BOARD[move[0]][move[1]] = temp_board.BOARD[x][y];
                    temp_board.BOARD[x][y] = "--";

                    current_moves.emplace_back(x, y, move[0], move[1], score);

                    find_best_moves_recursive(temp_board, num_moves - 1, current_moves, all_moves);

                    current_moves.pop_back();
                }
            }
        }
    }
}


int main() {
    int white_pawns, white_bishops, white_knights, white_rooks;
    int black_pawns, black_bishops, black_knights, black_rooks;

    cout << "Enter the number of figures for black pieces (pawns bishops knights rooks): " << endl;
    cin >> white_pawns >> white_bishops >> white_knights >> white_rooks;
    if (white_pawns < 0 || white_pawns > 8 || white_bishops < 0 || white_bishops > 2 ||
        white_knights < 0 || white_knights > 2 || white_rooks < 0 || white_rooks > 2) {
        cout << "Error: Invalid number of figures for black pieces." << endl;
        return 1;
    }

    cout << "Enter the number of figures for white pieces (pawns bishops knights rooks): " << endl;
    cin >> black_pawns >> black_bishops >> black_knights >> black_rooks;
    if (black_pawns < 0 || black_pawns > 8 || black_bishops < 0 || black_bishops > 2 ||
        black_knights < 0 || black_knights > 2 || black_rooks < 0 || black_rooks > 2) {
        cout << "Error: Invalid number of figures for white pieces." << endl;
        return 1;
    }

    Board desk(white_pawns, white_bishops, white_knights, white_rooks, black_pawns, black_bishops, black_knights,
               black_rooks);
    for (int i = 0; i < 20; i++) {
        desk = generate_moves(desk, 1, true, true);
    }
    show_board(desk);

    vector<Move> current_moves;
    vector<vector<Move>> all_moves;
    find_best_moves_recursive(desk, 3, current_moves, all_moves);
    sort(all_moves.begin(), all_moves.end(),
         [](const vector<Move> &a, const vector<Move> &b) {
             int score_a = accumulate(a.begin(), a.end(), 0,
                                      [](int sum, const Move &move) { return sum + move.score; });
             int score_b = accumulate(b.begin(), b.end(), 0,
                                      [](int sum, const Move &move) { return sum + move.score; });
             return score_a > score_b;
         });

    int num_combinations_to_show = min(3, static_cast<int>(all_moves.size()));
    for (int i = 0; i < num_combinations_to_show; ++i) {
        const auto &move_sequence = all_moves[i];

        cout << "Move sequence (Total Score: "
             << accumulate(move_sequence.begin(), move_sequence.end(), 0,
                           [](int sum, const Move &move) { return sum + move.score; })
             << "):" << endl;

        Board board_after_moves = desk;
        for (const auto &move: move_sequence) {
            cout << "Move: " << board_after_moves.BOARD[move.from_x][move.from_y]
                 << " from " << static_cast<char>('a' + move.from_y) << 8 - move.from_x
                 << " to " << static_cast<char>('a' + move.to_y) << 8 - move.to_x
                 << " (Score: " << move.score << ")" << endl;

            board_after_moves.BOARD[move.to_x][move.to_y] = board_after_moves.BOARD[move.from_x][move.from_y];
            board_after_moves.BOARD[move.from_x][move.from_y] = "--";
            show_board(board_after_moves);
        }
    }

    return 0;
}