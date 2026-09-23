/// @file types/_tokens.h

#ifndef TOKENS_H_
#define TOKENS_H_

#define num_1(a		 ) ((a) <<  0)
#define num_2(a,b	 ) ((a) <<  8) | num_1(b)
#define num_3(a,b,c	 ) ((a) << 16) | num_2(b,c)
#define num_4(a,b,c,d) ((a) << 24) | num_3(b,c,d)

#define num_dispatch(_1, _2, _3, _4, NAME, ...) NAME
#define num(...) num_dispatch(__VA_ARGS__, num_4, num_3, num_2, num_1)(__VA_ARGS__)

/**
 * @enum RxToken
 * @brief An enum holding every regex token to match against.
 */
enum RxToken {
	RXT_1 = num('1'),
	RXT_2 = num('1', '2'),
	RXT_3 = num('1', '2', '3'),
	RXT_4 = num('1', '2', '3', '4'),
};

#endif /* !TOKENS_H_ */
