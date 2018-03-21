#include "Position.hpp"
#include "Queue.hpp"
#include <vector>

std::vector<Position> pathPos = {Position{12,0}, Position{11,0}, Position{10,0},
                        Position{9,0}, Position{8,0}, Position{7,0}, Position{7,1},
                        Position{7,2}, Position{7,3}, Position{7,4}, Position{7,5},
                        Position{7,6}};

Queue<Position> queue;

int main(void)
{
    queue.init();
    for (int i = 0; i < pathPos.size(); i++) {
        queue.pushToBack(pathPos[i]);
    }
    return 0;
}