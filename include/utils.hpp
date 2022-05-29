#pragma once

#include <poll.h>

template <class T>
struct greaterFd : std::binary_function <T,T,bool> {
  bool operator() (const pollfd& x, const pollfd& y) const {return x.fd > y.fd ;}
};


pollfd make_fd(int fd, int event);