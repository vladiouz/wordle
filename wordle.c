#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void menu(int *game_over, int yMax, int xMax, int *attempt_no, WINDOW * win, 
int *r){
    int yMenu = yMax/2 + 7;
    int xMenu = xMax/2 - 11;
    mvprintw(yMenu, xMenu, "Vrei sa mai joci?(y/n)");
    refresh();
    char raspuns = getch();
    if(raspuns == 'n'){
        *game_over = 1;
    }else{
        *r = rand() % 27;
        *attempt_no = 1;
        int yDelete = yMax/2 + 5;
        int xDelete = xMax/2 - 18;
        mvprintw(yDelete, xDelete, 
        "                                    ");
        yDelete += 2;
        mvprintw(yDelete, xDelete, 
        "                                    ");
        refresh();
        int i, j;
        for(i = 1; i < 7; i ++){
            for(j = 1; j < 6; j ++){
                mvwprintw(win, i, j, " ");
            }
        }
        wmove(win, 1, 1);
        wrefresh(win);
    }
}

void alegere_culori(int *culori, int *litere_folosite, char *input, 
char *cuvant, int *game_won){
    //verific mai intai pentru literele verzi
    int i;
    for(i = 0; i < 5; i ++){
        if(input[i] == cuvant[i]){
            culori[i] = 1;
            litere_folosite[i] = 1;
        }else{
            *game_won = 0;
        }
    }

    //verific si pentru galben
    int j;
    for(i = 0; i < 5; i ++){
        for(j = 0; j < 5; j ++){
            if(input[i] == cuvant[j] && !litere_folosite[j] 
            && culori[i] != 1){
                culori[i] = 2;
                litere_folosite[j] = 1;
                break;
            }
        }
    }
}

int main()
{
    initscr();
    noecho();
    cbreak();

    //initializez culorile de care am nevoie
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_GREEN);
    init_pair(2, COLOR_BLACK, COLOR_YELLOW);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);

    char *cuvinte[] = {"arici", "atent", "baiat", "ceata", "debut", "peste",
    "fixat", "hamac", "harta", "jalon", "jucam", "lacat", "magie", "nufar",
    "oaste", "perus", "rigle", "roman", "sanie", "scris", "sonda", "texte",
    "tipar", "titan", "zebra", "vapor", "vatra"};

    //aleg la intamplare cuvantul
    srand(time(NULL));
    int r = rand() % 27;

    //iau dimensiunile
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);

    //ASCII Art pentru titlu
    int xAsciiArt = xMax/2 - 17;
    mvprintw(3, xAsciiArt, " __      _____  ___ ___  _    ___ ");
    mvprintw(4, xAsciiArt, R"( \ \    / / _ \| _ \   \| |  | __|)");
    mvprintw(5, xAsciiArt, R"(  \ \/\/ / (_) |   / |) | |__| _| )");
    mvprintw(6, xAsciiArt, R"(   \_/\_/ \___/|_|_\___/|____|___|)");
    
    //tabla de joc
    int yGame = yMax/2 - 4;
    int xGame = xMax/2 - 3;
    WINDOW * win = newwin(8, 7, yGame, xGame);
    refresh();
    box(win, 0, 0);
    wmove(win, 1, 1);
    wrefresh(win);

    int game_over = 0, game_won = 0, game_lost = 0, attempt_no = 1;
    int word_length = 0;
    char input[6];
    input[5] = '\0';

    while(!game_over){
        char c = getch();
        int yDelete = yMax/2 + 5;
        int xDelete = xMax/2 - 18;
        mvprintw(yDelete, xDelete, 
        "                                    ");
        refresh();
        wmove(win, attempt_no, word_length + 1);
        wrefresh(win);
        
        //dupa ce intreg cuvantul a fost introdus
        if(c == 10 || c == KEY_ENTER){
            if(word_length == 5){
                int culori[5] = {3, 3, 3, 3, 3};
                int litere_folosite[5] = {0};
                game_won = 1;

                alegere_culori(culori, litere_folosite, input, cuvinte[r], 
                &game_won);

                //colorez
                int i;
                for(i = 0; i < 5; i ++){
                    wattron(win ,COLOR_PAIR(culori[i]));
                    mvwprintw(win, attempt_no, i + 1, "%c", input[i]);
                    wattroff(win, COLOR_PAIR(culori[i]));
                }

                //daca a fost ghicit cuvantul
                if(game_won){
                    int yWin = yMax/2 + 5;
                    int xWin = xMax/2 - 15;
                    mvprintw(yWin, xWin, "Felicitari, ai gasit cuvantul!");
                    refresh();
                }

                //daca nu a fost ghicit cuvantul
                if(attempt_no == 6 && !game_won){
                    game_lost = 1;
                    int yLose = yMax/2 + 5;
                    int xLose = xMax/2 - 17;
                    mvprintw(yLose, xLose, "Ai pierdut =( cuvantul era %s", 
                    cuvinte[r]);
                    refresh();
                }

                //trec la urmatoarea incercare
                word_length = 0;
                attempt_no += 1;
                wmove(win, attempt_no, 1);
                wrefresh(win);

                //daca jocul s-a terminat, afisez meniul
                if(game_won || game_lost){
                    menu(&game_over, yMax, xMax, &attempt_no, win, &r);
                    game_lost = 0;
                    game_won = 0;
                }
            }else{
                int yError = yMax/2 + 5;
                int xError = xMax/2 - 15;
                mvprintw(yError, xError, "Introdu un cuvant din 5 litere");
                refresh();
                wmove(win, attempt_no, word_length + 1);
                wrefresh(win);
            }

        //pentru a sterge un caracter din cuvant
        }else if(c == 127 || c == KEY_BACKSPACE){
            if(word_length > 0){
                mvwprintw(win, attempt_no, word_length, " ");
                wmove(win, attempt_no, word_length);
                word_length -= 1;
                wrefresh(win);
            }
        }else if(c == ':'){
            menu(&game_over, yMax, xMax, &attempt_no, win, &r);
        }else{

            //daca e introdusa o majuscula, o transform in minuscula
            if( c >= 65 && c <= 90){
                c += 32;
            }

            //verifica daca lucram cu o litera minuscula
            if(c >= 97 && c <= 122){
                if(word_length < 5){
                    input[word_length] = c;
                    word_length ++;
                    mvwprintw(win, attempt_no, word_length, "%c", c);
                    wrefresh(win);
                }else{
                    int yError = yMax/2 + 5;
                    int xError = xMax/2 - 13;
                    mvprintw(yError, xError, "Cuvantul are deja 5 litere");
                    refresh();
                    wmove(win, attempt_no, word_length + 1);
                    wrefresh(win);
                    
                }
            }
            
        }
        
    }

    endwin();
    return 0;
}

//gcc -o wordle wordle.c -lncurses
//./wordle