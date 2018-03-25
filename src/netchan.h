#pragma once
bool net_init();
void net_close();
std::string net_send_post( char *header, char *post );