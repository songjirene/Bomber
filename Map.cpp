
#include "Map.h"
#include "Errors.h"

// Member Functions

Map::~Map() {
    if (grid) {
        for (int i = 0; i < rows; ++i) {
            delete[] grid[i];
        }
        delete[] grid;
        grid = nullptr;
    }

    columns = 0;
    rows = 0;
}

Map::Map(std::istream& stream) {
    std::vector<std::string> allData;
    std::string line;
    while (getline(stream, line)) {
        if (!line.empty()) {
            allData.push_back(line);
        }
    }

    rows = static_cast<int>(allData.size());
    columns = static_cast<int>(allData[0].length());

    uf = UnionFind(rows, columns);
    grid = new Node*[rows]();

    for (int i = 0; i < rows; i++) {
        grid[i] = new Node[columns];
    }

    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < columns; x++) {
            //update maxBombCount
            if(allData[y][x] == '*')
            {
                maxBombCount++;
                int bombIndex = y*columns + x;
                allBombs[bombIndex] = Node(allData[y][x],y,x);
            }
            //update maxBouldersCount
            else if(allData[y][x] == '#')
                maxBouldersCount++;
            grid[y][x] = Node(allData[y][x], y, x);
        }
    }

    //connect the connected nodes in uf
    uf.connectAll((const Node**)grid);

    //updates num of bombs per region
    uf.assignBombs((const Node**)grid);
}

std::string Map::route(Point src, Point dst) {
    if(!CheckStartPoint(src)) {
        throw PointError(src);
    }

    else if(!CheckEndPoint(dst)) {
        throw PointError(dst);
    }

    // fin = dst; // do we need?
    int initialBombCount = 0;
    
    std::set<std::tuple<int, int, int>> visited;
    std::priority_queue<SearchState, std::vector<SearchState>, CompareStates> stateQueue(CompareStates(SearchState(dst.lat, dst.lng, 0, "", {}, {}, 0)));

    std::set<int> initialBombs;
    std::set<int> bombedBoulders;
    if (grid[src.lat][src.lng].type == '*') {
        initialBombCount = 1;
        initialBombs.insert(src.lat*columns + src.lng);
    }
    SearchState initialState(src.lat, src.lng, initialBombCount,"",initialBombs, bombedBoulders, 0);

    stateQueue.push(initialState);
    
    visited.insert(std::make_tuple(src.lat, src.lng, initialBombCount));

    while (!stateQueue.empty()) {
        SearchState current = stateQueue.top();
        stateQueue.pop();

        if (current.lat == dst.lat && current.lng == dst.lng) {
            return current.route;
        }
        neighbors(current, dst, stateQueue, uf, visited);
    }
    throw RouteError(src, dst);
}

void Map::neighbors(SearchState &current, const Point &dst, std::priority_queue<SearchState,  std::vector<SearchState>, CompareStates> &stateQueue, UnionFind& thisUF, std::set<std::tuple<int,int,int>>& visited) {
    // cout << "Current bombs for " << current.lat << " " << current.lng << ": " << current.bombs << endl;
    for (int d = 0; d < 4; d++) {
        int neighborY = current.lat + dr[d];
        int neighborX = current.lng + dc[d];

        if (neighborY < 0 || neighborX < 0 || neighborX >= columns || neighborY >= rows) {
            continue;
        }

        char cellType = grid[neighborY][neighborX].type;
        int newBombCount = current.bombs;
        bool canVisit = false;
        std::set<int> newUsedBombs = current.pickedUpBombs;
        std::set<int> newBombedBoulders = current.bombedBoulders;

        if (cellType == '.' || cellType == '*') {
            canVisit = true;
            if (cellType == '*') { //&& !visited.count(std::make_tuple(neighborY, neighborX, newBombCount))
                int bombID = neighborY*columns + neighborX;
                if(!newUsedBombs.count(bombID)) { //if not alr picked up
                    newUsedBombs.insert(bombID);
                    newBombCount++;
                }
                //std::min(newBombCount + 1, maxBouldersCount);
            }
        }
        else if (cellType == '#') {
            if(current.bombedBoulders.count(neighborY*columns + neighborX)) //if boulder was already bombed
            {
                canVisit = true;
            }
            else if (newBombCount > 0) {
                //std::cout << "num of bombs: " << newBombCount << std::endl;
                int neighborDist = abs(dst.lat-neighborY) + abs(dst.lng-neighborX);
                int currentDist = abs(dst.lat-current.lat) + abs(dst.lng-current.lng);
                if (thisUF.shouldBomb((const Node**)grid, grid[current.lat][current.lng], grid[neighborY][neighborX], grid[dst.lat][dst.lng], newBombCount) || neighborDist<currentDist) { 
                    canVisit = true;
                    //std::cout << "bombing " << neighborY << " " << neighborX << std::endl;
                    newBombedBoulders.insert(neighborY*columns + neighborX);
                    //bombingSim(grid[neighborY][neighborX], thisUF);
                    newBombCount--;   
                }
            }
        }

        if (canVisit) {
            auto neighborState = std::make_tuple(neighborY, neighborX, newBombCount);
            if (!visited.count(neighborState)) {
                visited.insert(neighborState);
                int newCost = current.cost + 1;
                SearchState next(neighborY, neighborX, newBombCount, current.route+directions[d], newUsedBombs, newBombedBoulders, newCost);
                //std::cout << "current route: " << current.route << directions[d] << std::endl;
                stateQueue.push(next);
            }
        }
    }
}

bool Map::isWalkable(Node cell) {
    if (cell.type == '*' || cell.type == '.') {
        return true;
    }
    return false;
}

// void Map::bombingSim(Node boulder, UnionFind& thisUF) {
//     int bRow = boulder.y;
//     int bCol = boulder.x;
//     int boulderIdx = thisUF.getIndex(boulder);

//     if (bCol + 1 < columns && isWalkable(grid[bRow][bCol+1])) {
//         thisUF.unite(boulderIdx, thisUF.getIndex(grid[bRow][bCol+1]));
//     }
//     if (bCol - 1 >= 0 && isWalkable(grid[bRow][bCol-1])) {
//         thisUF.unite(boulderIdx, thisUF.getIndex(grid[bRow][bCol-1]));
//     }
//     if (bRow + 1 < rows && isWalkable(grid[bRow+1][bCol])) {
//         thisUF.unite(boulderIdx, thisUF.getIndex(grid[bRow+1][bCol]));
//     }
//     if (bRow - 1 >= 0 && isWalkable(grid[bRow-1][bCol])) {
//         thisUF.unite(boulderIdx, thisUF.getIndex(grid[bRow-1][bCol]));
//     }
// }

bool Map::CheckStartPoint(Point start) {
    if (start.lat < 0 || start.lng < 0 || start.lat > rows - 1 || start.lng > columns - 1) {
        return false;
    }

    char t = grid[start.lat][start.lng].type;
    if (t != '.' && t != '*') {
        return false;
    }
    return true;
}

bool Map::CheckEndPoint(Point end) {
    if (end.lat < 0 || end.lng < 0 || end.lat > rows - 1 || end.lng > columns - 1) {
        return false;
    }
    return true;
}