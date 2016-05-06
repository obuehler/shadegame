#ifndef __SHADOWCOUNT_H__
#define __SHADOWCOUNT_H__

/** Wrapper class for the integer used to count the shadows a sensor fixture of
 * the character is currently in. */
class ShadowCount {
private:
	int scount;
public:
	ShadowCount() : scount(0) {}
	inline void inc() { scount++; }
	inline void dec() { scount--; }
	int count() const { return scount; }
};

#endif /** __SHADOWCOUNT_H__ */