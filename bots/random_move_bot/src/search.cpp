#include <chrono>
#include <atomic>
#include <stop_token>
#include <mutex>

#include "search.hpp"
#include "game_state.hpp"
#include "util.hpp"


void search_position(const std::stop_token& stop_token, SearchRequest& search_request, const std::atomic<bool>& new_request, std::mutex& mutex) {

	auto start = std::chrono::steady_clock::now();

	while (true) {

		

	}

	
}


/*
To Prevent Data Race:
SearchRequest& search_request
std::atomic<bool>& new_request


*/