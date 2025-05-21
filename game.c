#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lab8part2.h"
#include "liblab8part2.h"

#define unoccupied 'U'
#define black 'B'
#define white 'W'

void initalizeBoard(char board[][26], int n);
void configureBoard(char board[][26]);
bool moveAvailable(char board[][26], int n, char colour);
bool validMove(char board[][26], int n, int row, int col, char colour);
int flipTiles(char board[][26], int n, int row, int col, char colour,
              bool flip);
bool playHumanMove(char board[][26], int n, char humanColour);
bool terminalState(char board[][26]);
bool fullBoard(char board[][26], int n);
int countPieces(char board[][26], int n, char colour);
char oppositeColour(char colour);
char switchTurns(char turn);
int scoreOfBoard(char board[][26], char colour);
int minimax(char board[][26], char turn, int score[8][8], int depth);
int minimaxHelper(char testBoard[][26], char turn, char maxTurn,
                  bool maximizingPlayer, int depth);
void testMove(char copy[][26], char colour, int row, int col);
void copyBoard(const char board[][26], char copy[][26], int copySize);
int checkWinner(char board[][26], int n, bool humanValidMove,
                char computerColour);

int main(void) {
  char board[26][26];
  int n = 8;
  char computerColour = unoccupied;
  char turn = black;

  printf("Computer plays (B/W) : ");
  scanf(" %c", &computerColour);

  initalizeBoard(board, n);
  printBoard(board, n);

  while (checkWinner(board, n, true, computerColour) == 0) {
    bool skippedCurrentTurn = false;
    if (turn == computerColour) {
      bool computerMoveAvailable = moveAvailable(board, n, computerColour);
      if (computerMoveAvailable == true) {
        int row = 0;
        int col = 0;
        makeMove(board, n, computerColour, &row, &col);
        if (validMove(board, n, row, col, computerColour)) {
          flipTiles(board, n, row, col, computerColour, true);
        }
        printf("Testing Computer move (row, col): %c%c\n", row + 'a',
               col + 'a');
      } else {
        skippedCurrentTurn = true;
      }
    } else {
      int row = 0;
      int col = 0;
      bool aiMoveAvailable =
          moveAvailable(board, n, oppositeColour(computerColour));
      if (aiMoveAvailable == true) {
        findSmarterMove(board, n, oppositeColour(computerColour), &row,
        &col); if (validMove(board, n, row, col,
        oppositeColour(computerColour))) {
          flipTiles(board, 8, row, col, oppositeColour(computerColour),
          true); printf("Testing AI move (row, col): %c%c\n", row + 'a', col
          + 'a');
        } else {
          skippedCurrentTurn = true;
        }
      } else {
        skippedCurrentTurn = true;
      }
    }
    if (skippedCurrentTurn == false) {
      printBoard(board, n);
    }
    turn = switchTurns(turn);
  }

  int gameState = checkWinner(board, n, true, computerColour);

  if (gameState == 2) {
    printf("W player wins.\n");
  } else if (gameState == 1) {
    printf("B player wins.\n");
  } else if (gameState == -1) {
    printf("Draw!\n");
  }

  return EXIT_SUCCESS;
}

int checkWinner(char board[][26], int n, bool humanValidMove,
                char computerColour) {
  int winner = 0;

  bool noMovesAvailable = (moveAvailable(board, n, black) == false) &&
                          (moveAvailable(board, n, white) == false);

  if (humanValidMove == false) {
    if (computerColour == black) {
      winner = 1;
    } else {
      winner = 2;
    }
  } else if (fullBoard(board, n) == true || noMovesAvailable == true) {
    int numBlackPieces = countPieces(board, n, black);
    int numWhitePieces = countPieces(board, n, white);

    if (numBlackPieces > numWhitePieces) {
      winner = 1;
    } else if (numWhitePieces > numBlackPieces) {
      winner = 2;
    } else {
      winner = -1;
    }
  }

  return winner;
}

bool playHumanMove(char board[][26], int n, char humanColour) {
  bool moveIsValid = true;

  char inputedMove[3] = {0};
  printf("Enter move for colour %c (RowCol): ", humanColour);
  scanf("%s", inputedMove);

  if (validMove(board, n, inputedMove[0] - 'a', inputedMove[1] - 'a',
                humanColour) == true) {
    flipTiles(board, n, inputedMove[0] - 'a', inputedMove[1] - 'a', humanColour,
              true);
  } else {
    moveIsValid = false;
  }

  return moveIsValid;
}

void initalizeBoard(char board[][26], int n) {
  // Calculating center of board
  int center = (n / 2) - 1;
  for (int row = 0; row < n; ++row) {
    for (int col = 0; col < n; ++col) {
      // Placing white and black pieces per lab handout
      if ((row == center && col == center) ||
          (row == center + 1 && col == center + 1)) {
        board[row][col] = white;
      } else if ((row == center && col == center + 1) ||
                 (row == center + 1 && col == center)) {
        board[row][col] = black;
      } else {
        board[row][col] = unoccupied;
      }
    }
  }
}

void printBoard(char board[][26], int n) {
  for (int row = -1; row < n; ++row) {
    if (row == -1) {
      printf("  ");
    }
    for (int col = -1; col < n; ++col) {
      // Labelling rows when row = -1 and cols when col = -1
      if (row == -1 && col < n - 1) {
        printf("%c", 'a' + col + 1);
      } else if (col == -1) {
        printf("%c ", 'a' + row);
      } else if (row != -1 && col != -1) {
        printf("%c", board[row][col]);
      }
    }
    printf("\n");
  }
}

bool moveAvailable(char board[][26], int n, char colour) {
  bool moveIsAvailable = false;

  for (int row = 0; row < n; ++row) {
    for (int col = 0; col < n; ++col) {
      if (validMove(board, n, row, col, colour) == true) {
        moveIsAvailable = true;
      }
    }
  }

  return moveIsAvailable;
}

void configureBoard(char board[][26]) {
  char input[4] = "";

  printf("Enter board configuration:");
  while (strcmp(input, "!!!") != 0) {
    scanf("%s", input);
    int row = input[1] - 'a';
    int col = input[2] - 'a';
    if (input[0] == black) {
      board[row][col] = black;
    } else if (input[0] == white) {
      board[row][col] = white;
    }
  }
  printf("\n");
}

bool validMove(char board[][26], int n, int row, int col, char colour) {
  for (int deltaRow = -1; deltaRow < 2; ++deltaRow) {
    for (int deltaCol = -1; deltaCol < 2; ++deltaCol) {
      if (deltaRow != 0 || deltaCol != 0) {
        if (checkLegalInDirection(board, n, row, col, colour, deltaRow,
                                  deltaCol) == true) {
          return true;
        }
      }
    }
  }

  return false;
}

bool positionInBounds(int n, int row, int col) {
  bool inBounds = true;
  if (row < 0 || row >= n) {
    inBounds = false;
  } else if (col < 0 || col >= n) {
    inBounds = false;
  }

  return inBounds;
}

bool checkLegalInDirection(char board[][26], int n, int row, int col,
                           char colour, int deltaRow, int deltaCol) {
  bool legalMove = true;
  // Checking if inputed move is inside the gameboard
  if (positionInBounds(n, row + deltaRow, col + deltaCol) == false ||
      positionInBounds(n, row, col) == false) {
    return false;
  }

  // Checking if input is next to the opposite colour in an occupied cell
  if (board[row + deltaRow][col + deltaCol] != oppositeColour(colour) ||
      board[row][col] != unoccupied) {
    return false;
  }

  // Checking if line subsquently ends with piece of same colour
  int currentRow = row + deltaRow;
  int currentCol = col + deltaCol;
  bool sameColourInLine = false;
  while (positionInBounds(n, currentRow, currentCol) == true &&
         board[currentRow][currentCol] != unoccupied) {
    if (board[currentRow][currentCol] == colour &&
        board[currentRow - deltaRow][currentCol - deltaCol] ==
            oppositeColour(colour)) {
      sameColourInLine = true;
    }
    currentRow += deltaRow;
    currentCol += deltaCol;
  }

  if (sameColourInLine == false) {
    legalMove = false;
  }

  return legalMove;
}

char oppositeColour(char colour) {
  // Returning black if input is white | white if input is black
  char oppositeColourIs = ' ';
  if (colour == white) {
    oppositeColourIs = black;
  } else if (colour == black) {
    oppositeColourIs = white;
  }

  return oppositeColourIs;
}

int flipTiles(char board[][26], int n, int row, int col, char colour,
              bool flip) {
  int tilesFlipped = 0;

  if (validMove(board, n, row, col, colour)) {
    for (int deltaRow = -1; deltaRow < 2; ++deltaRow) {
      for (int deltaCol = -1; deltaCol < 2; ++deltaCol) {
        if (deltaRow != 0 || deltaCol != 0) {
          if (checkLegalInDirection(board, n, row, col, colour, deltaRow,
                                    deltaCol) == true) {
            int currentRow = row + deltaRow;
            int currentCol = col + deltaCol;
            do {
              if (flip == true) {
                board[currentRow][currentCol] = colour;
              }
              currentRow += deltaRow;
              currentCol += deltaCol;
              ++tilesFlipped;
            } while (board[currentRow][currentCol] == oppositeColour(colour));
          }
        }
      }
    }
    if (flip == true) {
      board[row][col] = colour;
    }
  }

  return tilesFlipped;
}

char switchTurns(char turn) {
  char updatedTurn;
  if (turn == black) {
    updatedTurn = white;
  } else {
    updatedTurn = black;
  }

  return updatedTurn;
}

bool terminalState(char board[][26]) {
  bool noMovesAvailable = (moveAvailable(board, 8, black) == false) &&
                          (moveAvailable(board, 8, white) == false);

  if (noMovesAvailable == true || fullBoard(board, 8) == true) {
    return true;
  }
  return false;
}

bool fullBoard(char board[][26], int n) {
  bool boardIsFull = true;

  for (int row = 0; row < n; ++row) {
    for (int col = 0; col < n; ++col) {
      if (board[row][col] == unoccupied) {
        boardIsFull = false;
      }
    }
  }

  return boardIsFull;
}

int countPieces(char board[][26], int n, char colour) {
  int numPieces = 0;

  for (int row = 0; row < n; ++row) {
    for (int col = 0; col < n; ++col) {
      if (board[row][col] == colour) {
        ++numPieces;
      }
    }
  }

  return numPieces;
}

int makeMove(const char board[][26], int n, char turn, int *row, int *col) {
  /* Only playing 8x8 grid*/
  char copy[26][26] = {0};
  copyBoard(board, copy, n);
  int score[8][8] = {0};

  int depth = 4;
  int maxScore = -1 * RAND_MAX;
  minimax(copy, turn, score, depth);

  char validBoard[26][26] = {0};
  copyBoard(board, validBoard, 8);
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      if (score[i][j] > maxScore && validMove(validBoard, 8, i, j, turn)) {
        maxScore = score[i][j];
        *row = i;
        *col = j;
      }
    }
  }

  return -1;
}

int scoreOfBoard(char board[][26], char colour) {
  int cellScores[8][8] = {{150, -20, 20, 10, 10, 20, -20, 150},
                          {-20, -50, -2, -2, -2, -2, -50, -20},
                          {20, -2, 5, 1, 1, 5, -2, 20},
                          {10, -2, 1, 0, 0, 1, -2, 10},
                          {10, -2, 1, 0, 0, 1, -2, 10},
                          {20, -2, 5, 1, 1, 5, -2, 20},
                          {-20, -50, -2, -2, -2, -2, -50, -20},
                          {150, -20, 20, 10, 10, 20, -20, 150}};
  int multipler = 0;
  for (int row = 0; row < 8; ++row) {
    for (int col = 0; col < 8; ++col) {
      if (board[row][col] == colour) {
        multipler += cellScores[row][col];
      } else if (board[row][col] == oppositeColour(colour)) {
        multipler -= cellScores[row][col];
      }
    }
  }

  int score = countPieces(board, 8, colour) + multipler +
              moveAvailable(board, 8, colour) -
              3 * moveAvailable(board, 8, oppositeColour(colour));

  return score;
}

// CS50x AI (Lecture 0), https://www.youtube.com/watch?v=WbzNRTTrX0g&t=6385s,
// March 22, 2025
int minimax(char board[][26], char turn, int score[8][8], int depth) {
  int value = -1 * RAND_MAX;
  for (int row = 0; row < 8; ++row) {
    for (int col = 0; col < 8; ++col) {
      int max = 0;
      if (validMove(board, 8, row, col, turn)) {
        char test[26][26] = {0};
        copyBoard(board, test, 8);
        int tilesFlipped = flipTiles(test, 8, row, col, turn, false);
        testMove(test, turn, row, col);
        max = minimaxHelper(test, oppositeColour(turn), turn, false, depth - 1);
      } else {
        max = 0;
      }
      score[row][col] = max;
      if (score[row][col] > value) {
        value = score[row][col];
      }
    }
  }

  return value;
}

int minimaxHelper(char testBoard[][26], char turn, char maxTurn,
                  bool maximizingPlayer, int depth) {
  if (depth == 0 || terminalState(testBoard)) {
    return scoreOfBoard(testBoard, maxTurn);
  }
  int value = 0;
  if (maximizingPlayer == true) {
    value = -1 * RAND_MAX;
  } else {
    value = RAND_MAX;
  }

  bool validMoveExists = false;
  for (int row = 0; row < 8; ++row) {
    for (int col = 0; col < 8; ++col) {
      if (validMove(testBoard, 8, row, col, turn)) {
        validMoveExists = true;
        char test[26][26] = {0};
        copyBoard(testBoard, test, 8);
        testMove(test, turn, row, col);
        if (maximizingPlayer == true) {
          int max = minimaxHelper(test, oppositeColour(turn), maxTurn, false,
                                  depth - 1);
          if (max > value) {
            value = max;
          }
        } else {
          int min = minimaxHelper(test, oppositeColour(turn), maxTurn, true,
                                  depth - 1);
          if (min < value) {
            value = min;
          }
        }
      }
    }
  }

  if (validMoveExists == true) {
    return value;
  } else {
    return minimaxHelper(testBoard, oppositeColour(turn), maxTurn,
                         !maximizingPlayer, depth - 1);
  }
}

void testMove(char copy[][26], char colour, int row, int col) {
  if (validMove(copy, 8, row, col, colour) == true) {
    flipTiles(copy, 8, row, col, colour, true);
  }
}

void copyBoard(const char board[][26], char copy[][26], int copySize) {
  for (int row = 0; row < copySize; ++row) {
    for (int col = 0; col < copySize; ++col) {
      copy[row][col] = board[row][col];
    }
  }
}