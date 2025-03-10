#include <stdio.h> 
#include <stdbool.h>
#include <gtk/gtk.h>

#define BOARD_SIZE 9
#define WINDOW_WIDTH 630
#define WINDOW_HEIGHT 730

int sudokuBoard[BOARD_SIZE][BOARD_SIZE];
static GtkWidget *tiles[BOARD_SIZE][BOARD_SIZE];

/* void printBoard() {

  for (int i=0; i<BOARD_SIZE; i++) {

      for (int j=0; j<BOARD_SIZE; j++) {
      
          printf("%d ", sudokuBoard[i][j]);
      }

      printf("\n");
  }
} */ 

bool findEmptyCell(int gameBoard[BOARD_SIZE][BOARD_SIZE], int *row, int *col) {
  
  for (int i=0; i<BOARD_SIZE; i++) {

      for (int j=0; j<BOARD_SIZE; j++) {

          if (gameBoard[i][j] == 0) {

              // printf("Empty cell found in row %d, column %d\n", i, j);
              *row = i;
              *col = j;
              return true;
          }
      }
  }

  // printf("%s", "Board is full. Sudoku solved!\n");
  return false;
}

bool isValidMove(int gameBoard[BOARD_SIZE][BOARD_SIZE], int candidate, int emptyCellRow, int emptyCellColumn) {

  // Check if candidate is in the row
  for (int i=0; i<BOARD_SIZE; i++) {
      
      if (gameBoard[emptyCellRow][i] == candidate) {

          // printf("Row contains candidate %d\n", candidate);
          return false;
      }
  }

  // Check if candidate in in the column
  for (int i=0; i<BOARD_SIZE; i++) {
      
      if (gameBoard[i][emptyCellColumn] == candidate) {

          // printf("Column contains candidate %d\n", candidate);
          return false;
      }
  }

  // Check if candidate is in the 3x3 subgrid
  int startRow = emptyCellRow - emptyCellRow % 3;
  int startCol = emptyCellColumn - emptyCellColumn % 3;

  for (int i=0; i<3; i++) {

      for (int j=0; j<3; j++) {

          if (gameBoard[startRow + i][startCol + j] == candidate) {

              // printf("Subgrid contains candidate %d\n", candidate);
              return false;
          }
      }
  }

  // printf("Does not contain candidate %d\n", candidate);
  return true;
}

bool solveSudoku(int board[BOARD_SIZE][BOARD_SIZE]) {

  int row;
  int col;

  // The board is solved
  if (!findEmptyCell(board, &row, &col)) return true;

  for (int candidate=1; candidate<10; candidate++) {

      if (isValidMove(board, candidate, row, col)) {

          board[row][col] = candidate;
          // printf("Inserted %d into the Sudoku board\n", candidate);

          // Successfully inserted an element into the board
          if (solveSudoku(board)) return true;

          // Backtrack if there was an invalid move
          board[row][col] = 0;
      }
  }

  // No valid candidate found
  return false;
}

void loadBoardDataToUI() {

  for (int i=0; i<BOARD_SIZE; i++) {

    for (int j=0; j<BOARD_SIZE; j++) {

      char tileData[2]; 
      snprintf(tileData, sizeof(tileData), "%d", sudokuBoard[i][j]);
      gtk_editable_set_text(GTK_EDITABLE(tiles[i][j]), tileData);      
    }
  }
}

static void onStartButtonClicked(GtkWidget *widget, gpointer user_data) {

  // Load the UI data into the sudokuBoard array
  for (int i=0; i<BOARD_SIZE; i++) {
    
    for (int j=0; j<BOARD_SIZE; j++) {

      if (GTK_IS_EDITABLE(tiles[i][j])) {
        
        const char *tileData = gtk_editable_get_text(GTK_EDITABLE(tiles[i][j]));
        sudokuBoard[i][j] = atoi(tileData);
      } else {

        sudokuBoard[i][j] = 0;
      }

    }
  }

  solveSudoku(sudokuBoard);
  loadBoardDataToUI();
}

static void activate(GtkApplication* app, gpointer user_data) {
    
    GtkWidget *gameWindow;
    GtkWidget *displayBox;
    GtkWidget *gameBoard;
    GtkWidget *startButton;

    gameWindow = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(gameWindow), "Solve");
    gtk_window_set_default_size(GTK_WINDOW(gameWindow), WINDOW_WIDTH, WINDOW_HEIGHT);
    gtk_window_set_resizable(GTK_WINDOW(gameWindow), FALSE);

    displayBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_window_set_child(GTK_WINDOW(gameWindow), displayBox);

    gameBoard = gtk_grid_new();
    gtk_box_append(GTK_BOX(displayBox), gameBoard);

    for (int i=0; i<BOARD_SIZE; i++) {

        for (int j=0; j<BOARD_SIZE; j++) {
          
          tiles[i][j] = gtk_entry_new();
    
          gtk_widget_set_size_request(tiles[i][j], 70, 70); 
          gtk_entry_set_max_length(GTK_ENTRY(tiles[i][j]), 1);
    
          gtk_entry_set_alignment(GTK_ENTRY(tiles[i][j]), 0.5);
          gtk_grid_attach(GTK_GRID(gameBoard), tiles[i][j], j, i, 1, 1);
        }
      }

    startButton = gtk_button_new_with_label("Solve the Sudoku");
    gtk_widget_set_size_request(startButton, WINDOW_WIDTH, 100);
    g_signal_connect(startButton, "clicked", G_CALLBACK(onStartButtonClicked), &tiles);

    gtk_box_append(GTK_BOX(displayBox), startButton);
    gtk_window_present(GTK_WINDOW(gameWindow));
}

int main(int argc, char **argv) {

    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}