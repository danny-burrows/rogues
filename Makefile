incl_dir = include
src_dir  = src
obj_dir  = obj
libs 	 = -lm -lpthread

srcs = $(wildcard src/*.c)
objs = $(srcs:src/%.c=obj/%.o)

ifeq ($(config), debug)
    flags = -I include -g -D DCONFIGSET
else
	flags = -I include -O3
endif

$(obj_dir)/%.o: $(src_dir)/%.c | $(obj_dir)
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
