src_dir  = src
obj_dir  = obj
libs 	 = -lm -lpthread

srcs = $(wildcard src/*.c)
hdrs = $(wildcard src/*.h)
objs = $(srcs:src/%.c=obj/%.o)

ifeq ($(config), debug)
    flags = -I src -g -D DCONFIGSET
else
	flags = -I src -O3
endif

$(obj_dir)/%.o: $(src_dir)/%.c $(hdrs) | $(obj_dir)
	gcc -c -o $@ $< $(flags)

rogues: $(objs)
	gcc -o $@ $^ $(libs)

$(obj_dir): 
	mkdir $(obj_dir)

.PHONY: tools clean

tools:
	gcc -o ./tools/makemap ./tools/makemap.c -g

clean:
	rm -rf $(obj_dir) ./tools/makemap rogues 
