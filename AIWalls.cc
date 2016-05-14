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
    struct cellDistance {
        Pos position;
        int movements;
        Dir firstDirection;
    };

    struct decision {
        string action;
        Dir direction;
    };

    vector<Dir> directions;
    typedef vector<vector<bool> > Matrix;

    /**
     * Function for my player
     */
    vector<Dir> createDirections() {
        return {Top, Bottom, Left, Right};
    }

    queue<cellDistance> breadthFirstSearch(const Pos &poquemonPosition) const {
        queue<cellDistance> q;
        q.push({poquemonPosition, 0});
        Matrix board (rows(), vector<bool> (cols(), false));
        board[poquemonPosition.i][poquemonPosition.j] = true;
        queue<cellDistance> pq;

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
                        if (cellType != Empty) pq.push({nextPosition, movements, direction});
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
        queue<cellDistance> pq;
        pq = breadthFirstSearch(poquemonPosition);
        if (not pq.empty()) {
            string s = "move";
            Dir d = pq.front().firstDirection;
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
