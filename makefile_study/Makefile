c_srcs = $(shell find src -name *.c)
c_objs = $(patsubst src/%.c,obj/%.o,$(c_srcs))
run : exe
	@./exe

exe : $(c_objs) 
	@echo "executive program:"
	@gcc $^ -o $@

obj/%.o : src/%.c
	@mkdir -p $(dir $@)
	@gcc -c $^ -o $@

compile : $(c_objs)

debug :
	@echo $(c_srcs)
	@echo $(c_objs)
.PHONY : debug  compile exe
