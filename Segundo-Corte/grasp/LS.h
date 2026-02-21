#ifndef LOCAL_SEARCH_H
#define LOCAL_SEARCH_H

#include <vector>

std::vector<int> local_search_insertion(std::vector<int> secuencia,
                                        const std::vector<std::vector<int>>& tiempos,
                                        int m);

#endif