#include <World.hpp>
#include <algorithm>
World::World() {
	for (int i = 0; i < W; ++i)
		for (int j = 0; j < W; ++j)
			for (int k = 0; k < W; ++k)
				block[i][j][k] = 0;
}

int World::get_block_type(int x, int y, int z) {
	if (std::min({ x, y, z }) < 0 || std::max({ x, y, z }) >= W) {
		return 0;
	}
	return block[x][y][z];
}
void World::modify_block_type(int x, int y, int z, int t) {
	if (std::min({ x, y, z }) < 0 || std::max({ x, y, z }) >= W) {
		return ;
	}
	block[x][y][z] = t;
}