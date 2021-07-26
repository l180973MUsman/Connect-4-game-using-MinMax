#ifndef L180973_H
#define L180973_H
#include "Player.h"
#include "gamestate.h"
#include "connect4move.h"
#include "connect4state.h"
#include <vector>
#include <array>
#include <iostream>
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))
using namespace std;

unsigned int NUM_COL = 7; // how wide is the board
unsigned int NUM_ROW = 6; // how tall
unsigned int PLAYER = 1; // player number
unsigned int COMPUTER = 2; // AI number
unsigned int MAX_DEPTH = 5; // the default "difficulty" of the computer controlled AI

class l180973 : public Player
{
    public:
        l180973(char Color = 'G'):Player("Auto:Random", Color){}
        Connect4Move* SuggestMove(GameState* State)
        {
            Connect4Move * Move = new Connect4Move;  //making new Connect4Move obj pointer

            std::vector<GameMove*> Moves = State->GetPossibleMoves(); //Getting possible moves list
            int Total = Moves.size();

            if(Total == 0){
                return nullptr;
            }
            //procedure to convert GameSate to Connect4state
            Connect4State * current_state = static_cast<Connect4State*>(State);
            /**
              MinMaxAlphaBeta function to return the best suitable column along with the score it got.
              SelectedMove[0] has score
              SelectedMove[1] has column number*/
            std::array<int, 2> SelectedMove = MinMaxAlphaBeta(current_state,MAX_DEPTH, 0 - INT_MAX, INT_MAX, COMPUTER);
            int MoveIndex  = SelectedMove[1];
            //to set move into the original board
            Move->SetMove( static_cast<Connect4Move*>( Moves[MoveIndex])->GetMove() );

            for(int i = 0; i< Total; i++)
                delete Moves[i];

            Moves.clear();
            return Move;
        }


        std::array<int, 2> MinMaxAlphaBeta(Connect4State * St , unsigned int depth, int alf, int bet, unsigned int p)
        {
            if (depth == 0) {
                /**
                 * if maximum depth is achieved. The node value is attained from EvaluateSate
                 */
                GameBoard * board = St->GetBoard();
                return std::array<int, 2>{EvaluateState(board), -1};
            }
                /**
                 * Getting the possible moves list.
                 */

                std::vector<GameMove*> Moves = St->GetPossibleMoves();
                int Total = Moves.size();

            if (p == COMPUTER) {

                /**
                 *  When its computer turn i.e.AI turn.
                 *  PossibleMove is an array such that:
                 *  PossibleMove[0] = scores
                 *  PossibleMove[1] = column
                 *  Since maximizing, we want lowest possible value initially
                 */

                std::array<int, 2> PossibleMove = {INT_MIN, -1};


                for (unsigned int c = 0; c < Total; c++) { // for each possible move
                        /**
                         * Cloning the Current State as
                         * Then setting up the Move and Call ApplyMove to make the given Move
                         * ApplyMove will check if the current move has make the Computer winner or not
                         */

                        Connect4State * Stemp = static_cast<Connect4State*>(St->Clone());
                        Connect4Move * Move = new Connect4Move;
                        Move->SetMove( static_cast<Connect4Move*>( Moves[c])->GetMove() );
                        Stemp->ApplyMove(Move);
                        int score= 0;
                        /** Checking whether Move has made Computer winner
                            if yes then select this move
                        */
                        if(Stemp->WhoWon() != Stemp->GetTurningPlayer()){
                            score = MinMaxAlphaBeta(Stemp, depth - 1, alf, bet, PLAYER)[0]; // find move based on that new board state
                        }
                        else{
                            PossibleMove[0] = INT_MAX;
                            PossibleMove[1] = -1;
                        }
                        // if better score, replace it, and consider that best move (for now)
                        if (score > PossibleMove[0]) {
                            PossibleMove = {score, (int)c};
                        }
                        alf = max(alf, PossibleMove[0]);
                        if (alf >= bet) { break; } // for Alpha Beta pruning

                }
                return PossibleMove; // return best possible move
            }
            else {
                /**
                 *  When its computer turn i.e.AI turn.
                 *  PossibleMove is an array such that:
                 *  PossibleMove[0] = scores
                 *  PossibleMove[1] = column
                 *  Since minimizing, we want maximum possible value initially
                 */

                std::array<int, 2> PossibleMove = {INT_MAX, -1};

                for (unsigned int c = 0; c < Total; c++) {
                        /**
                         * Cloning the Current State as
                         * Then setting up the Move and Call ApplyMove to make the given Move
                         * ApplyMove will check if the current move has make the Human winner or not
                         */

                        Connect4State * Stemp = static_cast<Connect4State*>(St->Clone());
                        Connect4Move * Move = new Connect4Move;
                        Move->SetMove( static_cast<Connect4Move*>( Moves[c])->GetMove() );
                        Stemp->ApplyMove(Move);
                        int score = 0;

                        /** Checking whether Move has made Human winner
                            if yes then select this move
                        */
                        if(Stemp->WhoWon() != Stemp->GetTurningPlayer()){
                             score = MinMaxAlphaBeta(Stemp, depth - 1, alf, bet, COMPUTER)[0]; // find move based on that new board state
                        }
                        else{
                            PossibleMove[0] = INT_MAX;
                            PossibleMove[1] = -1;

                        }
                        // if better score, replace it, and consider that best move (for now)
                        if (score < PossibleMove[0]) {
                            PossibleMove = {score, (int)c};
                        }
                        bet = min(bet, PossibleMove[0]);
                        if (alf >= bet) { break; } // for Alpha Beta pruning

                }
                return PossibleMove; // return best possible move
            }
        }
        //////////////////////////////////////////////////////////
        ////////////////////Taken from Internet//////////////////
        /**
        * EvaluationState is taken form Internet and changed according to the MinMaxAlphaBeta
        */
        double EvaluateState(GameBoard* Board)
        {
            Connect4Board * current_board = static_cast<Connect4Board*>(Board);
            int p = COMPUTER;
            int score = 0;
                std::vector<unsigned int> rs(NUM_COL);
                std::vector<unsigned int> cs(NUM_ROW);
                std::vector<unsigned int> set(4);
                /**
                 * horizontal checks, we're looking for sequences of 4
                 * containing any combination of AI, PLAYER, and empty pieces
                 */
                for (unsigned int r = 0; r < NUM_ROW; r++) {
                    for (unsigned int c = 0; c < NUM_COL; c++) {
                        if(current_board->State[r][c] == 'H')
                            rs[c] =  PLAYER;// this is a distinct row alone
                        else if (current_board->State[r][c] == '.')
                            rs[c] = 0;
                        else
                            rs[c] = COMPUTER;
                    }
                    for (unsigned int c = 0; c < NUM_COL - 3; c++) {
                        for (int i = 0; i < 4; i++) {
                            set[i] = rs[c + i]; // for each possible "set" of 4 spots from that row
                        }
                        score += scoreSet(set, p); // find score
                    }
                }
                 /**
                 * Vertical checks, we're looking for sequences of 4
                 * containing any combination of AI, PLAYER, and empty pieces
                 */
                for (unsigned int c = 0; c < NUM_COL; c++) {
                    for (unsigned int r = 0; r < NUM_ROW; r++) {

                        if(current_board->State[r][c] == 'H')
                            cs[r] =  PLAYER;// this is a distinct row alone
                        else if (current_board->State[r][c] == '.')
                            cs[r] = 0;
                        else
                            cs[r] = COMPUTER;
                    }
                    for (unsigned int r = 0; r < NUM_ROW - 3; r++) {
                        for (int i = 0; i < 4; i++) {
                            set[i] = cs[r + i];
                        }
                        score += scoreSet(set, p);
                    }
                }
                /**
                 * Diagonal checks, we're looking for sequences of 4
                 * containing any combination of AI, PLAYER, and empty pieces
                 */
                for (unsigned int r = 0; r < NUM_ROW - 3; r++) {
                    for (unsigned int c = 0; c < NUM_COL; c++) {

                        if(current_board->State[r][c] == 'H')
                            rs[c] =  PLAYER;// this is a distinct row alone
                        else if (current_board->State[r][c] == '.')
                            rs[c] = 0;
                        else
                            rs[c] = COMPUTER;
                    }
                    for (unsigned int c = 0; c < NUM_COL - 3; c++) {
                        for (int i = 0; i < 4; i++) {
                            //set[i] = b[r + i][c + i];
                            if(current_board->State[r + i][c + i] == 'H')
                                set[i] =  PLAYER;// this is a distinct row alone
                            else if (current_board->State[r + i][c + i] == '.')
                                set[i] = 0;
                            else
                                set[i] = COMPUTER;
                        }
                        score += scoreSet(set, p);
                    }
                }
                for (unsigned int r = 0; r < NUM_ROW - 3; r++) {
                    for (unsigned int c = 0; c < NUM_COL; c++) {
                       // rs[c] = b[r][c];
                       if(current_board->State[r][c] == 'H')
                            rs[c] =  PLAYER;// this is a distinct row alone
                       else if (current_board->State[r][c] == '.')
                            rs[c] = 0;
                        else
                            rs[c] = COMPUTER;
                    }
                    for (unsigned int c = 0; c < NUM_COL - 3; c++) {
                        for (int i = 0; i < 4; i++) {
                            //set[i] = b[r + 3 - i][c + i];
                            if(current_board->State[r + 3 - i][c + i] == 'H')
                                set[i] =  PLAYER;// this is a distinct row alone
                            else if (current_board->State[r + 3 - i][c + i] == '.')
                                set[i] = 0;
                            else
                                set[i] = COMPUTER;
                        }
                        score += scoreSet(set, p);
                    }
                }

                return double(score);

        }


        int scoreSet(std::vector<unsigned int> v, unsigned int p)
        {
            	unsigned int good = 0; // points in favor of p
                unsigned int bad = 0; // points against p
                unsigned int empty = 0; // neutral spots
                for (unsigned int i = 0; i < v.size(); i++) { // just enumerate how many of each
                    good += (v[i] == p) ? 1 : 0;
                    bad += (v[i] == PLAYER || v[i] == COMPUTER) ? 1 : 0;
                    empty += (v[i] == 0) ? 1 : 0;
                }
                // bad was calculated as (bad + good), so remove good
                bad -= good;
                return heurFunction(good, bad, empty);

        }
        int heurFunction(unsigned int g, unsigned int b, unsigned int z)
        {
            int score = 0;
            if (g == 4) { score += 500001; } // preference to go for winning move vs. block
            else if (g == 3 && z == 1) { score += 5000; }
            else if (g == 2 && z == 2) { score += 500; }
            else if (b == 2 && z == 2) { score -= 501; } // preference to block
            else if (b == 3 && z == 1) { score -= 5001; } // preference to block
            else if (b == 4) { score -= 500000; }
            return score;
        }
        //////////////////////////////////////////////////////////
};

#endif // AUTOC4PLAYER_H
