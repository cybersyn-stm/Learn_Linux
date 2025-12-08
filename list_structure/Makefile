#找到src目录下所有.c文件
c_src := $(shell find src -name *.c)
#修改字符串，将%.c修改为%.o
c_objs := $(patsubst src/%.c,objs/%.o,$(c_src))
EXECUTABLE := myprogram  # 可执行文件名

# 默认目标：生成可执行文件并执行
all: compile exe run

# 编译所有源文件为目标文件
objs/%.o : src/%.c
	@mkdir -p $(dir $@)
	gcc -c $< -o $@

# 链接所有目标文件生成可执行文件
$(EXECUTABLE) : $(c_objs)
	gcc $^ -o $@

# 编译目标
compile : $(c_objs)

# 生成可执行文件
exe : $(EXECUTABLE)

# 运行可执行文件
run : $(EXECUTABLE)
	@echo "运行程序: ./$(EXECUTABLE)"
	@./$(EXECUTABLE)

# 清理生成的文件
clean:
	rm -rf objs $(EXECUTABLE)

# 调试信息
debug:
	@echo "源文件: $(c_src)"
	@echo "目标文件: $(c_objs)"
	@echo "可执行文件: $(EXECUTABLE)"

.PHONY : all compile exe run clean debug
