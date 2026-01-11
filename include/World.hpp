#ifndef WORLD_HPP
#define WORLD_HPP

const int W = 64;

struct World {
	public:
	World();
	int get_block_type(int x, int y, int z);
	void modify_block_type(int x, int y, int z, int t);

	private:
		int block[W][W][W];
};


#endif