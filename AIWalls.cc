#include "Player.hh"

using namespace std;

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Walls

struct PLAYER_NAME : public Player {

    /**
     * Factory: returns a new instance of this class.
     * Do not modify this function.
     */
    static Player *factory() {
        return new PLAYER_NAME;
    }

    /**
     * Attributes for your player can be defined here.
     */
    struct cellInfo {
        Pos position;
        int movements;
        Dir firstDirection;
        CType cellType;
        int pointsValue;
    };

    struct decision {
        string action;
        Dir direction;
    };

    struct bestCell {
        bool operator()(const cellInfo& a, const cellInfo& b) const {

            CType cellTypeA = a.cellType;
            CType cellTypeB = b.cellType;

            if (cellTypeA == cellTypeB) {
                if (cellTypeA == Point) return a.pointsValue/a.movements < b.pointsValue/b.movements;
                else return a.movements > b.movements;
            }
            else if (cellTypeA == Point) return false;
            else if (cellTypeB == Point) return true;
            else if (cellTypeA == Stone) return false;
            else if (cellTypeB == Stone) return true;
            else if (cellTypeA == Scope) return false;
            else if (cellTypeB == Scope) return true;
            else if (cellTypeA == Attack) return false;
            else if (cellTypeB == Attack) return true;
            else if (cellTypeA == Defense) return false;
            return true;

        }
    };

    vector<Dir> directions;
    typedef vector<vector<bool> > Matrix;

    /**
     * Function for my player
     */
    vector<Dir> createDirections() {
        return {Top, Bottom, Left, Right};
    }

    bool hasMaxStones() {
        return poquemon(me()).stones >= max_stone();
    }

    bool hasMaxScope() {
        return poquemon(me()).scope >= max_scope();
    }

    priority_queue<cellInfo, vector<cellInfo>, bestCell> breadthFirstSearch(const Pos &poquemonPosition) const {
        queue<cellInfo> q;
        q.push({poquemonPosition, 0});
        Matrix board (rows(), vector<bool> (cols(), false));
        board[poquemonPosition.i][poquemonPosition.j] = true;
        priority_queue<cellInfo, vector<cellInfo>, bestCell> pq;

        while (not q.empty()) {
            Pos position = q.front().position;
            int movements = q.front().movements + 1;

            for (auto it : directions) {
                Pos nextPosition = position + it;
                if (pos_ok(nextPosition)) {
                    CType cellType = cell_type(nextPosition);
                    Dir direction;

                    if (movements == 1) direction = it;
                    else direction = q.front().firstDirection;

                    if (cellType != Wall and not board[nextPosition.i][nextPosition.j]) {
                        if (cellType != Empty) pq.push({nextPosition, movements, direction, cellType, points_value(nextPosition)});
                        q.push({nextPosition, movements, direction});
                    }

                    board[nextPosition.i][nextPosition.j] = true;
                }
            }

            board[position.i][position.j] = true;
            q.pop();
        }

        return pq;
    }

    decision chooseAction(const Poquemon& p) {
        Pos poquemonPosition = p.pos;
        priority_queue<cellInfo, vector<cellInfo>, bestCell> pq;
        pq = breadthFirstSearch(poquemonPosition);
        if (not pq.empty()) {
            string s = "move";
            Dir d = pq.top().firstDirection;
            return {s, d};
        }
        else {
            return {"move", Top};
        }
    }

    /**
         * Play method.
         *
         * This method will be invoked once per each round.
         * You have to read the board here to place your actions
         * for this round.
         *
         */
    virtual void play() {

        Poquemon p = poquemon(me());
        if (round() == 0)
            directions = createDirections();
        if (p.alive) {
            decision d = chooseAction(p);
            if (d.action == "move") move(d.direction);
            else attack(d.direction);
        }

    }


};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
