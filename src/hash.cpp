#include "hash.hpp"

// Jenkins_one_at_a_time_hash Jenkins OAAT
unsigned int hash(const char *str) {
	unsigned hash, i;
	for (hash = i = 0; str[i] != '\0'; ++i) {
		hash += str[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}
