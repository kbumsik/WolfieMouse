
#ifndef MATH_H_
#define MATH_H_

#define xControlMaxLimitter(VALUE,MAX)  (VALUE<MAX?VALUE:MAX)
#define xControlMinLimitter(VALUE,MIN)  (VALUE>MIN?VALUE:MIN)
#define xControlLimitter(VALUE,MIN,MAX) (xControlMinLimitter(xControlMaxLimitter(VALUE,MAX),MIN))

#define xControlMinusAndScale(VALUE,OFFSET,SCALE) (((VALUE)-(OFFSET))*(SCALE))
#define xControlMinusAndScaleReversed(VALUE,OFFSET,SCALE) ((VALUE)*(SCALE)-(OFFSET))
#define xControlPlusAndScale(VALUE,OFFSET,SCALE) (((VALUE)+(OFFSET))*(SCALE))
#define xControlPlusAndScaleReversed(VALUE,OFFSET,SCALE) ((VALUE)*(SCALE)+(OFFSET))

#endif /* MATH_H_ */