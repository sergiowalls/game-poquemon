#include "Player.hh"

using namespace std;

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Wallsi

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

            if (a.movements != b.movements) return a.movements > b.movements;
            else {
                CType cellTypeA = a.cellType;
                CType cellTypeB = b.cellType;
                if (cellTypeA == cellTypeB and cellTypeA == Point) return a.pointsValue < b.pointsValue;
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

    priority_queue<cellInfo, vector<cellInfo>, bestCell> breadthFirstSearch(const Pos& poquemonPosition) {
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

                    if (cellType != Wall and not board[nextPosition.i][nextPosition.j] and
                            (ghost_wall(nextPosition) == -1 or ghost_wall(nextPosition) > movements)) {
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

    vector<int> findEnemies(const Pos& poquemonPosition, int scope) {
        vector<int> enemies(4, -1);
        for (int i = 0; i < 4; ++i) {
            Pos nextPosition = poquemonPosition + directions[i];
            int j = 1;
            while (j <= scope and cell_type(nextPosition) != Wall and cell_id(nextPosition) == -1) {
                nextPosition = nextPosition + directions[i];
                ++j;
            }
            enemies[i] = cell_id(nextPosition);
        }
        return enemies;
    }

    pair<bool, Dir> possibleAttack(const Pos& p, int scope) {
        vector<int> nearEnemies = findEnemies(p, scope);
        Poquemon myPoquemon = poquemon(me());
        for (int i = 0; i < 4; ++i) {
            if (nearEnemies[i] != -1) {
                Dir d = directions[i];
                Poquemon opponent = poquemon(nearEnemies[i]);
                if (opponent.defense <= myPoquemon.attack or
                        opponent.points >= myPoquemon.points or
                        opponent.attack < myPoquemon.defense) return {true, d};
            }
        }
        return {false, Top};
    };

    decision chooseAction(const Poquemon& p) {
        Pos poquemonPosition = p.pos;
        priority_queue<cellInfo, vector<cellInfo>, bestCell> pq;
        pq = breadthFirstSearch(poquemonPosition);
        Dir d;
        string s;
        pair<bool, Dir> attack = possibleAttack(poquemonPosition, p.scope);
        if (attack.first) {
            s = "attack";
            d = attack.second;
            return {s, d};
        }
        while (not pq.empty()) {
            cellInfo cell = pq.top();
            if (cell.cellType == Stone and hasMaxStones() or cell.cellType == Scope and hasMaxScope()) {
                d = cell.firstDirection;
                pq.pop();
            }
            else {
                s = "move";
                d = cell.firstDirection;
                return {s, d};
            }
        }
        string s = "move";
        d = Top;
        return {s, d};
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
