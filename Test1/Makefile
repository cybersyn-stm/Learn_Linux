#Makefile



c_srcs := $(shell find src -name *.c)#shell 寻找src文件夹中.c后最文件
c_objs := $(patsubst src/%.c,obj/%.o,$(c_srcs))

include_path := /home/cybersyn/my-c-project/Test1/include

I_flag := $(include_path:%=-I%)

obj/%.o : src/%.c
	@mkdir -p $(dir $@)
	@gcc -c $^ -o $@ $(I_flag)
	
workspace : $(c_objs)
	@mkdir -p $(dir $@)
	@gcc $^ -o $@

run : workspace
	@./$<
clean : 
	@rm obj/*.o
debug :
	@echo $(c_srcs)
	@echo $(c_objs)
	@echo $(I_flag)
.PHONY : debug run clean
