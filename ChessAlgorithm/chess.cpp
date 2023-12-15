#include <iostream>
#include <vector>
#include <random>
#include <map>

using namespace std;

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

vector<vector<string>> create_empty_board() {
    vector<vector<string>> board(8, vector<string>(8, "--"));
    return board;
}

void initialize_starting_positions(vector<vector<string>> &board) {
    board[0] = {"er", "ek", "eb", "eg", "--", "eb", "ek", "er"};
    board[1] = {"ep", "ep", "ep", "ep", "ep", "ep", "ep", "ep"};
    board[6] = {"mp", "mp", "mp", "mp", "mp", "mp", "mp", "mp"};
    board[7] = {"mr", "mk", "mb", "mg", "--", "mb", "mk", "mr"};
}

void show_board(const vector<vector<string>> &board) {
    cout << "a\t \tb\t \tc\t \td\t \te\t \tf\t \tg\t \th" << endl;
    for (int row_num = 0; row_num < 8; row_num++) {
        string row_str = "";
        for (const string &figure: board[row_num]) {
            if (figure != "--") {
                row_str += SYMBOL_DICT[figure] + "\t|\t";
            } else {
                row_str += "-\t|\t";
            }
        }
        cout << row_str << 7 - row_num + 1 << endl;
    }
    cout << "=================================" << endl;
}

vector<vector<string>> rotate_board(const vector<vector<string>> &board) {
    vector<vector<string>> reversed_board(8, vector<string>(8));
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            reversed_board[i][j] = board[7 - i][7 - j];
            if (reversed_board[i][j][0] == 'e') {
                reversed_board[i][j] = 'm' + reversed_board[i][j].substr(1);
            } else if (reversed_board[i][j][0] == 'm') {
                reversed_board[i][j] = 'e' + reversed_board[i][j].substr(1);
            }
        }
    }
    return reversed_board;
}
bool is_in_border(int coord) {
    return 0 <= coord && coord <= 7;
}

vector<vector<int>> make_moves(const vector<vector<string>> &board, int x, int y) {
    vector<vector<int>> possible_moves;
    auto add_move = [&](int move_x, int move_y) {
        if (is_in_border(x + move_x) && is_in_border(y + move_y)) {
            possible_moves.push_back({x + move_x, y + move_y});
        }
    };
    if (board[x][y] == "mp") {
        add_move(-1, 0);
        add_move(-1, 1);
        add_move(-1, -1);
    }
    if (board[x][y] == "mk") {
        for (int mk_x: {-2, -1, 1, 2}) {
            for (int mk_y: {-2, -1, 1, 2}) {
                if (abs(mk_x) != abs(mk_y) && is_in_border(x + mk_x) && is_in_border(y + mk_y) &&
                    (board[x + mk_x][y + mk_y][0] == 'e' || board[x + mk_x][y + mk_y][0] == '-')) {
                    possible_moves.push_back({x + mk_x, y + mk_y});
                }
            }
        }
    }
    auto moves_on_line = [&](int add_x, int add_y) {
        int r_x = x + add_x;
        int r_y = y + add_y;
        while (is_in_border(r_x) && is_in_border(r_y) && board[r_x][r_y] == "--") {
            possible_moves.push_back({r_x, r_y});
            r_x += add_x;
            r_y += add_y;
        }
        if (is_in_border(r_x) && is_in_border(r_y) && board[r_x][r_y][0] == 'e') {
            possible_moves.push_back({r_x, r_y});
        }
    };
    if (board[x][y] == "mr") {
        moves_on_line(1, 0);
        moves_on_line(-1, 0);
        moves_on_line(0, 1);
        moves_on_line(0, -1);
    }
    if (board[x][y] == "mb") {
        moves_on_line(1, 1);
        moves_on_line(1, -1);
        moves_on_line(-1, 1);
        moves_on_line(-1, -1);
    }
    if (board[x][y] == "mg") {
        for (int add_x = -1; add_x <= 1; add_x++) {
            for (int add_y = -1; add_y <= 1; add_y++) {
                if (abs(add_x) + abs(add_y) > 0 && is_in_border(x + add_x) && is_in_border(y + add_y) &&
                    (board[x + add_x][y + add_y][0] == '-' || board[x + add_x][y + add_y][0] == 'e')) {
                    possible_moves.push_back({x + add_x, y + add_y});
                }
            }
        }
    }
    return possible_moves;
}

vector<vector<string>> generate_moves(vector<vector<string>> board, int num_moves) {
    random_device rd;
    mt19937 gen(rd());
    for (int i = 0; i < num_moves; i++) {
        uniform_int_distribution<int> dist(0, 7);
        vector<int> cell = {dist(gen), dist(gen)};
        while (make_moves(board, cell[0], cell[1]).empty()) {
            cell = {dist(gen), dist(gen)};
        }
        vector<vector<int>> possible_moves = make_moves(board, cell[0], cell[1]);
        uniform_int_distribution<int> move_dist(0, possible_moves.size() - 1);
        vector<int> chosen_move = possible_moves[move_dist(gen)];
        board[chosen_move[0]][chosen_move[1]] = board[cell[0]][cell[1]];
        board[cell[0]][cell[1]] = "--";
    }
    return rotate_board(board);
}

int main() {
    vector<vector<string>> desk = create_empty_board();
    initialize_starting_positions(desk);
    for (int i = 0; i < 30; i++) {
        desk = generate_moves(desk, 1);
    }

    vector<vector<string>> rotated_desk = rotate_board(desk);
    show_board(rotated_desk);
    return 0;
}