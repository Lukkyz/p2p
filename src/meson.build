inih_dep = dependency('inih', fallback : ['inih', 'inih_dep'])
openssl_dep = dependency('openssl')
libevent_dep = dependency('libevent')
thread_dep = dependency('threads')
eventthread_dep = dependency('libevent_pthreads')
msgpack_dep = dependency('msgpack')
sources = ['message.c', 'hash.c', 'utils.c', 'main.c', 'network.c', 'file.c']
exe = executable('main', sources, dependencies: [inih_dep, openssl_dep, libevent_dep, thread_dep, eventthread_dep, msgpack_dep])
