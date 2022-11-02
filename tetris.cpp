#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <queue>
#include <random>
#include <vector>
#ifdef _WIN32
#define cl "cls"
#define wait system("pause");
#include <WinUser.h>
#include <windows.h>
#define CHECK_KEY(V) (GetKeyState(V) < 0)
#define sleep(n) Sleep(n)
#define VK_Z 0x5A
#define VK_C 0x43
#else
#define wait                                  \
    printf("Press Enter key to continue..."); \
    fgetc(stdin);                             \
    fflush(stdin);
#include <termios.h>
#include <unistd.h>
int32_t take_keypress() {
    struct termios attr;
    struct termios tmp_attr;

    tcgetattr(fileno(stdin), &attr);
    tmp_attr = attr;
    tmp_attr.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(fileno(stdin), TCSANOW, &tmp_attr);
    setbuf(stdin, NULL);

    struct timeval tv = {.tv_sec = 0, .tv_usec = 1000};

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(fileno(stdin), &fds);

    int32_t key_code = getchar();

    int offset = 1;
    while (select(fileno(stdin) + 1, &fds, NULL, NULL, &tv) == 1) {
        if (key_code < 1 << 16) {
            key_code = (key_code << 8 * offset++) | getchar();
        } else {
            break;
        }
    }

    tcsetattr(fileno(stdin), TCSANOW, &attr);

    return key_code;
}
int ANSKEY;
int KeyTest() {
    struct timeval tv = {.tv_sec = 0, .tv_usec = 1000000 / 120};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(fileno(stdin), &fds);
    int sel = select(fileno(stdin) + 1, &fds, NULL, NULL, &tv);
    if (sel == 1)
        return take_keypress();
    else
        return 0;
}
void updateKey() {
    ANSKEY = KeyTest();
    if (ANSKEY <= 'Z' || ANSKEY >= 'A') ANSKEY += 32;
}
#define VK_ESCAPE '\e'
#define VK_UP 458948673
#define VK_DOWN 458948674
#define VK_RIGHT 458948675
#define VK_LEFT 458948676
#define VK_Z 'z'
#define VK_C 'c'
#define VK_SPACE 32
#define CHECK_KEY(V) (ANSKEY == V)
#define cl "clear"
#endif

#define SQUARE_PRINT "\e[43;33m  \e[m"
#define LINE_PRINT "\e[46;36m  \e[m"
#define LSHAPE_PRINT "\e[48;5;214m  \e[m"
#define JSHAPE_PRINT "\e[44;34m  \e[m"
#define SSHAPE_PRINT "\e[42;32m  \e[m"
#define ZSHAPE_PRINT "\e[41;31m  \e[m"
#define TSHAPE_PRINT "\e[45;35m  \e[m"
#define SHADOW_PRINT "\e[48;5;244m  \e[m"

using namespace std;
const int32_t levelSpeed[30] = {48, 43, 38, 33, 28, 23, 18, 13, 8, 6,
                                5,  5,  5,  4,  4,  4,  3,  3,  3, 2,
                                2,  2,  2,  2,  2,  2,  2,  2,  2, 1};
const int32_t LEVELUP[20] = {10,  20,  30,  40,  50,  60,  70,  80,  90,  100,
                             100, 100, 100, 100, 100, 100, 110, 120, 130, 10};
const bool PIECE[7][4][4][4] = {
    {{{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
     {{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
     {{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
     {{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}},
    {{{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
     {{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}},
     {{0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}},
     {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}}},
    {{{0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
     {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
     {{0, 0, 0, 0}, {1, 1, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}},
     {{1, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}},
    {{{1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
     {{0, 1, 1, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
     {{0, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}},
     {{0, 1, 0, 0}, {0, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}}},
    {{{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
     {{0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}},
     {{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},
     {{1, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}},
    {{{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
     {{0, 0, 1, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
     {{0, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
     {{0, 1, 0, 0}, {1, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}}},
    {{{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
     {{0, 1, 0, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
     {{0, 0, 0, 0}, {1, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
     {{0, 1, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}}};
const int32_t KICKWALL[4][5][2] = {
    {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}},
    {{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}},
    {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},
    {{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}}};
const int32_t IKICKWALL[4][5][2] = {
    {{0, 0}, {-2, 0}, {1, 0}, {-2, 1}, {1, -2}},
    {{0, 0}, {-1, 0}, {2, 0}, {-1, -2}, {2, 1}},
    {{0, 0}, {2, 0}, {-1, 0}, {2, -1}, {-1, 2}},
    {{0, 0}, {1, 0}, {-2, 0}, {1, 2}, {-2, -1}}};
const int32_t SCORE_LINE[5] = {0, 40, 100, 300, 1200};
const double FPS = 120;
int32_t nowPieceShape = 0;
int32_t nowLocate[2];
int32_t frame[25][10];
int32_t SOFT_DROP = 1;
/*0:Square
  1:Line
  2:Lshape
  3:J shape
  4:S shape
  5:Z shape
  6:T shape*/

int64_t score = 0;
int32_t level = 0;
int32_t totalLine = 0;
int32_t levelLine = 0;
int32_t pieceRotate = 0;
int32_t frameCounter = 0;
int32_t moveCounter = 0;
int32_t holdPiece = -1;
int32_t haveHold = 0;
queue<int32_t> nextPiece;
int32_t BacktoBack = 0;
int32_t attack = 0;
int32_t combo = 0;
int32_t lastRotateType = 0;
int32_t update() {
    bool check;
    int32_t cnt = 0;
    for (int32_t i = 0; i < 25; ++i) {
        check = 1;
        for (int32_t j = 0; j < 10; ++j) {
            if (!frame[i][j]) {
                check = 0;
                break;
            }
        }
        if (check) {
            for (int32_t k = i; k > 0; --k) {
                for (int32_t j = 0; j < 10; ++j) {
                    frame[k][j] = frame[k - 1][j];
                }
            }
            for (int32_t j = 0; j < 10; ++j) frame[0][j] = 0;
            cnt++;
        }
    }
    return cnt;
}
bool testMove(int32_t dx, int32_t dy) {
    for (int32_t i = 0; i < 4; ++i) {
        for (int32_t j = 0; j < 4; ++j) {
            if (PIECE[nowPieceShape][pieceRotate][i][j]) {
                if (nowLocate[0] + i + dy > 24 || nowLocate[0] + i + dy < 0 ||
                    nowLocate[1] + j + dx > 9 || nowLocate[1] + j + dx < 0)
                    return 0;
                if (frame[nowLocate[0] + i + dy][nowLocate[1] + j + dx]) {
                    return 0;
                }
            }
        }
    }
    return 1;
}
int32_t view() {
    for (int32_t i = nowLocate[0]; i < 25; ++i) {
        if (!testMove(0, i - nowLocate[0])) {
            return i - 1 - nowLocate[0];
        }
    }
    return -1;
}
void resetNowPiece() {
    nowLocate[1] = 3;
    nowLocate[0] = 2;
    pieceRotate = 0;
    lastRotateType = 0;
}
bool chooseNewPiece() {
    nowPieceShape = nextPiece.front();
    nextPiece.pop();
    if (nextPiece.empty()) {
        vector<int32_t> pieceShuffle;
        for (int i = 0; i < 7; ++i) pieceShuffle.push_back(i);
        for (int i = 0; i < 7; ++i) pieceShuffle.push_back(i);
        shuffle(pieceShuffle.begin(), pieceShuffle.end(),
                default_random_engine(
                    chrono::system_clock::now().time_since_epoch().count()));
        for (int i = 0; i < 14; ++i) nextPiece.push(pieceShuffle[i]);
    }
    resetNowPiece();
    return testMove(0, 0);
}
void setUpGame() {
    SOFT_DROP = 1;
    totalLine = 0;
    levelLine = 0;
    frameCounter = 0;
    moveCounter = 0;
    score = 0;
    holdPiece = -1;
    haveHold = 0;
    level = 0;
    BacktoBack = 0;
    attack = 0;
    combo = 0;
    while (!nextPiece.empty()) nextPiece.pop();
    vector<int32_t> pieceShuffle;
    for (int i = 0; i < 7; ++i) pieceShuffle.push_back(i);
    shuffle(pieceShuffle.begin(), pieceShuffle.end(),
            default_random_engine(
                chrono::system_clock::now().time_since_epoch().count()));
    for (int i = 0; i < 7; ++i) nextPiece.push(pieceShuffle[i]);
    for (int32_t i = 0; i < 20; ++i) {
        for (int32_t j = 0; j < 10; ++j) {
            frame[i][j] = 0;
        }
    }
    chooseNewPiece();
}
char* toString(int64_t n) {
    int64_t num = 1;
    int32_t tmpc = 0;
    while (n / num) {
        num *= 10;
        tmpc++;
    }
    char* ret;
    if (n == 0) {
        ret = (char*)malloc(sizeof(char) * (2));

        ret[0] = '0';
        ret[1] = '\0';
        return ret;
    }
    ret = (char*)malloc(sizeof(char) * (tmpc + 10));
    ret[tmpc] = '\0';
    tmpc--;
    while (tmpc >= 0) {
        ret[tmpc] = '0' + (n % 10);
        n /= 10;
        tmpc--;
    }
    return ret;
}
char output[1000000];
int32_t cnt = 0;
int32_t mini = 0;
void showFrame() {
    system(cl);
    output[0] = '\0';
    strcat(output, ".--------------------. next:\n");
    for (int32_t i = 5; i < 25; ++i) {
        strcat(output, "|");
        for (int32_t j = 0; j < 10; ++j) {
            bool check = 0;
            bool shadow = 1;
            for (int32_t x = 0; x < 4; ++x) {
                for (int32_t y = 0; y < 4; ++y) {
                    if (PIECE[nowPieceShape][pieceRotate][x][y]) {
                        if (nowLocate[0] + x == i && nowLocate[1] + y == j) {
                            shadow = 0;
                            check = 1;
                            switch (nowPieceShape) {
                                case 0:
                                    strcat(output, SQUARE_PRINT);
                                    break;
                                case 1:
                                    strcat(output, LINE_PRINT);
                                    break;
                                case 2:
                                    strcat(output, LSHAPE_PRINT);
                                    break;
                                case 3:
                                    strcat(output, JSHAPE_PRINT);
                                    break;
                                case 4:
                                    strcat(output, SSHAPE_PRINT);
                                    break;
                                case 5:
                                    strcat(output, ZSHAPE_PRINT);
                                    break;
                                case 6:
                                    strcat(output, TSHAPE_PRINT);
                                    break;
                            }
                        }
                    }
                }
            }
            if (shadow) {
                for (int32_t sx = 0; sx < 4; ++sx) {
                    for (int32_t sy = 0; sy < 4; ++sy) {
                        if (PIECE[nowPieceShape][pieceRotate][sx][sy]) {
                            if (nowLocate[0] + sx + view() == i &&
                                nowLocate[1] + sy == j) {
                                check = 1;
                                strcat(output, SHADOW_PRINT);
                            }
                        }
                    }
                }
            }
            if (!check) {
                switch (frame[i][j] - 1) {
                    case 0:
                        strcat(output, SQUARE_PRINT);
                        break;
                    case 1:
                        strcat(output, LINE_PRINT);
                        break;
                    case 2:
                        strcat(output, LSHAPE_PRINT);
                        break;
                    case 3:
                        strcat(output, JSHAPE_PRINT);
                        break;
                    case 4:
                        strcat(output, SSHAPE_PRINT);
                        break;
                    case 5:
                        strcat(output, ZSHAPE_PRINT);
                        break;
                    case 6:
                        strcat(output, TSHAPE_PRINT);
                        break;
                    default:
                        strcat(output, "  ");
                        break;
                }
            }
        }
        strcat(output, "|");
        if (i < 11) {
            if (i == 5 || i == 10)
                strcat(output, " .--------. ");
            else {
                strcat(output, " |");
                for (int32_t j = 0; j < 4; ++j) {
                    if (nextPiece.front() == -1)
                        strcat(output, "  ");
                    else if (PIECE[nextPiece.front()][0][i - 6][j]) {
                        switch (nextPiece.front()) {
                            case 0:
                                strcat(output, SQUARE_PRINT);
                                break;
                            case 1:
                                strcat(output, LINE_PRINT);
                                break;
                            case 2:
                                strcat(output, LSHAPE_PRINT);
                                break;
                            case 3:
                                strcat(output, JSHAPE_PRINT);
                                break;
                            case 4:
                                strcat(output, SSHAPE_PRINT);
                                break;
                            case 5:
                                strcat(output, ZSHAPE_PRINT);
                                break;
                            case 6:
                                strcat(output, TSHAPE_PRINT);
                                break;
                        }
                    } else {
                        strcat(output, "  ");
                    }
                }
                strcat(output, "| ");
            }
        } else if (i == 11) {
            strcat(output, " hold:");
        } else if (i > 11 && i < 18) {
            if (i == 12 || i == 17)
                strcat(output, " .--------. ");
            else {
                strcat(output, " |");
                for (int32_t j = 0; j < 4; ++j) {
                    if (holdPiece == -1)
                        strcat(output, "  ");
                    else if (PIECE[holdPiece][0][i - 13][j]) {
                        switch (holdPiece) {
                            case 0:
                                strcat(output, SQUARE_PRINT);
                                break;
                            case 1:
                                strcat(output, LINE_PRINT);
                                break;
                            case 2:
                                strcat(output, LSHAPE_PRINT);
                                break;
                            case 3:
                                strcat(output, JSHAPE_PRINT);
                                break;
                            case 4:
                                strcat(output, SSHAPE_PRINT);
                                break;
                            case 5:
                                strcat(output, ZSHAPE_PRINT);
                                break;
                            case 6:
                                strcat(output, TSHAPE_PRINT);
                                break;
                        }
                    } else {
                        strcat(output, "  ");
                    }
                }
                strcat(output, "|");
            }
        } else if (i == 19) {
            strcat(output, "level:");
            strcat(output, toString(level));

        } else if (i == 20) {
            strcat(output, "score:");
            strcat(output, toString(score));

        } else if (i == 21) {
            strcat(output, "line:");
            strcat(output, toString(totalLine));
        } else if (i == 22) {
            strcat(output, "next level:");
            strcat(output,
                   toString((LEVELUP[level >= 20 ? 19 : level] - levelLine)));
        } else if (i == 23) {
            strcat(output, "attack:");
            strcat(output, toString(attack));
        } else if (i == 24) {
            strcat(output, "combo:");

            strcat(output, toString(combo));
            strcat(output, BacktoBack ? "    B2B" : "\0");
        }
        strcat(output, "\n");
    }
    strcat(output, ".--------------------.\n\0");
    printf("%s", output);
}
void hold() {
    if (holdPiece == -1) {
        holdPiece = nowPieceShape;
        chooseNewPiece();
    } else {
        int32_t s = nowPieceShape;
        nowPieceShape = holdPiece;
        holdPiece = s;
        resetNowPiece();
    }
}

int32_t isTspin() {
    cnt = 0;
    mini = 0;
    if (nowPieceShape != 6) return 0;
    for (int32_t i = 0; i <= 2; i += 2) {
        for (int32_t j = 0; j <= 2; j += 2) {
            if (nowLocate[0] + i > 24 || nowLocate[0] + i < 0 ||
                nowLocate[1] + j > 9 || nowLocate[1] + j < 0) {
                ++cnt;
            } else if (frame[nowLocate[0] + i][nowLocate[1] + j]) {
                ++cnt;
            }
        }
    }
    int32_t dx1, dx2, dy1, dy2, dx, dy;
    switch (pieceRotate) {
        case 0:
            dx = 1;
            dy = 0;
            dx1 = -1;
            dy1 = 0;
            dx2 = 1;
            dy2 = 0;
            break;
        case 1:
            dx = 2;
            dy = 1;
            dx1 = 0;
            dy1 = -1;
            dx2 = 0;
            dy2 = 1;
            break;
        case 2:
            dx = 1;
            dy = 2;
            dx1 = -1;
            dy1 = 0;
            dx2 = 1;
            dy2 = 0;
            break;
        case 3:
            dx = 0;
            dy = 1;
            dx1 = 0;
            dy1 = -1;
            dx2 = 0;
            dy2 = 1;
            break;
    }
    if (nowLocate[0] + dy + (dy1) > 24 || nowLocate[0] + dy + (dy1) < 0)
        ++mini;
    else if (nowLocate[1] + dx + dx1 > 9 || nowLocate[1] + dx + dx1 < 0)
        ++mini;
    else if (frame[nowLocate[0] + dy + (dy1)][nowLocate[1] + dx + dx1])
        ++mini;
    if (nowLocate[0] + dy + (dy2) > 24 || nowLocate[0] + dy + (dy2) < 0)
        ++mini;
    else if (nowLocate[1] + dx + dx2 > 9 || nowLocate[1] + dx + dx2 < 0)
        ++mini;
    else if (frame[nowLocate[0] + dy + (dy2)][nowLocate[1] + dx + dx2])
        ++mini;
    if (mini < 2 && cnt >= 3 && lastRotateType != 5 && lastRotateType)
        return -1;
    return (cnt >= 3 && lastRotateType);
}
bool frameClear() {
    for (int i = 24; i >= 0; --i) {
        for (int j = 0; j < 10; ++j) {
            if (frame[i][j]) return 0;
        }
    }
    return 1;
}
bool move(int32_t dx, int32_t dy) {
    int32_t lineCnt = 0;
    if (testMove(dx, 0)) {
        if (dx) lastRotateType = 0;
        nowLocate[1] += dx;
    }
    if (testMove(0, dy)) {
        if (dy) lastRotateType = 0;
        nowLocate[0] += dy;
    } else {
        for (int32_t i = 0; i < 4; ++i) {
            for (int32_t j = 0; j < 4; ++j) {
                if (PIECE[nowPieceShape][pieceRotate][i][j]) {
                    frame[nowLocate[0] + i][nowLocate[1] + j] =
                        nowPieceShape + 1;
                }
            }
        }
        int32_t tmp = isTspin();
        lineCnt = update();
        if (lineCnt && frameClear()) attack += 20;
        if (tmp == 1 && nowPieceShape == 6) {
            attack += (BacktoBack + 2) * lineCnt;
            if (lineCnt) BacktoBack = 1;
        } else if (tmp == -1 && nowPieceShape == 6) {
            attack += (BacktoBack + 1) * lineCnt;
            if (lineCnt) BacktoBack = 1;
        } else {
            if (lineCnt == 4) {
                attack += (BacktoBack * 2) + 4;
                BacktoBack = 1;
            } else if (lineCnt) {
                attack += lineCnt - 1;
                BacktoBack = 0;
            }
        }
        if (lineCnt)
            if (combo > 7)
                attack += 4;
            else if (combo > 0) {
                attack += ((combo - 1) / 2) + 1;
            }
        if (!lineCnt) {
            combo = 0;
        } else {
            combo++;
        }
        score += SCORE_LINE[lineCnt] * (level + 1);
        totalLine += lineCnt;
        levelLine += lineCnt;
        if (levelLine >= LEVELUP[level >= 20 ? 19 : level]) {
            level++;
            levelLine = 0;
        }
        if (chooseNewPiece()) {
            haveHold = 0;
            return 0;
        } else {
            return 1;
        }
    }

    return 0;
}
void hardDrop() {
    move(0, view());
    for (int32_t i = 0; i < 4; ++i) {
        for (int32_t j = 0; j < 4; ++j) {
            if (PIECE[nowPieceShape][pieceRotate][i][j]) {
                frame[nowLocate[0] + i][nowLocate[1] + j] = nowPieceShape + 1;
            }
        }
    }
    int32_t tmp = isTspin();
    int32_t lineCnt = update();
    if (lineCnt && frameClear()) attack += 20;
    if (tmp == 1 && nowPieceShape == 6) {
        attack += (BacktoBack + 2) * lineCnt;
        BacktoBack = 1;
    } else if (tmp == -1 && nowPieceShape == 6) {
        attack += (BacktoBack + 1) * lineCnt;
        BacktoBack = 1;
    } else {
        if (lineCnt == 4) {
            attack += (BacktoBack * 2) + 4;
            BacktoBack = 1;
        } else if (lineCnt) {
            attack += lineCnt - 1;
            BacktoBack = 0;
        }
    }
    if (combo > 7)
        attack += 4;
    else if (combo > 0) {
        attack += ((combo - 1) / 2) + 1;
    }
    if (!lineCnt) {
        combo = 0;
    } else {
        combo++;
    }
    score += SCORE_LINE[lineCnt] * (level + 1);
    totalLine += lineCnt;
    levelLine += lineCnt;
    if (levelLine >= LEVELUP[level >= 20 ? 19 : level]) {
        level++;
        levelLine = 0;
    }
    if (chooseNewPiece()) {
        haveHold = 0;
    }
}
void nowPieceRotate(int32_t r) {
    int32_t before = pieceRotate;
    pieceRotate = (pieceRotate + r + 4) % 4;
    int32_t dx, dy;
    for (int32_t i = 0; i < 5; ++i) {
        dx = (nowPieceShape == 1)
                 ? IKICKWALL[(r > 0) ? before : pieceRotate][i][0]
                 : KICKWALL[(r > 0) ? before : pieceRotate][i][0];
        dy = (nowPieceShape == 1)
                 ? IKICKWALL[(r > 0) ? before : pieceRotate][i][1]
                 : KICKWALL[(r > 0) ? before : pieceRotate][i][1];
        if (testMove(dx * r, dy * r)) {
            nowLocate[1] += dx * r;
            nowLocate[0] += dy * r;
            lastRotateType = i + 1;
            return;
        }
    }
    pieceRotate = before;
    return;
}
void startGame() {
    setUpGame();
#ifndef _WIN32
    struct termios attr;
    struct termios tmp_attr;

    tcgetattr(fileno(stdin), &attr);
    tmp_attr = attr;
    tmp_attr.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(fileno(stdin), TCSANOW, &tmp_attr);
#endif
    int32_t rotate = 0;
    int32_t harddropcheck = 0;
    while (1) {
        //↑:clockwise ↓:soft drop ←→:move   space:hard drop z:counterclockwise
        // c:hold
        int32_t deltax = 0;
        int32_t tmpRotate = CHECK_KEY(VK_UP) - CHECK_KEY(VK_Z);
        int32_t tmpharddrop = CHECK_KEY(VK_SPACE);
#ifndef _WIN32

        updateKey();
#endif
        moveCounter += (int32_t)(FPS / 30);
        if (harddropcheck && !tmpharddrop) {
            hardDrop();
            showFrame();
            harddropcheck = tmpharddrop;
        } else if (tmpharddrop) {
            harddropcheck = tmpharddrop;
        } else {
            if (moveCounter / ((int32_t)(FPS / 30) * 2)) {
                deltax = CHECK_KEY(VK_RIGHT) - CHECK_KEY(VK_LEFT);
            }

            if (CHECK_KEY(VK_C) && !haveHold) {
                haveHold = 1;
                hold();
                showFrame();
            }
            SOFT_DROP = CHECK_KEY(VK_DOWN) ? 10 : 1;
            if (rotate && !tmpRotate) {
                nowPieceRotate(rotate);
                showFrame();
                rotate = tmpRotate;
            } else if (tmpRotate) {
                rotate = tmpRotate;
            }
        }
        if (CHECK_KEY(VK_ESCAPE)) {
            break;
        }
        frameCounter += (int32_t)(FPS / 30) * (SOFT_DROP);

        if (move(deltax,
                 frameCounter / ((int32_t)(FPS / 30) *
                                 levelSpeed[(level > 29 ? 29 : level)]))) {
            printf("GAME OVER!\n");
            wait;
            break;
        }

        if (deltax || frameCounter / ((int32_t)(FPS / 30) *
                                      levelSpeed[(level > 29 ? 29 : level)])) {
            showFrame();
        }

        frameCounter %= (int32_t)(FPS / 30) * levelSpeed[level];
        moveCounter %= (int32_t)(FPS / 30) * 2;

        sleep(1000 / FPS);
    }
#ifndef _WIN32
    tcsetattr(fileno(stdin), TCSANOW, &attr);
#endif
}
void menu() {
    system(cl);
    printf(
        "1) single player\n2) two player(to be continue\n3) how to play?\n4) "
        "exit\n");
}
void howToPlay() {
    system(cl);
    printf("Rule:\n");
    printf(
        "you can only move the pieces in specific ways; \nyour game is over if "
        "your pieces reach the top of the screen; \nand you can only remove "
        "pieces from the screen by filling all the blank space in a line.\n");
    printf("Control:\n");
    printf("1.use left and right arrow key move the block\n");
    printf("2.use Z and up arrow key rotate the block\n");
    printf(
        "3.use C to hold the block(you can use only one time before the block "
        "touch the ground)\n");
    printf("4.use down arrow key to speed up falling\n");
    printf("5.use space key to let block drop on ground\n");
}
int main() {
    srand(time(NULL));
    int32_t step;
    while (1) {
        menu();
        scanf("%d", &step);
        switch (step) {
            case 1:
                startGame();
                break;
            case 2:
                printf("comming soon\n");
                wait;
                break;
            case 3:
                howToPlay();
                wait;
            case 4:
                return 0;
                break;
            default:
                printf("not this option\n");
        }
    }
    return 0;
}