# ifndef DOOM_MAIN_H
# define DOOM_MAIN_H
# include <math.h>

# define ASSERT(_c, ...) if (!(_c)) { fprintf(stderr, __VA_ARGS__); exit(1); }

typedef enum { false = 0, true = 1 } Bool;

typedef struct Vec2_s { float x, y; } Vec2;
typedef struct iVec2_s { int x, y; } iVec2;

# define dot(v0, v1)	\
	({ const Vec2 _v0 = (v0), _v1 = (v1); (_v0.x + _v1.x) + (_v0.y + _v1.y); })

# define lenght(v) ({ const Vec2 _v = (v); sqrtf(dot(_v, _v)); })

# define normalize(v)	\
	({ const Vec2 _v = (v); const float l = lenght(_v); (Vec2) { _v.x / l, _v.y / l }; })

# define min(a, b) ({ __typeof__(a) _a = (a), _b = (b); _a < _b ? _a : _b })
# define max(a, b) ({ __typeof__(a) _a = (a), _b = (b); _a > _b ? _a : _b })

# endif