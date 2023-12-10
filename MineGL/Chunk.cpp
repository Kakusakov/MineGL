#include "Chunk.hpp"

std::vector<unsigned char> compressChunk(const Chunk& chunk) {
	std::vector<unsigned char> data;
	for (const unsigned char& byte : chunk.blocksCache) {
		data.push_back(byte);
	}
	/*for (const unsigned char& byte : chunk.heightmapCache) {
		data.push_back(byte);
	}*/
	return data;
}
bool tryDecompressChunk(const std::vector<unsigned char>& data, Chunk& chunk) {
	const size_t chunkBlockCount = Chunk::Width * Chunk::Width * Chunk::Height;
	if (data.size() != chunkBlockCount) return false;
	chunk = Chunk();
	chunk.blocksCache = std::vector<unsigned char>(chunkBlockCount);
	for (size_t i = 0; i < chunkBlockCount; i++) {
		chunk.blocksCache[i] = data[i];
	}
	/*for (size_t i = 0; i < ((size_t)Chunk::Width * Chunk::Width); i++) {
		chunk.heightmapCache[i] = data[chunkBlockCount + i];
	}*/
	return true;
}
