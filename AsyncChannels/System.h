#pragma once
#include <boost/asio.hpp>


extern boost::asio::io_service io_service;

template<typename Func>
void dispatch(Func&& func) {
	io_service.dispatch(func);
}

//void main();

