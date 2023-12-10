#pragma once
#include <chrono>

// TODO: this is unnecessary wrapping
class Timer {
private:
	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
public:
	inline void reset() {
		start = std::chrono::system_clock::now();
	}
	inline float time() const {
		return std::chrono::duration<float>(
			std::chrono::system_clock::now() - start
		).count();
	}
};

