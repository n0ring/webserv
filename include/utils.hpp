#pragma once

#include <poll.h>

pollfd make_fd(int fd, int event); // inline? 