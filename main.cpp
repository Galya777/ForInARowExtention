//
// Created by galya777 on 15.08.24.
//

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <cstring>

const int MAX_WID=1000;
const int MAX_HEI=1000;

int xWins = 0;
int oWins = 0;
int draws = 0;
std::mutex mtx;
bool winner(char **board, int wid, int hei, int toWin, char player) {
// Проверка по редове
    for (int r = 0; r < hei; ++r) {
        for (int c = 0; c <= wid - toWin; ++c) {
            bool win = true;
            for (int i = 0; i < toWin; ++i) {
                if (board[r][c + i] != player) {
                    win = false;
                    break;
                }
            }
            if (win) return true;
        }
    }
    // Проверка по колони
    for (int c = 0; c < wid; ++c) {
        for (int r = 0; r <= hei- toWin; ++r) {
            bool win = true;
            for (int i = 0; i < toWin; ++i) {
                if (board[r + i][c] != player) {
                    win = false;
                    break;
                }
            }
            if (win) return true;
        }
    }
    // Проверка по диагоналите (дясно-надолу)
    for (int r = 0; r <= hei- toWin; ++r) {
        for (int c = 0; c <= wid- toWin; ++c) {
            bool win = true;
            for (int i = 0; i < toWin; ++i) {
                if (board[r + i][c + i] != player) {
                    win = false;
                    break;
                }
            }
            if (win) return true;
        }
    }
    // Проверка по диагоналите (ляво-надолу)
    for (int r = 0; r <= hei - toWin; ++r) {
        for (int c = toWin- 1; c < wid; ++c) {
            bool win = true;
            for (int i = 0; i < toWin; ++i) {
                if (board[r + i][c - i] != player) {
                    win = false;
                    break;
                }
            }
            if (win) return true;
        }
    }
    return false;
}

void play(char **board, int width, int high, int connectCountToWin, char gamer, int depth)
{
    if (depth == 0) return;


    if(winner(board, width, high, connectCountToWin, gamer)){
        mtx.lock();
        if (gamer == 'X') {
            ++oWins;
        } else {
            ++xWins;
        }
        mtx.unlock();
        return;
    }


    for (int col = 0; col < width; ++col) {
        int rowToPlace = -1;
        for (int row = high- 1; row >= 0; --row) {
            if (board[row][col] == '_') {
                rowToPlace = row;
                break;
            }
        }
        if (rowToPlace == -1) continue;  // Колоната е запълнена

        board[rowToPlace][col] = gamer;
        play(board, width, high, connectCountToWin, gamer == 'X' ? 'O' : 'X', depth - 1);
        board[rowToPlace][col] = '_';  // Връщане на предишното състояние
    }

    // Ако всички колони са запълнени, то е равенство
    mtx.lock();
    ++draws;
    mtx.unlock();
}

void bruteFOURce(const char * board, const int width, const int height, const int connectCountToWin, const char nextToMove, const int calcDepth){

    //convert board into vector. It may slow down the process but will simplify the solution
    char** boardArray = new char*[height];
    for (int i = 0; i < height; ++i) {
        boardArray[i] = new char[width];
        for (int j = 0; j < width; ++j) {
            boardArray[i][j] = board[i * width + j];
        }
    }

    // Създаване на масив от нишки
    std::thread* threads = new std::thread[width];

    for (int col = 0; col < width; ++col) {
        threads[col] = std::thread([=] {
            // Създаване на копие на дъската
            char** boardCopy = new char*[height];
            for (int i = 0; i < height; ++i) {
                boardCopy[i] = new char[width];
                std::memcpy(boardCopy[i], boardArray[i], width * sizeof(char));
            }

            // Намиране на реда, в който може да се постави пул в тази колона
            int rowToPlace = -1;
            for (int row = height - 1; row >= 0; --row) {
                if (boardCopy[row][col] == '_') {
                    rowToPlace = row;
                    break;
                }
            }

            // Ако има валидна позиция, поставяме пул и симулираме играта
            if (rowToPlace != -1) {
                boardCopy[rowToPlace][col] = nextToMove;
                play(boardCopy, width, height, connectCountToWin, nextToMove == 'X' ? 'O' : 'X', calcDepth - 1);
            }

            // Освобождаване на паметта за копието на дъската
            for (int i = 0; i < height; ++i) {
                delete[] boardCopy[i];
            }
            delete[] boardCopy;
        });
    }

    // Изчакване на всички нишки да приключат
    for (int col = 0; col < width; ++col) {
        threads[col].join();
    }

    // Освобождаване на паметта за основната дъска и нишките
    for (int i = 0; i < height; ++i) {
        delete[] boardArray[i];
    }
    delete[] boardArray;
    delete[] threads;

    std::cout << "X won " << xWins << ", Y won " << oWins << ", " << draws << " draws." << std::endl;
}
int main(){


    bruteFOURce("__O__X", 3, 2, 2, 'X', 10000);
    bruteFOURce("__O__X", 3, 2, 2, 'X', 10000000);
    bruteFOURce("__O__X", 30, 20, 10, 'X', 10000000);
    return 0;
}