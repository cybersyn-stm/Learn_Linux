


c_src := $(shell find src -name *.c)
c_objs := $(patsubst src/%.c,objs/%.o,$(c_src))


objs/%.o : src/%.c
	@mkdir -p $(dir $@)
	@gcc -c $< -o $@

workspace/exec : $(c_objs)
	@mkdir -p $(dir $@)
	@gcc $^ -o $@

run : workspace/exec
	@./$<

debug:
	@echo $(c_objs)

clean:
	@rm -f src/*.o


.PHONY : debug
