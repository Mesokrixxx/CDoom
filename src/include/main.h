# ifndef DOOM_MAIN_H
# define DOOM_MAIN_H
# include <stdint.h>

# define ASSERT(_c, ...) if (!(_c)) { fprintf(stderr, __VA_ARGS__); exit(1); }

typedef float    f32;
typedef double   f64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef enum { false = 0, true = 1 } Bool;

typedef struct Vec2_s { f32 x, y; } Vec2;
typedef struct Vec2i_s { i32 x, y; } Vec2i;

#define dot(v0, v1)		\
    ({ const Vec2 _v0 = (v0), _v1 = (v1); (_v0.x * _v1.x) + (_v0.y * _v1.y); })

#define length(v) ({ const Vec2 _v = (v); sqrtf(dot(_v, _v)); })

#define normalize(u)	\
    ({ const Vec2 _u = (u); const f32 l = length(_u); (Vec2) { _u.x / l, _u.y / l }; })

#define min(a, b) ({ __typeof__(a) _a = (a), _b = (b); _a < _b ? _a : _b; })
#define max(a, b) ({ __typeof__(a) _a = (a), _b = (b); _a > _b ? _a : _b; })

#define sign(a) 
	({ __typeof__(a) _a = (a); (__typeof__(a))(_a < 0 ? -1 : (_a > 0 ? 1 : 0)); })

# endif