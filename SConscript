import os

env = Environment(
    CPPPATH=['include'],
    CCFLAGS=[
        '-g',
        '-Os',
        '-ffunction-sections',
        '-fdata-sections',
        '-flto',
    ],
    CFLAGS=['-std=c99'],
    LINKFLAGS=['-flto'],
)

# for color terminal output when available
if 'TERM' in os.environ:
    env['ENV']['TERM'] = os.environ['TERM']

if env['PLATFORM'] in 'darwin':
    env['CC'] = 'clang'
    env.Append(
        CCFLAGS=['-Weverything', '-Werror', '-Wno-padded'],
        LINKFLAGS=['-dead_strip'],
    )
elif env['PLATFORM'] in 'linux':
    env['CC'] = 'gcc'
    env.Append(LINKFLAGS=['--gc-sections'])

librt = env.StaticLibrary(target='rt', source=['src/rt.c', 'src/delay.c', 'src/queue.c', 'src/sem.c'])

pthread_env = env.Clone()
pthread_env.Append(
    CPPPATH='pthread',
    CCFLAGS='-pthread',
    LINKFLAGS='-pthread',
)

pthread_simple = pthread_env.Object(target='pthread_simple', source='examples/simple.c')
pthread_delay = pthread_env.Object(target='pthread_delay', source='examples/delay.c')
pthread_queue = pthread_env.Object(target='pthread_queue', source='examples/queue.c')
pthread_env.Program([pthread_simple, librt, 'pthread/rt_port.c'])
pthread_env.Program([pthread_delay, librt, 'pthread/rt_port.c'])
pthread_env.Program([pthread_queue, librt, 'pthread/rt_port.c'])
