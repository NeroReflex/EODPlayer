#pragma once

#include <mutex>
#include <atomic>
#include <optional>

template <typename T, typename Allocator = std::allocator<T>>
class DequeHM {
	
public:
	void push_back(T&& input) noexcept;
	void push_front(T&& input) noexcept;
	std::optional<T> pop_front() noexcept;
	std::optional<T> pop_back() noexcept;
	std::atomic_bool is_empty() noexcept;
	std::atomic_long size() noexcept;
	
	DequeHM() noexcept;
	~DequeHM();
private:
    class Node {
    public:
        T val;
        Node* next = nullptr;
        Node* prev = nullptr;
        Node(T&& input) : val(std::move(input)) {};
    };

	std::mutex start_mutex;
	std::condition_variable cv;
	std::mutex wait_mutex;
	std::mutex front_mutex;
	std::mutex back_mutex;

    Node* first_node = nullptr;
	Node* last_node = nullptr;
	std::atomic_long deque_size;
};

template <typename T, typename Allocator>
DequeHM<T, Allocator>::DequeHM() noexcept : deque_size(0) {}


template <typename T, typename Allocator>
void DequeHM<T, Allocator>::push_back(T&& input) noexcept {

	// allocator
	using node_alloc_t = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
	node_alloc_t node_alloc;

	// allocate and call the move constructor
	auto new_node = node_alloc.allocate(1);
	using traits_t = std::allocator_traits<decltype(node_alloc)>;
	traits_t::construct(node_alloc, new_node, std::move(input));

	std::lock_guard<std::mutex> guard(back_mutex);
	
	deque_size++;

	if (first_node == nullptr) {
		
		if (start_mutex.try_lock()) {
			first_node = new_node;
			last_node = new_node;
			start_mutex.unlock();
			cv.notify_all();
			return;
		}
		else {
			std::unique_lock<std::mutex> lk(wait_mutex);
			cv.wait(lk);
		}
	}
	new_node->prev = last_node;
	last_node->next = new_node;
	last_node = new_node;
}

template <typename T, typename Allocator>
void DequeHM<T, Allocator>::push_front(T&& input) noexcept {
	
	// allocator
	using node_alloc_t = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
	node_alloc_t node_alloc;

	// allocate and call the move constructor
	auto new_node = node_alloc.allocate(1);
	using traits_t = std::allocator_traits<decltype(node_alloc)>;
	traits_t::construct(node_alloc, new_node, std::move(input));

	std::lock_guard<std::mutex> guard(front_mutex);
	
	deque_size++;
	if (first_node == nullptr) {
		
		if (start_mutex.try_lock()) {
			first_node = new_node;
			last_node = new_node;
			start_mutex.unlock();
			cv.notify_all();
			return;
		}
		else {
			std::unique_lock<std::mutex> lk(wait_mutex);
			cv.wait(lk);
		}
	}
	new_node->next = first_node;
	first_node->prev = new_node;
	first_node = new_node;
}

template <typename T, typename Allocator>
std::optional<T> DequeHM<T, Allocator>::pop_front() noexcept {

	std::lock_guard<std::mutex> guard(front_mutex);

	std::optional<T> result;
	if (first_node != nullptr) {
		result.emplace(std::move(first_node->val));
		//result = first_node->val;
		Node* new_first_node = first_node->next;
		delete(first_node);
		first_node = new_first_node;
		deque_size--;
	}

	return result;
}

template <typename T, typename Allocator>
std::optional<T> DequeHM<T, Allocator>::pop_back() noexcept {

	std::lock_guard<std::mutex> guard(back_mutex);
	
	std::optional<T> result;
	if (last_node != nullptr) {
		//result = std::move(last_node->val);
		result.emplace(std::move(last_node->val));
		Node* new_last_node = last_node->prev;
		delete(last_node);
		last_node = new_last_node;
		deque_size--;
	}
	else throw std::exception("Deque is empty!");

	return result;
}

template <typename T, typename Allocator>
std::atomic_long DequeHM<T, Allocator>::size() noexcept {

	return deque_size;
}

template <typename T, typename Allocator>
std::atomic_bool DequeHM<T, Allocator>::is_empty() noexcept {

	return deque_size == 0;
}

template <typename T, typename Allocator>
DequeHM<T, Allocator>::~DequeHM() noexcept {
	// Cleaning node's chain if not empty
	if (deque_size > 0) {
		Node* node = first_node;
		while (node != nullptr) {
			Node* nextNode = node->next;
			delete(node);
			node = nextNode;
		}
	}
}